//
//  BasicTest.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/12/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef DistributedKeyValueStore_BasicTest_h
#define DistributedKeyValueStore_BasicTest_h

#include "Test/TestBase.h"
#include "BasicTransportTask.h"
#include <string>

using namespace std;
using namespace chrono;

template<class T1, class T2>
class BasicTest  : public TestBase
{
    
public:
    BasicTest(const string& name, TaskOperations<T2>* delegate=nullptr): TestBase(name), _delegate(delegate), _context(1){}
    
    virtual void start(map<string, string>& settings) override
    {
        TestBase::start(settings);
        
        int instanceNo = 0;
        int serverNo;
        
        instanceNo = stoi(settings["instance_no"]);
        serverNo = stoi(settings["server_no"]);
        
        long long totalBytesReceived = 0;
        
        for(int clientNo = 0; clientNo < _noClients; clientNo++)
        {
            shared_ptr<T1> currentClient;
            currentClient.reset(new T1(_context, instanceNo, serverNo, clientNo, _noWorkerThreads, _noRequests, _requestSize, _serverAddress, _bulkSize, _delegate));
            _clientThreads.push_back(shared_ptr<thread>(new thread(std::bind(&T1::run, currentClient))));
            _clientTasks.push_back(currentClient);
        }
        
        for(int i = 0; i < _clientThreads.size(); i++)
        {
            _clientThreads[i]->join();
            totalBytesReceived += _clientTasks[i]->getTotalBytesReceived();
        }
        
        _totalDataSentReceived = _requestSize*_noRequests*_noClients + totalBytesReceived;
    }

private:
    zmq::context_t _context;
    vector<shared_ptr<thread>> _clientThreads;
    vector<shared_ptr<T1>> _clientTasks;
    TaskOperations<T2>* _delegate;
};

#endif
