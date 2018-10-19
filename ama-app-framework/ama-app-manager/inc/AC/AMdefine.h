/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef AMDEFINE_H_
#define AMDEFINE_H_

//sdk->AMACInterface
#include <AMACInterface.h>
#include <assert.h>

#include "AMGRLogContext.h"
#include "ama_types.h"
#include "ama_audioTypes.h"

using std::string;
using std::pair;
using std::make_pair;
using std::map;

typedef pair<AMAC_SourceID_t,AMAC_SinkID_t> AMConnectionIDPair;
typedef pair<string,string> AMConnectionNamePair;

#define PCM_DOMAIN          1
#define CODEC_DOMAIN        2

#define AM_MUTE_FLAG        1

#define AM_MUTE_DB              (-3000)

#define AMAC_CHANNEL_NULL       AMAC_MAX

#define MIN_AM_GAIN_DB          (-800)
#define MAX_AM_GAIN_DB          (0)

#define _BASS_FREQ_SET_         50
#define _MID_FREQ_SET_          775
#define _TREBLE_FREQ_SET_       8000

#define LONG_GAIN_NUM           5
#define SHORT_GAIN_NUM          3
#define _MAX_GAINLISTNUM_       LONG_GAIN_NUM
#define NO_GAIN_SET             0

#define ENABLE_MUTE             1
#define DISABLE_MUTE            2

static string PULSEAUDIO_STREAM = "PULSEAUDIO_STREAM";

typedef struct tNameRoutingTable
{
    //audioManager connect info
    int connectionCntOfPath;
    AMConnectionNamePair conNameArray[_MAX_CONNECT_STEP_];

    //start with soc connect then codec connect
    //so only codecConnectStartIndex we can know which pair is codec connect
    //we only set volume with codec connect in HS7 project
    int codecConnectStartIndex;
    //volGainTable pointer
    int *pVolGainTable;
    //audioManager gain info
    AMAC_Channel_t amacChannel;
    int gainNum;
    AMAC_Gain_t gainList[_MAX_GAINLISTNUM_];
}AMRoutingNameTable_s;

typedef struct tIDRoutingTable
{
    //audioManager connect info
    int connectionCntOfPath;
    AMConnectionIDPair conIDArray[_MAX_CONNECT_STEP_];
    //start with soc connect then codec connect
    //so only codecConnectStartIndex we can know which pair is codec connect
    //we only set volume with codec connect in HS7 project
    int codecConnectStartIndex;
    int volume_db;
    //pointer to nameRoutingTable,it's contains gain info!
    AMRoutingNameTable_s* pRoutingNameTable;
}AMRoutingIDTable_s;

class AMSource
{
public:
    AMSource(AMAC_SourceID_t sourceID,string name){
        mSourceID = sourceID;
        mName = name;
    }
    virtual ~AMSource(){}

    AMAC_SourceID_t sourceID(void){
        return mSourceID;
    }

    string name(void){
        return mName;
    }
private:
    AMAC_SourceID_t mSourceID;
    string mName;
};

class AMSink
{
public:
    AMSink(AMAC_SinkID_t sinkID,string name){
        mSinkID = sinkID;
        mName = name;
    }
    virtual ~AMSink(){}

    AMAC_SinkID_t sinkID(void){
        return mSinkID;
    }

    string name(void){
        return mName;
    }
private:
    AMAC_SinkID_t mSinkID;
    string mName;
};

/******************************************************************************/
/*                         IVI media to speaker                               */
/******************************************************************************/
//create ivi music source to speaker routing map-------->tested ok
static AMRoutingNameTable_s iviMusicToSpeaker_RT;
//create ivi tuner source to speaker routing map-------->tested ok
static AMRoutingNameTable_s iviVideoToSpeaker_RT;
//create ivi BtCall source to speaker routing map------->tested ok
static AMRoutingNameTable_s iviBTMediaToSpeaker_RT;
//create ivi carplayAudio to speaker routing map------->tested ok
static AMRoutingNameTable_s iviCarplayAudioToSpeaker_RT;
//create ivi Ipod to speaker routing map------->tested ok
static AMRoutingNameTable_s iviIpodToSpeaker_RT;
//create ivi third app music routing map------->
static AMRoutingNameTable_s ivi3ThAppAudioToSpeaker_RT;

