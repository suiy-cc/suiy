/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file KeyValuePairReader.cpp
/// @brief contains class KeyValuePairReader
/// 
/// Created by zs on 2017/05/25.
///
/// this file contains the implementation of class KeyValuePairReader
///

#include <fstream>
#include <iostream>
#include <limits>

#include "COMMON/KeyValuePairReader.h"
#include "COMMON/StringOperation.h"

KeyValuePairReader::KeyValuePairReader()
{}

KeyValuePairReader::KeyValuePairReader(const std::string &path)noexcept
    : path(path)
{
    try
    {
        ParseFile(path);
    }
    catch(std::exception& e)
    {
        std::cout << "KeyValuePairReader::KeyValuePairReader(" << path << "): exception is caught: " << (&e)->what() << std::endl;
    }
    catch(...)
    {
        std::cout << "KeyValuePairReader::KeyValuePairReader(" << path << "): unexpected exception is caught!" << std::endl;
    }
}

void KeyValuePairReader::SetFileName(const std::string &path)
{
    if(this->path!=path)
    {
        this->path = path;
        ParseFile(path);
    }
}

std::string KeyValuePairReader::GetFileName()const
{
    return path;
}

bool KeyValuePairReader::IsValidKey(const std::string &key)const
{
    return keyValueMap.find(key)!=keyValueMap.end();
}

std::string KeyValuePairReader::GetValue(const std::string &key)const
{
    if(IsValidKey(key))
        return keyValueMap.at(key);
    else
        return std::string();
}

std::vector<std::string> KeyValuePairReader::GetAllKeys()const
{
    std::vector<std::string> keys;
    for(const auto& pair: keyValueMap)
        keys.push_back(pair.first);

    return std::move(keys);
}

void KeyValuePairReader::ParseFile(const std::string &filePath)
{
    std::ifstream inFile(filePath.c_str());

    if(inFile.is_open())
    {
        int pairFlag = 0;
        std::string key;
        while(inFile.peek()!=EOF)
        {
            std::string tmp;
            inFile >> tmp;

            // filter comment
            if(IsComment(tmp))
            {
                // ignore the rest of current line if a comment header is found.
                inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            // end of file
            if(tmp.empty())
                break;

            if(pairFlag==0)
            {
                key.swap(tmp); // key = tmp;
                pairFlag = 1;
            }
            else if(pairFlag==1)
            {
                keyValueMap.insert(std::make_pair(key, tmp));
                pairFlag = 0;
            }
            else
                throw std::logic_error("KeyValuePairReader::ParseFile(): impossible branch reached.");
        }

        inFile.close();
    }
}
