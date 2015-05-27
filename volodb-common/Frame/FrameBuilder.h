//
//  FrameBuilder.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/5/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__FrameBuilder__
#define __DistributedKeyValueStore__FrameBuilder__

#include <vector>
#include <string>
#include <google/protobuf/message.h>
#include "../ProtoBuf/Frame.pb.h"

class Frame;

using namespace std;

class FrameBuilder
{
private:
    Frame _frame;
    
public:
    FrameBuilder(bool isAtomic=false, const string& uuid="");
    string addFrame(::google::protobuf::Message& message, const string& uuid="");
    bool getBytes(string& bytes);
    
    static bool getBytesOfFrameWithMessage(::google::protobuf::Message& message, string& bytes, string* identifier);
    
    string getIdentifier();
};

#endif /* defined(__DistributedKeyValueStore__FrameBuilder__) */
