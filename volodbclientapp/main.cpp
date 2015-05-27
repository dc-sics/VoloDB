//
//  main.cpp
//  DistributedKeyValueStoreClient
//
//  Created by Ali Dar on 2/16/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include <iostream>
#include "Logging/log1.h"
#include "boost/program_options.hpp"
#include "Config/ConfigFileReader.h"
#include "Test/TestExecutor.h"


using namespace std;

int main(int argc, const char * argv[])
{
    try
    {
        namespace po = boost::program_options;
        po::options_description description("Options");
        description.add_options()
        ("test-config-path,a", po::value<string>()->default_value("/etc/volodb-client.conf"), "Path to test config file")
        ("instance_no,i", po::value<string>()->default_value("0"), "Instance no of the client")
        ("server_address,s", po::value<string>()->default_value("localhost"), "Address of key value store instance")
        ("server_no,n", po::value<string>()->default_value("0"), "No of key value store instance");
        
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, description), vm);
        
        
        string testConfigFilePath = vm["test-config-path"].as<string>();
        ConfigFileReader testReader;
        testReader.read(testConfigFilePath);

        testReader.setValueForAllSections("instance_no", vm["instance_no"].as<string>());
        testReader.setValueForAllSections("server_address", vm["server_address"].as<string>());
        testReader.setValueForAllSections("server_no", vm["server_no"].as<string>());

        TestExecutor::handle(testReader);
        
    }
    catch(std::exception& e)
    {
        FILE_LOG(logERROR) << "Unhandled Exception reached the top of main: "
        << e.what() << ", application will now exit";
        
        return 1;
    }
    return 0;
}
