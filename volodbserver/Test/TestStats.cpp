//
//  TestStats.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 4/10/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "TestStats.h"
#include <future>
#include <fstream>
#include <algorithm>
#include <functional>
#include <numeric>

std::once_flag TestStats::winner_flag;
bool TestStats::shouldRun=false;
bool TestStats::shouldRunContinuous=false;
bool TestStats::isRunning = 0;
int TestStats::startTime = 0;
int TestStats::warmUpTime = 0;
int TestStats::totalTime = 0;
atomic<int> TestStats::totalOperationsExecutedByServerAtomic;
int* TestStats::totalOperationsExecutedByServer = nullptr;
shared_ptr<thread> TestStats::notifyThread;
int TestStats::noOfExecutors = 0;


//TestStats::shouldRun = true;
//TestStats::warmUpTime = testWarmupTime;
//TestStats::totalTime = testTotalTime;

void TestStats::init(bool doRun, int numberOfExecutors, int wTime, int tTime)
{
    shouldRun = doRun;
    noOfExecutors = numberOfExecutors;
    warmUpTime = wTime;
    totalTime = tTime;
    
    isRunning = startTime = 0;
    
    if(totalOperationsExecutedByServer)
        delete totalOperationsExecutedByServer;
    
    totalOperationsExecutedByServer = new int[numberOfExecutors];
    noOfExecutors = numberOfExecutors;
}

void TestStats::runInternal()
{
    if(notifyThread)
        notifyThread->join();
    
    notifyThread.reset(new thread(run));
}

void TestStats::runContinuous()
{
    isRunning = true;

    int totalOperationsExecutedBefore[noOfExecutors];
    int totalOperationsExecutedAfter[noOfExecutors];
    int result[noOfExecutors];

    while(true)
    {
        std::copy_n(totalOperationsExecutedByServer, noOfExecutors, totalOperationsExecutedBefore);
        
        this_thread::sleep_for(chrono::seconds(1));
        
        std::copy_n(totalOperationsExecutedByServer, noOfExecutors, totalOperationsExecutedAfter);
        
        std::transform(totalOperationsExecutedAfter, totalOperationsExecutedAfter+noOfExecutors, totalOperationsExecutedBefore, result, std::minus<int>());
        
        int totalOperationsExecutedInTime = std::accumulate(result,result+noOfExecutors,0);
        
//        std::time_t t = std::time(nullptr);
        
        cout<<totalOperationsExecutedInTime<<" tps"<<endl;
    }

}

void TestStats::run()
{
    isRunning = true;
    shouldRun = false;
    this_thread::sleep_for(chrono::seconds(warmUpTime));
    
    int* totalOperationsExecutedBefore = new int[noOfExecutors];
    
    std::copy_n(totalOperationsExecutedByServer, noOfExecutors, totalOperationsExecutedBefore);
    
    cout<<"Will start collecting stats"<<endl;

    this_thread::sleep_for(chrono::seconds(totalTime));
    
    int* totalOperationsExecutedAfter = new int[noOfExecutors];
    
    std::copy_n(totalOperationsExecutedByServer, noOfExecutors, totalOperationsExecutedAfter);
    
    printStats(totalOperationsExecutedBefore, totalOperationsExecutedAfter);
    
    isRunning = false;

    delete[] totalOperationsExecutedBefore;
    delete[] totalOperationsExecutedAfter;
    
    init(false, noOfExecutors, warmUpTime, totalTime);
    
//    abort();
}

void TestStats::printStats(int* before, int* after)
{
    int* result = new int[noOfExecutors];
    
    std::transform(after, after+noOfExecutors, before, result, std::minus<int>());

    int totalOperationsExecutedInTime = std::accumulate(result,result+noOfExecutors,0);
    
    FILE* file = fopen("/root/vdb/throughput.out", "w");
    if(file)
    {
        
        fprintf(file, "%d\n", totalOperationsExecutedInTime/totalTime);
        fprintf(file, "\n");
        fprintf(file, "Total requests processed: %d\n", totalOperationsExecutedInTime);
        fprintf(file, "Total Time Bechmaked: %d\n", totalTime);

        fclose(file);
    }
    
//    ofstream outFile("/root/vdb/throughput.out"/*"/Users/alidar/throughput.out"*/, std::ofstream::trunc);
//    if(!outFile.bad())
//    {
//    //    cout<<"*************STATS***************"<<endl;
//        outFile<<totalOperationsExecutedInTime/(double)totalTime<<endl;
//        outFile<<endl;
//        outFile<<"Total requests processed: "<<totalOperationsExecutedInTime<<endl;
//        outFile<<"Total Time Bechmaked : "<<totalTime<<endl;
//        
//        outFile.close();
//    }
    
    cout<<totalOperationsExecutedInTime/(double)totalTime<<endl;
    cout<<endl;
    cout<<"Total requests processed: "<<totalOperationsExecutedInTime<<endl;
    cout<<"Total Time Bechmaked : "<<totalTime<<endl;
    
    delete [] result;
    
    cout<<"Stats generatation completed"<<endl;
}

void TestStats::start()
{
    call_once(winner_flag, runInternal);
}

void TestStats::startContinuous()
{
    if(notifyThread)
    notifyThread->join();
    
    notifyThread.reset(new thread(runContinuous));
}

