//
//  TestBase.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/26/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__TestBase__
#define __DistributedKeyValueStore__TestBase__

#include "TaskBase.h"

using namespace std;
using namespace chrono;

class TestBase/*: public TaskBase*/
{
private:
    long long _totalSecondsTaken = 0;
    
protected:
    int _noClients;
    int _noWorkerThreads;
    int _noRequests;
    int _requestSize;
    int _bulkSize;
    string _serverAddress;
//    int _serverPort;
    
    long long _totalDataSentReceived = 0;
    string _name;

public:
    TestBase(const string& name);
    virtual ~TestBase(){}

    virtual void start(map<string, string>& settings);
    virtual void run(map<string, string>& settings) final;
    
    long long getTotalSecondsTaken();
    
    void printHeader(map<string, string>& settings);
    void printFooter();
    
    long double toGB(long double bytes);
    long double toMB(long double bytes);
};

#endif /* defined(__DistributedKeyValueStore__TestBase__) */
