/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "pers_installer.h"

#include <iostream>

#include "log.h"

extern LOG_DECLARE_CONTEXT(CS);

void PersInstaller::setPath(std::string path)
{
    path_ = path;
}

bool PersInstaller::install()
{
    logInfo(CS, " PersInstaller::install()--> exec persAdminResourceConfigAdd()!!");
    auto result = persAdminResourceConfigAdd(path_.c_str()) ;
    std::cout << "\n persAdminResourceConfigAdd " << path_ << " returned << " << path_.c_str() << " >> " << result << std::endl;
    //DLT_LOG(persadminToolDLTCtx, DLT_LOG_INFO, DLT_STRING(LT_HDR), DLT_STRING(g_msg));
    logInfo(CS, "persAdminResourceConfigAdd : ", path_, " returned ", result);
    if (result <= 0) {
        return false;
    }
    return true;
}
