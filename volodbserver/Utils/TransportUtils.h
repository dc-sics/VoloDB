//
//  TransportUtils.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/23/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef DistributedKeyValueStore_TransportUtils_h
#define DistributedKeyValueStore_TransportUtils_h

#include <vector>
#include <memory>
#include "Transport/zmq.hpp"
#include <stdexcept>

using namespace std;

namespace TransportUtils
{
    using StoreMsg = zmq::message_t;
    using StoreMsgPtr = shared_ptr<StoreMsg>;
    using StoreMsgs = vector<StoreMsgPtr>;

    StoreMsgs receiveQueuedMessages(zmq::socket_t& socket, int readLimit=-1);
    string getSinkName(const char* baseName, unsigned short threadNo);
    
    template<class T>
    void printMessages(T messageCollection);
}


#endif
