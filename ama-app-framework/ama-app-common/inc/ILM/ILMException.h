/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ILMException.h
/// @brief contains class ILMException
///
/// Created by zs on 2016/6/22.
/// this file contains the definition of class ILMException
///

#ifndef ILMEXCEPTION_H
#define ILMEXCEPTION_H

#include <exception>
#include <string>

#include <ILM/HMIType.h>

/// @class ILMException
/// @brief this class carries the error code of ilm functions.
/// throw one if you encounter any ilm error. Do remenber to
/// catch exceptions if you are using classes like Sruface,
/// Layer, Screen, ILMClient.
/// @sa Surface
/// @sa Layer
/// @sa Screen
/// @sa ILMClient
class ILMException
: public std::exception
{
public:
    ILMException();
    explicit ILMException(ErrorType error, const std::string& place);
    ~ILMException();
public:
    virtual const char * what()const noexcept;

    void SetErrorType(ErrorType error);
    ErrorType GetErrorType()const;
    void SetWhere(const std::string& place);
    std::string GetWhere()const;
private:
    ErrorType errorType;
    std::string where;
};


#endif // ILMEXCEPTION_H
