//
//  Utils.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/4/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "Utils.h"
#include "boost/uuid/uuid.hpp"
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>


void Utils::setUnlimitedQueueLength(zmq::socket_t* socket)
{
    int unlimitedQueueLengthFlag = 0;
    socket->setsockopt(ZMQ_RCVHWM, &unlimitedQueueLengthFlag, sizeof(unlimitedQueueLengthFlag));
    socket->setsockopt(ZMQ_SNDHWM, &unlimitedQueueLengthFlag, sizeof(unlimitedQueueLengthFlag));
}

void Utils::setLingerOff(zmq::socket_t* socket)
{
    int time = 0;
    socket->setsockopt(ZMQ_LINGER, &time, sizeof(time));
}

string Utils::getUUID()
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    return boost::uuids::to_string(uuid);
}

string Utils::toLower(string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

bool Utils::toBool(string str)
{
    string lowerCaseStr = toLower(str);
    
    if(lowerCaseStr == "true" || lowerCaseStr == "1" || lowerCaseStr == "yes")
        return true;
    
    return false;
}
