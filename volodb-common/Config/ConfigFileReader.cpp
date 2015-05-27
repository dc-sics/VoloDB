//
//  ConfigFileReader.cpp
//  DistributedKeyValueStore
//
//  Created by Ali Dar on 2/26/15.
//  Copyright (c) 2015 Ali Dar. All rights reserved.
//

#include "ConfigFileReader.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <iostream>

#include <string>

map<string, map<string, string>>& ConfigFileReader::getSettings()
{
    return _settings;
}

void ConfigFileReader::setValueForAllSections(string attribute, string value)
{
    for(auto& section: _settings)
        section.second[attribute] = value;
}

void ConfigFileReader::read(string path)
{
    using boost::property_tree::ptree;
    ptree pt;
    
    read_ini(path, pt);
    
    _settings.clear();
    
    for (auto& section : pt)
    {
        map<string, string> currentSection;

        for (auto& key : section.second)
        {
            currentSection[key.first] = key.second.get_value<std::string>();
        }
        
        _settings[section.first] = currentSection;
    }
}

string ConfigFileReader::getValue(string section, string attribute)
{
    return _settings.at(section).at(attribute);
}


map<string, string> ConfigFileReader::getSectionMap(string section)
{
    return _settings.at(section);
}

void ConfigFileReader::print()
{
    for (auto& section : _settings)
    {
        std::cout << '[' << section.first << "]\n";
        for (auto& key : section.second)
            std::cout << key.first << "=" << key.second << "\n";
    }
}

int ConfigFileReader::getValueInt(string section, string attribute, int defaultValue)
{
    try
    {
        return stoi(getValue(section, attribute));
    }
    catch(exception& ex)
    {
        return defaultValue;
    }
}

string ConfigFileReader::getValue(string section, string attribute, string defaultValue)
{
    try
    {
        return getValue(section, attribute);
    }
    catch(exception& ex)
    {
        return defaultValue;
    }
}