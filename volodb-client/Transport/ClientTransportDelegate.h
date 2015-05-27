//
//  ClientTransportDelegate.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/11/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__ClientTransportDelegate__
#define __DistributedKeyValueStore__ClientTransportDelegate__

#include <stdio.h>
#include "ClientTransport.h"

class ClientTransportDelegate
{
public:
    virtual void didReceiveMessage(ClientTransport* clientTransport, vector<shared_ptr<zmq::message_t>>& messages) = 0;
//    virtual void didReceiveMessage(ClientTransport* clientTransport, forward_list<void*> data) = 0;
};

#endif /* defined(__DistributedKeyValueStore__ClientTransportDelegate__) */
