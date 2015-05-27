//
//  TableInfoCache.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 3/27/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "TableInfoCache.h"

const NdbDictionary::Table* TableInfoCache::get(/*const string& databaseName, */const string& storeName, Ndb* ndb)
{
//    string fullTableName = _getFullTableName(databaseName, storeName);
    boost::shared_lock< boost::shared_mutex > lock(_access);
    
    decltype(_entries.find(storeName)) foundEntry;
    foundEntry = _entries.find(storeName);
    
    if(foundEntry != _entries.end())
        return foundEntry->second;
    else
        return nullptr;
}

const NdbDictionary::Table* TableInfoCache::set(/*const string& databaseName, */const string& storeName, Ndb* ndb)
{
//    string fullTableName = _getFullTableName(databaseName, storeName);
//    cout<<"Table name "<<fullTableName<<endl;
    
    const NdbDictionary::Table* newTableInfo = _getInternal(storeName, ndb);
    if(newTableInfo)
    {
//        cout<<"Setting up cache"<<endl;
        boost::unique_lock< boost::shared_mutex > lock(_access);
        _entries[storeName] = newTableInfo;
    }
    
    return newTableInfo;
}

const NdbDictionary::Table* TableInfoCache::_getInternal(const string& fullStoreName, Ndb* ndb)
{
    const NdbDictionary::Dictionary* dictionary = ndb->getDictionary();

    if(dictionary)
        return  dictionary->getTable(fullStoreName.c_str());

    return nullptr;
}

void TableInfoCache::invalidate(/*const string& databaseName, */const string& storeName)
{
    boost::unique_lock< boost::shared_mutex > lock(_access);
    _entries.erase(storeName);
}

//string TableInfoCache::_getFullTableName(const string& databaseName, const string& storeName)
//{
//    return databaseName + "." + storeName;
//}

