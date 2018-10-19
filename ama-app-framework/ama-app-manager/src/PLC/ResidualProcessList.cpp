/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ResidualProcessList.cpp
/// @brief contains the implementation of class ResidualProcessList
///
/// Created by zs on 2016/10/11.
/// this file contains the implementation of class ResidualProcessList
///


#include "ResidualProcessList.h"
#include "log.h"
#include "AMGRLogContext.h"

#include <fstream>
#include <algorithm>

const std::string ResidualProcessList::filePath = "/tmp/ama-app-manager-residual-process-list.txt";

void ResidualProcessList::Save(const std::string &unitName)
{
    logVerbose(PLC, "ResidualProcessList::Save(", unitName, ")--->IN");

    std::ofstream file(filePath.c_str(), std::ios_base::app);
    file << unitName << std::endl;
    file.close();

    logVerbose(PLC, "ResidualProcessList::Save(", unitName, ")--->OUT");
}

void ResidualProcessList::Delete(const std::string &unitName)
{
    logVerbose(PLC, "ResidualProcessList::Delete(", unitName, ")--->IN");

    std::vector<std::string> records;

    // read records
    records = ReadRecords();

    // delete specified record
    auto iter = std::find(records.begin(), records.end(), unitName);
    while(iter!=records.end())
    {
        records.erase(iter);
        iter = std::find(records.begin(), records.end(), unitName);
    }
    logInfo(PLC, "ResidualProcessList::Delete(): specified record is removed.");

    // write record back to the file
    std::ofstream ofile(filePath.c_str());
    for(auto& record: records)
    {
        ofile << record << std::endl;
        logInfo(PLC, "ResidualProcessList::Delete(): record ", record, " has been writen to file.");
    }

    logInfo(PLC, "ResidualProcessList::Delete(): .");
    ofile.close();

    logVerbose(PLC, "ResidualProcessList::Delete(", unitName, ")--->OUT");
}

std::vector<std::string> ResidualProcessList::ReadRecords()
{
    logVerbose(PLC, "ResidualProcessList::ReadRecords()--->IN");
    std::vector<std::string> records;

    // read records
    std::ifstream ifile(filePath.c_str());
    while(ifile.is_open() && ifile.peek()!=EOF)
    {
        std::string record;
        ifile >> record;
        records.push_back(std::move(record));
    }
    ifile.close();

    logVerbose(PLC, "ResidualProcessList::ReadRecords()--->OUT");
    return std::move(records);
}
