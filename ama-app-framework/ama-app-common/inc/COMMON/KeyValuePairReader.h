/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file KeyValuePairReader.h
/// @brief contains class KeyValuePairReader
/// 
/// Created by zs on 2017/05/25.
///
/// this file contains the definition of class KeyValuePairReader
/// 

#ifndef KEYVALUEPAIRREADER_H
#define KEYVALUEPAIRREADER_H

#include <string>
#include <map>
#include <vector>

class KeyValuePairReader
{
public:
    KeyValuePairReader();
    explicit KeyValuePairReader(const std::string &path)noexcept;

    void SetFileName(const std::string& path);
    std::string GetFileName()const;
    bool IsValidKey(const std::string& key)const;
    std::string GetValue(const std::string& key)const;

    std::vector<std::string> GetAllKeys()const;

protected:
    void ParseFile(const std::string &filePath);

private:
    std::string path;
    std::map<std::string, std::string> keyValueMap;
};


#endif // KEYVALUEPAIRREADER_H
