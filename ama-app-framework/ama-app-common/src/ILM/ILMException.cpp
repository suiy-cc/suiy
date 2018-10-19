/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ILMException.cpp
/// @brief contains the implementation of class ILMException
///
/// Created by zs on 2016/6/22.
/// this file contains the implementation of class ILMException
///

#include <mutex>
#include <sstream>

#include "ILM/ILMException.h"

ILMException::ILMException()
: errorType(ILM_SUCCESS)
, where()
{
    ;
}

ILMException::ILMException(ErrorType error, const std::string& place)
: errorType(error)
, where(place)
{
    ;
}

ILMException::~ILMException()
{
    ;
}

const char * ILMException::what()const noexcept
{
    static std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);
    static std::string where = GetWhere();
    static std::string what;
    static std::string errorMessage;
    switch(errorType)
    {
        case ILM_FAILED:                        what = "the method call has failed!";                   break;
        case ILM_ERROR_INVALID_ARGUMENTS:       what = "the method was called with invalid arguments!"; break;
        case ILM_ERROR_ON_CONNECTION:           what = "connection error has occured!";                 break;
        case ILM_ERROR_RESOURCE_ALREADY_INUSE:  what = "resource is already in use!";                   break;
        case ILM_ERROR_RESOURCE_NOT_FOUND:      what = "resource was not found!";                       break;
        case ILM_ERROR_NOT_IMPLEMENTED:         what = "feature is not implemented!";                   break;
        case ILM_ERROR_UNEXPECTED_MESSAGE:      what = "received message has unexpected type!";         break;
        default:
            what = "Illegal Error Type! Error number: ";
            std::stringstream i2s;
            i2s << static_cast<int>(errorType);
            what += i2s.str();
    }

    errorMessage = where + " : " + what;
    return errorMessage.c_str();
}

void ILMException::SetErrorType(ErrorType error)
{
    errorType = error;
}

ErrorType ILMException::GetErrorType()const
{
    return errorType;
}

void ILMException::SetWhere(const std::string& place)
{
    where = place;
}

std::string ILMException::GetWhere()const
{
    return where;
}
