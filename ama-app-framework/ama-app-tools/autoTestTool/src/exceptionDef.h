/*
* Copyright (C) 2016-2017 Neusoft, Inc.
* All Rights Reserved.
*
* The following programs are the sole property of Neusoft Inc.,
* and contain its proprietary and confidential information.
*/

#ifndef EXCEPTIONDEF_H_
#define EXCEPTIONDEF_H_

enum ama_exceptionType_e{
    E_RECORDING = 0x00,
    E_NOT_RECORDING,
    E_NO_RECORD,
    E_PLAYING,
    E_NOT_PLAYING,
    E_IS_OPERATING,
    E_NOT_OPERATING,
    E_NO_EVENTS,
    E_LOAD_FILE_FAILED,
    E_SAVE_FILE_FAILED,
    E_NO_OPERAT_THING,
};

void printException(ama_exceptionType_e e);

#endif//EXCEPTIONDEF_H_
