//
//  NDBMessageExecutor.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/10/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__NDBMessageExecutor__
#define __DistributedKeyValueStore__NDBMessageExecutor__

#include "../Utils/TransportUtils.h"
#include "Frame/FrameDecoder.h"
#include "ProtoBuf/DefinitionOperation.pb.h"
#include "ProtoBuf/ManipulationOperations.pb.h"
#include <tuple>
#include <memory>
#include <mysql.h>
#include "../Cache/TableInfoCache.h"
#include "NdbApi.hpp"
#include "Logging/log1.h"
#include "Frame/FrameBuilder.h"
#include <tuple>

using namespace TransportUtils;

class Result;
class OperationResult;

using ExecutionResult = shared_ptr<Result>;
using NdbOperationPtr = shared_ptr<NdbOperation>;

using NdbTransactionStateOperationsColumnValues = vector<NdbRecAttr*>;
using NdbTransactionStateOperationsColumnValuesPtr = shared_ptr<NdbTransactionStateOperationsColumnValues>;

using NdbTransactionStateOperationsItem = vector<tuple<FrameItem_Type, NdbOperation*, NdbTransactionStateOperationsColumnValuesPtr, int>>;
using NdbTransactionStateOperationsItemPtr = shared_ptr<NdbTransactionStateOperationsItem>;
using NdbTransactionState = tuple<NdbConnection*, string*, zmq::message_t*, zmq::socket_t*, NdbTransactionStateOperationsItemPtr>;

class NDBMessageExecutor
{
private:
    
    inline static void addRecordToResult(OperationResult* currentOperationResult, NdbTransactionStateOperationsColumnValues& currentOperationColumnValues);
    
    inline static ExecutionResult makeResultForError(const string& transactionId, const string& description, const int& errorCode = 0);
    inline static ExecutionResult makeResult(const string& transactionId);
    inline static bool replyWithResult(ExecutionResult& result, zmq::message_t& identifer, zmq::socket_t& taskSenderSocket);
    static void ndbExecuteCallback(int result, NdbConnection* transaction, void* aObject);
    
    inline static bool execute(FrameMessagesTuple& messagesTuplePointer, zmq::socket_t& taskSenderSocket, const int& threadNo);
    
    inline static ExecutionResult execute(CreateStoreOperation* createStoreOperation, string& transactionID, const int& threadNo);
    inline static ExecutionResult execute(DropStoreOperation* dropStoreOperation, string& transactionID, const int& threadNo);
    
    inline static ExecutionResult execute(CreateDatabaseOperation* createDatabaseOperation, string& transactionID, const int& threadNo);
    inline static ExecutionResult execute(DropDatabaseOperation* dropDatabaseOperation, string& transactionID, const int& threadNo);
    
    inline static NdbOperation* execute(SetOperation* setOperation, NdbTransaction*& transaction, string& transactionID, const int& threadNo);
    inline static NdbOperation* execute(DeleteOperation* setOperation, NdbTransaction*& transaction, string& transactionID, const int& threadNo);
    inline static NdbOperation* execute(GetOperation* getOperation, NdbTransactionStateOperationsColumnValues* columnValueRecord, NdbTransaction*& transaction, string& transactionID, const int& threadNo);
    
    inline static ExecutionResult runQuery(string& queryString, string& transactionID, const int& threadNo);
    inline static ExecutionResult createDatabase(const string& name, string& transactionID, const int& threadNo);
    inline static string getTypeName(Column_Type type);
    
    inline static void waitForTransactionsToComplete(const int& totalTransactions, const int& threadNo);
    
    template <class T>
    inline static int setEqualPrimaryKey(T* operation, NdbOperation* ndbOperation);

    template <class T>
    inline int static setEqualDistributedKey(T* operation, NdbIndexScanOperation* ndbOperation);
    
    template <class T>
    inline int static setEqualValue(T* operation, NdbScanOperation* ndbOperation, const NdbDictionary::Table* tableInfo);
    
    static void _free_string_handler(void* data, void* hint);

    template<class T>
    inline static NdbTransaction* _startTransaction(const ::google::protobuf::RepeatedPtrField<T>& columns, /*const string& databaseName, */const string& storeName, const int& threadNo);
    
//    static void _addTableInfoToCache(string databaseName, string tableName, const int& threadNo);

    static string _userName;
    static string _password;
    static MYSQL* _mysql;
    static Ndb** _ndb;
    static TableInfoCache* _tableInfoCache;
    
public:
    NDBMessageExecutor() = delete;
    static void execute(StoreMsgs& messages, zmq::socket_t& taskSenderSocket, const int& threadNo);
    static void initialize(MYSQL* _mysql, Ndb** ndb, TableInfoCache* tableInfoCache);
};

#endif /* defined(__DistributedKeyValueStore__NDBMessageExecutor__) */
