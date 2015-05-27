//
//  FrameDecoder.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/5/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__FrameDecoder__
#define __DistributedKeyValueStore__FrameDecoder__

#include <google/protobuf/message.h>
#include <vector>
#include "../Transport/zmq.hpp"
#include <memory>

class NdbTransaction;

using namespace std;

class Frame;
class FrameItem;

using FrameMessage = ::google::protobuf::Message;
using FrameMessagePointer = shared_ptr<FrameMessage>;

using FrameMessages = vector<FrameMessagePointer>;
using FrameMessagesPointer = shared_ptr<FrameMessages>;

using FrameMessagesTuple = tuple<string, zmq::message_t, FrameMessagesPointer>;
using FrameMessagesTuplePointer = shared_ptr<FrameMessagesTuple>;

//using FrameMessagesTupleInt = tuple<int, FrameMessagesPointer>;
//using FrameMessagesTupleIntPointer = shared_ptr<FrameMessagesTuple>;

using FrameMessagesList = vector<FrameMessagesPointer>;
using FrameMessagesListPointer = shared_ptr<FrameMessagesList>;

using FrameMessagesTupleList = vector<FrameMessagesTuplePointer>;
using FrameMessagesTupleListPointer = shared_ptr<FrameMessagesTupleList>;

//using FrameMessagesTupleIntList = vector<FrameMessagesTupleIntPointer>;
//using FrameMessagesTupleIntListPointer = shared_ptr<FrameMessagesTupleIntList>;

using FlattenMessagesList = vector<vector<reference_wrapper<FrameMessage>>>;
using FlattenMessages = vector<reference_wrapper<FrameMessage>>;


class FrameDecoder
{
private:
    template<class T>
    static string getID(Frame& frame, FrameItem& frameItem, T operation);
//    static bool isAtomic(Frame& frame);
public:
    FrameDecoder()=delete;
    static FrameMessagesTupleListPointer decode(zmq::message_t& message, zmq::message_t* identifier);
    static FrameMessagesTupleListPointer decode(vector<shared_ptr<zmq::message_t>>& messages, const bool& containsIdentifier=true);
    
    static FlattenMessagesList flattenFrameMessagesTupleList(FrameMessagesTupleListPointer& frameMessagesTupleListPointer);
    static FlattenMessages flattenFrameMessagesTupleListToSingleList(FrameMessagesTupleListPointer& frameMessagesTupleListPointer);
};

#endif /* defined(__DistributedKeyValueStore__FrameDecoder__) */
