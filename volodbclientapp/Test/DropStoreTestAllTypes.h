//
//  DropTableTest.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/18/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef DistributedKeyValueStore_DropTableTest_h
#define DistributedKeyValueStore_DropTableTest_h

#include "BasicTest.h"
#include "ProtoBuf/DefinitionOperation.pb.h"
#include "Executor/Executor.h"
#include "BasicTaskExecution.h"

using namespace std;

template<class T1>
class DropStoreTestAllTypes : public BasicTest<T1, shared_ptr< ::google::protobuf::Message>>, public TaskOperations<shared_ptr< ::google::protobuf::Message>>
{
    
public:
    DropStoreTestAllTypes(const string& name): BasicTest<T1, shared_ptr< ::google::protobuf::Message>>(name, this){}
    
//    virtual void start(map<string, string>& settings) override
//    {
//        BasicTest<T1, shared_ptr< ::google::protobuf::Message>>::start(settings);
//    }

    shared_ptr< ::google::protobuf::Message> getData(int instanceNo, int serverNo, int requestNo, int clientNo, int requestSize)
    {
        shared_ptr<DropStoreOperation> drop_store(new DropStoreOperation);
        
        DropStoreOperation& drop_store_operation = *drop_store;
        
        drop_store_operation.mutable_store_info()->set_store_name("Test_" + to_string(clientNo) + "_" + to_string(requestNo));
//        drop_store_operation.mutable_store_info()->set_database_name("vdb");
        
        return drop_store;
        
    }
};

#endif
