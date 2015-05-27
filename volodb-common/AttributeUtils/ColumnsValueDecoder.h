//
//  ColumnsValueDecoder.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 5/1/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__ColumnsValueDecoder__
#define __DistributedKeyValueStore__ColumnsValueDecoder__

#include "../ProtoBuf/Column.pb.h"
#include <memory>

using namespace std;

class ColumnsValueDecoder
{
private:
    static int getLength(const ColumnValue& columnValue);
public:
    ColumnsValueDecoder()=delete;
    static bool getBool(const ColumnValue& columnValue);
    static int32_t getInt32(const ColumnValue& columnValue);
    static uint32_t getUInt32(const ColumnValue& columnValue);
    static int64_t getInt64(const ColumnValue& columnValue);
    static uint64_t getUInt64(const ColumnValue& columnValue);
    static float getFloat(const ColumnValue& columnValue);
    static double getDouble(const ColumnValue& columnValue);
    static shared_ptr<char> getChar(const ColumnValue& columnValue);
    static string getVarChar(const ColumnValue& columnValue);
    static string getVarBinary(const ColumnValue& columnValue);
};

#endif /* defined(__DistributedKeyValueStore__ColumnsValueDecoder__) */
