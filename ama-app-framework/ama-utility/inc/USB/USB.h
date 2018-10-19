/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef USB_H
#define USB_H
#include "log.h"
extern LOG_DECLARE_CONTEXT(USBM);
enum USBType{
  E_TYPE_OTG  = 0,
  E_TYPE_HOST = 1
};
enum USBState{
 E_STATE_NORMAL     = 0,
 E_STATE_OVERCHARGE = 1
};

class USB
{
public :
    USB();
    ~USB();
    USBType getType() const;
    void setType(const USBType &value);

    USBState getState() const;
    void setState(const USBState &value);

private :
    USBType type;
    USBState state;
};

#endif // USB_H
