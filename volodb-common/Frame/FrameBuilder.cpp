//
//  FrameBuilder.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/5/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "FrameBuilder.h"
#include "FrameUtils.h"
#include "../Utils/Utils.h"
#include "../Logging/log1.h"
#include <stdexcept>

FrameBuilder::FrameBuilder(bool isAtomic, const string& uuid)
{
    if(isAtomic)
        _frame.set_identifier(uuid);
}

string FrameBuilder::getIdentifier()
{
    return _frame.identifier();
}

string FrameBuilder::addFrame(::google::protobuf::Message& message, const string& uuid)
{
    FrameItem_Type messageType = FrameUtils::getFrameType(message);
    
    if(messageType == FrameItem_Type_NONE)
        throw runtime_error(message.GetTypeName() + " message is not supported to be framed");

    string frameItemIdentifier;
    FrameItem* frameItem = _frame.add_item();
    
    if(!_frame.has_identifier())
    {
        frameItemIdentifier = uuid;
        frameItem->set_identifier(frameItemIdentifier);
    }

    string frameData;
    message.SerializeToString(&frameData);
    frameItem->set_type(messageType);
    frameItem->set_data(frameData);
    
    return frameItemIdentifier;
}

bool FrameBuilder::getBytes(string& bytes)
{
    return _frame.SerializeToString(&bytes);
}

bool FrameBuilder::getBytesOfFrameWithMessage(::google::protobuf::Message& message, string& bytes, string* identifier)
{
    FrameBuilder frameBuilder;
    
    frameBuilder.addFrame(message);
    if(identifier)
        *identifier = frameBuilder.getIdentifier();
    
    return frameBuilder.getBytes(bytes);
}