/******************************************************************************/
/*                         IVI VRTTS to speaker                               */
/******************************************************************************/
//create ivi VRTTS source to speaker routing map
static AMRoutingNameTable_s iviVRTTSToSpeaker_RT;
//create ivi SIRI source to speaker routing map
static AMRoutingNameTable_s iviCarplaySiriToSpeaker_RT;

/******************************************************************************/
/*                         IVI VRTTS to speaker                               */
/******************************************************************************/
//create ivi SMSTTS source to speaker routing map
static AMRoutingNameTable_s iviSMSTTSToSpeaker_RT;

/******************************************************************************/
/*                       IVI phone to speaker FL                              */
/******************************************************************************/
//create ivi BtCall source to speaker routing map------->tested ok
static AMRoutingNameTable_s iviBTCallToSpeaker_RT;

//create ivi CallRing source to speaker routing map------->tested ok
static AMRoutingNameTable_s iviCallRingToSpeaker_RT;

//create ivi carplay call source to speaker routing map------->tested ok
static AMRoutingNameTable_s iviCarplayCallToSpeaker_RT;

/******************************************************************************/
/*                     IVI ?source to L-RSE heakset                           */
/******************************************************************************/
//create ivi music source to L-RSE heakset routing map
static AMRoutingNameTable_s iviMusicToLHeadset_RT;
//create ivi tuner source to L-RSE heakset routing map
static AMRoutingNameTable_s iviVideoToLHeadset_RT;
//create ivi BtCall source to L-RSE heakset routing map
static AMRoutingNameTable_s iviBTMediaToLHeadset_RT;

/******************************************************************************/
/*                     IVI ?source to R-RSE heakset                           */
/******************************************************************************/
//create ivi music source to L-RSE heakset routing map
static AMRoutingNameTable_s iviMusicToRHeadset_RT;
//create ivi tuner source to L-RSE heakset routing map
static AMRoutingNameTable_s iviVideoToRHeadset_RT;
//create ivi BtCall source to L-RSE heakset routing map
static AMRoutingNameTable_s iviBTMediaToRHeadset_RT;


/******************************************************************************/
/*                          IVI Tbox to speaker                               */
/******************************************************************************/
//create BCall source to speaker routing map
static AMRoutingNameTable_s iviBCallToSpeaker_RT;
//create ICall source to speaker routing map
static AMRoutingNameTable_s iviICallToSpeaker_RT;
//create ECall source to speaker routing map
static AMRoutingNameTable_s iviECallToSpeaker_RT;


/******************************************************************************/
/*                          IVI tuner to speaker                              */
/******************************************************************************/
//create ivi tuner source to speaker routing map------->tested ok
static AMRoutingNameTable_s iviTunerToSpeaker_RT;
//create ivi FM source to speaker routing map
static AMRoutingNameTable_s iviFMToSpeaker_RT;
//create ivi AM source to speaker routing map
static AMRoutingNameTable_s iviAMToSpeaker_RT;
//create ivi FM Second source to speaker routing map
static AMRoutingNameTable_s iviFMSecToSpeaker_RT;

/******************************************************************************/
/*                       L-RSE tuner to L-headset                             */
/******************************************************************************/
//create L-RSE tuner source to L-headset routing map
static AMRoutingNameTable_s LRSETunerToLHeadset_RT;
//create L-RSE FM source to L-headset routing map
static AMRoutingNameTable_s LRSEFMToLHeadset_RT;
//create L-RSE AM source to L-headset routing map
static AMRoutingNameTable_s LRSEAMToLHeadset_RT;

/******************************************************************************/
/*                       L-RSE tuner to speaker                               */
/******************************************************************************/
//create L-RSE FM source to speaker routing map
static AMRoutingNameTable_s LRSEFMToSpeaker_RT;
//create L-RSE AM source to speaker routing map
static AMRoutingNameTable_s LRSEAMToSpeaker_RT;

