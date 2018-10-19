/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "cs_task.h"
#include "wd.h"

CSTask::CSTask() : keyid_(KEY_NONE), keyEvent_(KEY_EVENT_NONE)
{

}

void CSTask::setKeyEvent(KeyID keyid, KeyEventID keyEvent, void * data)
{
    keyid_ = keyid;
    keyEvent_ = keyEvent;
    keyUsr_ = data;
}

KeyID CSTask::getKeyId() const
{
    return keyid_;
}

KeyEventID CSTask::getKeyEventId() const
{
    return keyEvent_;
}

void CSTask::setPopInfo(const PopInfo & pi)
{
    popInfo_ = pi;
}

PopInfo CSTask::getPopInfo() const
{
    return popInfo_;
}
