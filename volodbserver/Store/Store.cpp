//
//  Store.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/19/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "Store.h"
#include "../Transport/ServerTransport.h"
#include "NDBMessageProcessor.h"
#include "NDBMessageExecutor.h"
#include "Utils/Utils.h"
#include "Logging/log1.h"
//#include <NdbThread.h>
#include <stdlib.h>
#include "../Test/TestStats.h"

//can be unset if _isLockFreeExecutorAssignment = true
#define INCLUDE_LOCK_STATEMENTS 0
//can be unset if _extraDefinerLayer = true
#define INCLUDE_DEFINER_STATEMENTS 0

Store::Store(string mySQLAddress, unsigned int mySQLPort, string mySQLUserName, string mySQLPassword, string ndbConnectString, int ndbPort,
             string database, int storePort, unsigned short noDefinerThread, unsigned short noExecutorThreads, bool singleClusterConnection,
             int transportPollWait, int storePollWait):
    _mySQLAddress(mySQLAddress),
    _mySQLPort(mySQLPort),
    _mySQLUserName(mySQLUserName),
    _mySQLPassword(mySQLPassword),
    _ndbConnectString(ndbConnectString),
    _ndbPort(ndbPort),
    _database(database),
    _storePort(storePort),
    _noDefinerThread(noDefinerThread),
    _noExecutorThreads(noExecutorThreads),
    _storePollWait(storePollWait),
    _serverTransport(new ServerTransport(this, noDefinerThread, transportPollWait))
{
    _isRunning = false;
    _currentExecutorInitCount = 0;
    _stopFlag = false;
    _isLockFreeExecutorAssignment = true;
    _extraDefinerLayer = true;
    
    if(singleClusterConnection)
        _totalClusterConnections = 1;
    else
        _totalClusterConnections = _noExecutorThreads;
    
    srand ((unsigned)time(NULL));
    
    _mysql.reset(new MYSQL[_noExecutorThreads], ArrayDeleter<MYSQL>());
}

void Store::_initNDB(shared_ptr<Ndb*>& ndb, const int& total)
{
    ndb.reset(new Ndb*[total], ArrayDeleter<Ndb*>());
    for(int i = 0; i < total; i++)
    {
        if(i < _totalClusterConnections)
            ndb.get()[i] = new Ndb(_clusterConnections.get() + i, _database.c_str());
        else
            ndb.get()[i] = new Ndb(_clusterConnections.get(), _database.c_str());
        
        ndb.get()[i]->init(_MAX_TRANSACTIONS);
        
        if (ndb.get()[i]->waitUntilReady(10000) != 0)
        {
            FILE_LOG(logERROR)<<"NDB is not ready # "<<i+1;
            exit(EXIT_FAILURE);
        }
    }
}

void Store::_initDB()
{
    string connectionString = string(_ndbConnectString) + ":" + to_string(_ndbPort);
    setenv("NDB_CONNECTSTRING", connectionString.c_str(), 1);
    
    ndb_init();

    //_mysql.reset(new MYSQL[_noExecutorThreads], ArrayDeleter<MYSQL>());

    _clusterConnections.reset(new Ndb_cluster_connection[_totalClusterConnections], ArrayDeleter<Ndb_cluster_connection>());

    for (int i = 0; i < _totalClusterConnections; i++)
    {
        if(_clusterConnections.get()[i].connect(12, 5, 1) != 0)
        {
            FILE_LOG(logERROR)<<"Failed to connect to ndb cluster connection no "<<i+1;
            exit(EXIT_FAILURE);
        }
    }
    
   FILE_LOG(logINFO)<<"Initialzing NDB ";
    
//    _initNDB(_ndbDefiners, _noDefinerThread);
    _initNDB(_ndbExecutors, _noExecutorThreads);
    
   FILE_LOG(logINFO)<<"NDB Initialzed";
    
    NDBMessageExecutor::initialize(_mysql.get(), _ndbExecutors.get(), &_tableInfoCache);
//    NDBMessageProcessor::initialize(_ndbDefiners.get(), &_tableInfoCache);

}

void Store::waitForExecutorsToInitialize()
{
    std::unique_lock<std::mutex> lck(_mutexForExecutorReadyStatus);
    while(_currentExecutorInitCount < _noExecutorThreads)
        _conditionVariableForExecutorReadyStatus.wait(lck);
}

void Store::addExecutorInitializationCount()
{
    std::unique_lock<std::mutex> lck(_mutexForExecutorReadyStatus);
    _currentExecutorInitCount++;
    _conditionVariableForExecutorReadyStatus.notify_all();
}

