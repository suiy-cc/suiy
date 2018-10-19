/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef TESTCASE_H_
#define TESTCASE_H_

#include "Events.h"
#include "exceptionDef.h"
#include "IPlayAble.h"

#include <mutex>
#include <list>
#include <thread>
#include <memory>

namespace ama
{

class TestCase : public IPlayAble
{
public:
    TestCase();
    ~TestCase();
    virtual void load(string filePath);
    virtual void save(string filePath);
    virtual void play(int playCnt=1) throw(ama_exceptionType_e);
    virtual void stopPlay() throw(ama_exceptionType_e);
    virtual void record(string filter="") throw(ama_exceptionType_e);
    virtual void stopRecord() throw(ama_exceptionType_e);
    virtual void setPlayer(IPlayer *pPlayer);

    void printTestResultInfo(void);
    void generateTestReport(string filePath="");

    void cbTouchPlayOver(void);
private:
    IPlayer* mpPlayer;
    TouchEvents mTouchEvents;
    TestEvents mExpectResultEvents;
    TestEvents mResultEvents;
};

}//end namespace ama

#endif //TESTCASE_H_
