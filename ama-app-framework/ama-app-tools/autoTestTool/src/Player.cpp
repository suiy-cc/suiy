/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "Player.h"
#include "InputEvent.h"
#include "TestCase.h"
#include "Touch.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


namespace ama
{

Player::Player()
:mpPlayAble(nullptr)
{

}

Player::~Player()
{

}

void Player::playTouch(string filePath,int playCnt) throw(ama_exceptionType_e)
{
    if(""!=filePath){
        mpPlayAble = make_shared<Touch>();
        mpPlayAble->load(filePath);
    }

    if(mpPlayAble){
        mpPlayAble->play(playCnt);
    }else{
        throw E_NO_OPERAT_THING;
    }
}

void Player::playTestCase(string filePath) throw(ama_exceptionType_e)
{
    if(""!=filePath){
        mpPlayAble = make_shared<TestCase>();
        mpPlayAble->load(filePath);
    }

    if(mpPlayAble){
        mpPlayAble->play();
    }else{
        throw E_NO_OPERAT_THING;
    }
}

void Player::stopPlay() throw(ama_exceptionType_e)
{
    if(mpPlayAble){
        mpPlayAble->stopPlay();
    }else{
        throw E_NO_OPERAT_THING;
    }
}

void Player::recordTouch(void) throw(ama_exceptionType_e)
{
    mpPlayAble = make_shared<Touch>();
    mpPlayAble->record();
}

void Player::recordTestCase(string filter) throw(ama_exceptionType_e)
{
    mpPlayAble = make_shared<TestCase>();
    mpPlayAble->record(filter);
}

void Player::stopRecord() throw(ama_exceptionType_e)
{
    if(mpPlayAble){
        mpPlayAble->stopRecord();
    }else{
        throw E_NO_OPERAT_THING;
    }
}

void Player::save(string filePath)
{
    if(mpPlayAble){
        mpPlayAble->save(filePath);
    }else{
        throw E_NO_OPERAT_THING;
    }
}

void Player::cbPlayingStateChanged(bool isPlayingOver)
{

}

}//end namespace ama
