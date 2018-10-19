/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef TOUCH_H_
#define TOUCH_H_

#include "Events.h"
#include "exceptionDef.h"
#include "IPlayAble.h"

#include <string>

namespace ama
{

class Touch : public IPlayAble
{
public:
    Touch();
    ~Touch();
    virtual void load(string filePath);
    virtual void save(string filePath);
    virtual void play(int playCnt=1) throw(ama_exceptionType_e);
    virtual void stopPlay() throw(ama_exceptionType_e);
    virtual void record(string filter="") throw(ama_exceptionType_e);
    virtual void stopRecord() throw(ama_exceptionType_e);
    virtual void setPlayer(IPlayer *pPlayer);

    void cbTouchPlayOver(void);
    void playOnce(void) throw(ama_exceptionType_e);
private:
    IPlayer* mpPlayer;
    TouchEvents mTouchEvents;
    int mPlayCnt;
};

}//end namespace ama

#endif //TOUCH_H_
