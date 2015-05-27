//
//  ExecutorDelegate.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/6/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef DistributedKeyValueStore_ExecutorDelegate_h
#define DistributedKeyValueStore_ExecutorDelegate_h

class Executor;
class Result;

class ExecutorDelegate
{
public:
    //backed by vector with shared_ptrs, will be deleted automatically.
    virtual void didReceiveResponse(Executor* executor, vector<Result*>& result) = 0;
};


#endif
