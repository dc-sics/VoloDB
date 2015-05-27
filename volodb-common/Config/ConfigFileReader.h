//
//  ConfigFileReader.h
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/26/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#ifndef __DistributedKeyValueStore__ConfigFileReader__
#define __DistributedKeyValueStore__ConfigFileReader__

#include <stdio.h>
#include <map>
#include <string>
using namespace std;

class ConfigFileReader
{
    
private:
    map<string, map<string, string>> _settings;
    
public:
    void read(string path);
    string getValue(string section, string attribute);
    map<string, string> getSectionMap(string section);
    string getValue(string section, string attribute, string defaultValue);
    int getValueInt(string section, string attribute, int defaultValue);
    
    void setValueForAllSections(string attribute, string value);
    
    void print();
    
    
    map<string, map<string, string>>& getSettings();
    
};

#endif /* defined(__DistributedKeyValueStore__ConfigFileReader__) */