/******************************************************************************/
/*                       R-RSE tuner to R-headset                             */
/******************************************************************************/
//create R-RSE tuner source to R-headset routing map
static AMRoutingNameTable_s RRSETunerToRHeadset_RT;
//create R-RSE FM source to R-headset routing map
static AMRoutingNameTable_s RRSEFMToRHeadset_RT;
//create R-RSE AM source to R-headset routing map
static AMRoutingNameTable_s RRSEAMToRHeadset_RT;

/******************************************************************************/
/*                       R-RSE tuner to speaker                               */
/******************************************************************************/
//create L-RSE FM source to speaker routing map
static AMRoutingNameTable_s RRSEFMToSpeaker_RT;
//create L-RSE AM source to speaker routing map
static AMRoutingNameTable_s RRSEAMToSpeaker_RT;

/******************************************************************************/
/*                    L-RSE ?sources to L-headset                             */
/******************************************************************************/
//create L-RSE music source to L-headset routing map
static AMRoutingNameTable_s LRSEMusicToLHeadset_RT;
//create L-RSE tuner source to L-headset routing map
static AMRoutingNameTable_s LRSEVideoToLHeadset_RT;
//create L-RSE BtCall source to L-headset routing map
static AMRoutingNameTable_s LRSEBTMediaToLHeadset_RT;

/******************************************************************************/
/*                    L-RSE ?sources to R-headset                             */
/******************************************************************************/
//create L-RSE music source to R-headset routing map
static AMRoutingNameTable_s LRSEMusicToRHeadset_RT;
//create L-RSE tuner source to R-headset routing map
static AMRoutingNameTable_s LRSEVideoToRHeadset_RT;
//create L-RSE BtCall source to R-headset routing map
static AMRoutingNameTable_s LRSEBTMediaToRHeadset_RT;

/******************************************************************************/
/*                    L-RSE ?sources to speaker                               */
/******************************************************************************/
//create L-RSE music source to speaker routing map
static AMRoutingNameTable_s LRSEMusicToSpeaker_RT;
//create L-RSE tuner source to speaker routing map
static AMRoutingNameTable_s LRSEVideoToSpeaker_RT;
//create L-RSE BtCall source to speaker routing map
static AMRoutingNameTable_s LRSEBTMediaToSpeaker_RT;

/******************************************************************************/
/*                    R-RSE ?sources to R-headset                             */
/******************************************************************************/
//create R-RSE music source to R-headset routing map-------->tested ok
static AMRoutingNameTable_s RRSEMusicToRHeadset_RT;
//create R-RSE tuner source to R-headset routing map-------->tested ok
static AMRoutingNameTable_s RRSEVideoToRHeadset_RT;
//create R-RSE BtCall source to R-headset routing map------->tested ok
static AMRoutingNameTable_s RRSEBTMediaToRHeadset_RT;

/******************************************************************************/
/*                    R-RSE ?sources to L-headset                             */
/******************************************************************************/
//create R-RSE music source to L-headset routing map-------->tested ok
static AMRoutingNameTable_s RRSEMusicToLHeadset_RT;
//create R-RSE tuner source to L-headset routing map-------->tested ok
static AMRoutingNameTable_s RRSEVideoToLHeadset_RT;
//create R-RSE BtCall source to L-headset routing map------->tested ok
static AMRoutingNameTable_s RRSEBTMediaToLHeadset_RT;

/******************************************************************************/
/*                    R-RSE ?sources to speaker                               */
/******************************************************************************/
//create R-RSE music source to speaker routing map-------->tested ok
static AMRoutingNameTable_s RRSEMusicToSpeaker_RT;
//create R-RSE tuner source to speaker routing map-------->tested ok
static AMRoutingNameTable_s RRSEVideoToSpeaker_RT;
//create R-RSE BtCall source to speaker routing map------->tested ok
static AMRoutingNameTable_s RRSEBTMediaToSpeaker_RT;

