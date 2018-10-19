/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file InputPeripheral.h
/// @brief contains input device contorl API
/// 
/// Created by zs on 2017/10/27.
///
/// this file contains the definition of class Input
/// 

#ifndef INPUTPERIPHERAL_H
#define INPUTPERIPHERAL_H

#include <string>
#include <vector>

enum E_INPUTPERIPHERAL_TYPE{
    E_INPUTPERIPHERAL_TYPE_INVALIDE = 0,
    E_INPUTPERIPHERAL_TYPE_KEYBOARD = 1,
    E_INPUTPERIPHERAL_TYPE_POINTER = 2,
    E_INPUTPERIPHERAL_TYPE_TOUCH = 4,
    E_INPUTPERIPHERAL_TYPE_ALL = ~0,
};

class InputPeripheral
{
public:
    InputPeripheral(const std::string &name);
    InputPeripheral();
    virtual ~InputPeripheral();

    E_INPUTPERIPHERAL_TYPE GetType()const;
    const std::string& GetName()const;
    void SetName(const std::string& name);

    static std::vector<InputPeripheral> GetInputPeripheral(E_INPUTPERIPHERAL_TYPE type);
    typedef std::function<void(const std::string&, bool)> InputPeripheralChangedCallback;
    static unsigned int AddInputPeripheralChangedCallback(const InputPeripheralChangedCallback &callback);
    static void DeleteInputPeripheralChangedCallback(unsigned int callbackHandle);

private:
    std::string deviceName;
};

#endif // INPUTPERIPHERAL_H
