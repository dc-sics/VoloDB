    //
//  FrameDecoder.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/5/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "FrameDecoder.h"
#include "../ProtoBuf/Frame.pb.h"
#include "../ProtoBuf/DefinitionOperation.pb.h"
#include "../ProtoBuf/ManipulationOperations.pb.h"
#include "../ProtoBuf/Result.pb.h"
#include "../Logging/log1.h"
#include <tuple>

template<class T>
string FrameDecoder::getID(Frame& frame, FrameItem& frameItem, T operation)
{
    if(frameItem.has_identifier())
        return frameItem.identifier();
    
    return frame.identifier();
}

//bool FrameDecoder::isAtomic(Frame& frame)
//{
//    if(frame.item_size() <= 1)
//        return true;
//    else
//    {
//        FrameItem firstFrameItem = frame.item(0);
//        FrameItem secondFrameItem = frame.item(1);
//        
//        if(cur)
//    }
//        
//    return false;
//}

FrameMessagesTupleListPointer FrameDecoder::decode(zmq::message_t& message, zmq::message_t* identifier)
{
    FrameMessagesTupleListPointer messagesPointer(new FrameMessagesTupleList());
    
    FrameMessagesTuplePointer messages;
    
    Frame frame;
    
    bool parseResult = frame.ParseFromArray(message.data(), static_cast<int>(message.size()));
    
    if(!parseResult)
    {
        FILE_LOG(logWARNING)<<"Decoding of message failed. Ignoring";
        return messagesPointer;
    }
    
    bool isAtomic = frame.has_identifier();
    
    if(isAtomic)
    {
        messages.reset(new FrameMessagesTuple());
        if(identifier)
            get<1>(*messages).copy(identifier);
        get<2>(*messages) = FrameMessagesPointer(new FrameMessages);
        messagesPointer->push_back(messages);
    }
    
    for(int i = 0; i < frame.item_size(); i++)
    {
        parseResult = false;
        string id;
        FrameMessage* currentMessage = nullptr;
        if(!isAtomic)
        {
            messages.reset(new FrameMessagesTuple());
            if(identifier)
                get<1>(*messages).copy(identifier);
            get<2>(*messages) = FrameMessagesPointer(new FrameMessages);
            
            messagesPointer->push_back(messages);
        }
        
        FrameItem currentFrameItem = frame.item(i);
        
        if(currentFrameItem.type() == FrameItem_Type_CREATE_STORE)
        {
            CreateStoreOperation* createStoreOperation = new CreateStoreOperation();
            parseResult = createStoreOperation->ParseFromString(currentFrameItem.data());//TODO:check return type
            
            currentMessage = createStoreOperation;
        }
        else if(currentFrameItem.type() == FrameItem_Type_DROP_STORE)
        {
            DropStoreOperation* dropStoreOperation = new DropStoreOperation();
            parseResult = dropStoreOperation->ParseFromString(currentFrameItem.data());
            
            currentMessage = dropStoreOperation;
        }
        else if(currentFrameItem.type() == FrameItem_Type_SERVER_RESPONSE)
        {
            Result* serverResponse = new Result();
            parseResult = serverResponse->ParseFromString(currentFrameItem.data());
            
            currentMessage = serverResponse;
        }
        else if(currentFrameItem.type() == FrameItem_Type_SET)
        {
            SetOperation* setOperation = new SetOperation();
            parseResult = setOperation->ParseFromString(currentFrameItem.data());
            
            currentMessage = setOperation;
        }
        
        else if(currentFrameItem.type() == FrameItem_Type_DELETE)
        {
            DeleteOperation* deleteOperation = new DeleteOperation();
            parseResult = deleteOperation->ParseFromString(currentFrameItem.data());
            
            currentMessage = deleteOperation;
        }
        else if(currentFrameItem.type() == FrameItem_Type_GET)
        {
            GetOperation* getOperation = new GetOperation();
            parseResult = getOperation->ParseFromString(currentFrameItem.data());
            
            currentMessage = getOperation;
        }

        if(!parseResult)
        {
            FILE_LOG(logWARNING)<<"Unknown message received. Ignoring";
            continue;
        }
        
        get<2>(*messages)->push_back(shared_ptr<FrameMessage>(currentMessage));
        
        if(isAtomic) 
            id = frame.identifier();
        else
            get<0>(*messages) = currentFrameItem.identifier();
    }
    
    if(isAtomic)
        get<0>(*messages) = frame.identifier();
    
    return messagesPointer;
}


FrameMessagesTupleListPointer FrameDecoder::decode(vector<shared_ptr<zmq::message_t>>& messages, const bool& containsIdentifier)
{
    FrameMessagesTupleListPointer decodedMessages(new FrameMessagesTupleList());
    
    int incrementAmount = 1;
    
    if(containsIdentifier)
        incrementAmount = 2;
    
    for(int i = 0; i < messages.size(); i+=incrementAmount)
    {
        FrameMessagesTupleListPointer decodedCurrentMessage;
        if(containsIdentifier)
            decodedCurrentMessage = decode(*messages[i+1], &(*messages[i]));
        else
            decodedCurrentMessage = decode(*messages[i], nullptr);
        
        decodedMessages->insert( decodedMessages->end(), decodedCurrentMessage->begin(), decodedCurrentMessage->end() );
    }
    
    return decodedMessages;
}


FlattenMessagesList FrameDecoder::flattenFrameMessagesTupleList(FrameMessagesTupleListPointer& frameMessagesTupleListPointer)
{
    FlattenMessagesList frameMessages;
    
    for(FrameMessagesTuplePointer messagesTuplePointer : *frameMessagesTupleListPointer)
    {
        FrameMessagesTuple& messagesTuple = *messagesTuplePointer;
        FrameMessages& messages = *(get<2>(messagesTuple));
        
        vector<reference_wrapper<FrameMessage>> messageVector;
        
        for(auto messagePointer: messages)
            messageVector.push_back(*messagePointer);
        
        frameMessages.push_back(messageVector);
    }
    
    return frameMessages;
}

FlattenMessages FrameDecoder::flattenFrameMessagesTupleListToSingleList(FrameMessagesTupleListPointer& frameMessagesTupleListPointer)
{
    FlattenMessages frameMessages;
    
    for(FrameMessagesTuplePointer messagesTuplePointer : *frameMessagesTupleListPointer)
    {
        FrameMessagesTuple& messagesTuple = *messagesTuplePointer;
        FrameMessages& messages = *(get<2>(messagesTuple));
        
        if(messages.size() > 0)
            frameMessages.push_back(*messages[0]);
    }
    
    return frameMessages;
}