/******************************************************************************/
/*                    L-RSE softKey to L-headset                              */
/******************************************************************************/
//create L-RSE softKey source to L-headset routing map
static AMRoutingNameTable_s LRSESoftKeyToLHeadset_RT;

/******************************************************************************/
/*                    L-RSE softKey to speaker                                */
/******************************************************************************/
//create L-RSE softKey source to speaker routing map
static AMRoutingNameTable_s LRSESoftKeyToSpeaker_RT;

/******************************************************************************/
/*                    R-RSE softKey to R-headset                              */
/******************************************************************************/
//create R-RSE softKey source to R-headset routing map
static AMRoutingNameTable_s RRSESoftKeyToRHeadset_RT;

/******************************************************************************/
/*                    R-RSE softKey to speaker                                */
/******************************************************************************/
//create R-RSE softKey source to speaker routing map
static AMRoutingNameTable_s RRSESoftKeyToSpeaker_RT;

/******************************************************************************/
/*                    ivi naviTTS to speaker FL                               */
/******************************************************************************/
//create ivi naviTTS source to speaker routing map
static AMRoutingNameTable_s iviNaviToSpeaker_RT;

/******************************************************************************/
/*                    ivi FL-radar to speaker FL                              */
/******************************************************************************/
//create ivi FL-radar source to speaker routing map
static AMRoutingNameTable_s iviFLRadarToSpeaker_RT;

/******************************************************************************/
/*                    ivi FR-radar to speaker FR                              */
/******************************************************************************/
//create ivi FR-radar source to speaker routing map
static AMRoutingNameTable_s iviFRRadarToSpeaker_RT;

/******************************************************************************/
/*                    ivi RL-radar to speaker RL                              */
/******************************************************************************/
//create ivi RL-radar source to speaker routing map
static AMRoutingNameTable_s iviRLRadarToSpeaker_RT;

/******************************************************************************/
/*                    ivi RR-radar to speaker RR                              */
/******************************************************************************/
//create ivi RR-radar source to speaker routing map
static AMRoutingNameTable_s iviRRRadarToSpeaker_RT;

/******************************************************************************/
/*                    ivi radarAlert to speaker FL                            */
/******************************************************************************/
//create ivi radarAlert source to speaker routing map
static AMRoutingNameTable_s iviRadarAlertToSpeaker_RT;

/******************************************************************************/
/*                      ivi prompt to speaker FL                              */
/******************************************************************************/
//create ivi prompt source to speaker routing map
static AMRoutingNameTable_s iviPromptToSpeaker_RT;

//create ivi carplay alt source to RR&RL speaker routing map
static AMRoutingNameTable_s iviCarplayAltToSpeaker_RT;

/******************************************************************************/
/*                      ivi AVMALert to speaker FL                              */
/******************************************************************************/
//create ivi AVMAlert source to speaker routing map
static AMRoutingNameTable_s iviAVMAlertToSpeaker_RT;

/******************************************************************************/
/*                      ivi backdoorPrompt to speaker RR&RL                   */
/******************************************************************************/
//create ivi backdoorPrompt source to RR&RL speaker routing map
static AMRoutingNameTable_s iviBackdoorPromptToSpeaker_RT;

/******************************************************************************/
/*                      ivi feedbackSound to speaker FL                       */
/******************************************************************************/
//create ivi feedbackSound source to speaker routing map
static AMRoutingNameTable_s iviFeedbackToSpeaker_RT;

/******************************************************************************/
/*                        ivi softKey to speaker                              */
/******************************************************************************/
//create ivi softKey source to speaker routing map
static AMRoutingNameTable_s iviSoftKeyToSpeaker_RT;


/******************************************************************************/
/*                        mic to btPhone paSink                               */
/******************************************************************************/
//create mic source to btPhone routing map
static AMRoutingNameTable_s micToBTPhone_RT;

/******************************************************************************/
/*                            mic to VR paSink                                */
/******************************************************************************/
//create mic source to VR routing map
static AMRoutingNameTable_s micToVR_RT;

#define BMT_MAX    13    //-6~+6 defined in soundFunctionSpec
#define VOL_MAX    31    //0~30 defined in soundFunctionSpec

