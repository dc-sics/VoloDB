//
//  Store.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/19/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__Store__
#define __DistributedKeyValueStore__Store__

#include <stdio.h>
#include <vector>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "../Utils/TransportUtils.h"
#include "../Transport/ServerTransportDelegate.h"
#include "Frame/FrameDecoder.h"
#include <mysql.h>
#include "NdbApi.hpp"
#include "../Cache/TableInfoCache.h"


using namespace std;
using namespace TransportUtils;

class ServerTransport;

class Store : public ServerTransportDelegate
{
private:
    vector<shared_ptr<thread>> _definerThreads;
    vector<shared_ptr<thread>> _executorThreads;
    
    shared_ptr<zmq::socket_t> _definerPushSocket;
    shared_ptr<zmq::socket_t> _executorPushSocket;
    
    vector<vector<shared_ptr<zmq::socket_t>>> _taskSenderSockets;
    mutex executorAssignerMutex;
    
    shared_ptr<ServerTransport> _serverTransport;
    int _storePort;
    unsigned short _noDefinerThread;
    unsigned short _noExecutorThreads;
    
    int _storePollWait;
    
    bool _isLockFreeExecutorAssignment;
    bool _extraDefinerLayer;
    
    bool _isRunning;
    bool _stopFlag;
    
    std::mutex _mtx;
    std::condition_variable _cv;
    
    shared_ptr<MYSQL> _mysql;
    mutex mySQLInitMutex;
    string _mySQLAddress;
    unsigned int _mySQLPort;
    string _mySQLUserName;
    string _mySQLPassword;
    
    shared_ptr<Ndb_cluster_connection> _clusterConnections;
    shared_ptr<Ndb*> _ndbDefiners;
    shared_ptr<Ndb*> _ndbExecutors;
    
    string _ndbConnectString;
    int _ndbPort;
    string _database;
    int _totalClusterConnections;
    
    TableInfoCache _tableInfoCache;
    
    std::mutex _mutexForExecutorReadyStatus;
    std::condition_variable _conditionVariableForExecutorReadyStatus;
    int _currentExecutorInitCount;
    
    constexpr static const char* _DEFINER_SINK_NAME =  "inproc://definersink";
    constexpr static const char* _EXECUTOR_SINK_NAME =  "inproc://executorsink";
    
    constexpr static const int _MAX_TRANSACTIONS = 1024;
    
    void _stopWorkerThreads(vector<shared_ptr<thread>> workerThreads);
    void _definerWorkerThreadFunc(unsigned short threadNo);
    void _executorWorkerThreadFunc(unsigned short threadNo);
    
    inline void _processMessages(StoreMsgs& messages, vector<vector<shared_ptr<zmq::socket_t>>>& taskSenderSockets, const unsigned short& threadNo);
    inline void _executeMessages(StoreMsgs& messages, zmq::socket_t& taskSenderSocket, unsigned short threadNo);
    
    vector<vector<shared_ptr<zmq::socket_t>>> createExecutorTaskSenderSockets();
    inline shared_ptr<zmq::socket_t>& getResponsibleExecutorTaskSenderSocket(vector<vector<shared_ptr<zmq::socket_t>>>& taskSenderSockets, const unsigned short& threadNo, const unsigned short& executorThreadNo);
    
    inline void sendTaskToExecutor(FrameMessagesTupleListPointer& messagesListPointer, shared_ptr<zmq::socket_t>& taskSenderSocket);
    
    static void _free_handler_default(void* data, void* hint);
    
    void didReceiveMessage(ServerTransport* serverTransport, TransportUtils::StoreMsgs& messages, const unsigned short& responsibleWorkerThread) override;
    
    void _initNDB(shared_ptr<Ndb*>& ndb, const int& total);
    void _initDB();
    
    void initMySQL(MYSQL* mySQL, const int& executorNo);
    
    void _cleanUp();
    void _clearNDb(shared_ptr<Ndb*>& ndb, const int& total);
    
    void waitForExecutorsToInitialize();
    void addExecutorInitializationCount();
    
    inline bool shouldIgnoreRequest();
    inline void resetTransactionMonitorCounterIfRequired(time_t& currentTime);
    
public:
    void run();
    void stop();
    void waitTillNotRunning();
    Store(string mySQLAddress, unsigned int mySQLPort, string mySQLUserName, string mySQLPassword, string ndbConnectString, int ndbPort = 1186,
          string database="vdb", int storePort = 5570, unsigned short noDefinerThread=5, unsigned short noExecutorThreads=5,
          bool singleClusterConnection=true, int transportPollWait=10, int storePollWait=10);
    ~Store();
};


#endif /* defined(__DistributedKeyValueStore__Store__) */
