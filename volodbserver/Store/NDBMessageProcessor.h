//
//  NDBMessageProcessor.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/9/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__NDBMessageProcessor__
#define __DistributedKeyValueStore__NDBMessageProcessor__

#include <vector>
#include <memory>
#include <atomic>
#include <unordered_map>
#include <google/protobuf/message.h>
//#include <ndb_global.h>
#include <mysql.h>
#include "Frame/FrameDecoder.h"
#include "NdbApi.hpp"
#include "ProtoBuf/ManipulationOperations.pb.h"
#include "../Cache/TableInfoCache.h"

using namespace std;

using ExecutorTaskMap = unordered_map<int, FrameMessagesTupleListPointer>;
using ExecutorTaskMapPointer = shared_ptr<ExecutorTaskMap>;

class NDBMessageProcessor
{
private:
    static Ndb** _ndb;
    static TableInfoCache* _tableInfoCache;

    template<class T>
    inline static int _getResonsibleDataNodeInternal(/*const string& databaseName, */const string& storeName, const ::google::protobuf::RepeatedPtrField<T>& columns, const int& noDistributionKeyIndexes, const int& threadNo);
    inline static int _getResonsibleDataNode(FrameMessage& message, const int& threadNo);
public:
    NDBMessageProcessor()=delete;
    template<class T>
    inline static void getHint(const ::google::protobuf::RepeatedPtrField<T>& columns, Ndb::Key_part_ptr* hint)
    {
        int i = 0;
        for(auto iter = columns.begin(); iter != columns.end();iter++)
        {
            if(iter->has_distribution_key() && iter->distribution_key())
            {
                hint[i].ptr = iter->value().data(); //TODO: might cause bus error on older architecture because of memory alignment
                hint[i].len = static_cast<unsigned>(iter->value().size());
                
                i++;
            }
        }
        
        hint[i].ptr = 0;
        hint[i].len = 0;
    }
    template<class T>
    inline static int getNoDistributionKeyIndexes(const ::google::protobuf::RepeatedPtrField<T>& columns)
    {
        int distributionKeyColumnIndexes = 0;
        
        for(auto iter = columns.begin(); iter != columns.end();iter++)
            if(iter->has_distribution_key() && iter->distribution_key())
                distributionKeyColumnIndexes++;
        
        return distributionKeyColumnIndexes;
    }
    inline static int getResponsibleExecutor(FrameMessage& message, const int& noOfExecutors, const int& threadNo);
    inline static void addToExecutorList(ExecutorTaskMap& executorMap, const int& executorNo, FrameMessagesTuplePointer& messagesTuplePointer);
    static ExecutorTaskMapPointer processForExecutor(FrameMessagesTupleListPointer& messagesList, const int& noOfExecutors, const int& threadNo);
    
    template<class T>
    inline static void extractInfoForResponsibleDataNode(FrameMessage& message, const StoreInfo*& storeInfo, const google::protobuf::RepeatedPtrField< ::ColumnValue >*& primaryColumns);
    
    static void initialize(Ndb** _ndb, TableInfoCache* tableInfoCache);
    
    static atomic<int> _noRecentFailedTransactions;
    static atomic<int> _noRecentTransactions;
    static atomic<time_t> _lastFailedTransactionsStartTime;
};

#endif /* defined(__DistributedKeyValueStore__NDBMessageProcessor__) */
