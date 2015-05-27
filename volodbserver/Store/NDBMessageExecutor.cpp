//
//  NDBMessageExecutor.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/10/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "NDBMessageExecutor.h"
#include "Frame/FrameUtils.h"
#include "ProtoBuf/Result.pb.h"
#include "Logging/log1.h"
#include "NDBMessageProcessor.h"
#include "../Test/TestStats.h"
#include <thread>
#include <chrono>

string NDBMessageExecutor::_userName;
string NDBMessageExecutor::_password;
MYSQL* NDBMessageExecutor::_mysql;
Ndb** NDBMessageExecutor::_ndb;
TableInfoCache* NDBMessageExecutor::_tableInfoCache;

void NDBMessageExecutor::initialize(MYSQL* mysql, Ndb** ndb, TableInfoCache* tableInfoCache)
{
    _mysql = mysql;
    _ndb = ndb;
    _tableInfoCache = tableInfoCache;
}

inline void NDBMessageExecutor::addRecordToResult(OperationResult* currentOperationResult, NdbTransactionStateOperationsColumnValues& currentOperationColumnValues)
{
    Row* currentOperationRow = currentOperationResult->add_row();
    for(NdbRecAttr* currentOperationColumnRecord: currentOperationColumnValues)
    {
        ColumnValue* currentOperationColumnValue = currentOperationRow->add_column();
        
        currentOperationColumnValue->set_name(currentOperationColumnRecord->getColumn()->getName());
        currentOperationColumnValue->set_type((Column_Type)currentOperationColumnRecord->getType());
        currentOperationColumnValue->set_value(string(currentOperationColumnRecord->aRef(), currentOperationColumnRecord->get_size_in_bytes()));
    }
}

#pragma mark - NDB callback handling
void NDBMessageExecutor::ndbExecuteCallback(int result, NdbConnection* transaction, void* aObject)
{
    shared_ptr<NdbTransactionState> state((NdbTransactionState*)aObject);

    NdbConnection* transactionRef;
    string* transactionID;
    zmq::message_t* identifier;
    zmq::socket_t* taskSenderSocket;
    NdbTransactionStateOperationsItemPtr transactionOperations;
    zmq::message_t newIdentifier;
    
    tie(transactionRef, transactionID, identifier, taskSenderSocket, transactionOperations) = *state;
    
    newIdentifier.copy(identifier);
    
    assert(transaction == transactionRef);
    ExecutionResult executionResult;

    if(result == -1)
    {
        executionResult = makeResultForError(*transactionID, transaction->getNdbError().message, transaction->getNdbError().code);
        
        FILE_LOG(logINFO)<<"transaction executed unsuccessfully with ID: "<<*transactionID<<". Error: "<<transaction->getNdbError().message;
        
        transaction->close();
        
        replyWithResult(executionResult, newIdentifier, *taskSenderSocket);
    }
    else
    {
        int executorThreadNo = 0;
        shared_ptr<Result> transactionResult = makeResult(*transactionID);
        for(tuple<FrameItem_Type, NdbOperation*, NdbTransactionStateOperationsColumnValuesPtr, int>& currentOperationInfo: *transactionOperations)
        {
            FrameItem_Type& currentOperationType = get<0>(currentOperationInfo);
            executorThreadNo = get<3>(currentOperationInfo);
            
            OperationResult* currentOperationResult = transactionResult->add_result();
            currentOperationResult->set_type(currentOperationType);
            
            if(currentOperationType == FrameItem_Type_GET)
            {
                NdbTransactionStateOperationsColumnValues& currentOperationColumnValues = *get<2>(currentOperationInfo);
                NdbScanOperation* currentOperation = dynamic_cast<NdbScanOperation*>(get<1>(currentOperationInfo));
                if(currentOperation)
                {
                    while(currentOperation->nextResult(true) == 0)
                        addRecordToResult(currentOperationResult, currentOperationColumnValues);
                }
                else
                    addRecordToResult(currentOperationResult, currentOperationColumnValues);
            }
        }
        
        transaction->close();

        replyWithResult(transactionResult, newIdentifier, *taskSenderSocket);

        if(TestStats::shouldRun && !TestStats::isRunning)
        {
//            FILE_LOG(logINFO)<<"Will start collecting stats";
            TestStats::start();
        }
        else if(TestStats::isRunning)
        {
            TestStats::totalOperationsExecutedByServer[executorThreadNo]+=transactionOperations->size();
        }
        
//        int x = TestStats::totalOperationsExecutedByServerAtomic.fetch_add(1) + 1;
//        FILE_LOG(logINFO)<<x;

    }
}

