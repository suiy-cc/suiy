/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "USB.h"


USB::USB(){}
USB::~USB(){}

USBType USB::getType() const
{
    return type;
}

void USB::setType(const USBType &value)
{
    type = value;
}

USBState USB::getState() const
{
    return state;
}

void USB::setState(const USBState &value)
{
    state = value;
}