vector<vector<shared_ptr<zmq::socket_t>>> Store::createExecutorTaskSenderSockets()
{
    vector<vector<shared_ptr<zmq::socket_t>>> executorTaskSenderSockets;
    
#if INCLUDE_LOCK_STATEMENTS == 1
    if(!_isLockFreeExecutorAssignment)
    {
        vector<shared_ptr<zmq::socket_t>> excutorTaskSubSockets;
        
        for(unsigned short threadNo = 0; threadNo < _noExecutorThreads; threadNo++)
        {
            shared_ptr<zmq::socket_t> taskSenderSocket(new zmq::socket_t(*_serverTransport->getContext(), ZMQ_PUSH));
            taskSenderSocket->connect(TransportUtils::getSinkName(_EXECUTOR_SINK_NAME, threadNo).c_str());
            
            excutorTaskSubSockets.push_back(taskSenderSocket);
        }
        executorTaskSenderSockets.push_back(excutorTaskSubSockets);

    }
    else
#endif
    {
        for(unsigned short definerThreadNo = 0; definerThreadNo < _noDefinerThread; definerThreadNo++)
        {
            vector<shared_ptr<zmq::socket_t>> excutorTaskSubSockets;
            for(unsigned short executorThreadNo = 0; executorThreadNo < _noExecutorThreads; executorThreadNo++)
            {
                shared_ptr<zmq::socket_t> taskSenderSocket(new zmq::socket_t(*_serverTransport->getContext(), ZMQ_PUSH));
                taskSenderSocket->connect(TransportUtils::getSinkName(_EXECUTOR_SINK_NAME, executorThreadNo).c_str());
                
                excutorTaskSubSockets.push_back(taskSenderSocket);
            }
            executorTaskSenderSockets.push_back(excutorTaskSubSockets);
        }
    }
    
    return executorTaskSenderSockets;
}

inline shared_ptr<zmq::socket_t>& Store::getResponsibleExecutorTaskSenderSocket(vector<vector<shared_ptr<zmq::socket_t>>>& taskSenderSockets,
                                                                                const unsigned short& threadNo, const unsigned short& executorThreadNo)
{
    
#if INCLUDE_LOCK_STATEMENTS == 1
    if(!_isLockFreeExecutorAssignment)
        return taskSenderSockets[0][executorThreadNo];
    else
#endif
    {    
        return taskSenderSockets[threadNo][executorThreadNo];
    }
}

void Store::run()
{
    if(!_isRunning)
    {
#if INCLUDE_DEFINER_STATEMENTS == 1
        if(!_extraDefinerLayer)
        {
            vector<shared_ptr<zmq::socket_t>> definerSinkVector;
            
            for(unsigned short threadNo = 0; threadNo < _noDefinerThread; threadNo++)
            {
                shared_ptr<zmq::socket_t> taskSenderSocket(new zmq::socket_t(*_serverTransport->getContext(), ZMQ_PUSH));
                taskSenderSocket->connect(TransportUtils::getSinkName(_DEFINER_SINK_NAME, threadNo).c_str());
                
                definerSinkVector.push_back(taskSenderSocket);
            }
            
            _taskSenderSockets.push_back(definerSinkVector);
            
            for(unsigned short threadNo = 0 ; threadNo < _noDefinerThread; threadNo++)
                _definerThreads.push_back(shared_ptr<thread>(new thread(&Store::_definerWorkerThreadFunc, this, threadNo)));

        }
        else
#endif
        {
            _taskSenderSockets = createExecutorTaskSenderSockets();
        }
        
        _serverTransport->start(_storePort);
        
        for(unsigned short threadNo = 0 ; threadNo < _noExecutorThreads; threadNo++)
            _executorThreads.push_back(shared_ptr<thread>(new thread(&Store::_executorWorkerThreadFunc, this, threadNo)));
        
        waitForExecutorsToInitialize();
        _initDB();
        
        _isRunning = true;
    }
}

void Store::_stopWorkerThreads(vector<shared_ptr<thread>> workerThreads)
{
    for(shared_ptr<thread>& workerThread: workerThreads)
    {
        try
        {
            workerThread->join();
        }
        catch(exception& ex)
        {
            FILE_LOG(logWARNING)<<"EXCEPTION WHILE CLEANING UP WORKER THREADS: "<<ex.what();
        }
    }
    workerThreads.clear();
}

void Store::_cleanUp()
{
    _stopFlag = true;
    _stopWorkerThreads(_definerThreads);
    _stopWorkerThreads(_executorThreads);
    _stopFlag = false;
 
    _taskSenderSockets.clear();
    _serverTransport.reset();
}

void Store::stop()
{
    if(_isRunning)
    {
        _cleanUp();
        
        std::unique_lock<std::mutex> lck(_mtx);
        _isRunning = false;
        _cv.notify_all();
    }
    
}

