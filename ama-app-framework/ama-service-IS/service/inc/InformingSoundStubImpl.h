/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef INFORMINGSOUNDSTUBIMPL_H
#define INFORMINGSOUNDSTUBIMPL_H

#include <CommonAPI/CommonAPI.hpp>
#include <v0_1/org/neusoft/InformingSoundStubDefault.hpp>


class InformingSoundStubImpl
    : public v0_1::org::neusoft::InformingSoundStubDefault
{
public:
    InformingSoundStubImpl();
    virtual ~InformingSoundStubImpl();

    void reqToPlayInformingSound(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _connectID, bool _isPlay);

    void reqToPlaytRadarSound(const std::shared_ptr<CommonAPI::ClientId> _client, uint8_t _soundType, uint16_t _highestLevelChannel);

    void reqToPlayFeedbackSound(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _volumeType, uint32_t _volume);
};

extern void commonAPIServerInit();

#endif // INFORMINGSOUNDSTUBIMPL_H
