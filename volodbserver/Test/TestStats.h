//
//  TestStats.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 4/10/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef DistributedKeyValueStore_TestStats_h
#define DistributedKeyValueStore_TestStats_h

#include <atomic>
#include <chrono>
#include <thread>
#include <iostream>
#include <memory>
#include <mutex>

using namespace std;

class TestStats
{
private:
    static void runInternal();
    static void runContinuous();
    static void run();
    static void printStats(int* before, int* after);
    
public:
    static std::once_flag winner_flag;
    static bool isRunning;
    static bool shouldRun;
    static bool shouldRunContinuous;
    static int startTime;
    static int warmUpTime;
    static int totalTime;
    static int noOfExecutors;
    static atomic<int> totalOperationsExecutedByServerAtomic;
    static int* totalOperationsExecutedByServer;
    static shared_ptr<thread> notifyThread;
    
    TestStats()=delete;
    
    static void init(bool doRun, int numberOfExecutors, int wTime, int tTime);
    static void start();
    static void startContinuous();
};


#endif
