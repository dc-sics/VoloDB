//
//  Cache.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/27/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef DistributedKeyValueStore_TableInfo_h
#define DistributedKeyValueStore_TableInfo_h

#include <unordered_map>
#include "NdbApi.hpp"
#include <boost/thread/shared_mutex.hpp>

using namespace std;

class TableInfoCache
{
private:
    boost::shared_mutex _access;
    unordered_map<string, const NdbDictionary::Table*> _entries;

    inline const NdbDictionary::Table* _getInternal(const string& fullStoreName, Ndb* ndb);    
    
//    inline string _getFullTableName(const string& databaseName, const string& storeName);
    
public:
    
    const NdbDictionary::Table* get(/*const string& databaseName, */const string& storeName, Ndb* ndb);
    
    const NdbDictionary::Table* set(/*const string& databaseName, */const string& storeName, Ndb* ndb);
    void invalidate(/*const string& databaseName, */const string& storeName);
};

#endif