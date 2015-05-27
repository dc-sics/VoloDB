//
//  TestExecutor.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/26/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef DistributedKeyValueStore_TestExecutor_h
#define DistributedKeyValueStore_TestExecutor_h

#include "Config/ConfigFileReader.h"

class TestExecutor
{
public:
    TestExecutor() = delete;
    static void handle(ConfigFileReader& testSettings);
    static bool startsWith(const std::string a, const std::string b);
    static bool endsWith(std::string const & value, std::string const & ending);
};


#endif
