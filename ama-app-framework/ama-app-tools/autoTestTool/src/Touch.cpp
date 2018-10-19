/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "Touch.h"

#include <unistd.h>
#include <fstream>
#include <iostream>

namespace ama
{

Touch::Touch()
:mpPlayer(nullptr)
,mPlayCnt(1)
{

}

Touch::~Touch()
{

}

void Touch::load(string filePath)
{
    mTouchEvents.loadFrom(filePath);
}

void Touch::save(string filePath)
{
    mTouchEvents.saveAs(filePath);
}

void Touch::play(int playCnt) throw(ama_exceptionType_e)
{
    mPlayCnt = playCnt;
    playOnce();
}

void Touch::stopPlay() throw(ama_exceptionType_e)
{
    mTouchEvents.stopPlay();
}

void Touch::record(string filter) throw(ama_exceptionType_e)
{
    mTouchEvents.record();
}

void Touch::stopRecord() throw(ama_exceptionType_e)
{
    mTouchEvents.stopRecord();
}

void Touch::setPlayer(IPlayer *pPlayer)
{
    mpPlayer = pPlayer;
}

void Touch::playOnce(void) throw(ama_exceptionType_e)
{
    mTouchEvents.play(std::bind(&Touch::cbTouchPlayOver,this));
}

void Touch::cbTouchPlayOver(void)
{
    if((--mPlayCnt)>0){
        playOnce();
        std::cout << "touch playOnce over!" << std::endl;
    }else{
        std::cout << "touch play over!" << std::endl;
    }

}

}//end namespace ama
