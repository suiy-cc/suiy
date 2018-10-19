/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef IPLAYABLE_H_
#define IPLAYABLE_H_

#include "exceptionDef.h"

#include <string>

namespace ama
{

class IPlayer
{
public:
    IPlayer(){
    }

    virtual ~IPlayer(){

    }

    virtual void cbPlayingStateChanged(bool isPlayingOver)=0;
};//end Iplayer

class IPlayAble
{
public:
    IPlayAble(){

    }
    virtual ~IPlayAble(){

    }

    virtual void load(std::string filePath)=0;
    virtual void save(std::string filePath)=0;
    virtual void play(int playCnt=1) throw(ama_exceptionType_e)=0;
    virtual void stopPlay() throw(ama_exceptionType_e)=0;
    virtual void record(std::string filter="") throw(ama_exceptionType_e)=0;
    virtual void stopRecord() throw(ama_exceptionType_e)=0;
    virtual void setPlayer(IPlayer *pPlayer)=0;
};//end IPlayAble

}//end namespace ama

#endif //IPLAYABLE_H_
