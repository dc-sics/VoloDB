//
//  ClientTransport.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/9/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "ClientTransport.h"
#include <string>
#include "ClientTransportDelegate.h"
#include <stdexcept>
#include "Logging/log1.h"
#include "Utils/Utils.h"

const shared_ptr<zmq::socket_t> ClientTransport::NULL_SOCKET_SHARED_PTR;

ClientTransport::ClientTransport(ClientTransportDelegate* delegate, unsigned short totalWorkerThreads):
    _delegate(delegate),
    _totalWorkerThreads(totalWorkerThreads),//TODO: uncomment later
    _context(nullptr),
    _isConnected(false),
    _stopFlag(false)
{
    _senderSinkName = string("inproc://sendersink_") + Utils::getUUID();
}

void ClientTransport::connect(zmq::context_t* context, string serverAddress, unsigned int port)
{
    if(!_isConnected)
    {
//        _context.reset(&context);
        _context = context;
        
        if(_context == nullptr)
            throw runtime_error("Context can not be null");
        
        string serverURL = string("tcp://") + serverAddress + ":" + to_string(port);
        

        if(_totalWorkerThreads == 0)
        {
            _clientSocket.reset(new zmq::socket_t(*_context, ZMQ_DEALER));
            Utils::setUnlimitedQueueLength(_clientSocket.get());
            _clientSocket->connect(serverURL.c_str());
        }
        else
        {
            _sendPushSocket.reset(new zmq::socket_t(*_context, ZMQ_PUSH));
            Utils::setUnlimitedQueueLength(_sendPushSocket.get());
            
            _sendPushSocket->bind(_senderSinkName.c_str());
            
            for(unsigned short threadNo = 0 ; threadNo < _totalWorkerThreads; threadNo++)
                _workerThreads.push_back(shared_ptr<thread>(new thread(&ClientTransport::worker_thread_func, this, threadNo, serverURL)));
        }

        _isConnected = true;
    }
}


void ClientTransport::send(void* data, size_t size, zmq::free_fn freeHandler, void* hint)
{
    if(_isConnected)
    {
        zmq::message_t sendMessage(data, size, freeHandler, hint);
        
        if(_totalWorkerThreads == 0)
        {
            _clientSocket->send(sendMessage);
        }
        else
        {
            bool res = _sendPushSocket->send(sendMessage);
            if(!res)
            {
                FILE_LOG(logERROR)<<"Failed: ";
            }
        }
    }
    else
        throw runtime_error("Not connected to the server");
}

shared_ptr<zmq::message_t> ClientTransport::receive(bool& error, bool doBlock)
{
    if(_totalWorkerThreads == 0)
    {
        error = false;
        
        shared_ptr<zmq::message_t> message(new zmq::message_t);
        bool success = _clientSocket->recv(&message, doBlock ? 0: ZMQ_NOBLOCK);
        
        if(!success)
        {
            if(zmq_errno() != EAGAIN)
                error = true;
            
            return nullptr;
        }
        else
            return message;
    }
    else
        throw runtime_error("Receive is only allowed on non multi-threaded client");
}

vector<shared_ptr<zmq::message_t>> ClientTransport::receiveAll(bool& error, bool doBlock)
{
    vector<shared_ptr<zmq::message_t>> allMessages;
    shared_ptr<zmq::message_t> currentMessage;
    
    do
    {
        currentMessage = receive(error, doBlock);
        
        if(currentMessage)
            allMessages.push_back(currentMessage);
        
    } while(currentMessage);
        
    return allMessages;
}


void ClientTransport::cleanUp()
{
    if(_totalWorkerThreads > 0)
    {
        _stopFlag = true;
        for(shared_ptr<thread>& workerThread: _workerThreads)
        {
            try
            {
                workerThread->join();
            }
            catch(exception& ex)
            {
                FILE_LOG(logWARNING)<<"EXCEPTION WHILE CLEANING UP CLIENT: "<<ex.what();
            }
        }
        _workerThreads.clear();
        _stopFlag = false;
    }
    else
        _clientSocket.reset();
    
    _sendPushSocket.reset();
//    _context.reset();
}

void ClientTransport::close()
{
    cleanUp();
    
    _isConnected = false;
}

bool ClientTransport::receiveQueuedMessages(zmq::socket_t* socket, zmq::socket_t* forward_socket)
{
    vector<shared_ptr<zmq::message_t>> receivedMessages;

    while(true)
    {
        shared_ptr<zmq::message_t> message(new zmq::message_t);

        bool success = socket->recv(message.get(), ZMQ_NOBLOCK);
        
        if(success)
        {
            //optimization to store all the message and send them together in bulk
            receivedMessages.push_back(message);
        }
        else
        {
            if(receivedMessages.size() > 0)
            {
                if(forward_socket)
                {
    //                FILE_LOG(logWARNING)<<receivedMessages.size();
                    for(shared_ptr<zmq::message_t>& currentMessage: receivedMessages)
                        forward_socket->send(*currentMessage);
                }
                else
                {
                    if(_delegate)
                        _delegate->didReceiveMessage(this, receivedMessages);
                }
            }
            
            if(zmq_errno() != EAGAIN)
                return false;
            
            break;
        }
    }
    
    return true;
}

void ClientTransport::worker_thread_func(unsigned short threadNo, string serverURL)
{
    try
    {
        zmq::socket_t clientSocket(*_context, ZMQ_DEALER);
        zmq::socket_t sinkSocket(*_context, ZMQ_PULL);
        
        Utils::setUnlimitedQueueLength(&clientSocket);
        Utils::setUnlimitedQueueLength(&sinkSocket);
        
        Utils::setLingerOff(&clientSocket);
        
        clientSocket.connect(serverURL.c_str());
        sinkSocket.connect(_senderSinkName.c_str());
        
        const int CLIENT_SOCKET_INDEX = 0;
        const int SINK_SOCKET_INDEX = 1;
        const int TOTAL_POLL_ITEMS = 2;
        const int POLL_WAIT_MS = 10;
        
        zmq::pollitem_t items[] = {
            {clientSocket, 0, ZMQ_POLLIN, 0},
            {sinkSocket, 0, ZMQ_POLLIN, 0}
        };
        
        bool isSocketValid = true;
        
        while(isSocketValid)
        {
            if(_stopFlag)
                break;

//            zmq::poll(items, TOTAL_POLL_ITEMS, POLL_WAIT_MS);
            std::this_thread::sleep_for(chrono::milliseconds(10));
            
//            if (items[CLIENT_SOCKET_INDEX].revents & ZMQ_POLLIN)
                isSocketValid = receiveQueuedMessages(&clientSocket, nullptr);
//            if (items[SINK_SOCKET_INDEX].revents & ZMQ_POLLIN)
                isSocketValid = receiveQueuedMessages(&sinkSocket, &clientSocket);
        }
    }
    catch(exception& ex)
    {
        FILE_LOG(logERROR)<<"EXCEPTION IN CLIENT WORKER: "<<ex.what();
    }
}

void ClientTransport::free_handler_default(void* data, void* hint)
{
    delete [] (char*)data;
}


ClientTransport::~ClientTransport()
{
    close();
}

