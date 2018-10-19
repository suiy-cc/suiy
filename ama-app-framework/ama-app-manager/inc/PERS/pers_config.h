/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef _PERS_CONFIG_H
#define _PERS_CONFIG_H

#include <string>
#include <vector>
#include <map>

#include "pers_installer.h"

struct PersVersion
{
    int Major;
    int Minor;
    int Revision;
};

struct PersConfigItem
{
    std::string   Name;
    PersVersion   PersVer;
};

class PersConfig
{
public:
    PersConfig(std::string path, std::string filePath);
    bool installPersFiles();

private:
    static PersConfigItem extractFilenameInfo(std::string filename);
    void updateConfig();
    void loadFilenames();
    void loadConfigItems();

    std::string persFilesPath_;
    std::string persConfigPath_;
    std::vector<std::string> allFilePaths_;
    std::map<std::string, PersVersion> persConfigItems_;
    // pers install
    PersInstaller persInstaller_;
};

#endif
