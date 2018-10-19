/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef USB_OTG_H
#define USB_OTG_H
#include "USB.h"

enum ChargeMode{
  E_MODE_DCP  = 0, //charge only
  E_MODE_CDP  = 1  //charge and read disk
};

class USB_OTG :public USB
{
public :
    USB_OTG();
    ~USB_OTG();
    bool setFastCharge();
    bool getFastCharge();
    bool getHavChargeDev() const;
    void setHavChargeDev(bool value);

private :
    bool havChargeDev=false;
    ChargeMode chargeMode=E_MODE_DCP;
};

#endif // USB_OTG_H

