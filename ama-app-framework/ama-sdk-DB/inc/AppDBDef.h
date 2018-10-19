/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef APPDBDEF_H
#define APPDBDEF_H

#include <string>
#include <map>
#include <memory>

using namespace std;

enum ama_DB_Error_e
{
    E_DB_OK = 0,
    E_DB_UNKNOWN = -1,
    E_DB_NULL_PTR = -2,
    E_DB_KEY_NOT_EXIST = -3,
    E_DB_NOT_EXIST = -4,

    E_DB_MAX
};


typedef std::map<string ,string> constructorMap;
typedef std::map<string ,string> keyValueMap;

#endif//APPDBDEF_H
