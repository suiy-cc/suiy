/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Input.cpp
/// @brief contains class InputPeripheral
///
/// Created by zs on 2017/10/27.
///
/// this file contains the implementation of class Input
///

#include <ilm_types.h>
#include <ilm_input.h>
#include <ilm_control.h>
#include <log.h>

#include "ILM/ILMClient.h"
#include "ILM/ILMException.h"
#include "ILM/InputPeripheral.h"
#include "COMMON/Broadcaster.h"

//Be sure to put it at the end of statement
#include "ama_mem.h"

extern LOG_DECLARE_CONTEXT(ILMA);

InputPeripheral::InputPeripheral(const std::string &name)
    : deviceName(name)
{}

InputPeripheral::InputPeripheral()
{}

InputPeripheral::~InputPeripheral()
{

}

E_INPUTPERIPHERAL_TYPE InputPeripheral::GetType() const
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%s)", deviceName.c_str()));

    ilmInputDevice device = 0;
    auto name = deviceName.c_str();
    char* mutableName = (char*)ama_malloc(strlen(name)+1);
    strncpy(mutableName, name, strlen(name)+1);
    auto errorTypes = ILM_SUCCESS;
#ifndef CREATE_LAYERS_BY_AMGR
    errorTypes = ilm_getInputDeviceCapabilities(mutableName, &device);
#endif
    ama_free(mutableName);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, std::string("InputPeripheral::GetType(")+deviceName+")");
        throw e;
    }

    return static_cast<E_INPUTPERIPHERAL_TYPE>(device);
}

const std::string &InputPeripheral::GetName() const
{
    return deviceName;
}

void InputPeripheral::SetName(const std::string &name)
{
    deviceName = name;
}

std::vector<InputPeripheral> InputPeripheral::GetInputPeripheral(E_INPUTPERIPHERAL_TYPE type)
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%d)", type));

    std::vector<InputPeripheral> inputs;

    unsigned int deviceCount = 0;
    char** deviceNames = nullptr;
    auto errorTypes = ILM_SUCCESS;
#ifndef CREATE_LAYERS_BY_AMGR
    errorTypes = ilm_getInputDevices(type, &deviceCount, &deviceNames);
#endif
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, std::string("GetInputPeripheral(")+std::to_string(type)+")");
        throw e;
    }

    // copy outcome into inputs array
    for(int i = 0; i < deviceCount; ++i)
    {
        if(deviceNames && deviceNames[i])
        {
            inputs.push_back(InputPeripheral(deviceNames[i]));
            ama_free(deviceNames[i]);
        }
    }
    ama_free(deviceNames);

    return std::move(inputs);
}

Broadcaster<void(const std::string&, bool)> inputPeripheralChangedCBKeeper;
static void OnInputPeripheralChanged(const char* deviceName, t_ilm_bool isCreate, void*)
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%s, %s)", deviceName, isCreate?"true":"false"));

    inputPeripheralChangedCBKeeper.NotifyAll([deviceName, isCreate](unsigned int handle, InputPeripheral::InputPeripheralChangedCallback callback){
        DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%s, %s)", deviceName, isCreate?"true":"false"));

        callback(deviceName, isCreate);
    });
}

unsigned int InputPeripheral::AddInputPeripheralChangedCallback(const InputPeripheralChangedCallback &callback)
{
    FUNCTION_SCOPE_LOG_C(ILMA);

    if(!ILMClient::isInputPeripheralChangedCBRegistered)
    {
        ilmErrorTypes errorTypes = ILM_SUCCESS;
#ifndef CREATE_LAYERS_BY_AMGR
        errorTypes = ilm_seatAddNotification(OnInputPeripheralChanged, nullptr);
#endif
        if(errorTypes!=ILM_SUCCESS)
        {
            ILMException e(errorTypes, "InputPeripheral::AddInputPeripheralChangedCallback()");
            throw e;
        }
        ILMClient::isInputPeripheralChangedCBRegistered = true;
    }

    return inputPeripheralChangedCBKeeper.Register(callback);
}

void InputPeripheral::DeleteInputPeripheralChangedCallback(unsigned int callbackHandle)
{
    inputPeripheralChangedCBKeeper.Unregister(callbackHandle);
}
