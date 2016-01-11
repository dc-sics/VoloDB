# Introduction
VoloDB is a NoSQL key value store that provides high throughput in terms of both reads and writes but without compromising on ACID properties. VoloDB is built over MySQL Cluster and instead of using high-level abstractions, it communicates with the cluster using the highly efficient native low level C++ asynchronous NDB API. VoloDB talks directly to the data nodes without the need to go through MySQL Server that further enhances the performance. It exploits many of MySQL Cluster’s features such as primary and partition key lookups and prune index scans to hit only one of the data nodes to achieve maximum performance. VoloDB offers a high level abstraction that hides the com- plexity of the underlying system without requiring the user to think about internal details. Our key value store also offers various additional features such as multi-query transactions and bulk operation support. C++ client libraries are also provided to allow developers to interface easily with our server.

VoloDB was implemented as part of Masters's thesis at KTH Royal Institute of Technology under supervision of Dr Jim Dowling.

# Project Structure

VoloDB directory strucutre is described below.

* *volodb-common*, it is a shared static library containing functionality used by both VoloDB server and its client library.
* *volodbserver*, it is a project directory of VoloDB server.
* *volodb*, is a static library of VoloDB server to allow other projects to use
VoloDB server functionality.
* *volodb-client*, it is a static library used by client application to communicate with VoloDB.
* *volodbclientapp*, contains tests programs to query VoloDB.
* *volodb-install.sh*, is an install script that will install all required binaries, libraries, header and configuration files.
* *binaries*, contains precompiled executables and libraries for VoloDB server and client.
* *protocol buffers definitions*, contains definitions of protocol buffer classes that are used by client and server.

# Prerequisites
Many packages need to be installed before VoloDB can be compiled. Please note that the version numbers mentioned for packages are those with which VoloDB has been tested. It should work with older versions if they contain the functionality used by VoloDB.


