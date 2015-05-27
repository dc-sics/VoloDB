//
//  main.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/6/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include <stdio.h>
#include "ProtoBuf/DefinitionOperation.pb.h"
#include "ProtoBuf/Column.pb.h"
#include "boost/program_options.hpp"
#include "Config/ConfigFileReader.h"
#include "Logging/log1.h"
#include "Store/Store.h"
#include "Test/TestExecutor.h"
#include "Test/TestStats.h"
#include "Utils/Utils.h"
#include <csignal>


static shared_ptr<Store> store;

static void signal_handler(int signal)
{
    FILE_LOG(logINFO)<<"\n\nShutting down VoloDB\n\n";
    abort();
//    FILE_LOG(logINFO)<<"In signal handler...";
//    if(store)
//    {
//        FILE_LOG(logINFO)<<"Stopping server...";
//        store->stop();
//        std::signal(SIGINT, SIG_DFL);
//    }
}

int main(int argc, const char * argv[])
{
    try
    {
        namespace po = boost::program_options;
        po::options_description description("Options");
        description.add_options()
        ("config-path,c", po::value<string>()->default_value("/etc/volodb.conf"), "Path to config file")
//        ("test-config-path,a", po::value<string>()->default_value("test.cfg"), "Path to test config file")
        ("test,t", "Run in test mode")
        ("test-throughput,p", "Run in test mode with continuous throughput/sec information");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, description), vm);
        
        bool shouldRunTest = vm.count("test")!=0;
        bool shouldRunTestInThrouhputMode = vm.count("test-throughput")!=0;
        
        if(shouldRunTestInThrouhputMode && shouldRunTest)
            shouldRunTest = false;
        
        //run in test mode
//        if(shouldRunTest)
//        {
//            string testConfigFilePath = vm["test-config-path"].as<string>();
//            ConfigFileReader testReader;
//            testReader.read(testConfigFilePath);
//            
//            TestExecutor::handle(testReader);
//        }
//        else
        {
            string configFilePath = vm["config-path"].as<string>();
            ConfigFileReader settingsReader;
            try{
                settingsReader.read(configFilePath);
            }catch(exception&){}
            
            const string SETTINGS_SECTION = "settings";
//single_cluster_connection
            int noDefinerThreads = stoi(settingsReader.getValue(SETTINGS_SECTION, "no_definer_threads", "5"));
            int noExecutorThreads = stoi(settingsReader.getValue(SETTINGS_SECTION, "no_executor_threads", "30"));
            bool singleClusterConnection = Utils::toBool(settingsReader.getValue(SETTINGS_SECTION, "single_cluster_connection", "1"));
            
            int transportPollWait = stoi(settingsReader.getValue(SETTINGS_SECTION, "transport_poll_wait", "10"));
            int storePollWait = stoi(settingsReader.getValue(SETTINGS_SECTION, "store_poll_wait", "10"));
            int storePort = stoi(settingsReader.getValue(SETTINGS_SECTION, "store_port", "5570"));
            string mySQLAddress = settingsReader.getValue(SETTINGS_SECTION, "mysql_server_ip", "localhost");
            unsigned int mySQLPort = static_cast<unsigned int>(stoi(settingsReader.getValue(SETTINGS_SECTION, "mysql_port", "3306")));
            string ndbConnectString = settingsReader.getValue(SETTINGS_SECTION, "ndb_connect_string", "localhost");
            unsigned int ndbPort = static_cast<unsigned int>(stoi(settingsReader.getValue(SETTINGS_SECTION, "ndb_port", "1186")));
            string userName = settingsReader.getValue(SETTINGS_SECTION, "mysql_user_name", "hop");
            string password = settingsReader.getValue(SETTINGS_SECTION, "mysql_password", "hop");
            string database = settingsReader.getValue(SETTINGS_SECTION, "database", "vdb");
            string logLevel = settingsReader.getValue(SETTINGS_SECTION, "log_level", "INFO");
            
            if(shouldRunTest)
            {
                const string TEST_SECTION = "test";
                int testWarmupTime = stoi(settingsReader.getValue(TEST_SECTION, "warmup_time", "4"));
                int testTotalTime = stoi(settingsReader.getValue(TEST_SECTION, "total_time", "5"));
                
                TestStats::init(true, noExecutorThreads, testWarmupTime, testTotalTime);
            }
            else if(shouldRunTestInThrouhputMode)
            {
                TestStats::init(false, noExecutorThreads, 0, 0);
            }
            
            Log::ReportingLevel() = Log::FromString(logLevel);
            
            FILE_LOG(logINFO)<<"Starting Server...";
            
            store = shared_ptr<Store>(new Store(mySQLAddress, mySQLPort, userName, password, ndbConnectString, ndbPort, database,
                                                storePort, noDefinerThreads, noExecutorThreads, singleClusterConnection,
                                                transportPollWait, storePollWait));
            store->run();
            
            std::signal(SIGINT, signal_handler);
            
            FILE_LOG(logINFO)<<"Server Started!!!";
            
            if(shouldRunTestInThrouhputMode)
            TestStats::startContinuous();
            
            store->waitTillNotRunning();
            FILE_LOG(logINFO)<<"Server Stopped!";
            
            store.reset();

        }
    }
    catch(std::exception& e)
    {
        FILE_LOG(logERROR) << "Unhandled Exception reached the top of main: "
        << e.what() << ", application will now exit";
        
        return 1;
    }
    
    return 0;
}