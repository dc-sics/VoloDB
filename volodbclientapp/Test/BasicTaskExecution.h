//
//  BasicTaskExecution.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/13/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef DistributedKeyValueStore_BasicTaskExecution_h
#define DistributedKeyValueStore_BasicTaskExecution_h


#include <google/protobuf/message.h>
#include "Logging/log1.h"
#include "Test/TestBase.h"
#include <string>
#include <unistd.h>
#include <atomic>
#include "Executor/Executor.h"
#include "Executor/ExecutorDelegate.h"
#include "ProtoBuf/Result.pb.h"
#include <vector>
template<class T>
class BasicTaskExecution: public TaskBase<T>, public ExecutorDelegate
{
private:
    
    Executor _executor;
public:
    
    BasicTaskExecution(zmq::context_t& context, int instanceNo, int serverNo, int clientNo, int noWorkerThreads, int noRequests, int requestSize, string serverAddress, int bulkSize, TaskOperations<T>* delegate):
    TaskBase<T>(context, instanceNo, serverNo, clientNo, noWorkerThreads, noRequests, requestSize, serverAddress, bulkSize, delegate), _executor(this, serverAddress, 5570, noWorkerThreads)
    {}
    
    virtual void didReceiveResponse(Executor* executor, vector<Result*>& result)
    {
        long long totalBytesOfMessage = 0;
        for(Result* res: result)
            totalBytesOfMessage+=res->ByteSize();
        
        this->updateStats(result.size(),totalBytesOfMessage);
    }
    
    virtual void sendData(T data, size_t size) override
    {
       /*std::vector<reference_wrapper<::google::protobuf::Message>> v;
    
        v.push_back(ref(*data));
        v.push_back(ref(*data));
        _executor.execute(v, true);*/
                                             
        _executor.execute(*data);
    }
    
    virtual void sendData(const vector<T>& data)
    {
        _executor.execute(data, {"id"}, true);
    }
    
};


#endif
