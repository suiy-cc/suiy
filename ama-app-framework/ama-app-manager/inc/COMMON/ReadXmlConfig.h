/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef READXMLCONFIG_H
#define READXMLCONFIG_H


#include <map>
#include <vector>
#include <string>

enum RXC_Error{
    Error_None                       = 0,
    Error_Read_XML_File_Failed       = 1,//the file is not exist or the file is broken
    Error_Load_XML_File_Failed       = 2,//maybe the file we load is not a xml format file
    Error_No_This_Config_Key         = 10,
};

typedef  std::map <std::string,std::string> XMLConfigMap;       // key-value map
typedef  std::multimap <std::string,std::string> XMLConfigMMap; // key-value map

RXC_Error GetConfigFromMMap(const std::string& configKey, XMLConfigMMap& dataMap);
RXC_Error GetConfigFromMap(const std::string& configKey, XMLConfigMap& dataMap);
RXC_Error GetConfigFromVector(const std::string& configKey, std::vector<std::string>& dataVector);
RXC_Error GetConfigFromString(const std::string& configKey, std::string& dataString );

#endif // READXMLCONFIG_H
