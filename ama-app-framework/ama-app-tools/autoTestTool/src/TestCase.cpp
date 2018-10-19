/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "TestCase.h"

#include <unistd.h>


namespace ama
{

TestCase::TestCase()
:mpPlayer(nullptr)
{

}

TestCase::~TestCase()
{

}

void TestCase::load(string filePath)
{
    mTouchEvents.loadFrom(filePath);
    mExpectResultEvents.loadFrom(filePath);
}

void TestCase::save(string filePath)
{
    mTouchEvents.saveAs(filePath);
    mExpectResultEvents.saveAs(filePath);
}

void TestCase::play(int playCnt) throw(ama_exceptionType_e)
{
    //testCase only play one time,ignor playCnt arg
    mTouchEvents.play(std::bind(&TestCase::cbTouchPlayOver,this));
    mExpectResultEvents.play();
    mResultEvents.record();
}

void TestCase::stopPlay() throw(ama_exceptionType_e)
{
    mTouchEvents.stopPlay();
    mExpectResultEvents.stopPlay();
    mResultEvents.stopRecord();
}

void TestCase::record(string filter) throw(ama_exceptionType_e)
{
    mTouchEvents.record();
    mExpectResultEvents.record(filter);
}

void TestCase::stopRecord() throw(ama_exceptionType_e)
{
    mTouchEvents.stopRecord();
    mExpectResultEvents.stopRecord();
}

void TestCase::setPlayer(IPlayer *pPlayer)
{
    mpPlayer = pPlayer;
}

void TestCase::printTestResultInfo(void)
{
    mExpectResultEvents.printWithEx("expect :");
    mResultEvents.printWithEx("receive:");
}

void TestCase::generateTestReport(string filePath)
{
    mExpectResultEvents.generateReport(mResultEvents,filePath);
}

void TestCase::cbTouchPlayOver(void)
{
    sleep(1);
    mResultEvents.stopRecord();
    printTestResultInfo();
    generateTestReport();
}

}//end namespace ama