#pragma mark - Operation processing general
inline bool NDBMessageExecutor::execute(FrameMessagesTuple& messagesTuple, zmq::socket_t& taskSenderSocket, const int& threadNo)
{
    NdbTransaction* transaction = nullptr;
    NdbTransactionStateOperationsItemPtr transactionOperations(new NdbTransactionStateOperationsItem);
    zmq::message_t* originalIdentifier;
    string* transactionID;
    
    FrameMessages& frameMessages = *(get<2>(messagesTuple));
    for(FrameMessagePointer& message: frameMessages)
    {
        FrameItem_Type type = FrameUtils::getFrameType(*message);
        transactionID = &get<0>(messagesTuple);
        originalIdentifier = &get<1>(messagesTuple);
        zmq::message_t identifier;
        
        //copy is requied, or a transaciton will more than one operation will cause the
        //message to lost, since identifer message will be emptied after first send.
        identifier.copy(originalIdentifier);
        
        if(FrameUtils::isDefinitionFrame(*message))
        {
            ExecutionResult result;

            if(type == FrameItem_Type_CREATE_DATABASE)
                result = execute(dynamic_cast<CreateDatabaseOperation*>(message.get()), *transactionID, threadNo);
            else if (type == FrameItem_Type_DROP_DATABASE)
                result = execute(dynamic_cast<DropDatabaseOperation*>(message.get()), *transactionID, threadNo);
            else if(type == FrameItem_Type_CREATE_STORE)
                result = execute(dynamic_cast<CreateStoreOperation*>(message.get()), *transactionID, threadNo);
            else if (type == FrameItem_Type_DROP_STORE)
                result = execute(dynamic_cast<DropStoreOperation*>(message.get()), *transactionID, threadNo);

            if(result)
            {
                replyWithResult(result, identifier, taskSenderSocket);
            }
            else
            {
                ExecutionResult failureResult = makeResultForError(*transactionID, "Failed to run definition operation.");
                replyWithResult(failureResult, identifier, taskSenderSocket);
            }
        }
        else
        {
            NdbOperation* newOperation = nullptr;
            NdbTransactionStateOperationsColumnValuesPtr operationColumnValues;

            if(type == FrameItem_Type_SET)
            {
                SetOperation* setOperation = dynamic_cast<SetOperation*>(message.get());
                const StoreInfo& storeInfo = setOperation->store_info(0);
                
                if(!transaction)
                    transaction = _startTransaction(setOperation->primary_key(), /*storeInfo.database_name(), */storeInfo.store_name(), threadNo);

                if(transaction)
                    newOperation = execute(setOperation, transaction, *transactionID, threadNo);
            }
            else if(type == FrameItem_Type_DELETE)
            {
                DeleteOperation* deleteOperation = dynamic_cast<DeleteOperation*>(message.get());
                const StoreInfo& storeInfo = deleteOperation->store_info(0);
                
                if(!transaction)
                    transaction = _startTransaction(deleteOperation->primary_key(), /*storeInfo.database_name(), */storeInfo.store_name(), threadNo);
                
                if(transaction)
                    newOperation = execute(deleteOperation, transaction, *transactionID, threadNo);
            }
            else if(type == FrameItem_Type_GET)
            {
                
                operationColumnValues.reset(new NdbTransactionStateOperationsColumnValues);
                
                GetOperation* getOperation = dynamic_cast<GetOperation*>(message.get());
                const StoreInfo& storeInfo = getOperation->store_info(0);
                
                if(!transaction)
                    transaction = _startTransaction(getOperation->primary_key(), /*storeInfo.database_name(), */storeInfo.store_name(), threadNo);
                
                if(transaction)
                    newOperation = execute(getOperation, operationColumnValues.get(), transaction, *transactionID, threadNo);
            }
            
            if(newOperation)
            {
                transactionOperations->push_back(make_tuple(type, newOperation, operationColumnValues, threadNo));
            }
            else
            {
                ExecutionResult result;
                if(transaction)
                {
                    string error = "Transaction started successfully, but error occured while adding query. Verify that columns flagged as primary or distributed keys and their corresponding types are correct.";
                    error+=string(" Internal error: ") + transaction->getNdbError().message;

                    result = makeResultForError(*transactionID, error, transaction->getNdbError().code);
                    
                    FILE_LOG(logWARNING)<<"Transaction # "<<*transactionID<<" failed: "<<transaction->getNdbError().message;
                    
                    transaction->close();
                    transaction = nullptr;
                }
                else
                {
                    FILE_LOG(logWARNING)<<"Transaction # "<<*transactionID<<" failed";
                    
                    result = makeResultForError(*transactionID, "Unable to start transaction. Verify that store name and distribution key information was provided correctly.");
                }
                
                replyWithResult(result, identifier, taskSenderSocket);

                break;
            }
        }
    }
    
    if(transaction)
    {
        NdbTransactionState* state = new NdbTransactionState(transaction, transactionID, originalIdentifier, &taskSenderSocket, transactionOperations);
        transaction->executeAsynchPrepare(Commit, ndbExecuteCallback, (void*)state);
    }
    
    return transaction!=nullptr;
}

