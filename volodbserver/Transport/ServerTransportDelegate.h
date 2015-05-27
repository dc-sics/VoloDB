//
//  ServerTransportDelegate.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/13/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__ServerTransportDelegate__
#define __DistributedKeyValueStore__ServerTransportDelegate__

#include <stdio.h>
#include <vector>
#include <memory>
#include "Transport/zmq.hpp"
#include "../Utils/TransportUtils.h"

class ServerTransport;

using namespace std;

class ServerTransportDelegate
{
public:
    virtual void didReceiveMessage(ServerTransport* serverTransport, TransportUtils::StoreMsgs& messages, const unsigned short& responsibleWorkerThread) = 0;
};

#endif /* defined(__DistributedKeyValueStore__ServerTransportDelegate__) */
