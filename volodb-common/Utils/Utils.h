//
//  Utils.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/4/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__Utils__
#define __DistributedKeyValueStore__Utils__

#include "../Transport/zmq.hpp"

using namespace std;

class Utils
{
public:
    Utils()=delete;
    
    static void setUnlimitedQueueLength(zmq::socket_t* socket);
    static void setLingerOff(zmq::socket_t* socket);
    static string getUUID();
    
    static string toLower(string str);
    static bool toBool(string str);
    
};

template <typename T>
class ArrayDeleter
{
public:
    void operator () (T* d) const
    {
        delete [] d;
    }
};


#endif /* defined(__DistributedKeyValueStore__Utils__) */