void NDBMessageExecutor::execute(StoreMsgs& messages, zmq::socket_t& taskSenderSocket, const int& threadNo)
{
    vector<shared_ptr<FrameMessagesTupleListPointer>> taskListPointerSmartVector;
    //the above should be deleted after the function returns, since objects in it are pass in ndb execution callback
    
    int totalTransactions = 0;
    for(StoreMsgPtr& messagePtr : messages)
    {
        shared_ptr<FrameMessagesTupleListPointer> taskListPointerSmart;
        
        FrameMessagesTupleListPointer* taskListPointer;
        
        memcpy(&taskListPointer, messagePtr->data(), messagePtr->size());
        
        taskListPointerSmartVector.push_back(shared_ptr<FrameMessagesTupleListPointer>(taskListPointer));
        
        for(FrameMessagesTuplePointer& messagesTuplePointer: **taskListPointer)
        {
            
//            static int abc = 0;
//            abc++;
//            FILE_LOG(logERROR)<<"Executor"<<abc;
            
            bool result = execute(*messagesTuplePointer, taskSenderSocket, threadNo);
            if(result)
            {
                totalTransactions++;
                
                if(totalTransactions == 1024)
                {
                    waitForTransactionsToComplete(totalTransactions, threadNo);
                    totalTransactions = 0;
                }
            }
        }
    }
    
    if(totalTransactions > 0)
        waitForTransactionsToComplete(totalTransactions, threadNo);
    
//    if(totalTransactions > 0)
//    {
//        int completedTransactions = 0;
//
//        completedTransactions+=_ndb[threadNo]->sendPollNdb(WAITFOR_RESPONSE_TIMEOUT, totalTransactions, 0);
//        
//        while(completedTransactions < totalTransactions)
//            completedTransactions+=_ndb[threadNo]->pollNdb(WAITFOR_RESPONSE_TIMEOUT, totalTransactions - completedTransactions);
//    }
}

inline void NDBMessageExecutor::waitForTransactionsToComplete(const int& totalTransactions, const int& threadNo)
{
    if(totalTransactions > 0)
    {
        int completedTransactions = 0;
        
        completedTransactions+=_ndb[threadNo]->sendPollNdb(WAITFOR_RESPONSE_TIMEOUT, totalTransactions, 0);
        
        while(completedTransactions < totalTransactions)
            completedTransactions+=_ndb[threadNo]->pollNdb(WAITFOR_RESPONSE_TIMEOUT, totalTransactions - completedTransactions);
    }
    
}

#pragma mark - Database Operations

inline ExecutionResult NDBMessageExecutor::createDatabase(const string& name, string& transactionID, const int& threadNo)
{
    string createDatabaseOperationStr="CREATE DATABASE IF NOT EXISTS " + name;
    
    return runQuery(createDatabaseOperationStr, transactionID, threadNo);
}

