//
//  OperationUtils.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/20/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "OperationUtils.h"

void OperationUtils::PopulateKeys(SetOperation& setOperation, vector<ColumnValue> attributeKeys)
{
    for(ColumnValue keyValue: attributeKeys)
        *setOperation.add_primary_key() = keyValue;
}

void OperationUtils::PopulateValues(SetOperation& setOperation, vector<ColumnValue> attributeValues)
{
    for(ColumnValue attributeValue: attributeValues)
        *setOperation.add_value() = attributeValue;
}