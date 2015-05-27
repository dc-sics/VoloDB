//
//  OperationUtils.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/20/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__OperationUtils__
#define __DistributedKeyValueStore__OperationUtils__

#include "../ProtoBuf/ManipulationOperations.pb.h"

using namespace std;

class OperationUtils
{
public:
    OperationUtils()=delete;    
    static void PopulateKeys(SetOperation& setOperation, vector<ColumnValue> attributeKeys);
    static void PopulateValues(SetOperation& setOperation, vector<ColumnValue> attributeValues);
};

#endif /* defined(__DistributedKeyValueStore__OperationUtils__) */