static int GainToVolSet[81] =
{//| 0|| 1|| 2|| 3|| 4|| 5|| 6|| 7|| 8|| 9||10||11||12||13||14||15||16||17||18||19|
     0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  3,  3,
 //|20||21||22||23||24||25||26||27||28||29||30||31||32||33||34||35||36||37||38||39|
     3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,
 //|40||41||42||43||44||45||46||47||48||49||50||51||52||53||54||55||56||57||58||59|
     8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14,
 //|60||61||62||63||64||65||66||67||68||69||70||71||72||73||74||75||76||77||78||79||80|
    15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
};

//-------------BassSetTable[bassSet+6][volume]
static int BassGainTable[BMT_MAX][VOL_MAX] =
{//---| 0|-| 1|-| 2|-| 3|-| 4|-| 5|-| 6|-| 7|-| 8|-| 9|-|10|-|11|-|12|-|13|-|14|-|15|-|16|-|17|-|18|-|19|-|20|-|21|-|22|-|23|-|24|-|25|-|26|-|27|-|28|-|29|-|30|
/* 0*/-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,
/* 1*/-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,
/* 2*/-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,
/* 3*/-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,
/* 4*/-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,
/* 5*/-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,
/* 6*/  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
/* 7*/ 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133,  0 ,  0 ,
/* 8*/ 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 133,  0 ,  0 ,
/* 9*/ 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 266, 133,  0 ,  0 ,
/*10*/ 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 399, 399, 266, 133,  0 ,  0 ,
/*11*/ 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 532, 399, 399, 266, 133,  0 ,  0 ,
/*12*/ 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 665, 665, 665, 532, 399, 399, 266, 133,  0 ,  0 ,
};

//---------MidSetTable[MidSet+6][volume]
static int MidGainTable[BMT_MAX][VOL_MAX] =
{//---| 0|-| 1|-| 2|-| 3|-| 4|-| 5|-| 6|-| 7|-| 8|-| 9|-|10|-|11|-|12|-|13|-|14|-|15|-|16|-|17|-|18|-|19|-|20|-|21|-|22|-|23|-|24|-|25|-|26|-|27|-|28|-|29|-|30|
/* 0*/-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,-600,
/* 1*/-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,-500,
/* 2*/-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,-400,
/* 3*/-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,-300,
/* 4*/-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,
/* 5*/-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,
/* 6*/  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
/* 7*/ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  0 ,
/* 8*/ 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 100,  0 ,
/* 9*/ 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 200, 100,  0 ,
/*10*/ 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 300, 200, 100,  0 ,
/*11*/ 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 400, 300, 200, 100,  0 ,
/*12*/ 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 500, 400, 300, 200, 100,  0 ,
};

//---------TrebleSetTable[TrebleSet+6][volume]
static int TrebleGainTable[BMT_MAX][VOL_MAX] =
{//---| 0|-| 1|-| 2|-| 3|-| 4|-| 5|-| 6|-| 7|-| 8|-| 9|-|10|-|11|-|12|-|13|-|14|-|15|-|16|-|17|-|18|-|19|-|20|-|21|-|22|-|23|-|24|-|25|-|26|-|27|-|28|-|29|-|30|
/* 0*/-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,-931,
/* 1*/-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,-665,
/* 2*/-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,-532,
/* 3*/-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,-399,
/* 4*/-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,-266,
/* 5*/-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,-133,
/* 6*/  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
/* 7*/ 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133, 133,  0 ,  0 ,
/* 8*/ 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 133,  0 ,  0 ,
/* 9*/ 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 399, 266, 133,  0 ,  0 ,
/*10*/ 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 399, 399, 266, 133,  0 ,  0 ,
/*11*/ 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 665, 532, 399, 399, 266, 133,  0 ,  0 ,
/*12*/ 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 931, 665, 665, 665, 532, 399, 399, 266, 133,  0 ,  0 ,
};

