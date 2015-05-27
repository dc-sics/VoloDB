//
//  TestBase.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/2/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "TestBase.h"
#include "../Logging/log1.h"
#include <chrono>

TestBase::TestBase(const string& name):_name(name)
{}

void TestBase::start(map<string, string>& settings)
{
    _noClients = stoi(settings["no_clients"]);
    _noWorkerThreads = stoi(settings["transport_worker_threads"]);
    _noRequests = stoi(settings["no_requests"]);
    _serverAddress = settings["server_address"];
    
    _requestSize = 1;
    if(settings.find("request_size") != settings.end())
        _requestSize = stoi(settings["request_size"]);

    _bulkSize = 1;
    if(settings.find("bulk_size") != settings.end())
        _bulkSize = stoi(settings["bulk_size"]);

//    _serverPort = stoi(settings["server_port"]);
}

long double TestBase::toGB(long double bytes)
{
    return bytes/1000.0/1000.0/1000.0;
}
long double TestBase::toMB(long double bytes)
{
    return bytes/1000.0/1000.0;
}

void TestBase::run(map<string, string>& settings)
{
    printHeader(settings);
    
    auto startTime = system_clock::now();
    start(settings);
    auto endTime = system_clock::now();
    
    _totalSecondsTaken = duration_cast<seconds>(endTime-startTime).count();
    
    _totalSecondsTaken--;//adjustment because tasks are started with 1 second delay
    
   printFooter();
}

long long TestBase::getTotalSecondsTaken()
{
    return _totalSecondsTaken;
}

void TestBase::printHeader(map<string, string>& settings)
{
    FILE_LOG(logINFO)<<"-------------------------------";
    FILE_LOG(logINFO)<<"Test Case: "<<_name;
    
    for(auto currentSetting: settings)
    {
        FILE_LOG(logINFO)<<currentSetting.first<<" : "<<currentSetting.second;
    }
    
    FILE_LOG(logINFO)<<"-------------------------------";
}
void TestBase::printFooter()
{
    FILE_LOG(logINFO)<<"Results:";

//    FILE_LOG(logINFO)<<"Total Requests: "<<_noRequests;
//    FILE_LOG(logINFO)<<"Total Data Sent Received: "<<toGB(_totalDataSentReceived)<<" GB";
    FILE_LOG(logINFO)<<"Time Taken: "<<_totalSecondsTaken<<" seconds";
//    FILE_LOG(logINFO)<<"Data Rate: "<<toMB(_totalDataSentReceived)/_totalSecondsTaken<<" MB/sec";
    FILE_LOG(logINFO)<<"Transactions per second: "<<(_noRequests*_noClients)/(_totalSecondsTaken == 0 ? 1 :_totalSecondsTaken);
    FILE_LOG(logINFO)<<endl;
}


