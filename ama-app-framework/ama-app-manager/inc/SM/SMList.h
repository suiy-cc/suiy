/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef SMLIST_H
#define SMLIST_H
#include <list>
#include <vector>
#include <string>
#include <tinyxml2.h>
#include "AMGRLogContext.h"

struct appNoteStruct
{
    std::string value1;
    std::string value2;
    std::string value3;
    std::string value4;
    std::string value5;
    std::string value6;

};
//
//struct value_ampid
//{
//    std::string value;
 //   int ampid;
//};

class appNoteOperate
{
public:
    bool ReadXmlFile(std::string& szFileName, std::list<appNoteStruct> *listXml);
};

#endif // SMLIST_H