static int VolSetToGain_PRI[VOL_MAX] =
{//   | 0|-| 1|-| 2|-| 3|-| 4|-| 5|-| 6|-| 7|-| 8|-| 9|-|10|-|11|-|12|-|13|-|14|-|15|-|16|-|17|-|18|-|19|-|20|-|21|-|22|-|23|-|24|-|25|-|26|-|27|-|28|-|29|-|30|
AM_MUTE_DB,-730,-670,-620,-570,-520,-480,-440,-400,-370,-340,-310,-280,-250,-220,-200,-180,-160,-140,-120,-100,-90 ,-80 ,-70 ,-60 ,-50 ,-40 ,-30 ,-20 ,-10 , 0,
};

static int VolSetToGain_BT[VOL_MAX] =
{//   | 0|-| 1|-| 2|-| 3|-| 4|-| 5|-| 6|-| 7|-| 8|-| 9|-|10|-|11|-|12|-|13|-|14|-|15|-|16|-|17|-|18|-|19|-|20|-|21|-|22|-|23|-|24|-|25|-|26|-|27|-|28|-|29|-|30|
AM_MUTE_DB,-730,-680,-630,-580,-530,-480,-440,-400,-360,-330,-300,-270,-250,-230,-220,-220,-220,-220,-220,-220,-220,-220,-220,-220,-220,-220,-220,-220,-220,-220,
};

static int VolSetToGain_TE[VOL_MAX] =
{//   | 0|-| 1|-| 2|-| 3|-| 4|-| 5|-| 6|-| 7|-| 8|-| 9|-|10|-|11|-|12|-|13|-|14|-|15|-|16|-|17|-|18|-|19|-|20|-|21|-|22|-|23|-|24|-|25|-|26|-|27|-|28|-|29|-|30|
AM_MUTE_DB,-520,-470,-420,-380,-340,-310,-280,-250,-220,-200,-180,-160,-140,-120,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,
};

static int VolSetToGain_SS[VOL_MAX] =
{//   | 0|-| 1|-| 2|-| 3|-| 4|-| 5|-| 6|-| 7|-| 8|-| 9|-|10|-|11|-|12|-|13|-|14|-|15|-|16|-|17|-|18|-|19|-|20|-|21|-|22|-|23|-|24|-|25|-|26|-|27|-|28|-|29|-|30|
AM_MUTE_DB,-730,-690,-650,-610,-570,-540,-510,-480,-450,-420,-400,-380,-360,-340,-320,-300,-280,-260,-240,-220,-210,-200,-190,-180,-170,-160,-150,-140,-130,-120,
};

static int VolSetToGain_IS[VOL_MAX] =
{//   | 0|-| 1|-| 2|-| 3|-| 4|-| 5|-| 6|-| 7|-| 8|-| 9|-|10|-|11|-|12|-|13|-|14|-|15|-|16|-|17|-|18|-|19|-|20|-|21|-|22|-|23|-|24|-|25|-|26|-|27|-|28|-|29|-|30|
AM_MUTE_DB,-610,-570,-530,-490,-460,-430,-400,-370,-340,-310,-280,-260,-240,-220,-200,-180,-160,-140,-120,-100,-90 ,-80 ,-70 ,-60 ,-50 ,-40 ,-30 ,-20 ,-10 , 0,
};

static int VolSetToGain_RADAR[VOL_MAX] =
{//   | 0|-| 1|-| 2|-| 3|-| 4|-| 5|-| 6|-| 7|-| 8|-| 9|-|10|-|11|-|12|-|13|-|14|-|15|-|16|-|17|-|18|-|19|-|20|-|21|-|22|-|23|-|24|-|25|-|26|-|27|-|28|-|29|-|30|
AM_MUTE_DB,-460,-420,-380,-340,-310,-280,-260,-240,-220,-200,-180,-160,-140,-120,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,
};

static int VolSetToGain_SOFTKEY[VOL_MAX] =
{//   | 0|-| 1|-| 2|-| 3|-| 4|-| 5|-| 6|-| 7|-| 8|-| 9|-|10|-|11|-|12|-|13|-|14|-|15|-|16|-|17|-|18|-|19|-|20|-|21|-|22|-|23|-|24|-|25|-|26|-|27|-|28|-|29|-|30|
AM_MUTE_DB,-610,-570,-530,-490,-460,-430,-400,-370,-340,-310,-280,-260,-240,-220,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,-200,
};

