/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef STREAM_H_
#define STREAM_H_

//sdk->AMACInterface
#include <AMACInterface.h>

#include "AMGRLogContext.h"
#include "ama_types.h"
#include "ama_audioTypes.h"
#include "Connection.h"

using std::map;
using std::shared_ptr;

class Stream
{
public:
    Stream(ama_streamID_t streamID);
    virtual ~Stream();

    int getStreamID(void);

    void requestAudioDeviceFocus(ama_connectID_t connectID);
    void releaseAudioDeviceFocus(ama_connectID_t connectID);
    void setStreamMuteState(bool isMute);

    void execConnectWhenSourceCreated(void);
    void execDisconnectWhenSourceRemoved(void);

    void notifyFocusStateChanged(ama_connectID_t connectID,ama_focusSta_t oldFocusStaForApp,ama_focusSta_t newFocusStaForApp);

    //add this function in notifyFocusStateChanged()
    void notifyFocusStaChange2Process(ama_connectID_t connectID,ama_focusSta_t focusStaForApp);

    void removeConnectionFromMap(ama_connectID_t connectID);
private:
    ama_streamID_t mStreamID;

    map<ama_connectID_t,shared_ptr<Connection>> mConnectionMap;
};

#endif //STREAM_H_
