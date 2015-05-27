//
//  Executor.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/9/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "Executor.h"
#include "Frame/FrameBuilder.h"
#include "ExecutorDelegate.h"
#include "Frame/FrameDecoder.h"
#include "Frame/FrameUtils.h"
#include "ProtoBuf/Result.pb.h"
#include "AttributeUtils/ColumnsValueDecoder.h"
#include <iostream>
#include <stdexcept>

Executor::Executor(ExecutorDelegate* delegate, string serverAddress, unsigned int port, unsigned short workerThreads):
_context(1), _delegate(delegate), _clientTransport(this, workerThreads)
{
    _clientTransport.connect(&_context, serverAddress.c_str(), port);
}

void Executor::execute(::google::protobuf::Message& operation, const string& uuid)
{
    FrameBuilder frameBuilder;
    frameBuilder.addFrame(operation, uuid);
    
    _sendFrame(frameBuilder);
}

void Executor::execute(const vector<shared_ptr< ::google::protobuf::Message>>& operations, const vector<string>& uuids, bool runAtomic)
{
    if(!runAtomic && (operations.size() != uuids.size()))
        throw runtime_error("UUID must be provided for every operation for non atomic execution");
    else if(runAtomic && (uuids.size() < 1))
        throw runtime_error("At least one UUID must be provided for atomic operation");
    
    FrameBuilder frameBuilder(runAtomic, uuids[0]);

    for(int i=0; i< operations.size(); i++)
        string frameIdentifier = frameBuilder.addFrame(*operations[i], runAtomic ? "" : uuids[i]);
    
    _sendFrame(frameBuilder);
}

void Executor::_sendFrame(FrameBuilder& frame)
{
    string* bytesToSend = new string;
    frame.getBytes(*bytesToSend);
    
    _clientTransport.send((void*)bytesToSend->data(), bytesToSend->size(), free_handler, bytesToSend);
    
}

void Executor::free_handler(void* data, void* hint)
{
    delete (string*)hint;
}

void Executor::printResult(::google::protobuf::Message& response)
{
    Result* r = dynamic_cast<Result*>(&response);
    
    cout<<"Transaction ID # "<<r->transaction_identifier()<<endl;
    
    for(int i = 0; i < r->result_size(); i++)
    {
        const OperationResult& operationResult = r->result(i);
        
        cout<<"Operation # "<<i<<endl;
        
        for(int j = 0; j < operationResult.row_size(); j++)
        {
            const Row& row = operationResult.row(j);
            
            cout<<"Row # "<<j<<endl;
            
            for(int k = 0; k < row.column_size(); k++)
            {
                const ColumnValue& columnValue = row.column(k);

                if(columnValue.type() == Column_Type_BOOL)
                    cout<<columnValue.name()<<": "<<ColumnsValueDecoder::getBool(columnValue)<<endl;
                else if(columnValue.type() == Column_Type_INT32)
                    cout<<columnValue.name()<<": "<<ColumnsValueDecoder::getInt32(columnValue)<<endl;
                else if(columnValue.type() == Column_Type_UINT32)
                    cout<<columnValue.name()<<": "<<ColumnsValueDecoder::getUInt32(columnValue)<<endl;
                else if(columnValue.type() == Column_Type_INT64)
                    cout<<columnValue.name()<<": "<<ColumnsValueDecoder::getInt64(columnValue)<<endl;
                else if(columnValue.type() == Column_Type_UINT64)
                    cout<<columnValue.name()<<": "<<ColumnsValueDecoder::getUInt64(columnValue)<<endl;
                else if(columnValue.type() == Column_Type_FLOAT)
                    cout<<columnValue.name()<<": "<<ColumnsValueDecoder::getFloat(columnValue)<<endl;
                else if(columnValue.type() == Column_Type_DOUBLE)
                    cout<<columnValue.name()<<": "<<ColumnsValueDecoder::getDouble(columnValue)<<endl;
                else if(columnValue.type() == Column_Type_CHAR)
                    cout<<columnValue.name()<<": "<<ColumnsValueDecoder::getChar(columnValue)<<endl;
                else if(columnValue.type() == Column_Type_VARCHAR)
                    cout<<columnValue.name()<<": "<<ColumnsValueDecoder::getVarChar(columnValue)<<endl;
                else if(columnValue.type() == Column_Type_VARBINARY)
                    cout<<columnValue.name()<<": "<<ColumnsValueDecoder::getVarBinary(columnValue)<<endl;
            }
            
            cout<<"\n\n";
        }
    }
}

void Executor::didReceiveMessage(ClientTransport* clientTransport, vector<shared_ptr<zmq::message_t>>& messages)
{
    vector<Result*> results;

    auto responses = FrameDecoder::decode(messages, false);
    
    FlattenMessages flattenResponses = FrameDecoder::flattenFrameMessagesTupleListToSingleList(responses);
    
    for(::google::protobuf::Message& response: flattenResponses)
    {
        if(FrameUtils::getFrameType(response) == FrameItem_Type_SERVER_RESPONSE)
        {
//            printResult(response);
            if(_delegate)
                results.push_back(dynamic_cast<Result*>(&response));
        }
    }

    if(results.size() > 0)
        _delegate->didReceiveResponse(this, results);
}

Executor::~Executor()
{
    _clientTransport.close();
}