inline ExecutionResult NDBMessageExecutor::execute(CreateDatabaseOperation* createDatabaseOperation, string& transactionID, const int& threadNo)
{
    return createDatabase(createDatabaseOperation->database_name(), transactionID, threadNo);
    
}
inline ExecutionResult NDBMessageExecutor::execute(DropDatabaseOperation* dropDatabaseOperation, string& transactionID, const int& threadNo)
{
    string dropDatabaseOperationStr = "DROP DATABASE " + dropDatabaseOperation->database_name();
    
    return runQuery(dropDatabaseOperationStr, transactionID, threadNo);
}

#pragma mark - Store Operations

inline ExecutionResult NDBMessageExecutor::execute(CreateStoreOperation* createStoreOperation, string& transactionID, const int& threadNo)
{
    vector<int> primaryKeyAttributeIndexes;
    vector<int> distributionKeyAttributeIndexes;
    vector<int> indexAttributeIndexes;
    
    string createTableStr;
    
    const StoreInfo& storeInfo = createStoreOperation->store_info();
    
    createTableStr+="CREATE TABLE " + /*storeInfo.database_name() + "." + */storeInfo.store_name() + "(\n";

    for(int i = 0; i < createStoreOperation->column_size(); i++)
    {
        const ColumnInfo& currentAttribute = createStoreOperation->column(i);
        createTableStr+=(currentAttribute.name() + " " + getTypeName(currentAttribute.type()));
        if(currentAttribute.has_length())
        {
            if(currentAttribute.type() == Column_Type_VARCHAR ||
              currentAttribute.type() == Column_Type_VARBINARY)
            {
               createTableStr+="("+to_string(currentAttribute.length() < 255 ? 256 : currentAttribute.length())+")";
            }
            else if(currentAttribute.type() == Column_Type_CHAR)
            {
               createTableStr+="("+to_string(currentAttribute.length())+")";
            }
            
        }
        
        createTableStr+=" NOT NULL, \n";
        
        if(currentAttribute.has_primary_key())
            primaryKeyAttributeIndexes.push_back(i);
        if(currentAttribute.has_distribution_key())
            distributionKeyAttributeIndexes.push_back(i);
//        if(currentAttribute.has_index())
//            indexAttributeIndexes.push_back(i);
    }
    
    if(primaryKeyAttributeIndexes.size() == 0)
        return makeResultForError(transactionID, "No primary key indicated");
    
    
    createTableStr+="PRIMARY KEY (";
    
    for(int i = 0; i < primaryKeyAttributeIndexes.size(); i++)
    {
        createTableStr+=createStoreOperation->column(primaryKeyAttributeIndexes[i]).name();
        
        if(i != primaryKeyAttributeIndexes.size() - 1 )
            createTableStr+=",";
        else
            createTableStr+=")";
    }
    
    if(distributionKeyAttributeIndexes.size() > 0)
    {
        createTableStr+=",\nINDEX VOLODB_PRUNE_INDEX USING BTREE (";
        
        for(int i = 0; i < distributionKeyAttributeIndexes.size(); i++)
        {
            createTableStr+=createStoreOperation->column(distributionKeyAttributeIndexes[i]).name();
            
            if(i != distributionKeyAttributeIndexes.size() - 1 )
                createTableStr+=",";
            else
                createTableStr+=")";
        }
    }
    
    createTableStr+=")\nENGINE=NDB\n";
    
    if(distributionKeyAttributeIndexes.size() == 0)
        return makeResultForError(transactionID, "No distribution key indicated");
    

    createTableStr+="PARTITION BY KEY (";
    
    for(int i = 0; i < distributionKeyAttributeIndexes.size(); i++)
    {
        createTableStr+=createStoreOperation->column(distributionKeyAttributeIndexes[i]).name();
        
        if(i != distributionKeyAttributeIndexes.size() - 1 )
            createTableStr+=",";
        else
            createTableStr+=")";
    }
    
//    FILE_LOG(logINFO)<<createTableStr;
    ExecutionResult res;
//    ExecutionResult createDatabaseResult;
//    if(createStoreOperation->has_create_database_if_required())
//        createDatabaseResult = createDatabase(storeInfo.database_name(), transactionID, threadNo);
//    
//    if(createDatabaseResult.get() && !createDatabaseResult->has_error_code())
        res =  runQuery(createTableStr, transactionID, threadNo);
//    else
//        res =  createDatabaseResult;
    
//    if(!res->has_error_code())
//        _addTableInfoToCache(storeInfo.database_name(), storeInfo.store_name(), threadNo);
    
    return res;
}