static int VolSetToGain_LRSE[VOL_MAX] =
{//   | 0|-| 1|-| 2|-| 3|-| 4|-| 5|-| 6|-| 7|-| 8|-| 9|-|10|-|11|-|12|-|13|-|14|-|15|-|16|-|17|-|18|-|19|-|20|-|21|-|22|-|23|-|24|-|25|-|26|-|27|-|28|-|29|-|30|
AM_MUTE_DB,-610,-570,-530,-490,-460,-430,-400,-370,-340,-310,-280,-260,-240,-220,-200,-180,-160,-140,-120,-100,-90 ,-80 ,-70 ,-60 ,-50 ,-40 ,-30 ,-20 ,-10 , 0,
};

static int VolSetToGain_RRSE[VOL_MAX] =
{//   | 0|-| 1|-| 2|-| 3|-| 4|-| 5|-| 6|-| 7|-| 8|-| 9|-|10|-|11|-|12|-|13|-|14|-|15|-|16|-|17|-|18|-|19|-|20|-|21|-|22|-|23|-|24|-|25|-|26|-|27|-|28|-|29|-|30|
AM_MUTE_DB,-610,-570,-530,-490,-460,-430,-400,-370,-340,-310,-280,-260,-240,-220,-200,-180,-160,-140,-120,-100,-90 ,-80 ,-70 ,-60 ,-50 ,-40 ,-30 ,-20 ,-10 , 0,
};

static int VolSetToGain_NAVI[VOL_MAX] =
{//   | 0|-| 1|-| 2|-| 3|-| 4|-| 5|-| 6|-| 7|-| 8|-| 9|-|10|-|11|-|12|-|13|-|14|-|15|-|16|-|17|-|18|-|19|-|20|-|21|-|22|-|23|-|24|-|25|-|26|-|27|-|28|-|29|-|30|
AM_MUTE_DB,-330,-310,-290,-270,-250,-240,-230,-220,-210,-200,-190,-180,-170,-160,-150,-140,-130,-120,-110,-100,-90 ,-80 ,-70 ,-60 ,-50 ,-40 ,-30 ,-20 ,-10 , 0,
};

#ifdef CODE_FOR_EV
static int Fader_ffrTable[13] =
{//| 0|| 1|| 2|| 3|| 4|| 5|| 6|| 7|| 8|| 9||10||11||12|
    80, 30,  15, 10,  6,  3,  0,  0,  0,  0,  0,  0,  0,
};

static int Fader_freTable[13] =
{//| 0|| 1|| 2|| 3|| 4|| 5|| 6|| 7|| 8|| 9||10||11||12|
     0,  0,  0,  0,  0,  0,  0,  3,  6, 10, 15, 30, 80,
};
#else
static int Fader_ffrTable[13] =
{//| 0|| 1|| 2|| 3|| 4|| 5|| 6|| 7|| 8|| 9||10||11||12|
    80, 12,  7,  4,  2,  1,  0,  0,  0,  0,  0,  0,  0,
};

static int Fader_freTable[13] =
{//| 0|| 1|| 2|| 3|| 4|| 5|| 6|| 7|| 8|| 9||10||11||12|
     0,  0,  0,  0,  0,  0,  0,  1,  2,  4,  7, 12, 80,
};
#endif

static int Balance_BRTable[13] =
{//| 0|| 1|| 2|| 3|| 4|| 5|| 6|| 7|| 8|| 9||10||11||12|
    80, 12,  7,  4,  2,  1,  0,  0,  0,  0,  0,  0,  0,
};

static int Balance_BLTable[13] =
{//| 0|| 1|| 2|| 3|| 4|| 5|| 6|| 7|| 8|| 9||10||11||12|
     0,  0,  0,  0,  0,  0,  0,  1,  2,  4,  7, 12, 80,
};

#endif //AMDEFINE_H_
