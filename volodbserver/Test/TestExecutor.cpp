//
//  TestExecutor.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/26/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "TestExecutor.h"
#include "ServerTransportTest.h"
#include "Logging/log1.h"
#include <csignal>

static shared_ptr<TestBase> currentTestCase;
static mutex waitMutex;

static void signal_handler(int signal)
{
    FILE_LOG(logINFO)<<"Skipping current Test case";
    if(currentTestCase.get())
    {
        waitMutex.unlock();
    }
}

void TestExecutor::handle(ConfigFileReader& testSettings)
{
    std::signal(SIGINT, signal_handler);
    
    auto testCases = testSettings.getSettings();
    for(auto testCase: testCases)
    {
        waitMutex.lock();
        if(testCase.first == "ServerTransportTest")
        {
            currentTestCase.reset(new ServerTransportTest(testCase.first));
            currentTestCase->run(testCase.second);
            
            waitMutex.lock();
        }
        
        waitMutex.unlock();
    }
    
     FILE_LOG(logINFO)<<"Finished Tests";
}
