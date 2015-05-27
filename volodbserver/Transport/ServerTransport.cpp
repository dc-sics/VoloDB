//
//  ServerTransport.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/9/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "ServerTransport.h"
#include <string>
#include <thread>
#include "ServerTransportDelegate.h"
#include "Logging/log1.h"
#include "Utils/Utils.h"
#include <stdexcept>

using namespace std;

ServerTransport::ServerTransport(ServerTransportDelegate* delegate, unsigned short totalWorkerThreads, int pollWait):
    _context(new zmq::context_t(1)),
    _delegate(delegate),
    _totalWorkerThreads(totalWorkerThreads),
    _isStarted(false),
    _stopFlag(false),
    _pollWait(pollWait)
{
    if(_context == nullptr)
        throw runtime_error("Failed to allocate memory to zmq::context");
}

void ServerTransport::start(/*zmq::context_t& context, MYSQL* mysql,*/ int port)
{
    if(!_isStarted)
    {
//        _context.reset(&context);
        
//        if(_context == nullptr)
//            throw runtime_error("Context can not be null");
        
//        if(mysql == nullptr)
//            throw runtime_error("SQL connection pointer cant be null");
//        
//        _mySQL = mysql;
        
        _mainThread.reset(new thread([&,port]()
        {
            _frontend.reset(new zmq::socket_t(*_context, ZMQ_ROUTER));
            _backend.reset(new zmq::socket_t(*_context, ZMQ_DEALER));
            
            Utils::setUnlimitedQueueLength(_frontend.get());
            Utils::setUnlimitedQueueLength(_backend.get());
            
            string serverBindAddress = string("tcp://*:" + to_string(port));
            _frontend->bind(serverBindAddress.c_str());
            _backend->bind(_BACKEND_PROC_NAME);
            
            for(unsigned short threadNo = 0 ; threadNo < _totalWorkerThreads; threadNo++)
                _workerThreads.push_back(shared_ptr<thread>(new thread(&ServerTransport::worker_thread_handler, this, threadNo, serverBindAddress)));
            
            try
            {
                zmq::proxy(*_frontend, *_backend, nullptr);
            }
            catch (std::exception &ex)
            {
                FILE_LOG(logWARNING)<<"EXCEPTION FROM SERVER MAIN THREAD: "<<ex.what();
            }
        }));
        
        _isStarted = true;
    }
}

void ServerTransport::cleanUp()
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
            FILE_LOG(logWARNING)<<"EXCEPTION WHILE CLEANING UP SERVER: "<<ex.what();
        }
    }
    _workerThreads.clear();
    _stopFlag = false;
    
    _backend.reset();
    _frontend.reset();
    
    _context.reset();
    
    _mainThread->join();
    _mainThread.reset();
}

void ServerTransport::close()
{
    cleanUp();

    _isStarted = false;
}

shared_ptr<zmq::context_t> ServerTransport::getContext()
{
    return _context;
}

void ServerTransport::worker_thread_handler(unsigned short threadNo, string serverBindAddress)
{
    try
    {        
        zmq::socket_t workerSocket(*_context, ZMQ_DEALER);
        zmq::socket_t sinkSocket(*_context, ZMQ_PULL);
        
        Utils::setUnlimitedQueueLength(&workerSocket);
        Utils::setUnlimitedQueueLength(&sinkSocket);
        
        Utils::setLingerOff(&workerSocket);
        
        workerSocket.connect(_BACKEND_PROC_NAME);
        sinkSocket.bind(TransportUtils::getSinkName(SERVER_SINK_NAME, threadNo).c_str() );
        
        FILE_LOG(logINFO)<<"Initialized definer worker # "<<threadNo;
        
        const int TASK_RECEIVER_SOCKET_INDEX = 0;
        const int SINK_SOCKET_INDEX = 1;
        const int TOTAL_POLL_ITEMS = 2;
        const int POLL_WAIT_MS = _pollWait;
        
        zmq::pollitem_t items[] = {
            {workerSocket, 0, ZMQ_POLLIN, 0},
            {sinkSocket, 0, ZMQ_POLLIN, 0}
        };

        while(true)
        {
            if(_stopFlag)
                break;
            
          std::this_thread::sleep_for(chrono::milliseconds(10));
            
//            zmq::poll(items, TOTAL_POLL_ITEMS, POLL_WAIT_MS);
            
//            if (items[TASK_RECEIVER_SOCKET_INDEX].revents & ZMQ_POLLIN)
            {
                if(_delegate)
                {
                    TransportUtils::StoreMsgs allMessages = TransportUtils::receiveQueuedMessages(workerSocket);
                    if(allMessages.size() > 0)
                        _delegate->didReceiveMessage(this, allMessages, threadNo);
                }
            }
//            if (items[SINK_SOCKET_INDEX].revents & ZMQ_POLLIN)
            {
                TransportUtils::StoreMsgs allMessages = TransportUtils::receiveQueuedMessages(sinkSocket);
                for(int i = 0; i < allMessages.size();i+=2)
                {
                    workerSocket.send(*allMessages[i], ZMQ_SNDMORE);
                    workerSocket.send(*allMessages[i+1]);
                }
            }
        }
    }
    catch(exception& ex)
    {
        FILE_LOG(logERROR)<<"EXCEPTION IN SERVER WORKER # "<<threadNo<<": "<<ex.what();
    }
}


ServerTransport::~ServerTransport()
{
    close();
}
