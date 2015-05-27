//
//  ClientCreateTableTest.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/12/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef DistributedKeyValueStore_ClientCreateTableTest_h
#define DistributedKeyValueStore_ClientCreateTableTest_h

#include "BasicTest.h"
#include "ProtoBuf/DefinitionOperation.pb.h"
#include "ProtoBuf/ManipulationOperations.pb.h"
#include "Executor/Executor.h"
#include "BasicTaskExecution.h"

using namespace std;

template<class T1>
class CreateStoreTestAllTypes : public BasicTest<T1, shared_ptr< ::google::protobuf::Message>>, public TaskOperations<shared_ptr< ::google::protobuf::Message>>
{
    
public:
    CreateStoreTestAllTypes(const string& name): BasicTest<T1, shared_ptr< ::google::protobuf::Message>>(name, this){}
    
//    virtual void start(map<string, string>& settings) override
//    {
//        BasicTest<T1, shared_ptr< ::google::protobuf::Message>>::start(settings);
//    }

    shared_ptr< ::google::protobuf::Message> getData(int instanceNo, int serverNo, int requestNo, int clientNo, int requestSize)
    {
        shared_ptr<CreateStoreOperation> test_store(new CreateStoreOperation);

        CreateStoreOperation& create_store_operation = *test_store;

        create_store_operation.mutable_store_info()->set_store_name("Test_" + to_string(clientNo) + "_" + to_string(requestNo));        
//        create_store_operation.mutable_store_info()->set_database_name("vdb");
//        create_store_operation.set_create_database_if_required(true);
        
        ColumnInfo* column1 = create_store_operation.add_column();
        column1->set_name("col1");
        column1->set_primary_key(true);
        column1->set_type(Column_Type_UINT32);

        ColumnInfo* column2 = create_store_operation.add_column();
        column2->set_primary_key(true);
        column2->set_name("col2");
        column2->set_type(Column_Type_INT32);
        column2->set_distribution_key(true);
        
        ColumnInfo* column3 = test_store->add_column();
        column3->set_primary_key(true);
        column3->set_name("col3");
        column3->set_type(Column_Type_UINT64);

        ColumnInfo* column4 = create_store_operation.add_column();
        column4->set_primary_key(true);
        column4->set_name("col4");
        column4->set_type(Column_Type_INT64);

        ColumnInfo* column5 = create_store_operation.add_column();
        column5->set_name("col5");
        column5->set_type(Column_Type_FLOAT);
/*
        ColumnInfo* column6 = create_store_operation.add_column();
        column6->set_name("col6");
        column6->set_type(Column_Type_DOUBLE);
        
        ColumnInfo* column7 = create_store_operation.add_column();
        column7->set_name("col7");
        column7->set_type(Column_Type_BOOL);

        ColumnInfo* column8 = create_store_operation.add_column();
        column8->set_name("col8");
        column8->set_type(Column_Type_CHAR);
        column8->set_length(50);

        ColumnInfo* column9 = create_store_operation.add_column();
        column9->set_name("col9");
        column9->set_type(Column_Type_VARCHAR);
        column9->set_length(300);
        column9->set_primary_key(true);

        ColumnInfo* column10 = create_store_operation.add_column();
        column10->set_name("col10");
        column10->set_type(Column_Type_VARBINARY);
        column10->set_length(299);
*/
        return test_store;
    }
};

#endif
