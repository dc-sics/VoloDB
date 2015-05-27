//
//  AttributesValueBuilder.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/20/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__ColumnsValueBuilder__
#define __DistributedKeyValueStore__ColumnsValueBuilder__

#include <string>
#include "../ProtoBuf/Column.pb.h"
#include <memory>

using namespace std;

class SetOperation;

class ColumnsValueBuilder
{
private:
    vector<ColumnValue> _columns;
    
    void _addColumnInternal(const string& name, const Column_Type type, const void* value, const size_t& size, bool isPrimaryKey, bool isDistibutedKey);
    shared_ptr<char> _prependLength(const void* data, const uint16_t& length, const int& lengthSize);
    
public:
    void addBoolColumn(const string& name, const bool& value, bool isPrimaryKey = false, bool isDistibutedKey = false);
    void addInt32Column(const string& name, const int32_t& value, bool isPrimaryKey = false, bool isDistibutedKey = false);
    void addUInt32Column(const string& name, const uint32_t& value, bool isPrimaryKey = false, bool isDistibutedKey = false);
    void addInt64Column(const string& name, const int64_t& value, bool isPrimaryKey = false, bool isDistibutedKey = false);
    void addUInt64Column(const string& name, const uint64_t& value, bool isPrimaryKey = false, bool isDistibutedKey = false);
    void addFloatColumn(const string& name, const float& value, bool isPrimaryKey = false, bool isDistibutedKey = false);
    void addDoubleColumn(const string& name, const double& value, bool isPrimaryKey = false, bool isDistibutedKey = false);
    void addCharColumn(const string& name, const char* value, const size_t size, bool isPrimaryKey = false, bool isDistibutedKey = false);
    void addVarCharColumn(const string& name, const string& value, bool isPrimaryKey = false, bool isDistibutedKey = false);
    void addVarBinaryColumn(const string& name, const void* value, const size_t size, bool isPrimaryKey = false, bool isDistibutedKey = false);

    vector<ColumnValue> getColumns();
    
    template<class T>
    static void SetPrimaryKeys(T& operation, vector<ColumnValue> attributeKeys)
    {
        for(ColumnValue keyValue: attributeKeys)
            *operation.add_primary_key() = keyValue;
    }

    template<class T>
    static void SetValues(T& operation, vector<ColumnValue> attributeValues)
    {
        for(ColumnValue attributeValue: attributeValues)
            *operation.add_value() = attributeValue;
    }
    
};


#endif /* defined(__DistributedKeyValueStore__ColumnsValueBuilder__) */
