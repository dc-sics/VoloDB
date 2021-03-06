//
//  SetSimpleTest.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/20/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__DeleteSimpleTest__
#define __DistributedKeyValueStore__DeleteSimpleTest__

#include "BasicTest.h"
#include "ProtoBuf/ManipulationOperations.pb.h"
#include "Executor/Executor.h"
#include "BasicTaskExecution.h"
#include "AttributeUtils/ColumnsValueBuilder.h"

using namespace std;

template<class T1>
class DeleteTestSimple : public BasicTest<T1, shared_ptr< ::google::protobuf::Message>>, public TaskOperations<shared_ptr< ::google::protobuf::Message>>
{
    
public:
    DeleteTestSimple(const string& name): BasicTest<T1, shared_ptr< ::google::protobuf::Message>>(name, this){}
    /*
    virtual void start(map<string, string>& settings) override
    {
        BasicTest<T1, shared_ptr< ::google::protobuf::Message>>::start(settings);
    }*/

    shared_ptr< ::google::protobuf::Message> getData(int instanceNo, int serverNo, int requestNo, int clientNo, int requestSize)
    {
        shared_ptr<DeleteOperation> deleteOperation(new DeleteOperation);

        StoreInfo* storeInfo = deleteOperation->add_store_info();
//        storeInfo->set_database_name("vdb");
        storeInfo->set_store_name("Test_0_0");

        ColumnsValueBuilder primarykeyValueBuilder;
        
        primarykeyValueBuilder.addUInt32Column("col1", (uint32_t)serverNo, true);
        primarykeyValueBuilder.addInt32Column("col2", (int32_t)requestNo, true, true);
        primarykeyValueBuilder.addUInt64Column("col3", (uint64_t)clientNo, true);
        primarykeyValueBuilder.addInt64Column("col4", (int64_t)instanceNo, true);
        
        ColumnsValueBuilder::SetPrimaryKeys(*deleteOperation, primarykeyValueBuilder.getColumns());

        return deleteOperation;
    }
};

#endif /* defined(__DistributedKeyValueStore__DeleteSimpleTest__) */
