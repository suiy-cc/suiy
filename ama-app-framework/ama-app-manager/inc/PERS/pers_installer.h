/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef _PERS_INSTALLER_H
#define _PERS_INSTALLER_H

#include <string>

#include "persComTypes.h"
#include "persistence_admin_service.h"

class PersInstaller
{
public:
    void setPath(std::string path);
    bool install();

private:
    std::string path_;

};

#endif
