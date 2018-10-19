/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file BootArguments.cpp
/// @brief contains the implementation of function GetBootArguments
///
/// Created by zs on 2016/8/10.
/// this file contains the implementation of function GetBootArguments
///

#include <sstream>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <fstream>
#include <iostream>

#include "StringOperation.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

const std::string system(const std::string& cmd)
{
    char* outcome = nullptr;
    static const unsigned int minBufferSize = 100;
    unsigned int bufferSize = minBufferSize;
    unsigned int totalBytes = 0;

    // execute cmd, store the output into pipe
    FILE* pipe = popen(cmd.c_str(), "r");

    // write output to outcome
    int readBytes = 0;
    do
    {
        bufferSize *= 2;
        char *buf = new char[bufferSize];
        memset(buf, '\0', sizeof(char)*bufferSize);
        readBytes = fread(buf, sizeof(char), bufferSize, pipe);
        totalBytes += readBytes;

        if(totalBytes==0)
            break;

        if(!outcome)
        {
            outcome = new char[readBytes];
            memcpy(outcome, buf, readBytes);
        }
        else
        {
            char* tmp = outcome;
            outcome = new char[totalBytes];
            memcpy(outcome, tmp, totalBytes-readBytes);
            memcpy(outcome+totalBytes-readBytes, buf, readBytes);
            delete[] tmp;
        }
        delete[] buf;
    }while(readBytes >= bufferSize);

    // close pipe
    pclose(pipe);

    // add '\0' to the end of the string,
    // put it into a std::string object.
    if(outcome)
    {
        char *tmp = outcome;
        outcome = new char[totalBytes + 1];
        memcpy(outcome, tmp, totalBytes);
        memcpy(outcome + totalBytes, "\0", 1);
        delete[] tmp;
    }
    std::string output;
    if(outcome)
    {
        output = outcome;
        delete[] outcome;
    }

    // return output
    return std::move(output);
}

std::vector<std::string> GetBootArgumentsByPS()
{
    // boot arguments
    std::vector<std::string> bootArguments;

    // get pid
    std::string       sPid;
    long         pid = getpid();
    std::stringstream l2s;
    l2s << pid;
    l2s >> sPid;

    // read /proc/self/cmdline
    std::string command("ps -o pid,args | grep ");
    command += sPid + " | grep -v grep";
    std::string tmp;
    std::string output = system(command);
    std::stringstream split(output);
    split >> sPid;
    while(split>>tmp)
        bootArguments.push_back(std::move(tmp));

    return std::move(bootArguments);
}

std::vector<std::string> GetBootArgumentsByCMDLINE()
{
    // boot arguments
    std::vector<std::string> bootArguments;

    // get /proc/${pid}/cmdline
    long pid = getpid();
    std::string cmdlinePath = "/proc/" + std::to_string(pid) + "/cmdline";
    std::ifstream cmdlineFile(cmdlinePath);

    // read /proc/${pid}/cmdline
    if(cmdlineFile.is_open())
    {
        while(cmdlineFile.peek()!=EOF)
        {
            std::string delimeter;
            std::string tmp;
            std::vector< std::string > tmpvec;

            cmdlineFile >> tmp;
            delimeter += '\0';
            tmpvec = split(tmp, delimeter);
            for(auto& str:tmpvec)
                bootArguments.push_back(str);
        }
    }
    else
    {
        std::cout << "file \"" << cmdlinePath << "\" not found!" << std::endl;
    }
    cmdlineFile.close();

    return std::move(bootArguments);
}

std::vector<std::string> GetBootArguments()
{
    //return GetBootArgumentsByPS();
    return GetBootArgumentsByCMDLINE();
}
