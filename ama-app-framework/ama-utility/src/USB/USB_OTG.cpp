/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "USB_OTG.h"


USB_OTG::USB_OTG(){}
USB_OTG::~USB_OTG(){}

bool USB_OTG::getHavChargeDev() const
{
    return havChargeDev;
}

void USB_OTG::setHavChargeDev(bool value)
{
    havChargeDev = value;
}
