//
//  ColumnsValueDecoder.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 5/1/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "ColumnsValueDecoder.h"
#include "../Utils/Utils.h"

int ColumnsValueDecoder::getLength(const ColumnValue& columnValue)
{
    return ((int) ((unsigned char) columnValue.value().data()[0]) + ((int) ((unsigned char) columnValue.value().data()[1]) << 8));
}

bool ColumnsValueDecoder::getBool(const ColumnValue& columnValue)
{
    return *((bool*)columnValue.value().data());
}

int32_t ColumnsValueDecoder::getInt32(const ColumnValue& columnValue)
{
    return *((int32_t*)columnValue.value().data());
}

uint32_t ColumnsValueDecoder::getUInt32(const ColumnValue& columnValue)
{
    return *((uint32_t*)columnValue.value().data());
}

int64_t ColumnsValueDecoder::getInt64(const ColumnValue& columnValue)
{
    return *((int64_t*)columnValue.value().data());
}

uint64_t ColumnsValueDecoder::getUInt64(const ColumnValue& columnValue)
{
    return *((uint64_t*)columnValue.value().data());
}

float ColumnsValueDecoder::getFloat(const ColumnValue& columnValue)
{
    return *((float*)columnValue.value().data());
}

double ColumnsValueDecoder::getDouble(const ColumnValue& columnValue)
{
    return *((double*)columnValue.value().data());
}

shared_ptr<char> ColumnsValueDecoder::getChar(const ColumnValue& columnValue)
{
    shared_ptr<char> charValue(new char[columnValue.value().size()], ArrayDeleter<char>());
    memset(charValue.get(), 0, columnValue.value().size());
    
    memcpy(charValue.get(), columnValue.value().data(), columnValue.value().size());
    
    return charValue;
}

string ColumnsValueDecoder::getVarChar(const ColumnValue& columnValue)
{
    int length = getLength(columnValue) + 1;
    
    return string(columnValue.value().data() + 2, length);
}

string ColumnsValueDecoder::getVarBinary(const ColumnValue& columnValue)
{
    return getVarChar(columnValue);
}