void Store::waitTillNotRunning()
{
    std::unique_lock<std::mutex> lck(_mtx);
    while(_isRunning)
        _cv.wait(lck);
}

void Store::_clearNDb(shared_ptr<Ndb*>& ndb, const int& total)
{
    for(int i = 0; i < total; i++)
        delete ndb.get()[i];
}

Store::~Store()
{
    stop();
    
    _clearNDb(_ndbDefiners, _noDefinerThread);
    _clearNDb(_ndbExecutors, _noExecutorThreads);
}

inline bool Store::shouldIgnoreRequest()
{
    time_t currentTime = time(0);
    
    resetTransactionMonitorCounterIfRequired(currentTime);
    
    double percentFailed = 0;
    int recentTransactionsCount = NDBMessageProcessor::_noRecentTransactions;
    if(recentTransactionsCount > 0)
        percentFailed = NDBMessageProcessor::_noRecentFailedTransactions/recentTransactionsCount;
    
    if(percentFailed > 0)
    {
        int randVal = rand()%100;
        
        //ignore request, TODO: user should get a reponse that it's a temp error and should try again.
        if(randVal < (int)percentFailed)
        {
            FILE_LOG(logINFO)<<"Ignoring request because of failing transactions";
            return true;
        }
    }

    return false;
}

inline void Store::resetTransactionMonitorCounterIfRequired(time_t& currentTime)
{
    if(currentTime > NDBMessageProcessor::_lastFailedTransactionsStartTime + 3)
    {
        NDBMessageProcessor::_lastFailedTransactionsStartTime = currentTime;
        NDBMessageProcessor::_noRecentTransactions = 0;
        NDBMessageProcessor::_noRecentFailedTransactions = 0;
    }
}


#pragma mark - Worker Threads

inline void Store::sendTaskToExecutor(FrameMessagesTupleListPointer& messagesListPointer, shared_ptr<zmq::socket_t>& taskSenderSocket)
{
    size_t size = sizeof(FrameMessagesTupleListPointer*);
    FrameMessagesTupleListPointer* taskListPointerToPass = new FrameMessagesTupleListPointer(messagesListPointer);
    char* rawBuffer = new char[size];
    memcpy(rawBuffer, (void*)&taskListPointerToPass, size);
    
    zmq::message_t serverMessage(reinterpret_cast<void*>(rawBuffer), size, _free_handler_default);

    
#if INCLUDE_LOCK_STATEMENTS == 1
    if(!_isLockFreeExecutorAssignment)
    {
        executorAssignerMutex.lock();
        taskSenderSocket->send(serverMessage);
        executorAssignerMutex.unlock();
    }
    else
#endif
    {
        taskSenderSocket->send(serverMessage);
    }
}

inline void Store::_processMessages(StoreMsgs& messages, vector<vector<shared_ptr<zmq::socket_t>>>& taskSenderSockets, const unsigned short& threadNo)
{
//    if(!shouldIgnoreRequest())
    {
        FrameMessagesTupleListPointer messagesList = FrameDecoder::decode(messages);
        ExecutorTaskMapPointer tasksMapPointer = NDBMessageProcessor::processForExecutor(messagesList, _noExecutorThreads, threadNo);
        
        for(auto& key: *tasksMapPointer)
        {
            shared_ptr<zmq::socket_t>& responsibleExecutorSocket = getResponsibleExecutorTaskSenderSocket(taskSenderSockets, threadNo, key.first);
            sendTaskToExecutor(key.second, responsibleExecutorSocket);
        }
    }
}

void Store::_definerWorkerThreadFunc(unsigned short threadNo)
{
    try
    {
        zmq::socket_t taskReceiverSocket(*_serverTransport->getContext(), ZMQ_PULL);
        taskReceiverSocket.bind(TransportUtils::getSinkName(_DEFINER_SINK_NAME, threadNo).c_str());
        
        vector<vector<shared_ptr<zmq::socket_t>>> taskSenderSockets = createExecutorTaskSenderSockets();
        
//        zmq::socket_t taskSenderSocket(*_serverTransport->getContext(), ZMQ_PUSH);
//        taskSenderSocket.bind(TransportUtils::getSinkName(_EXECUTOR_SINK_NAME, threadNo).c_str());
        
        const int TASK_RECEIVER_SOCKET_INDEX = 0;
        const int TOTAL_POLL_ITEMS = 1;
        const int POLL_WAIT_MS = 10;
        
        zmq::pollitem_t items[] = {
            {taskReceiverSocket, 0, ZMQ_POLLIN, 0},
        };
        
        while(true)
        {
            if(_stopFlag)
                break;
            
            zmq::poll(items, TOTAL_POLL_ITEMS, POLL_WAIT_MS);
            
            if (items[TASK_RECEIVER_SOCKET_INDEX].revents & ZMQ_POLLIN)
            {
                auto messages = TransportUtils::receiveQueuedMessages(taskReceiverSocket);
                _processMessages(messages, taskSenderSockets, threadNo);
            }
        }
    }
    catch(exception& ex)
    {
        FILE_LOG(logERROR)<<"EXCEPTION IN DEFINER WORKER # "<<threadNo<<": "<<ex.what();
    }
}


