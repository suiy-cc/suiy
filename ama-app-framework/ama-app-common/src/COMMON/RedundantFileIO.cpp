/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file RedundantFileIO.cpp
/// @brief contains 
/// 
/// Created by zs on 2018/08/21.
///

/// this file contains the implementation of class RedundantFileIO
///

#include "COMMON/RedundantFileIO.h"

RedundantFileIO::RedundantFileIO()
{}

RedundantFileIO::RedundantFileIO(const std::string &path)
    : path(path)
    , redundantPath(path+".redundant")
{
    ;
}

RedundantFileIO::RedundantFileIO(const std::string &path, const DeserializeFuncType &deserializeCB, const SerializeFuncType &serializeCB)
    : path(path)
    , redundantPath(path+".redundant")
    , backupPath()
    , deserializeCB(deserializeCB)
    , serializeCB(serializeCB)
{
    ;
}

RedundantFileIO::RedundantFileIO(const std::string &path, const std::string &backupPath, const DeserializeFuncType &deserializeCB, const SerializeFuncType &serializeCB)
    : path(path)
    , redundantPath(path+".redundant")
    , backupPath(backupPath)
    , deserializeCB(deserializeCB)
    , serializeCB(serializeCB)
{}

RedundantFileIO::RedundantFileIO(const std::string &path, const std::string &redundantPath, const std::string &backupPath, const DeserializeFuncType &deserializeCB, const SerializeFuncType &serializeCB)
    : path(path)
    , redundantPath(redundantPath)
    , backupPath(backupPath)
    , deserializeCB(deserializeCB)
    , serializeCB(serializeCB)
{}

RedundantFileIO::~RedundantFileIO()
{

}

void RedundantFileIO::Serialize()
{
    if(serializeCB)
    {
        if(!path.empty())
            serializeCB(path);
        if(!redundantPath.empty())
            serializeCB(redundantPath);
    }
}

void RedundantFileIO::Deserialize()
{
    if(!path.empty() && !deserializeCB(path))
    {
        if(!redundantPath.empty() && !deserializeCB(redundantPath))
        {
            if(!backupPath.empty() && !deserializeCB(backupPath))
                return;
        }
    }
}

const std::string &RedundantFileIO::GetPath() const
{
    return path;
}

void RedundantFileIO::SetPath(const std::string &path)
{
    RedundantFileIO::path = path;
}

const std::string &RedundantFileIO::GetRedundantPath() const
{
    return redundantPath;
}

void RedundantFileIO::SetRedundantPath(const std::string &redundantPath)
{
    RedundantFileIO::redundantPath = redundantPath;
}

const std::string &RedundantFileIO::GetBackupPath() const
{
    return backupPath;
}

void RedundantFileIO::SetBackupPath(const std::string &backupPath)
{
    RedundantFileIO::backupPath = backupPath;
}

const DeserializeFuncType &RedundantFileIO::GetDeserializeCB() const
{
    return deserializeCB;
}
void RedundantFileIO::SetDeserializeCB(const DeserializeFuncType &deserializeCB)
{
    RedundantFileIO::deserializeCB = deserializeCB;
}

const SerializeFuncType &RedundantFileIO::GetSerializeCB() const
{
    return serializeCB;
}

void RedundantFileIO::SetSerializeCB(const SerializeFuncType &serializeCB)
{
    RedundantFileIO::serializeCB = serializeCB;
}
