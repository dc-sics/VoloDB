//
//  ClientTaskTest.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/17/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef DistributedKeyValueStore_ClientTransportTest_h
#define DistributedKeyValueStore_ClientTransportTest_h

#include "BasicTest.h"

using namespace std;

template<class T>
class ClientTransportTest : public BasicTest<T, char*>, public TaskOperations<char*>
{
    
public:
    ClientTransportTest(const string& name): BasicTest<T, char*>(name, this){}
    
    virtual void start(map<string, string>& settings) override
    {
        BasicTest<T, char*>::start(settings);
    }

    char* getData(int instanceNo, int serverNo, int requestNo, int clientNo, int requestSize)
    {
        return new char[requestSize];
    }
};

#endif