inline ExecutionResult NDBMessageExecutor::execute(DropStoreOperation* dropStoreOperation, string& transactionID, const int& threadNo)
{
    const StoreInfo& storeInfo = dropStoreOperation->store_info();
//    string completeTableName = storeInfo.database_name() + "." + storeInfo.store_name();
    string dropStoreOperationStr = "DROP TABLE " + /*storeInfo.database_name() + "." +*/ storeInfo.store_name();
    
    ExecutionResult res = runQuery(dropStoreOperationStr, transactionID, threadNo);
    
    if(!res->has_error_code())
        _tableInfoCache->invalidate(/*storeInfo.database_name(), */storeInfo.store_name());

    return res;
}

#pragma mark - Set Operation
inline NdbOperation*  NDBMessageExecutor::execute(SetOperation* setOperation , NdbTransaction*& transaction, string& transactionID, const int& threadNo)
{
    const StoreInfo& storeInfo = setOperation->store_info(0);
    int res;
    
    NdbOperation* ndbSetOperation = transaction->getNdbOperation(storeInfo.store_name().c_str());
    
    if(!ndbSetOperation)
        return nullptr;
    
    ndbSetOperation->writeTuple();
    
    res = setEqualPrimaryKey(setOperation, ndbSetOperation);
    if(res == -1)
        return nullptr;
    
//    for(int i = 0; i < setOperation->primary_key_size(); i++)
//    {
//        const ColumnValue& currentPrimaryKeyValue = setOperation->primary_key(i);
//        
//        res = ndbSetOperation->equal(currentPrimaryKeyValue.name().c_str(), currentPrimaryKeyValue.value().data(), static_cast<unsigned int>(currentPrimaryKeyValue.value().size()));
//        
//        if(res == -1)
//            return nullptr;
//        
//    }
    
    for(int i = 0; i < setOperation->value_size(); i++)
    {
        const ColumnValue& currentValue = setOperation->value(i);
        
        res = ndbSetOperation->setValue(currentValue.name().c_str(), currentValue.value().data(), static_cast<unsigned int>(currentValue.value().size()));
        
        if(res == -1)
            return nullptr;
    }
    
    return ndbSetOperation;
}

#pragma mark - Delete Operation
inline NdbOperation*  NDBMessageExecutor::execute(DeleteOperation* deleteOperation , NdbTransaction*& transaction, string& transactionID, const int& threadNo)
{
    const StoreInfo& storeInfo = deleteOperation->store_info(0);
    int res;
    
    NdbOperation* ndbDeleteOperation = transaction->getNdbOperation(storeInfo.store_name().c_str());

    if(!ndbDeleteOperation)
        return nullptr;
    
    ndbDeleteOperation->deleteTuple();
    
    res = setEqualPrimaryKey(deleteOperation, ndbDeleteOperation);
    if(res == -1)
        return nullptr;
    
//    for(int i = 0; i < deleteOperation->primary_key_size(); i++)
//    {
//        const ColumnValue& currentPrimaryKeyValue = deleteOperation->primary_key(i);
//        
//        res = ndbDeleteOperation->equal(currentPrimaryKeyValue.name().c_str(), currentPrimaryKeyValue.value().data(), static_cast<unsigned int>(currentPrimaryKeyValue.value().size()));
//        
//        if(res == -1)
//            return nullptr;
//    }
    
    return ndbDeleteOperation;
}

