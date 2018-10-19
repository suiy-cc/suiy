/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef CAPI_STUBIMPL_H
#define CAPI_STUBIMPL_H

#include <vector>

#include <CommonAPI/CommonAPI.hpp>
#include <v0_1/org/neusoft/AppMgrStubDefault.hpp>

typedef std::map<int32_t ,int32_t> LCDTempMap;
class NeusoftAppMgrStubImpl
    : public v0_1::org::neusoft::AppMgrStubDefault
{
public:
    NeusoftAppMgrStubImpl();
    virtual ~NeusoftAppMgrStubImpl();

    virtual void StartAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, std::string _itemID, std::string _action, std::string _data, bool _isQuiet) override;
    virtual void Move2BottomLayerReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _requester, Move2BottomLayerReqReply_t _reply) override;
    virtual void Return2HomeLayerReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _requester, Return2HomeLayerReqReply_t _reply) override;
    virtual void StopAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid) override;
    virtual void InitializeReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, uint64_t _id) override;
    virtual void ResumeReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, uint64_t _id) override;
    virtual void PreShowReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, uint64_t _id) override;
    virtual void PreHideReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, uint64_t _id) override;
    virtual void HideAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid) override;
	virtual void KillAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid) override;

    virtual void GetInstalledAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _requester, GetInstalledAppReqReply_t _reply);
    virtual void GetInstalledAppInfoReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _requester, GetInstalledAppInfoReqReply_t _reply);
    virtual void GetAppPropertiesReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _app, GetAppPropertiesReqReply_t _reply);
    virtual void PlayStartupAnimationReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid);
    virtual void StopStartupAnimationReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid);
	virtual void StopSuccessReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, uint64_t _id) override;
    virtual void StopAnimationReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid);
    virtual void GetStartupScreenAvailibilityReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, GetStartupScreenAvailibilityReqReply_t _reply) override;
	virtual void LightUpScreenReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seat) override;
	virtual void ShutdownReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid);
	virtual void GetAMPIDReq(const std::shared_ptr<CommonAPI::ClientId> _client, std::string _itemID, GetAMPIDReqReply_t _reply) override;
	virtual void StartHomeReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seat) override;
	virtual void SendBootLogReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, v0_1::org::neusoft::AppMgr::StringArray _bootLog) override;
	virtual void GetLogIDReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, GetLogIDReqReply_t _reply) override;
    virtual void GetActiveAppListReq(const std::shared_ptr<CommonAPI::ClientId> _client, GetActiveAppListReqReply_t _reply) override;
    virtual void GetAppStateReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, GetAppStateReqReply_t _reply) override;	virtual void EarlyInitializeReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid) override;
	virtual void GetSurfaceIDReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, GetSurfaceIDReqReply_t _reply) override;
	virtual void GetUnderLayerSurfaceIDReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, GetUnderLayerSurfaceIDReqReply_t _reply) override;
	virtual void GetBKGLayerSurfaceIDReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, GetBKGLayerSurfaceIDReqReply_t _reply) override;
	virtual void GetAnonymousAMPIDReq(const std::shared_ptr<CommonAPI::ClientId> _client, GetAnonymousAMPIDReqReply_t _reply);
	virtual void ForceMainSurfaceConfiguredReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _app)override ;
    virtual void EnableTouchAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _app, bool _enable) override;
    virtual void EnableChildSafetyLockReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seat, bool _enable, std::string _pngPath) override;
    virtual void EnableTouchScreenReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seat, bool _enable, std::string _pngPath) override;
	virtual void SetUnderLayerCenterReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, bool _isCenter) override;
	virtual void IsUnderLayerCenterReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, IsUnderLayerCenterReqReply_t _reply) override;
	virtual void GetScreensReq(const std::shared_ptr<CommonAPI::ClientId> _client, GetScreensReqReply_t _reply) override;
	virtual void GetScreenSizeReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seat, GetScreenSizeReqReply_t _reply) override;
	virtual void RemoveAppFromZOrderReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid) override;
	virtual void AddExtraSurfaceReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _surface, uint32_t _layer, AddExtraSurfaceReqReply_t _reply) override;
	virtual void SetRearTimerSTOP(const std::shared_ptr<CommonAPI::ClientId> _client,uint32_t _seat,bool _state);
    virtual void SetRearPowerOff(const std::shared_ptr<CommonAPI::ClientId> _client,uint32_t _seat);
    virtual void SetScreenOnByApp(const std::shared_ptr<CommonAPI::ClientId> _client,uint32_t _seat);
    virtual void AppAwakeRequest(const std::shared_ptr<CommonAPI::ClientId> _client,uint32_t _ampid ,bool _isAwake);
    virtual void SetBrightness(const std::shared_ptr<CommonAPI::ClientId> _client,uint16_t _seatid ,uint16_t _brightness);
    virtual void USBStateController(const std::shared_ptr<CommonAPI::ClientId> _client,uint16_t _op );
    virtual void getUSBDevState(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t __ampid);
	virtual void RequestLcdTemperature(const std::shared_ptr<CommonAPI::ClientId> _client, RequestLcdTemperatureReply_t _reply) override;
    virtual void GetPowerStateWhenSDKInitReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seat, GetPowerStateWhenSDKInitReqReply_t _reply) override;
    virtual void PowerAwakeReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid,bool _isPowerAwake) override;
    virtual void ScreenAwakeReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid,bool _isScreenAwake) override;
    virtual void AbnormalAwakeReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid,bool _isScreenAwake) override;
    virtual void SendPowerSignalReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _powerSignal) override;
    virtual void GetAllPowerStateWhenSDKInitReq(const std::shared_ptr<CommonAPI::ClientId> _client, GetAllPowerStateWhenSDKInitReqReply_t _reply);
	virtual void GetRawLastSourceReq(const std::shared_ptr<CommonAPI::ClientId> _client, GetRawLastSourceReqReply_t _reply) override;
	virtual void GetLastSourceReq(const std::shared_ptr<CommonAPI::ClientId> _client, GetLastSourceReqReply_t _reply) override;
	virtual void SetLastSourceReq(const std::shared_ptr<CommonAPI::ClientId> _client, v0_1::org::neusoft::AppMgr::IntentInfoArray _lastSourceSet, SetLastSourceReqReply_t _reply) override;
	virtual void requestAlwaysOn(const std::shared_ptr<CommonAPI::ClientId> _client,uint32_t _ampid ,bool _isAwaysOn );
    // SM-API
	virtual void get_orgStateInfo(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, get_orgStateInfoReply_t _reply) override;
    virtual void SM_SetReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid_req, std::string _key, std::string _value) override;
	virtual void SM_SetState(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid_req, std::string _key, std::string _value) override;

    // IME-API
    virtual void openIME(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, uint32_t _keyboardType, uint32_t _langType, std::string _paramStr, openIMEReply_t _reply) override;
    virtual void closeIME(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, closeIMEReply_t _reply) override;

	// popup-API
    virtual void sendPopupInfo(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, uint32_t _popupType, std::string _popupInfo, uint32_t _scrIdx, sendPopupInfoReply_t _reply);
    virtual void showPopup(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _popupHandle, showPopupReply_t _reply);
    virtual void closePopup(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _popupHandle, closePopupReply_t _reply);

    virtual void popupRespond(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _popinfoId, uint32_t _popRespondType, popupRespondReply_t _reply);
    virtual void popupRespondEx(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _popinfoId, uint32_t _popRespondType, std::string _popRespondMessage, popupRespondExReply_t _reply);

	//audio-API
    virtual void requestAudioDeviceFocusReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _connectID);
    virtual void releaseAudioDeviceFocusReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _connectID);
    virtual void setStreamMuteStateReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _streamID, bool _isMute);
    virtual void requestToUseSpeakerReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seatID);
    virtual void abandonToUseSpeakerReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seatID);
    virtual void getLastMediaAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint8_t _audioDevID, getLastMediaAppReqReply_t _reply);
    virtual void requestDuckAudio(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _streamID, bool _isDuck, uint32_t _durationsms, uint32_t _volume);
    virtual void isVolumeTypeActive(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _volumeType, isVolumeTypeActiveReply_t _reply);

    //set-API
    virtual void setByAppManagerReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint16_t _amPID, uint32_t _key, uint32_t _value);
	virtual void AMGRGetState(const std::shared_ptr<CommonAPI::ClientId> _client, uint16_t _amPID, uint32_t _state_type, AMGRGetStateReply_t _reply) override;

    // title bar API
    virtual void ExpandTitlebarReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _titlebarID);
    virtual void ShrinkTitlebarReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _titlebarID);
    virtual void SetTitlebarVisibilityReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _titlebarID, bool _isVisible, int32_t _style);
    virtual void GetTitlebarVisibilityReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _requester, GetTitlebarVisibilityReqReply_t _reply);
	virtual void MarkTitlebarDropDownStateReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _titlebar, bool _isDropDown) override;
	virtual void DropDownTitlebarReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _requester, bool _isVisible, uint32_t _style) override;
	virtual void GetTitlebarDropDownStateReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _requester, GetTitlebarDropDownStateReqReply_t _reply) override;

    //appClient debug API
    virtual void DebugReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _debugCmd);

    // app synchronize API
    virtual void StartShareAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _app, uint32_t _seat, bool isMove) override ;
    virtual void StopShareAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _app, uint32_t _seat, StopShareAppReqReply_t _reply)override ;
	virtual void GetCurrentShareReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seat, GetCurrentShareReqReply_t _reply)override;
	virtual void GetSourceSurfacesReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _app, GetSourceSurfacesReqReply_t _reply)override;
    virtual void GetWorkingLayersReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _app, GetWorkingLayersReqReply_t _reply)override;
    virtual void KeepAppAlive(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t __ampid, bool __state);
    
    void updateState(uint32_t state_type,std::string value);
private:
    std::map<int, std::string> mStatesMap;
};


#endif // CAPI_STUBIMPL_H
