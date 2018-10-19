/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef PLAYER_H_
#define PLAYER_H_

#include "Events.h"
#include "exceptionDef.h"
#include "IPlayAble.h"

#include <mutex>
#include <list>
#include <thread>
#include <memory>

namespace ama
{

class Player : public IPlayer
{
public:
    static Player* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static Player instance;
        return &instance;
    }
    ~Player();
    void playTouch(string filePath="",int playCnt=1) throw(ama_exceptionType_e);
    void playTestCase(string filePath="") throw(ama_exceptionType_e);
    void stopPlay() throw(ama_exceptionType_e);
    void recordTouch(void) throw(ama_exceptionType_e);
    void recordTestCase(string filter="") throw(ama_exceptionType_e);
    void stopRecord() throw(ama_exceptionType_e);
    void save(string filePath);

    virtual void cbPlayingStateChanged(bool isPlayingOver);
private:
    Player();
    shared_ptr<IPlayAble> mpPlayAble;
};//end Player

}//end namespace ama

#endif //PLAYER_H_