#pragma mark - Get Operation
inline NdbOperation* NDBMessageExecutor::execute(GetOperation* getOperation, NdbTransactionStateOperationsColumnValues* columnValueRecord, NdbTransaction*& transaction, string& transactionID, const int& threadNo)
{
    const StoreInfo& storeInfo = getOperation->store_info(0);
    int res = 0;

    NdbOperation* ndbGetOperation = nullptr;
    
    const NdbDictionary::Table* tableInfo = _tableInfoCache->get(/*storeInfo.database_name(), */storeInfo.store_name(), _ndb[threadNo]);
    
    if(!tableInfo)
        tableInfo = _tableInfoCache->set(/*storeInfo.database_name(), */storeInfo.store_name(), _ndb[threadNo]);
    
    if(!tableInfo)
        return nullptr;
    
    if(getOperation->primary_key_size() == tableInfo->getNoOfPrimaryKeys())
    {
        ndbGetOperation = transaction->getNdbOperation(storeInfo.store_name().c_str());
        
        if(!ndbGetOperation)
            return nullptr;
        
        ndbGetOperation->simpleRead();

       res = setEqualPrimaryKey(getOperation, ndbGetOperation);
        if(res == -1)
            return nullptr;
    }
    else if((getOperation->primary_key_size() != 0) && (getOperation->primary_key_size() < tableInfo->getNoOfPrimaryKeys()))
    {
        NdbIndexScanOperation* pruneIndexScanOperation = transaction->getNdbIndexScanOperation("VOLODB_PRUNE_INDEX", storeInfo.store_name().c_str());
        
        if(!pruneIndexScanOperation)
            return nullptr;
        
        pruneIndexScanOperation->readTuples();
        
        res = setEqualDistributedKey(getOperation, pruneIndexScanOperation);
        if(res == -1)
            return nullptr;
        
        ndbGetOperation = pruneIndexScanOperation;
    }
    else/* if(getOperation->value_size() > 0)*/
    {
        NdbScanOperation* scanOperation = transaction->getNdbScanOperation(storeInfo.store_name().c_str());
        
        if(!scanOperation)
            return nullptr;
        
        scanOperation->readTuples();
        
        res = setEqualValue(getOperation, scanOperation, tableInfo);
        if(res == -1)
            return nullptr;
        
        ndbGetOperation = scanOperation;
    }
    /*else
    {
        return nullptr;
    }*/

    int noOfColumns = tableInfo->getNoOfColumns();
    
    for(int i = 0; i < noOfColumns; i++)
    {
        NdbRecAttr* columnAttrRec =  ndbGetOperation->getValue(i);
        
        if(!columnAttrRec)
            return nullptr;

        columnValueRecord->push_back(columnAttrRec);
    }
 
    return ndbGetOperation;
}

#pragma mark - Helping functions

template <class T>
inline int NDBMessageExecutor::setEqualPrimaryKey(T* operation, NdbOperation* ndbOperation)
{
    int res = 0;
    for(int i = 0; i < operation->primary_key_size(); i++)
    {
        const ColumnValue& currentPrimaryKeyValue = operation->primary_key(i);
        
        res = ndbOperation->equal(currentPrimaryKeyValue.name().c_str(), currentPrimaryKeyValue.value().data(), static_cast<unsigned int>(currentPrimaryKeyValue.value().size()));
        
        if(res == -1)
            break;
    }
    
    return res;
}

template <class T>
inline int NDBMessageExecutor::setEqualDistributedKey(T* operation, NdbIndexScanOperation* ndbOperation)
{
    int res = 0;
    for(int i = 0; i < operation->primary_key_size(); i++)
    {
        const ColumnValue& currentDistributedKey = operation->primary_key(i);
        
        res = ndbOperation->equal(currentDistributedKey.name().c_str(), currentDistributedKey.value().data(), static_cast<unsigned int>(currentDistributedKey.value().size()));
        
//        res =  ndbOperation->setBound(currentDistributedKey.name().c_str(), NdbIndexScanOperation::BoundEQ, currentDistributedKey.value().data(), static_cast<int>(currentDistributedKey.value().size()));
        
        if(res == -1)
            break;
    }

//    if(res == 0)
//        res = ndbOperation->end_of_bound(0);
    
    return res;
}

template <class T>
inline int NDBMessageExecutor::setEqualValue(T* operation, NdbScanOperation* ndbOperation, const NdbDictionary::Table* tableInfo)
{
    int res = 0;
    NdbScanFilter filter(ndbOperation);
    
    res = filter.begin(NdbScanFilter::AND);
    
    if(res < 0)
        return res;
    
    for(int i = 0; i < operation->value_size(); i++)
    {
        const ColumnValue& currentColumn = operation->value(i);
        
        res = filter.cmp(NdbScanFilter::COND_EQ, tableInfo->getColumn(currentColumn.name().c_str())->getColumnNo(), currentColumn.value().data(),
                         static_cast<unsigned int>(currentColumn.value().size()));
        
        if(res == -1)
            break;
    }
    
    if(res == 0)
        res = filter.end();
    
    return res;
}


