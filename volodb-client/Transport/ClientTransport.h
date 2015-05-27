//
//  ClientTransport.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/9/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__ClientTransport__
#define __DistributedKeyValueStore__ClientTransport__

#include <stdio.h>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <functional>
#include "Transport/zmq.hpp"

class ClientTransportDelegate;
//class thread;

using namespace std;

class ClientTransport
{
private:
//    shared_ptr<zmq::context_t> _context;
    zmq::context_t* _context;
    shared_ptr<zmq::socket_t> _clientSocket;
    shared_ptr<zmq::socket_t> _sendPushSocket;
    bool _isConnected;
    unsigned short _totalWorkerThreads;
    ClientTransportDelegate* _delegate;
    vector<shared_ptr<thread>> _workerThreads;
    bool _stopFlag;
    string _senderSinkName;
    
    constexpr static const char* _SENDER_SINK_NAME = "inproc://sendersink";

    void worker_thread_func(unsigned short threadNo, string serverURL);
    void cleanUp();
    bool receiveQueuedMessages(zmq::socket_t* socket, zmq::socket_t* forward_socket);
    
    //for optimizatin purposes, so we don't have to create null shared ptr again and again in worker threads
    static const shared_ptr<zmq::socket_t> NULL_SOCKET_SHARED_PTR;
    
    static void free_handler_default(void* data, void* hint);
    
    
public:
    
    ClientTransport(ClientTransportDelegate* delegate, unsigned short totalWorkerThreads = 5);
    void connect(zmq::context_t* context, string serverAddress, unsigned int port=5570);
    void send(void* data, size_t size, zmq::free_fn freeHandler=free_handler_default, void* hint=nullptr);
    shared_ptr<zmq::message_t> receive(bool& hasError, bool doBlock = false);
    vector<shared_ptr<zmq::message_t>> receiveAll(bool& error, bool doBlock = false);
    void close();
    ~ClientTransport();
    
};

#endif /* defined(__DistributedKeyValueStore__ClientTransport__) */
