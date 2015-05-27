//
//  BasicTask.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/12/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef DistributedKeyValueStore_BasicTask_h
#define DistributedKeyValueStore_BasicTask_h

#include "Transport/ClientTransport.h"
#include "Transport/ClientTransportDelegate.h"
#include "Logging/log1.h"
#include "Test/TestBase.h"
#include <string>
#include <unistd.h>
#include <atomic>


template<class T>
class BasicTransportTask: public TaskBase<T>, public ClientTransportDelegate
{
private:
    ClientTransport _clientTransport;
    
public:
    
    BasicTransportTask(zmq::context_t& context, int instanceNo, int serverNo, int clientNo, int noWorkerThreads, int noRequests, int requestSize, string serverAddress, int bulkSize, TaskOperations<T>* delegate):
    TaskBase<T>(context, instanceNo, serverNo, clientNo, noWorkerThreads, noRequests, requestSize, serverAddress, bulkSize, delegate), _clientTransport(this, noWorkerThreads)
    {
        _clientTransport.connect(&this->_context, this->_serverAddress.c_str());
    }
    
    virtual void didReceiveMessage(ClientTransport* clientTransport, vector<shared_ptr<zmq::message_t>>& messages)
    {
        long long totalBytesOfMessage = 0;
        for(shared_ptr<zmq::message_t>& message: messages)
            totalBytesOfMessage+=message->size();
        
        this->updateStats(messages.size(),totalBytesOfMessage);
    }
    
    virtual void sendData(T data, size_t size) override
    {
        _clientTransport.send(data, size);
    }
    
};
#endif
