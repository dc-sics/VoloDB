//
//  FrameUtils.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/5/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "FrameUtils.h"
#include <string>

using namespace std;

FrameItem_Type FrameUtils::getFrameType(::google::protobuf::Message& message)
{
    string typeName = message.GetTypeName();

    if(typeName == "CreateDatabaseOperation")
        return FrameItem_Type_CREATE_DATABASE;
    else if(typeName == "DropDatabaseOperation")
        return FrameItem_Type_DROP_DATABASE;
    else if(typeName == "CreateStoreOperation")
        return FrameItem_Type_CREATE_STORE;
    else if(typeName == "DropStoreOperation")
        return FrameItem_Type_DROP_STORE;
    else if(typeName == "SetOperation")
        return FrameItem_Type_SET;
    else if(typeName == "DeleteOperation")
        return FrameItem_Type_DELETE;
    else if(typeName == "GetOperation")
        return FrameItem_Type_GET;
    else if(typeName == "Result")
        return FrameItem_Type_SERVER_RESPONSE;

    return FrameItem_Type_NONE;
}

bool FrameUtils::isDefinitionFrame(::google::protobuf::Message& message)
{
    FrameItem_Type type = getFrameType(message);
    
    return ( (type == FrameItem_Type_CREATE_DATABASE) || (type == FrameItem_Type_DROP_DATABASE) ||
            (type == FrameItem_Type_CREATE_STORE) || (type == FrameItem_Type_DROP_STORE) );
}