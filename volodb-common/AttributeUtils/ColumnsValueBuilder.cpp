//
//  AttributesValueBuilder.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/20/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "ColumnsValueBuilder.h"
#include "../ProtoBuf/ManipulationOperations.pb.h"
#include "../Utils/Utils.h"


static const int EXTENDED_ATTIBUTE_LENGTH_SIZE_FOR_PREPEND = 2;

shared_ptr<char> ColumnsValueBuilder::_prependLength(const void* data, const uint16_t& length, const int& lengthSize)
{
    shared_ptr<char> dataWithLength(new char[lengthSize + length], ArrayDeleter<char>());
    
    if(lengthSize == 1)
    {
        dataWithLength.get()[0]=(char) length;
    }
    else
    {
        dataWithLength.get()[0]=(char) ((length & 0x00FF))>>0;
        dataWithLength.get()[1]= (char)((length & 0xFF00)>>8);
    }
    
    memcpy(dataWithLength.get() + lengthSize, data, length);
    
    return dataWithLength;
}

void ColumnsValueBuilder::_addColumnInternal(const string& name, const Column_Type type, const void* value, const size_t& size, bool isPrimaryKey, bool isDistibutedKey)
{
    ColumnValue column;

    column.set_name(name);
    column.set_type(type);
    column.set_primary_key(isPrimaryKey);
    column.set_distribution_key(isDistibutedKey);
    column.set_value(value, size);
    
    _columns.push_back(column);
}

void ColumnsValueBuilder::addBoolColumn(const string& name, const bool& value, bool isPrimaryKey, bool isDistibutedKey)
{
    _addColumnInternal(name, Column_Type_BOOL, (const char*)&value, sizeof(value), isPrimaryKey, isDistibutedKey);
}

void ColumnsValueBuilder::addInt32Column(const string& name, const int32_t& value, bool isPrimaryKey, bool isDistibutedKey)
{
    _addColumnInternal(name, Column_Type_INT32, (const char*)&value, sizeof(value), isPrimaryKey, isDistibutedKey);
}

void ColumnsValueBuilder::addUInt32Column(const string& name, const uint32_t& value, bool isPrimaryKey, bool isDistibutedKey)
{
    _addColumnInternal(name, Column_Type_UINT32, (const char*)&value, sizeof(value), isPrimaryKey, isDistibutedKey);
}

void ColumnsValueBuilder::addInt64Column(const string& name, const int64_t& value, bool isPrimaryKey, bool isDistibutedKey)
{
    _addColumnInternal(name, Column_Type_INT64, (const char*)&value, sizeof(value), isPrimaryKey, isDistibutedKey);
}

void ColumnsValueBuilder::addUInt64Column(const string& name, const uint64_t& value, bool isPrimaryKey, bool isDistibutedKey)
{
    _addColumnInternal(name, Column_Type_UINT64, (const char*)&value, sizeof(value), isPrimaryKey, isDistibutedKey);
}

void ColumnsValueBuilder::addFloatColumn(const string& name, const float& value, bool isPrimaryKey, bool isDistibutedKey)
{
    _addColumnInternal(name, Column_Type_FLOAT, (const char*)&value, sizeof(value), isPrimaryKey, isDistibutedKey);
}

void ColumnsValueBuilder::addDoubleColumn(const string& name, const double& value, bool isPrimaryKey, bool isDistibutedKey)
{
    _addColumnInternal(name, Column_Type_DOUBLE, (const char*)&value, sizeof(value), isPrimaryKey, isDistibutedKey);
}

void ColumnsValueBuilder::addCharColumn(const string& name, const char* value, const size_t size, bool isPrimaryKey, bool isDistibutedKey)
{
    char newSpacePaddedValue[size];
    
    memset(newSpacePaddedValue,32,size);
    memcpy(newSpacePaddedValue, value, strlen(value));

    _addColumnInternal(name, Column_Type_CHAR, newSpacePaddedValue, size, isPrimaryKey, isDistibutedKey);
}

void ColumnsValueBuilder::addVarCharColumn(const string& name, const string& value, bool isPrimaryKey, bool isDistibutedKey)
{
    shared_ptr<char> dataWithLength = _prependLength(value.data(), value.size(), EXTENDED_ATTIBUTE_LENGTH_SIZE_FOR_PREPEND);
    _addColumnInternal(name, Column_Type_VARCHAR, (const char*)dataWithLength.get(), value.size() + EXTENDED_ATTIBUTE_LENGTH_SIZE_FOR_PREPEND, isPrimaryKey, isDistibutedKey);
}

void ColumnsValueBuilder::addVarBinaryColumn(const string& name, const void* value, const size_t size, bool isPrimaryKey, bool isDistibutedKey)
{
    shared_ptr<char> dataWithLength = _prependLength(value, size, EXTENDED_ATTIBUTE_LENGTH_SIZE_FOR_PREPEND);
    _addColumnInternal(name, Column_Type_VARBINARY, (const char*)dataWithLength.get(), size + EXTENDED_ATTIBUTE_LENGTH_SIZE_FOR_PREPEND, isPrimaryKey, isDistibutedKey);
}

vector<ColumnValue> ColumnsValueBuilder::getColumns()
{
    return _columns;
}

#pragma mark - Helping functions to populate operations
//template<class T>
//void ColumnsValueBuilder::SetPrimaryKeys(T& operation, vector<ColumnValue> attributeKeys)
//{
//    for(ColumnValue keyValue: attributeKeys)
//        *operation.add_primary_key() = keyValue;
//}

//void ColumnsValueBuilder::SetValues(SetOperation& setOperation, vector<ColumnValue> attributeValues)
//{
//    for(ColumnValue attributeValue: attributeValues)
//        *setOperation.add_value() = attributeValue;
//}