void Store::_free_handler_default(void* data, void* hint)
{
    delete [] (char*)data;
}


inline void Store::_executeMessages(StoreMsgs& messages, zmq::socket_t& taskSenderSocket, unsigned short threadNo)
{
    NDBMessageExecutor::execute(messages, taskSenderSocket, threadNo);
}

void Store::_executorWorkerThreadFunc(unsigned short threadNo)
{
    try
    {
        initMySQL(&_mysql.get()[threadNo], threadNo);
        
        zmq::socket_t taskReceiverSocket(*_serverTransport->getContext(), ZMQ_PULL);
        taskReceiverSocket.bind(TransportUtils::getSinkName(_EXECUTOR_SINK_NAME, threadNo).c_str());
        
        zmq::socket_t taskSenderSocket(*_serverTransport->getContext(), ZMQ_PUSH);
        taskSenderSocket.connect(TransportUtils::getSinkName(ServerTransport::SERVER_SINK_NAME, threadNo%_noDefinerThread).c_str());

        const int TASK_RECEIVER_SOCKET_INDEX = 0;
        const int TOTAL_POLL_ITEMS = 1;
        const int POLL_WAIT_MS = _storePollWait;
        
        FILE_LOG(logINFO)<<"Initialized executor worker # "<<threadNo;

        addExecutorInitializationCount();
        
        zmq::pollitem_t items[] = {
            {taskReceiverSocket, 0, ZMQ_POLLIN, 0},
        };
        
        while(true)
        {
            if(_stopFlag)
                break;
            
//            zmq::poll(items, TOTAL_POLL_ITEMS, POLL_WAIT_MS);
            
            std::this_thread::sleep_for(chrono::milliseconds(10));
            
//            if (items[TASK_RECEIVER_SOCKET_INDEX].revents & ZMQ_POLLIN)
            {
                auto messages = TransportUtils::receiveQueuedMessages(taskReceiverSocket, _MAX_TRANSACTIONS);
                if(messages.size() > 0)
                {
//                FILE_LOG(logINFO)<<"Received: ";
                    _executeMessages(messages, taskSenderSocket, threadNo);
                }
            }
        }
    }
    catch(exception& ex)
    {
        FILE_LOG(logERROR)<<"EXCEPTION IN EXECUTOR WORKER # "<<threadNo<<": "<<ex.what();
    }
}

#pragma mark - Server Transport Delegate Functions
inline void Store::didReceiveMessage(ServerTransport* serverTransport, TransportUtils::StoreMsgs& messages, const unsigned short& responsibleWorkerThread)
{
#if INCLUDE_DEFINER_STATEMENTS == 1
    if(!_extraDefinerLayer)
    {
        shared_ptr<zmq::socket_t>& responsibleDefinerSocket = _taskSenderSockets[0][responsibleWorkerThread];
        
        for(int i = 0; i < messages.size();i+=2)
        {
            responsibleDefinerSocket->send(*messages[i], ZMQ_SNDMORE);
            responsibleDefinerSocket->send(*messages[i+1]);
        }
    }
    else
#endif
    {
//        static int abc = 0;
//        abc+=messages.size()/2;
//        FILE_LOG(logERROR)<<"Processor: "<<abc;
//        FILE_LOG(logERROR)<<messages.size()/2;
        _processMessages(messages, _taskSenderSockets, responsibleWorkerThread);
    }
}

void Store::initMySQL(MYSQL* mySQL, const int& executorNo)
{
    std::lock_guard<std::mutex> lck (mySQLInitMutex);
//    mySQLInitMutex.lock();
    if (!mysql_init(mySQL))
    {
        FILE_LOG(logERROR)<<"MySQL init failed on executor worker # "<<executorNo <<" :" + string(mysql_error(mySQL));
        //throw runtime_error("MYSQL init faild");
//        exit(EXIT_FAILURE);
    }
    
    if (!mysql_real_connect(mySQL, _mySQLAddress.c_str(), _mySQLUserName.c_str(), _mySQLPassword.c_str(), _database.c_str(), _mySQLPort, 0, 0))
    {
        FILE_LOG(logERROR)<<"MySQL init failed on executor worker # "<<executorNo <<" :" + string(mysql_error(mySQL));
        //throw runtime_error("MYSQL init faild");
//        exit(EXIT_FAILURE);
    }
    
//    mySQLInitMutex.unlock();
}
