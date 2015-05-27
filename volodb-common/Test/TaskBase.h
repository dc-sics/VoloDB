//
//  TaskBase.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/27/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__TaskBase__
#define __DistributedKeyValueStore__TaskBase__

#include <stdio.h>


#include <mutex>
#include <map>
#include <vector>
#include <string>
#include <atomic>
#include <condition_variable>
#include "../Transport/zmq.hpp"
#include <type_traits>
#include <thread>
#include "../Logging/log1.h"
#include <iostream>
#include "../ProtoBuf/ManipulationOperations.pb.h"

using namespace std;

template<class T>
class TaskOperations
{
public:
    virtual T getData(int instanceNo, int serverNo, int requestNo, int clientNo, int requestSize=0) {T obj; return obj;}
//    virtual void didReceiveMessage(vector<shared_ptr<zmq::message_t>>& messages){}
};


template<class T>
class TaskBase: public TaskOperations<T>
{
public:
    void waitTillEnd()
    {
        std::unique_lock<std::mutex> lck(mtx);
        while(!_endFlag)
            cv.wait(lck);
    }
    
    virtual void sendData(T data, size_t size){}
    virtual void sendData(const vector<T>& data){}

    void notifyToEnd()
    {
        _endFlag = true;
        cv.notify_one();
    }

    
    TaskBase(zmq::context_t& context, int instanceNo, int serverNo, int clientNo, int noWorkerThreads, int noRequests, int requestSize, string serverAddress, int bulkSize, TaskOperations<T>* delegate):
    _context(context), _instanceNo(instanceNo), _serverNo(serverNo), _clientNo(clientNo), _noWorkerThreads(noWorkerThreads), _noRequests(noRequests), _requestSize(requestSize), _serverAddress(serverAddress), _bulkSize(bulkSize),
    _delegate(delegate)
    {}
    
    long long getTotalBytesReceived()
    {
        return this->_totalBytesReceived;
    }
    
    virtual void updateStats(size_t totalMessages, long long totalBytesOfMessages)
    {
        this->_responsesReceived.fetch_add(totalMessages*_bulkSize);
        this->_totalBytesReceived.fetch_add(totalBytesOfMessages);
    
        
//        for(shared_ptr<zmq::message_t>& message: messages)
//            this->_totalBytesReceived.fetch_add(message->size());
        if(this->_responsesReceived >= this->_noRequests)
        {
            this->notifyToEnd();
        }
//        
//        if(this->_delegate)
//            this->_delegate->didReceiveMessage(messages);
    }

    virtual void run()
    {
        try
        {
            this->_responsesReceived = 0;
            
            this_thread::sleep_for(chrono::seconds(1));

            vector<T> preparedMessages;
            int counter = 0;
            
            for(int i = 0; i < this->_noRequests; i++)
            {
                T message;
                
                if(this->_delegate)
                    message = this->_delegate->getData(_instanceNo, _serverNo, i, _clientNo, counter);
                else
                   message = this->getData(_instanceNo, _serverNo, i, _clientNo, _requestSize);
                
                preparedMessages.push_back(message);
//                sendData(message, _requestSize);

                if(counter == (_bulkSize - 1))
                {
                    sendData(preparedMessages);

                    preparedMessages.clear();
                    counter = 0;
                }
                else
                {
                    counter++;
                }

            }
            
            if(preparedMessages.size() > 0)
                sendData(preparedMessages);
            
//            for(T& message: preparedMessages)
//                sendData(preparedMessages);
            
            this->waitTillEnd();
        }
        catch (std::exception &e)
        {
            FILE_LOG(logINFO)<<"$$$ Exception in client #" + to_string(this->_clientNo)<<endl;
        }
    }
    
    virtual ~TaskBase(){}


private:
    bool _endFlag = false;

protected:
    mutex mtx;
    condition_variable cv;
    
    zmq::context_t& _context;
    int _noWorkerThreads;
    int _noRequests;
    int _requestSize;
    string _serverAddress;
    int _clientNo;
    int _instanceNo;
    int _serverNo;
    int _bulkSize;
    
    atomic<long long > _responsesReceived;
    atomic<long long> _totalBytesReceived;
    
    TaskOperations<T>* _delegate;
    
};

#endif /* defined(__DistributedKeyValueStore__TaskBase__) */
