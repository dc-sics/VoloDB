//
//  NDBMessageProcessor.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/9/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "NDBMessageProcessor.h"
#include "Frame/FrameUtils.h"
#include "NdbApi.hpp"
#include "../Cache/TableInfoCache.h"
#include "Logging/log1.h"
#include <tuple>

using namespace std;

Ndb** NDBMessageProcessor::_ndb;
TableInfoCache* NDBMessageProcessor::_tableInfoCache;

atomic<int> NDBMessageProcessor::_noRecentFailedTransactions;
atomic<int> NDBMessageProcessor::_noRecentTransactions;
atomic<time_t> NDBMessageProcessor::_lastFailedTransactionsStartTime;

void NDBMessageProcessor::initialize(Ndb** ndb, TableInfoCache* tableInfoCache)
{
    _ndb = ndb;
    _tableInfoCache = tableInfoCache;
}

//template<class T>
//int NDBMessageProcessor::getNoDistributionKeyIndexes(const ::google::protobuf::RepeatedPtrField<T>& columns)
//{
//    int distributionKeyColumnIndexes = 0;
//
//    for(auto iter = columns.begin(); iter != columns.end();iter++)
//        if(iter->has_distribution_key() && iter->distribution_key())
//            distributionKeyColumnIndexes++;
//    
//    return distributionKeyColumnIndexes;
//}

//template<class T>
//void NDBMessageProcessor::getHint(const ::google::protobuf::RepeatedPtrField<T>& columns, Ndb::Key_part_ptr* hint)
//{
//    int i = 0;
//    for(auto iter = columns.begin(); iter != columns.end();iter++)
//    {
//        if(iter->has_distribution_key() && iter->distribution_key())
//        {
//            //unsigned int v = *(unsigned int *)iter->value().data();
//            hint[i].ptr = iter->value().data(); //TODO: might cause bus error on older architecture because of memory alignment
//            hint[i].len = static_cast<unsigned>(iter->value().size());
//            
//            i++;
//        }
//    }
//    
//    hint[i].ptr = 0;
//    hint[i].len = 0;
//}

template<class T>
int NDBMessageProcessor::_getResonsibleDataNodeInternal(/*const string& databaseName, */const string& storeName,
                                                           const ::google::protobuf::RepeatedPtrField<T>& columns,
                                                           const int& noDistributionKeyIndexes, const int& threadNo)
{

    Ndb::Key_part_ptr hint[noDistributionKeyIndexes + 1];
    
    getHint(columns, hint);
    
//    int i = 0;
//    for(auto iter = columns.begin(); iter != columns.end();iter++)
//    {
//        if(iter->has_distribution_key() && iter->distribution_key())
//        {
//            hint[i].ptr = iter->value().data(); //TODO: might cause bus error on older architecture because of memory alignment
//            hint[i].len = static_cast<unsigned>(iter->value().size());
//        
//            i++;
//        }
//    }
//    
//    hint[i].ptr = 0;
//    hint[i].len = 0;
//
//    union {
//        Uint64 _align;
//        int32 Tkey32[1];
//    };
//    (void)_align;
//    
//    Tkey32[0] = 1;
//    
//    hint[1].ptr = Tkey32 + 0;
    
//    NdbDictionary::Dictionary* dictionary = _ndb[threadNo]->getDictionary();
    const NdbDictionary::Table* tableInfo = _tableInfoCache->get(/*databaseName, */storeName, _ndb[threadNo]);//nullptr;
//    FILE_LOG(logINFO)<<"fEtching table info";

//    if(dictionary)
//    {
//        tableInfo =  dictionary->getTable(storeName.c_str());
//    }
//    else
//    {
//        FILE_LOG(logWARNING)<<"Unable to fetch dictionary from NDB to  determine destination node by definer #"<<threadNo;
//        return -1;
//    }
    
    if(!tableInfo)
        tableInfo = _tableInfoCache->set(/*databaseName, */storeName, _ndb[threadNo]);

    if(tableInfo)
    {
        _noRecentTransactions++;
//        FILE_LOG(logINFO)<<"fEtching table info found";
        NdbTransaction* trans = _ndb[threadNo]->startTransaction(tableInfo, hint);
        
        if(trans)
        {
            int connectedNode = static_cast<int>(trans->getConnectedNodeId());
            trans->close();

            return connectedNode;
        }
        else
        {
            _noRecentFailedTransactions++;
            FILE_LOG(logWARNING)<<"Transaction could not started, unable to determine the destination data node by definer #"<<threadNo;
        }
    }
    else
        FILE_LOG(logWARNING)<<"Unable find table info to determine destination node by definer #"<<threadNo;

    return -1;
}

