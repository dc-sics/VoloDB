//
//  TestExecutor.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/26/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "TestExecutor.h"
#include "ClientTransportTest.h"
#include "CreateStoreTestAllTypes.h"
#include "DropStoreTestAllTypes.h"
#include "SetTestSimple.h"
#include "DeleteTestSimple.h"
#include "GetTestSimple.h"
#include "PruneIndexScan.h"
#include "BasicTest.h"
#include "BasicTransportTask.h"
#include "BasicTaskExecution.h"
#include "Logging/log1.h"
#include <csignal>


bool TestExecutor::endsWith(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

bool TestExecutor::startsWith(const std::string a, const std::string b) {
    if (a.size() > b.size()) {
        return a.substr(0,b.size()) == b;
    }
    else {
        return b.substr(0,a.size()) == a;
    }
}

void TestExecutor::handle(ConfigFileReader& testSettings)
{
    auto testCases = testSettings.getSettings();
    for(auto testCase: testCases)
    {
        shared_ptr<TestBase> currentTestCase;
        
        if(testCase.second.find("disabled") != testCase.second.end())
            continue;

        if(endsWith(testCase.first,"TransportTest"))
            currentTestCase.reset(new ClientTransportTest<BasicTransportTask<char*>>(testCase.first));
        else if(endsWith(testCase.first, "CreateStoreTest-AllTypes"))
            currentTestCase.reset(new CreateStoreTestAllTypes<BasicTaskExecution<shared_ptr< ::google::protobuf::Message>>>(testCase.first));
        else if(endsWith(testCase.first, "DropStoreTest-AllTypes"))
            currentTestCase.reset(new DropStoreTestAllTypes<BasicTaskExecution<shared_ptr< ::google::protobuf::Message>>>(testCase.first));
        else if(endsWith(testCase.first, "SetTest-Simple"))
            currentTestCase.reset(new SetTestSimple<BasicTaskExecution<shared_ptr< ::google::protobuf::Message>>>(testCase.first));
        else if(endsWith(testCase.first, "DeleteTest-Simple"))
            currentTestCase.reset(new DeleteTestSimple<BasicTaskExecution<shared_ptr< ::google::protobuf::Message>>>(testCase.first));
        else if(endsWith(testCase.first, "GetTest-Simple"))
            currentTestCase.reset(new GetTestSimple<BasicTaskExecution<shared_ptr< ::google::protobuf::Message>>>(testCase.first));
        else if(endsWith(testCase.first, "PruneIndexScan"))
            currentTestCase.reset(new PruneIndexScan<BasicTaskExecution<shared_ptr< ::google::protobuf::Message>>>(testCase.first));
        
        if(currentTestCase.get())
            currentTestCase->run(testCase.second);

    }
    
     FILE_LOG(logINFO)<<"Finished Tests";
}