inline ExecutionResult NDBMessageExecutor::runQuery(string& queryString, string& transactionID, const int& threadNo)
{
    if(mysql_query(&_mysql[threadNo], queryString.c_str()))
        return makeResultForError(transactionID, mysql_error(&_mysql[threadNo]));
    
    return makeResult(transactionID);
}

inline string NDBMessageExecutor::getTypeName(Column_Type type)
{
    if(type == Column_Type_BOOL)
        return "Tinyint";
    else if(type == Column_Type_INT32)
        return "Int";
    else if(type == Column_Type_UINT32)
        return "Int Unsigned";
    else if(type == Column_Type_INT64)
        return "Bigint";
    else if(type == Column_Type_UINT64)
        return "Bigint Unsigned";
    else if(type == Column_Type_FLOAT)
        return "Float";
    else if(type == Column_Type_DOUBLE)
        return "Double";
    else if(type == Column_Type_CHAR)
        return "Char";
    else if(type == Column_Type_VARCHAR)
        return "Varchar";
    else if(type == Column_Type_VARBINARY)
        return "Varbinary";
    
    return "Int";
}

inline ExecutionResult NDBMessageExecutor::makeResultForError(const string& transactionId, const string& description, const int& errorCode)
{
    ExecutionResult errorResult(new Result);
    errorResult->set_error_description(description);
    errorResult->set_error_code(errorCode);
    errorResult->set_transaction_identifier(transactionId);
    
    return errorResult;
}

inline ExecutionResult NDBMessageExecutor::makeResult(const string& transactionId)
{
    ExecutionResult result(new Result);
    result->set_transaction_identifier(transactionId);
    
    return result;
}

inline bool NDBMessageExecutor::replyWithResult(ExecutionResult& result, zmq::message_t& identifer, zmq::socket_t& taskSenderSocket)
{
    string* bytes = new string();
    
    if(!FrameBuilder::getBytesOfFrameWithMessage(*result, *bytes, nullptr))
    {
        FILE_LOG(logWARNING)<<"Failed to serialize protocol buffer message of server response. Client will not receive any response";
        return false;
    }
    
    zmq::message_t serverReply((void*)bytes->data(), bytes->size(), _free_string_handler, bytes);

    taskSenderSocket.send(identifer, ZMQ_SNDMORE);
    taskSenderSocket.send(serverReply);
    
    return true;
}


void NDBMessageExecutor::_free_string_handler(void* data, void* hint)
{
        delete (string*)hint;
}


template<class T>
inline NdbTransaction* NDBMessageExecutor::_startTransaction(const ::google::protobuf::RepeatedPtrField<T>& columns, /*const string& databaseName, */const string& storeName, const int& threadNo)
{
    const NdbDictionary::Table* tableInfo = _tableInfoCache->get(/*databaseName.c_str(), */storeName.c_str(), _ndb[threadNo]);
    
    if(!tableInfo)
        tableInfo = _tableInfoCache->set(/*databaseName, */storeName, _ndb[threadNo]);
    
    if(tableInfo)
    {
        NdbTransaction* trans;
        int noDistributionKeys = NDBMessageProcessor::getNoDistributionKeyIndexes(columns);
        
//        const NdbDictionary::Dictionary* dictionary = _ndb[threadNo]->getDictionary();
//        
//        if(dictionary)
//        {
//            string fullTableName = storeName;
//            tableInfo =  dictionary->getTable(fullTableName.c_str());
//        }

        if(noDistributionKeys > 0)
        {
            Ndb::Key_part_ptr hint[noDistributionKeys + 1];

            NDBMessageProcessor::getHint(columns, hint);
            trans = _ndb[threadNo]->startTransaction(tableInfo, hint);
        }
        else
            trans = _ndb[threadNo]->startTransaction(tableInfo);
        
        if(!trans)
        {
//            FILE_LOG(logWARNING)<<_ndb[threadNo]->getNdbError().classification;
            FILE_LOG(logWARNING)<<"Transaction failed with code: "<<_ndb[threadNo]->getNdbError().classification;
        }
        
        return trans;
    }
    else
    {
        FILE_LOG(logWARNING)<<"Unable to fetch tableInfo from in executor";
    }
    
    return nullptr;
    
}

