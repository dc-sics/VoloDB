//
//  ServerTransport.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/9/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__ServerTransport__
#define __DistributedKeyValueStore__ServerTransport__

#include <stdio.h>
#include <memory>
#include "Transport/zmq.hpp"
#include <vector>
#include <thread>
#include <memory>
#include <mysql.h>

class ServerTransportDelegate;

using namespace std;

class ServerTransport
{
private:
    shared_ptr<zmq::context_t> _context;
    shared_ptr<zmq::socket_t> _frontend;
    shared_ptr<zmq::socket_t> _backend;
    unsigned short _totalWorkerThreads;
    vector<shared_ptr<thread>> _workerThreads;
    shared_ptr<thread> _mainThread;
    bool _isStarted;
    bool _stopFlag;
    int _pollWait;
    ServerTransportDelegate* _delegate;
    
    MYSQL* _mySQL;
    
   constexpr static const char* _BACKEND_PROC_NAME = "inproc://backend";
    
    void cleanUp();
    
public:
    ServerTransport(ServerTransportDelegate* delegate, unsigned short totalWorkerThreads=5, int pollWait=10);
    void start(/*zmq::context_t& context, MYSQL* mysql = nullptr,*/ int port=5570);
    void close();
    void worker_thread_handler(unsigned short threadNo, string serverBindAddress);
    shared_ptr<zmq::context_t> getContext();
    ~ServerTransport();
    
    constexpr static const char* SERVER_SINK_NAME = "inproc://serversink";
};

#endif /* defined(__DistributedKeyValueStore__ServerTransport__) */
