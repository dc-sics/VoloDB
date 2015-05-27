//
//  FrameUtils.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/5/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__FrameUtils__
#define __DistributedKeyValueStore__FrameUtils__

#include <google/protobuf/message.h>
#include "../ProtoBuf/Frame.pb.h"

class FrameUtils
{
public:
    FrameUtils()=delete;
    
    static FrameItem_Type getFrameType(::google::protobuf::Message& message);
    static bool isDefinitionFrame(::google::protobuf::Message& message);
};

#endif /* defined(__DistributedKeyValueStore__FrameUtils__) */