1. [MySQL Cluster 7.4.5](https://dev.mysql.com/downloads/cluster/)
2. [ZeroMQ 4.0.5](http://zeromq.org/intro:get-the-software)
3. [Boost 1.57](http://www.boost.org/users/download/)
4. [ProtocolBuffers2.6.0](https://developers.google.com/protocol-buffers/docs/downloads)

Refer to project’s original guide to learn how to install them.

# Setting up VoloDB
VoloDB has been tested with OS X Yosemite 10.10, Ubuntu 12.04 and CentOS 6.6, however should work with other Linux flavors and versions.

## Installation
Go to directory of *volodb-server* project and run the make command. When the command completes successfully, the VoloDB server executable will be created in project’s dist folder.

## Quick Installation
*volodb-install.sh* present in the root directory can be executed to install VoloDB server binary, libraries, header and configuration files.

## Configuraion
In order to run VoloDB, a configuration file must be provided. A sample configuration file is shown below:

```ini
[settings]
no_definer_threads = 5
no_executor_threads = 30
transport_poll_wait = 10
store_poll_wait = 10
store_port = 5570
mysql_server_ip = 192.168.0.1
mysql_port = 3306
ndb_connect_string = 192.168.0.1
ndb_port = 1186
mysql_user_name = hop
mysql_password = hop
database=vdb
log_level=INFO
```

The settings are briefly explained below:

**no\_definer\_threads**: Number of definer threads. Must be at least 1.<br>
**no\_executor\_threads**: Number of executor threads. Must be at least 1.<br>
**transport\_poll\_wait**: Time in milliseconds before definer threads check for client messages on transport sockets. All messages queued in the specified time are pro- cessed in bulk.<br>
**store\_poll\_wait**: Time in milliseconds before executor threads check for pro- cessed messages sent by definer threads. All messages queued in the specified time are executed in bulk.<br>
**mysql\_port**: Port on which VoloDB listens for client requests.<br>
**mysql\_server\_ip**: IP address of MySQL Server.<br>
**mysql\_port**: Port number of MySQL Server.<br>
**ndb\_connect\_string**: IP address of NDB.<br>
**ndb\_port**: Port number of NDB.<br>
**mysql\_user\_name**: Username to use while connecting to MySQL Server.<br>
**mysql\_password**: Password to use while connecting to MySQL Server. database: Database/Schema name where VoloDB stores are created. This database must already exist before running VoloDB.<br>
**log\_level**: Logging level of the server. The supported values are: OFF, ERROR, WARNING, INFO, DEBUG, DEBUG1, DEBUG2, DEBUG3, DEBUG4.<br>


Create a file having settings similar as above with appropriate values. If any of the settings are missing, then default values will be used.

## Execution

To run VoloDB, go the directory containing the VoloDB executable and run the following command:

*./volodbserver -c \<path\_to\_config\_file\>*

where *-c* flag is used to pass file path of the configuration file. If VoloDB is run without any configuration file information, then it will try to open */etc/volodb.conf*.
If correct information is given in the configuration file, the server will run suc- cessfully and will start listening for client requests.

# Setting up Client Library

VoloDB also provides a C++ client library project that users can link against to communicate with VoloDB.
## Compilation
Go to directory of *volodb-client* project and run *make* command. When the command completes successfully, the library file will be created in project’s dist folder. If VoloDB was installed through *volodb-install.sh* script then library will be available at */usr/local/lib/volodb* without the need to run make.

## Usage
In order to use VoloDB client library, link it with your project. Since it is a static library, it contains all code it needs, no further configuration is required. Header files of library classes will be needed by client application which are available in *volodb-client* project or in */usr/local/include/volodb*, if VoloDB was installed using *volodb-install.sh* script file.

# Sample Application
We will step by step create a very simple application that will track user and their orders using VoloDB as a backend.

## Setup
Before writing an application, two steps have to be performed.

1. Link against *libvolodb-client* static library.
2. Include *volodb-client* project’s header files.

Let us suppose that we are creating an e-commerce website and need to store and manage relevant data. We want to save two types of information, one is to store user data and other is to save their order information. For the sake of simplicity, both the stores will have very simple and basic information. They will have the following structure:

**User Store**: User ID(Primary Key, Distribution Key), Name.<br>
**Order Store**: Order ID(Primary Key), User ID(Primary Key, Distribution Key), Price

## Important Classes
Before looking into specific examples, we will first go through few of the important classes that will be used throughout the application.

### Definition Operations
Definition operations are declared in *volodb-common/ProtoBuf/DefinitionOperations.pb.h* header and consists of classes to create and delete a store in VoloDB. The two relevant classes are:

* CreateStoreOperation to create a store.
* DeleteStoreOperation to delete a store.


### Manipulation Operations
These operations are declared in *volodb-common/ProtoBuf/ManipulationOperations.pb.h* header and consists of classes to manipulate data in VoloDB stores.

* *SetOperation* is used to insert or update a key value pair. 
* *GetOperation* is used for fetching key value pair[s].
* *DeleteOperation* is used for deleting a key value pair.

### Column Classes
These classes are declared in *volodb-common/ProtoBuf/Column.pb.h* header. There are two important classes related to store columns that are described below:


* *ColumnInfo* contains information of a column such as type, name, length and flags to indicate if it is a primary or a distribution key.
* *ColumnValue* contains value for a particular *ColumnInfo*.

### Builders
These classes are declared in *volodb-common/AttributeUtils/ColumnsValueBuilder.h* and *volodb-common/AttributeUtils/ColumnsValueDecoder.h* header files. These are helper classes to allow developers to encode or decode values in *ColumnValue*.

* *ColumnsValueBuilder* encodes native values into a form expected by VoloDB.
* *ColumnsValueDecoder* decodes values back into native form.


### Executor
It is declared in *volodb-client/Executor/Executor.h* and contains a class responsible for executing the user operations. The execution is completely asynchronous and user must register a callback to get notified of a response by VoloDB.

### Result:
Declared in *volodb-common/ProtoBuf/Result.pb.h* is a class that contains a response from VoloDB. It contains information such as returned rows, error information(if any) and transactionID.

Refer to header files for complete information of classes and functions provided.


## Store Creation
We need to create two stores for our sample e-commerce application. After creating an instance of *CreateStoreOperation*, we add ColumnInfo according to the requirement and set its properties.

```cpp
CreateStoreOperation user_info_operation;
user_info_operation.mutable_store_info()->set_store_name("UserInfo");
        
ColumnInfo* user_info_column1 = user_info_operation.add_column();
user_info_column1->set_name("user_id");
user_info_column1->set_primary_key(true);
user_info_column1->set_distribution_key(true);
user_info_column1->set_type(Column_Type_UINT32);

ColumnInfo* user_info_column2 = user_info_operation.add_column();
user_info_column2->set_name("name");
user_info_column2->set_type(Column_Type_VARCHAR);
user_info_column2->set_length(300);

CreateStoreOperation create_orders_operation;
create_orders_operation.mutable_store_info()->set_store_name("Orders");
        
ColumnInfo* orders_column1 = create_orders_operation.add_column();
orders_column1->set_name("order_id");
orders_column1->set_primary_key(true);
orders_column1->set_type(Column_Type_UINT32);

ColumnInfo* orders_column2 = create_orders_operation.add_column();
orders_column2->set_name("user_id");
orders_column2->set_primary_key(true);
orders_column2->set_distribution_key(true);
orders_column2->set_type(Column_Type_UINT32);

ColumnInfo* orders_column3 = create_orders_operation.add_column();
orders_column3->set_name("price");
orders_column3->set_type(Column_Type_FLOAT);
```


At some point of the application, instance of *Executor* must be initialized with callback information and VoloDB server details.

```cpp
Executor executor(this, "192.168.0.1", 5570);
//this refers to class conforming to result callback protocol
```

The operations are then executed using *execute* function either individually or in bulk. ID’s of operations are also passed which are received back in executor’s callback.

```cpp
//execute individually 
executor.execute(user_info_operation, "user_table_oper_id");
executor.execute(create_orders_operation, "orders_table_oper_id");

//execute in bulk
executor.execute({user_info_operation,create_orders_operation}, 
                "user_table_oper_id", "orders_table_oper_id");
```

The application should also conform with the *ExecutorDelegate* protocol to receive callbacks from server by overriding the following function:

```cpp
virtual void didReceiveResponse(Executor* executor, vector<Result*>& result)
{
    if(result[0]->has_error_code())
        cout<<result[0]->error_description();
    else
        cout<<result[0]->transaction_identifier()<<" executed successfully";
}
```

## Key Value Pair Insertion

We will now add a new user and two of his pending orders. Key value pairs are added using *SetOperation*. Let us first prepare values for user information.

```cpp
SetOperation new_user_info;
new_user_info.add_store_info()->set_store_name("UserInfo");

ColumnsValueBuilder userInfoPrimarykeyValueBuilder;
userInfoPrimarykeyValueBuilder.addUInt32Column("user_id", 1, true, true);

ColumnsValueBuilder userInfoValueBuilder;
userInfoValueBuilder.addVarCharColumn("name", string("Ali Dar"));

ColumnsValueBuilder::SetPrimaryKeys(new_user_info, userInfoPrimarykeyValueBuilder.getColumns());
ColumnsValueBuilder::SetValues(new_user_info, userInfoValueBuilder.getColumns());
```

For every *SetOperation* the user has to provide two sets of data. Data for both keys and values have to be provided separately. User can set *ColumnValue* for each column manually but this requires internal knowledge of the wire format. But VoloDB client library provides a helper class *ColumnsValueBuilder* to populate and setup column values. *ColumnsValueBuilder* provides overloaded functions for every supported data type. After data for key value pair is populated, they are assigned to *SetOperation* using *SetPrimaryKeys()* and *SetValues()* functions.


Similarly, setup two *SetOperation* for two orders of the user.

```cpp
//Setup Order no 1
SetOperation new_order_1;
new_order_1.add_store_info()->set_store_name("Orders");
        
ColumnsValueBuilder order1PrimarykeyValueBuilder;
order1PrimarykeyValueBuilder.addUInt32Column("order_id", 1, true);
order1PrimarykeyValueBuilder.addUInt32Column("user_id", 1, true, true);
ColumnsValueBuilder order1InfoValueBuilder;
order1InfoValueBuilder.addFloatColumn("price", 101.45);

ColumnsValueBuilder::SetPrimaryKeys(new_order_1, order1PrimarykeyValueBuilder.getColumns());
ColumnsValueBuilder::SetValues(new_order_1, order1InfoValueBuilder.getColumns());
                                
//Setup Order no 2
SetOperation new_order_2;
new_order_2.add_store_info()->set_store_name("Orders");
        
ColumnsValueBuilder order2PrimarykeyValueBuilder;
order2PrimarykeyValueBuilder.addUInt32Column("order_id", 2, true);
order2PrimarykeyValueBuilder.addUInt32Column("user_id", 1,
                                            true, true);
ColumnsValueBuilder order2InfoValueBuilder;
order2InfoValueBuilder.addFloatColumn("price", 200.0);
        
ColumnsValueBuilder::SetPrimaryKeys(new_order_2, order2PrimarykeyValueBuilder.getColumns());
ColumnsValueBuilder::SetValues(new_order_2, order2InfoValueBuilder.getColumns());
```

Now we can insert all the values together in an atomic way or individually. But we would like to execute them in a single transaction so either all are added or none.

```cpp
//true is passed to tell executor to run all operations 
//in a single transaction
executor.execute({new_user_info, new_order_1, new_order_2}, {"my_transaction_id"}, true);
```

Callback will be called when the transaction either completes successfully or fails. Refer to "*Store Creation*" example on how to receive a result from VoloDB.


## Fetching a Key Value Pair

Suppose we would like to fetch the name of a user with an id no 1. The value for a key can be fetched using *GetOperation* by providing full primary key values.

```cpp
GetOperation getOperation;

StoreInfo* storeInfo = getOperation.add_store_info();
storeInfo->set_store_name("UserInfo");

ColumnsValueBuilder primarykeyValueBuilder;
primarykeyValueBuilder.addUInt32Column("user_id", (uint32_t)1, true);
        
ColumnsValueBuilder::SetPrimaryKeys(getOperation, primarykeyValueBuilder.getColumns());                        
```

Values returned from VoloDB can be fetched in the callback.

```cpp
virtual void didReceiveResponse(Executor* executor, vector<Result*>& result)
{
    if(result[0]->has_error_code())
        cout<<result[0]->error_description();
    else
    {
        //fetching first result only in this case, system aggregates the
        //received responses from server and delivers them in one go
        Result* r = result[0];

        //fetching only one result because transaction contains only 
        //one operation
        const OperationResult& operationResult = r->result(0);

        //fetching only one row because it was a primary key lookup
        const Row& row = operationResult.row(0);
        
        for(int k = 0; k < row.column_size(); k++)
        {
            const ColumnValue& columnValue = row.column(k);
            
            if(columnValue.name().compare("name") == 0)
                cout<<"User name"<<": "<<
                ColumnsValueDecoder::getVarChar(columnValue)<<endl;
            else if(columnValue.name().compare("user_id") == 0)
                cout<<"User ID"<<": "<<
                ColumnsValueDecoder::getUInt32(columnValue)<<endl;                
            
        }  
    }
}
```
## Fetching Key Value Pairs using Partition Key (Prune Index Scan)
Let us now fetch all the orders of user no 1. In order to fetch all the orders of the user, a table scan on *user\_id* column is required, since they can not be fetched through primary key lookup. Table scans are discouraged because it is slow as it has to hit every data node. But if we observe the structure of tables, *user\_id* is the partition key in both *UserInfo* and *Orders* table. VoloDB supports queries based on partition key because they will only hit one data node. Since the query is not based on a primary key, more than one key value pairs can be returned.

To fetch all the orders of user no 1, we will query the *Orders* table on *user\_id* column to fetch all the orders of that user.

```cpp
GetOperation pruneIndexOperation;

StoreInfo* storeInfo = pruneIndexOperation.add_store_info();
storeInfo->set_store_name("Orders");

ColumnsValueBuilder primarykeyValueBuilder;
//passing (true, true) because user_id column is a part of
//primary and distribution key
primarykeyValueBuilder.addUInt32Column("user_id", (uint32_t)1, true, true);
        
ColumnsValueBuilder::SetPrimaryKeys(pruneIndexOperation, primarykeyValueBuilder.getColumns());
                                    
//now execute it
executor.execute(pruneIndexOperation, "prune_index_transaction_id");  
```

In Executor's callback function we can fetch all the returned rows similarly to an example of "*Fetching a Key Value Pair*". 

##Fetching Key Value Pairs using Non-Keyed Column (Full Table Scan)
Suppose we want to fetch all the orders that have the *price* of 200.0. price column is neither a primary key nor a distribution key, so we can not do a primary fetch or a prune index scan. We will have to initiate a table scan on the price column with 200.0 value. Instead of using *ColumnsValueBuilder::SetPrimaryKeys()* function in this case, we will use *ColumnsValueBuilder::SetValues()* because the *price* column is not a primary key. The user is also allowed to do a full table scan without qualification on any column that can end up returning all rows of the store. This operation though supported is not recommended because table scans are slow operations that can also slow down other users.

```cpp
GetOperation scanOperation;

StoreInfo* storeInfo = scanOperation.add_store_info();
storeInfo->set_store_name("Orders");

ColumnsValueBuilder valueBuilder;
//not passing true flag, since it is not a primary or a distribution key
valueBuilder.addFloatColumn("price", (float)200.0);
        
ColumnsValueBuilder::SetValues(scanOperation, valueBuilder.getColumns());                             
//now execute it
executor.execute(scanOperation, "scan_transaction_id");  
```

In Executor's callback function we can fetch all the returned rows similarly to an example of "*Fetching a Key Value Pair*". 

##Key Value Pair Deletion
Let us assume that an order with id no 2 is now fulfilled for user no 1 and we would like to delete it from the *Orders* store. Key value pair is deleted by executing a *DeleteOperation*. The user has to provide values for the primary key columns in order to delete the pair.

```cpp
DeleteOperation deleteOrderOperation;
StoreInfo* storeInfo = deleteOperation.add_store_info();
storeInfo->set_store_name("Orders");

ColumnsValueBuilder primarykeyValueBuilder;
primarykeyValueBuilder.addUInt32Column("order_id", (uint32_t)2, true);
primarykeyValueBuilder.addUInt32Column("user_id", (uint32_t)1, true, true);        
        
ColumnsValueBuilder::SetPrimaryKeys(deleteOperation, primarykeyValueBuilder.getColumns());

executor.execute(deleteOrderOperation, "my_transaction_id");
```

##Store Deletion
A store can be deleted by using *DeleteStoreOpertion*. The user only has to set the name of the store and then execute the operation using the *Executor*. Let us delete the user information and the order store that we created earlier.

```cpp
DropStoreOperation drop_user_operation;
drop_user_operation.mutable_store_info()->set_store_name("UserInfo");

DropStoreOperation drop_orders_operation;
drop_orders_operation.mutable_store_info()->set_store_name("Orders");

executor.execute({drop_user_operation,drop_orders_operation}, "user_table_oper_id", "orders_table_oper_id");
```

Build from Source on Ubuntu
==============================

1. install MySQL Cluster to /usr/local/mysql

2. Install all the libraries needed to build VoloDB

sudo apt-get install libtool pkg-config build-essential autoconf automake uuid-dev libzmq-dev libpgm-dev libzmq3-dev libsodium-dev libboost-dev libboost-program-options-dev libboost-thread-dev libboost-tools-dev libboost-system-dev libprotobuf-c-dev libprotobuf-dev protobuf-compiler 

git clone https://github.com/dc-sics/VoloDB.git
cd VoloDB/volodbserver
git checkout linux

3. Build and install using the bash script:
sudo ./volodb-install.sh

If you're stuck, here's a list of the static libraries linked in for 64-bit Ubuntu:

LDLIBSOPTIONS=/usr/local/mysql/lib/libndbclient_static.a /usr/local/mysql/lib/libmysqlclient.a /usr/local/mysql/lib/libmysqld.a ../volodb-common/dist/Release/GNU-Linux/libvolodb-common.a /usr/lib/x86_64-linux-gnu/libzmq.a /usr/lib/x86_64-linux-gnu/libprotobuf.a /usr/lib/x86_64-linux-gnu/libboost_program_options.a /usr/lib/x86_64-linux-gnu/libboost_thread.a /usr/lib/x86_64-linux-gnu/libboost_system.a /usr/lib/x86_64-linux-gnu/libpgm.a



