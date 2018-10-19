/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file RedundantFileIO.h
/// @brief contains 
/// 
/// Created by zs on 2018/08/21.
///

/// this file contains the definition of class RedundantFileIO
/// 

#ifndef REDUNDANTFILEIO_H
#define REDUNDANTFILEIO_H

#include <string>
#include <functional>

typedef std::function<bool(const std::string&)> DeserializeFuncType;
typedef std::function<void(const std::string&)> SerializeFuncType;

class RedundantFileIO
{
public:
    RedundantFileIO();
    RedundantFileIO(const std::string &path);
    RedundantFileIO(const std::string &path, const DeserializeFuncType &deserializeCB, const SerializeFuncType &serializeCB);
    RedundantFileIO(const std::string &path, const std::string &backupPath, const DeserializeFuncType &deserializeCB, const SerializeFuncType &serializeCB);
    RedundantFileIO(const std::string &path, const std::string &redundantPath, const std::string &backupPath, const DeserializeFuncType &deserializeCB, const SerializeFuncType &serializeCB);
    virtual ~RedundantFileIO();

    void Serialize();
    void Deserialize();

    const std::string &GetPath() const;
    void SetPath(const std::string &path);
    const std::string &GetRedundantPath() const;
    void SetRedundantPath(const std::string &redundantPath);
    const std::string &GetBackupPath() const;
    void SetBackupPath(const std::string &backupPath);
    const DeserializeFuncType &GetDeserializeCB() const;
    void SetDeserializeCB(const DeserializeFuncType &deserializeCB);
    const SerializeFuncType &GetSerializeCB() const;
    void SetSerializeCB(const SerializeFuncType &serializeCB);

private:
    std::string path;
    std::string redundantPath;
    std::string backupPath;

    DeserializeFuncType deserializeCB;
    SerializeFuncType serializeCB;
};


#endif // REDUNDANTFILEIO_H
