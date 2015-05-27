//
//  TransportUtils.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/23/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "TransportUtils.h"
#include "Logging/log1.h"

namespace TransportUtils
{
    StoreMsgs receiveQueuedMessages(zmq::socket_t& socket, int readLimit)
    {
        StoreMsgs receivedMessages;
        
        while(true)
        {
            shared_ptr<zmq::message_t> message(new zmq::message_t);
            
            bool success = socket.recv(message.get(), ZMQ_NOBLOCK);
            
            if(success)
            {
                receivedMessages.push_back(message);
                
                if(readLimit > 0 && receivedMessages.size() == readLimit)
                    break;
            }
            else
            {
                if(zmq_errno() != EAGAIN)
                    throw runtime_error("Error while receiving messages from socket");
                
                break;
            }
        }
        
        return receivedMessages;
    }
    
    string getSinkName(const char* baseName, unsigned short threadNo)
    {
        return string(baseName) + "_" + to_string(threadNo);
    }
    
    
    template<class T>
    void printMessages(T messageCollection)
    {
        for(StoreMsgPtr msg: messageCollection)
        {
            FILE_LOG(logERROR)<<"Message: "<<(char*)msg->data();
        }
    }
}