template<class T>
void NDBMessageProcessor::extractInfoForResponsibleDataNode(FrameMessage& message, const StoreInfo*& storeInfo, const google::protobuf::RepeatedPtrField< ::ColumnValue >*& primaryColumns)
{
    T* operation = dynamic_cast<T*>(&message);
    storeInfo = &operation->store_info(0);
    primaryColumns = &operation->primary_key();
}

int NDBMessageProcessor::_getResonsibleDataNode(FrameMessage& message, const int& threadNo)
{
    const StoreInfo* storeInfo = nullptr;
    const google::protobuf::RepeatedPtrField< ::ColumnValue >* primaryColumns = nullptr;
    
    if(FrameUtils::getFrameType(message) == FrameItem_Type_SET)
        extractInfoForResponsibleDataNode<SetOperation>(message, storeInfo, primaryColumns);
    else if (FrameUtils::getFrameType(message) == FrameItem_Type_DELETE)
        extractInfoForResponsibleDataNode<DeleteOperation>(message, storeInfo, primaryColumns);
    else if (FrameUtils::getFrameType(message) == FrameItem_Type_GET)
        extractInfoForResponsibleDataNode<GetOperation>(message, storeInfo, primaryColumns);
    
    if(primaryColumns)
    {
        int noDistributionKeyIndexes = getNoDistributionKeyIndexes(*primaryColumns);
        
        if(noDistributionKeyIndexes > 0)
            return _getResonsibleDataNodeInternal(/*storeInfo->database_name(), */storeInfo->store_name(), *primaryColumns, noDistributionKeyIndexes, threadNo);
        else //TODO:Disallow it, the query will still run as -1 is handled.
            FILE_LOG(logWARNING)<<"Operation received with no distribution key. Can not process it. Handled by definer thread #" << threadNo;
    }
    
    return -1;
}

int NDBMessageProcessor::getResponsibleExecutor(FrameMessage& message, const int& noOfExecutors, const int& threadNo)
{
    if(FrameUtils::isDefinitionFrame(message))
    {
        return rand()%noOfExecutors;
    }
    else
    {
        int responsibleDataNode = _getResonsibleDataNode(message, threadNo);
        if(responsibleDataNode == -1)
        {
            return -1;

//            FILE_LOG(logWARNING)<<"Failed to find responsible data node, assigning random executor thread. Handled by definer thread #" << threadNo;
//            return rand()%noOfExecutors;
        }
        else
        {
            return responsibleDataNode%noOfExecutors;
        }
    }
}

void NDBMessageProcessor::addToExecutorList(ExecutorTaskMap& executorMap, const int& executorNo, FrameMessagesTuplePointer& messagesTuplePointer)
{
    FrameMessagesTupleListPointer& bucket = executorMap[executorNo];
    
    if(bucket.get() == nullptr)
        bucket.reset(new FrameMessagesTupleList());

    bucket->push_back(messagesTuplePointer);
}

ExecutorTaskMapPointer NDBMessageProcessor::processForExecutor(FrameMessagesTupleListPointer& messagesList, const int& noOfExecutors, const int& threadNo)
{
    ExecutorTaskMapPointer executorMapPointer(new ExecutorTaskMap());
    
    for(FrameMessagesTuplePointer& messagesTuplePointer : *messagesList)
    {
//        FrameMessagesTuple& messagesTuple = *messagesTuplePointer;
//        FrameMessages& messages = *(get<2>(messagesTuple));
//        
//        int responsibleExecutorThread = getResponsibleExecutor(*messages[0], noOfExecutors, threadNo);
//        if(responsibleExecutorThread != -1)
            addToExecutorList(*executorMapPointer, rand()%noOfExecutors, messagesTuplePointer);
    }
    
    return executorMapPointer;
}