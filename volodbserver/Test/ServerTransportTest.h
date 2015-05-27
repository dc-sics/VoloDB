//
//  ServerTransportTest.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/17/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef DistributedKeyValueStore_ServerTransportTest_h
#define DistributedKeyValueStore_ServerTransportTest_h

#include "../Transport/ServerTransport.h"
#include "../Transport/ServerTransportDelegate.h"
#include "Logging/log1.h"
#include "Test/TestBase.h"
#include <string>
#include <iostream>

using namespace std;

class ServerTransportTest : public ServerTransportDelegate, public TestBase
{
public:
    ServerTransportTest(const string& name):TestBase(name){}
    virtual void start(map<string, string>& settings) override
    {
        int transportPollWait = stoi(settings["transport_poll_wait"]);
        int workerThreads = stoi(settings["transport_worker_threads"]);
        
        _serverTransport.reset(new ServerTransport(this, workerThreads, transportPollWait));
        
        _serverTransport->start();

        taskSenderSocket.reset(new zmq::socket_t(*_serverTransport->getContext(), ZMQ_PUSH));
        taskSenderSocket->bind(TransportUtils::getSinkName(ServerTransport::SERVER_SINK_NAME, 0).c_str());
    }

private:
    shared_ptr<zmq::socket_t> taskSenderSocket;
    shared_ptr<ServerTransport> _serverTransport;

    virtual void didReceiveMessage(ServerTransport* serverTransport, TransportUtils::StoreMsgs& messages, const unsigned short& responsibleWorkerThread) override
    {
        for(int i = 0; i < messages.size(); i+=2)
        {
            zmq::message_t& identity = *messages[i];
            zmq::message_t& message = *messages[i+1];
         /*
            string stringMessage("Server ");
            stringMessage+=to_string(responsibleWorkerThread) + ": ";
            //stringMessage+=(char*)message.data();
            
            const unsigned long size = stringMessage.length() + 1;
            
            char* rawBuffer = new char[size]();
            memcpy(rawBuffer, stringMessage.c_str(), size-1);
            
            zmq::message_t serverMessage(rawBuffer, size, _free_handler_default);
            */
            taskSenderSocket->send(identity, ZMQ_SNDMORE);
            taskSenderSocket->send(message);
        }
    }

    static void _free_handler_default(void* data, void* hint)
    {
        delete [] (char*)data;
    }

};


#endif
