//
//  Executor.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/9/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__Executor__
#define __DistributedKeyValueStore__Executor__

#include <google/protobuf/message.h>
#include "../Transport/ClientTransport.h"
#include "../Transport/ClientTransportDelegate.h"


class Transaction;
class ExecutorDelegate;
class FrameBuilder;
class ExecutorDelegate;

using namespace std;

class Executor: public ClientTransportDelegate
{
private:
    ClientTransport _clientTransport;
    zmq::context_t _context;
    ExecutorDelegate* _delegate;
    
    void _sendFrame(FrameBuilder& frame);
    static void free_handler(void* data, void* hint);
    virtual void didReceiveMessage(ClientTransport* clientTransport, vector<shared_ptr<zmq::message_t>>& messages) override;
    
    static void printResult(::google::protobuf::Message& response);
    
public:
    //wait for result
    ~Executor();
    Executor(ExecutorDelegate* delegate, string serverAddress, unsigned int port=5570, unsigned short workerThreads=5);

    void execute(::google::protobuf::Message& operation, const string& uuid="");
    void execute(const vector<shared_ptr< ::google::protobuf::Message>>& operations, const vector<string>& uuids=vector<string>(), bool runAtomic=false);
};

#endif /* defined(__DistributedKeyValueStore__Executor__) */
