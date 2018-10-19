/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <list>

#include "TestAPI.h"
#include "DoubleClickScannerAPI.h"
#include "AppSDK.h"
#include "HomePLC.h"
#include "COMMON/NotifyWaiter.h"
#include "log.h"
#include "AppDB.h"
#include "AppIS.h"

using namespace std;


void OnShowNotify(const unsigned int& val1)
{
	std::cout << "show app: 0x" << std::hex  << val1 << std::endl;
}

void OnHideNotify(const unsigned int& val1)
{
    std::cout << "hide app: 0x" << std::hex  << val1 << std::endl;
}

void OnStopNotify(const unsigned int &val1, const std::string &stopCMD)
{
    std::cout << "stop app: 0x" << std::hex  << val1 << " stop command:" << stopCMD << std::endl;
}

void OnInitializeNotify(const unsigned int &val1, const int &startArg, const bool &isVsible)
{
    std::cout << "app: 0x" << std::hex  << val1 << " started. " << "start argument: " << startArg << " visibility: " << isVsible << std::endl;
}

void OnIntentNotify(unsigned int app, const std::string& action, const std::string& data)
{
    std::cout << "intent app: 0x" << std::hex << app << ", action: " << action << ", data: " << data << std::endl;
}

//std::shared_ptr<AppMgrProxy<>> myProxy;
// -------------------------------------------------------------
struct Command
{
	Command(std::string cmd, std::function<void(void)> f, std::string info) {
		cmdStr = cmd; func = f; this->info = info;
	}
	std::string cmdStr;
	std::function<void(void)> func;
	std::string info;
};

bool quitFlag = true;
std::vector<std::string> args;

void help(void) ;
void quit(void) ;
void intentReq();
void startReq(void) ;
void stopReq(void);
void hideReq(void);
void getAMPID();
void debug(void);
void showDebugCmdList(void);
void screenShare(void);
void stopScreenShare(void);
void getShareApp(void);
void enableTouchApp(void);
void enableChildSafetyLock(void);
void enableTouchScreen(void);
void getLastMediaApp(void);
void playRadarSound(void);
void lightUpScreen(void);
void setUnderLayerCenter(void);
void isUnderLayerCenter(void);
void getAppPKGInfo(void);
void getInstalledApp(void);
void getAppStartupScreen(void);
void api_reqBackdoorPromptSound(void);
void api_reqPromptSound(void);
void setState(void);
void getState(void);
void setSysLang(void);
void getSysLang(void);
void watchAppStatus(void);
void getAppStatus(void);
void getActiveAppList(void);
void powerSDKTest(void);
void api_reqToPlayRadarSound(void);
void api_Mock(void);
void api_requestPowerAwakeByApp(void);
void api_abandonPowerAwakeByApp(void);
void api_requestScreenAwakeByApp(void);
void api_abandonScreenAwakeByApp(void);
void api_requestAbnormalAwakeByApp(void);
void api_abandonAbnormalAwakeByApp(void);
void api_duckAudio(void);
void api_unduckAudio(void);
void test_createStream(void);
void test_requestAudioFocus(void);
void test_abandonAudioFocus(void);
void dontPopApp(void);
void keepScreenOn(void);
void api_requestPowerGoodbye(void);
void test_appDB(void);
void test_appDBCache(void);
void test_appDBLarge(void);
void test_appState(void);
void api_startPlayAVMAlertSound(void);
void api_stopPlayAVMAlertSound(void);
void api_KeepAppAlive(void);
void test_radarSound(void);
void test_allSound(void);
void api_setVolume(void);


std::vector<Command> commandList = {
    {"-h",     		            help,          		    "-h"},
	{"help",     		        help,          		    "help"},
	{"quit",     		        quit,          		    "quit"},
	{"intent",   		        intentReq,     		    "intent    <Item ID> [<action> <data>] [<quiet mode(1/0)>]"},
    {"start",    		        startReq,      		    "start     <AMPID> "},
	{"stop",     		        stopReq,       		    "stop      <AMPID> "},
	{"hide",     		        hideReq,       		    "hide      <AMPID> "},
	{"showDebugCmdList",        showDebugCmdList,       "showDebugCmdList description:show all debug command"},
    {"screenShare",             screenShare,            "screenShare <source app> <destination screen> [<isShowMoveAnimation(1/0)>]"},
    {"stopScreenShare",         stopScreenShare,        "stopScreenShare <source app>"},
    {"getShareApp",             getShareApp,            "getShareApp <seat id>"},
    {"enableTouchApp",          enableTouchApp,         "enableTouchApp <AMPID> <isTouchable(1/0)>"},
    {"enableChildSafetyLock",   enableChildSafetyLock,  "enableChildSafetyLock <seat id> <isEnable(1/0)> [<png path to show>]"},
    {"lightUpScreen", 			lightUpScreen,      	"lightUpScreen <screen id(2 3)>"},
    {"setUnderLayerCenter", 	setUnderLayerCenter,    "setUnderLayerCenter <AMPID> <isCenter(1/0)>"},
    {"isUnderLayerCenter", 		isUnderLayerCenter,     "isUnderLayerCenter <AMPID>"},
    {"getAppPKGInfo",           getAppPKGInfo,          "getAppPKGInfo <AMPID>"},
    {"getInstalledApp",         getInstalledApp,        "getInstalledApp <home AMPID>"},
	{"api_reqBackdoorPromptSound",api_reqBackdoorPromptSound, "request to sound backdoor prompt"},
	{"api_reqPromptSound",		api_reqPromptSound, 	"request to sound normal prompt sound"},
    {"setState",                setState,               "setState <key> <value>"},
    {"getState",                getState,               "getState <key>"},
    {"setSysLang",              setSysLang,             "setSysLang <system language (C/zh_CN)>"},
    {"getSysLang",              getSysLang,             "getSysLang"},
    {"watchAppStatus",          watchAppStatus,         "watchAppStatus"},
    {"getAppStatus",            getAppStatus,           "getAppStatus"},
    {"getActiveAppList",        getActiveAppList,       "getActiveAppList"},
	{"powerSDKTest",        	powerSDKTest,       	"powerSDKTest"},
	{"api_reqToPlayRadarSound", api_reqToPlayRadarSound,"api_reqToPlayRadarSound <radarSoundType> <radarChannel>"},
	{"api_Mock",        		api_Mock,       		"api_Mock"},
	{"api_requestPowerAwakeByApp",api_requestPowerAwakeByApp,"api_requestPowerAwakeByApp"},
	{"api_abandonPowerAwakeByApp",api_abandonPowerAwakeByApp,"api_abandonPowerAwakeByApp"},
	{"api_abandonScreenAwakeByApp",api_abandonScreenAwakeByApp,"api_abandonScreenAwakeByApp"},
	{"api_requestScreenAwakeByApp",api_requestScreenAwakeByApp,"api_requestScreenAwakeByApp"},
    {"api_requestAbnormalAwakeByApp",api_requestAbnormalAwakeByApp,"api_requestAbnormalAwakeByApp"},
	{"api_abandonAbnormalAwakeByApp",api_abandonAbnormalAwakeByApp,"api_abandonAbnormalAwakeByApp"},
	{"api_duckAudio",        	api_duckAudio,       	"api_duckAudio"},
	{"api_unduckAudio",        	api_unduckAudio,       	"api_unduckAudio"},
	{"test_createStream",  		test_createStream, 		"test_createStream"},
	{"test_requestAudioFocus",  test_requestAudioFocus, "test_requestAudioFocus"},
	{"test_abandonAudioFocus",  test_abandonAudioFocus, "test_abandonAudioFocus"},
    {"dontPopApp",              dontPopApp,             "dontPopApp <AMPID>"},
    {"keepScreenOn",            keepScreenOn,           "keepScreenOn <seat id> [<isAlwaysOn (0/1)>]"},
	{"api_requestPowerGoodbye", api_requestPowerGoodbye,"api_requestPowerGoodbye"},
	{"test_appDB", 				test_appDB,				"test_appDB"},
	{"test_appDBCache", 		test_appDBCache,		"test_appDBCache"},
	{"test_appDBLarge", 		test_appDBLarge,		"test_appDBLarge"},
	{"test_appState", 			test_appState,			"test_appState"},
	{"api_startPlayAVMAlertSound",api_startPlayAVMAlertSound,"api_startPlayAVMAlertSound"},
	{"api_stopPlayAVMAlertSound", api_stopPlayAVMAlertSound,"api_stopPlayAVMAlertSound"},
    {"api_KeepAppAlive", 		api_KeepAppAlive,		"api_KeepAppAlive  <itemid>  <status>"},
	{"test_radarSound", 		test_radarSound,		"test_radarSound"},
	{"test_allSound", 			test_allSound,			"test_allSound"},
	{"api_setVolume", 			api_setVolume,			"api_setVolume <volumeType> <volume>"},
};

void splitString(std::string s,char splitchar, std::vector<std::string> & vec)
{
	if(vec.size()>0)
   		vec.clear();
	int length = s.length();
	int start=0;
	for(int i=0;i<length;i++)
	{
   		if(s[i] == splitchar && i == 0)
   		{
    		start += 1;
   		}
   		else if(s[i] == splitchar)
   		{
    		vec.push_back(s.substr(start,i - start));
    		start = i+1;
   		}
   		else if(i == length-1)
   		{
    		vec.push_back(s.substr(start,i+1 - start));
   		}
	}
}

void InteractiveMode();
void CommandMode(int argc, char **argv);
void ExecuteCMD();

// -------------------------------------------------------------

int main(int argc, char* argv[])
{
    //block console output
    std::fstream outFile("/tmp/tmpOutput.txt");
    auto stdOut = std::cout.rdbuf(outFile.rdbuf());

    // initialize AppSDK
    AppSDKInit();

	informingSoundInit();

    // recover console output
    std::cout.rdbuf(stdOut);

    if(argc==1)
        InteractiveMode();
    else
        CommandMode(argc, argv);

    return 0;
}

// -----------------------------------------------------

void InteractiveMode()
{
    std::cout << "interactive mode" << std::endl;

    std::string cmd;
    while (quitFlag)
    {
        std::cout << " --> ";
        getline(std::cin, cmd);
        splitString(cmd, ' ', args);
        if (args.empty()) {
            continue;
        }

        ExecuteCMD();

        args.clear();
    }
}

void CommandMode(int argc, char **argv)
{
    if(argc < 2)
        return;

    std::cout << "command mode" << std::endl;

    args.clear();
    for(int i=1; i<argc; ++i)
        args.push_back(argv[i]);

    ExecuteCMD();

    args.clear();
}

void ExecuteCMD()
{
    std::cout << "executing command..." << std::endl;

    auto it = std::find_if(begin(commandList), end(commandList), [&](const Command & cmd) {
        return cmd.cmdStr == args.at(0);
    });
    if (it != end(commandList))
    {
        it->func();
        std::cout << "command executed." << std::endl;
    }
    else
        std::cerr << " command not found!" << std::endl;
}

void help(void)
{
	std::cout << "All commands : " << std::endl;
	for (auto & command : commandList) {
		std::cout << command.info << std::endl;
    }
}

void quit(void)
{
	quitFlag = false;
}

void intentReq()
{
    // get intent information
    std::string itemID;
    std::string action;
    std::string data;
    bool isQuiet = false;
    try
    {
        itemID = args.at(1);
        if(args.size()>=3)
            action = args.at(2);
        if(args.size()>=4)
            data = args.at(3);
        if(args.size()>=5)
            isQuiet = args.at(4)=="1";
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    Intent(itemID, action, data, isQuiet);
}

void startReq(void)
{
    // get AMPID
	unsigned int ampid = 0;
    try
    {
    	ampid = std::stol(args.at(1), 0, 0);
	}
	catch (std::out_of_range & e)
	{
		std::cerr << "argument error " << std::endl;
	}

    // start app
    StartApp(ampid, 0);
}

void stopReq(void)
{
	int ampid = 0;
    try
    {
        ampid = std::stol(args.at(1), 0, 0);
	}
	catch (std::out_of_range & e)
	{
		std::cerr << "argument error " << std::endl;
	}

    StopApp(ampid);
}

void hideReq(void)
{
	int ampid = 0;
    try
    {
        ampid = std::stol(args.at(1), 0, 0);
	}
	catch (std::out_of_range & e)
	{
		std::cerr << "argument error " << std::endl;
	}

    HideApp(ampid);
}

void showDebugCmdList(void)
{
	//DebugCmdMap is defined in ama_types.h
	//you can add your debug command in ama_types.h too.
	std::map<std::string, E_DEBUG_CMD>::iterator it;
	for(it=DebugCmdMap.begin();it!=DebugCmdMap.end();++it){
		std::cout << it->first << std::endl;
	}
}

void screenShare(void)
{
    unsigned int app = 0;
    unsigned int seat = 0;
    bool isMove = true;
    try
    {
        app = std::stoul(args.at(1), 0, 0);
        seat = std::stoul(args.at(2), 0, 0);
        if(args.size() >= 4)
            isMove = args.at(3)=="1";
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    StartShareApp(app, static_cast<E_SEAT>(seat), isMove);
}

void stopScreenShare(void)
{
    unsigned int app = 0;
    unsigned int seat = 0;
    try
    {
        app = std::stoul(args.at(1), 0, 0);
        seat = std::stoul(args.at(2), 0, 0);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    StopShareApp(app, static_cast<E_SEAT>(seat));
}

void getShareApp(void)
{
    unsigned int app = 0;
    unsigned int seat = 0;
    try
    {
        seat = std::stoul(args.at(1), 0, 0);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    app = GetCurrentShare(static_cast<E_SEAT>(seat));

    std::cout << "seat: " << seat << " sharing app: 0x" << std::hex << app << std::endl;
}

void enableTouchApp(void)
{
    unsigned int app = 0;
    bool enable = true;
    try
    {
        app = std::stoul(args.at(1), 0, 0);
        enable = std::stoul(args.at(2), 0, 0)==1;
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    auto originalAMPID = GetAMPID();
    Mock(app);
    EnableTouch(enable);
    Mock(originalAMPID);
}

void enableChildSafetyLock(void)
{
    unsigned int seat = 0;
    bool enable = true;
    std::string pngPath;
    try
    {
        seat = std::stoul(args.at(1), 0, 0);
        enable = std::stoul(args.at(2), 0, 0)==1;
        if(args.size()>=4)
            pngPath = args.at(3);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    setByAppManager(E_IsOpenKidsMode, enable);
}

void lightUpScreen()
{
    unsigned int seat = 0;
    try{
        seat = std::stoul(args.at(1), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

    LightUpScreen(static_cast<E_SEAT>(seat));
}

void setUnderLayerCenter(void)
{
    unsigned int ampid = 0;
    unsigned int isCenter = 0;
    try{
        ampid = std::stoul(args.at(1), 0, 0);
        isCenter = std::stoul(args.at(2), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

    auto originalAMPID = GetAMPID();
    Mock(ampid);
    SetUnderLayerCenter(isCenter);
    Mock(originalAMPID);
}

void isUnderLayerCenter(void)
{
    unsigned int ampid = 0;
    bool isCenter = false;
    try
    {
        ampid = std::stol(args.at(1), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

    auto originalAMPID = GetAMPID();
    Mock(ampid);
    isCenter = IsUnderLayerCenter();
    Mock(originalAMPID);
    std::cout << "app 0x" << std::hex << ampid << "'s under-layer is " << (isCenter?"center":"not center") << std::endl;
}

void getAppPKGInfo(void)
{
    unsigned int ampid = 0;
    try
    {
        ampid = std::stol(args.at(1), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

    auto pkg = GetAppPKGInfoEx(ampid);
    std::cout << "app 0x" << std::hex << ampid << "'s status: " << std::endl;
    std::cout << "ItemID    = " << pkg.GetItemID() << std::endl;
    std::cout << "AppName   = " << pkg.GetAppName() << std::endl;
    std::cout << "AppType   = " << pkg.Get("AppType") << std::endl;
    std::cout << "BIcon     = " << pkg.GetBigIconPath() << std::endl;
    std::cout << "SIcon     = " << pkg.GetSmallIconPath() << std::endl;
}

void getInstalledApp(void)
{
    unsigned int ampid = 0;
    try
    {
        ampid = std::stol(args.at(1), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

    auto originalAMPID = GetAMPID();
    Mock(ampid);
    auto appList = GetAppListEx();
    Mock(originalAMPID);
    std::cout << "seat " << GET_SEAT(ampid) << "'s installed app: " << std::endl;
    for(auto& app: appList)
        std::cout << "app AMPID = 0x" << std::hex << app.GetAmpid() << std::endl;
}

void api_reqBackdoorPromptSound(void)
{
	unsigned int ampid = 0x1001;

	auto originalAMPID = GetAMPID();
    Mock(ampid);

	reqBackdoorPromptSound();

    Mock(originalAMPID);
}

void api_reqPromptSound(void)
{
	unsigned int ampid = 0x1001;

	auto originalAMPID = GetAMPID();
    Mock(ampid);

	reqPromptSound();

    Mock(originalAMPID);
}

static std::string key;
static std::string value;
static void OnStateChanged(uint16_t ampid_req, uint16_t ampid_reply, std::string  key,  std::string  value)
{
    std::cout << "SM: requester: " << ToHEX(ampid_req) << " maintainer: " << ToHEX(ampid_reply) << " state: " << key << " value: " << value << std::endl;
    if(key==::key && value==::value)
    {
        std::cout << key << " is set to " << value << std::endl;
        Notify(ampid_req);
    }
}

static void _setState(const std::string& key, const std::string& value)
{
    ::key = key;
    ::value = value;

    SM_regCb4StateChanged(OnStateChanged);
    RegisterWaiterID(GetAMPID());
    SM_req2SetState(key, value);

    int i = 0;
    const int maxRetry = 4;
    while(!Wait(GetAMPID(), 500))
    {
        ++i;
        std::cout << "wait timeout, retry ("<< i << ") ..." << std::endl;
        if(i >= maxRetry)
        {
            std::cerr << "too much retry, operation canceled." << std::endl;
            break;
        }
    }

    UnregisterWaiterID(GetAMPID());
}

void setState(void)
{
    std::string key;
    std::string value;
    try
    {
        key = args.at(1);
        value = args.at(2);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

    _setState(key, value);
}

void getState(void)
{
    std::string key;
    std::string value;
    try
    {
        key = args.at(1);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

    value = SM_getState(key);

    std::cout << "state " << key << "'s value = " << value << std::endl;
}

static const std::string languageKey = "syssetting_SYS_LANG";

void setSysLang(void)
{
    std::string sysLang;
    try
    {
        sysLang = args.at(1);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

    _setState(languageKey, sysLang);
}

void getSysLang(void)
{
    std::string value = SM_getState(languageKey);

    std::cout << "system language is: " << value << std::endl;
}

void OnAppStatusChange(const AMPID app, const AppStatus status)
{
    std::cout << "app: 0x" << std::hex << app << " status: " << std::hex << status << " visibility: " << IS_VISIBLE(status) << " availability: " << IS_AVAILABLE(status) << std::endl;
}

void watchAppStatus(void)
{
    ListenToAppStateNotify(OnAppStatusChange);
}

void getAppStatus(void)
{
    AMPID app = INVALID_AMPID;
    try
    {
        app = std::stol(args.at(1), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

    AppStatus status = GetAppState(app);
    std::cout << "app 0x" << std::hex << app << " visibility: " << IS_VISIBLE(status) << " availability: " << IS_AVAILABLE(status) << std::endl;
}

void getActiveAppList(void)
{
    auto list = GetActiveAppList();
    std::cerr << "active apps are as follows:" << std::endl;
    for(auto& app: list)
        std::cout << "AMPID: 0x" << std::hex << app << std::endl;
}

void powerStateChangedCallback(uint8_t seatID,ama_PowerState_e powerState)
{
	printf("changeOfPowerStaChangedCB-->>seatID:%d,powerState:%x \n",seatID,powerState);
}

void changeOfPowerStaChangedCallback(uint8_t seatID,ama_changeOfPowerSta_e changeOfPowerSta)
{
	printf("changeOfPowerStaChangedCB-->>seatID:%d,changeOfPowerSta:%d \n",seatID,changeOfPowerSta);
}

void PowerStateChangeNotifyCallback(bool isPowerOn)
{
	printf("PowerStateChangeNotifyCallback-->>isPowerOn:%d \n",isPowerOn);
}

void ACCStateChangeNotifyCallback(bool isAccOn)
{
	printf("ACCStateChangeNotifyCallback-->>isAccOn:%d \n",isAccOn);
}

void PowerStateChangeExNotifyCallback(ama_PowerState_e powerState,unsigned int seatID)
{
	printf("PowerStateChangeExNotifyCallback-->>seatID:%d,powerState:%x \n",static_cast<int>(seatID),powerState);
}

void changeOfPowerStateCallback(ama_changeOfPowerSta_e changeOfPowerSta,unsigned int seatID)
{
	printf("changeOfPowerStateCallback-->>seatID:%d,changeOfPowerSta:%d \n",static_cast<int>(seatID),changeOfPowerSta);
}

void powerSDKTest(void)
{
	std::cout << "powerSDKInit-->>"<< std::endl;
	powerSDKInit();

	checkIsPowerOn(E_SEAT_IVI);
	checkIsACCOn(E_SEAT_IVI);
	getPowerState(E_SEAT_IVI);

	registerPowerStateChangedCB(powerStateChangedCallback);
	registerChangeOfPowerStaChangedCB(changeOfPowerStaChangedCallback);

	isPowerOn(E_SEAT_IVI);
	isACCOn(E_SEAT_IVI);

	ListenToPowerStateChangeNotify(PowerStateChangeNotifyCallback);
	ListenToACCStateChangeNotify(ACCStateChangeNotifyCallback);
	ListenToPowerStateChangeExNotify(PowerStateChangeExNotifyCallback);
	ListenToChangeOfPowerStateNotify(changeOfPowerStateCallback);
}

void api_reqToPlayRadarSound(void)
{
    uint8_t radarSoundType = 0;
	uint16_t radarChannel = 0;
    try{
        radarSoundType = std::stoul(args.at(1), 0, 0);
        radarChannel = std::stoul(args.at(2), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

	reqToPlayRadarSound(radarSoundType,radarChannel);
}

void api_Mock(void)
{
	uint32_t ampid = 0;
    try{
        ampid = std::stoul(args.at(1), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

	Mock(ampid);
}

void api_requestPowerAwakeByApp(void)
{
	requestPowerAwakeByApp(true);
}

void api_abandonPowerAwakeByApp(void)
{
	requestPowerAwakeByApp(false);
}

void api_requestScreenAwakeByApp(void)
{
	requestScreenAwakeByApp(true);
}

void api_abandonScreenAwakeByApp(void)
{
	requestScreenAwakeByApp(false);
}

void api_requestAbnormalAwakeByApp(void)
{
	requestAbnormalAwakeByApp(true);
}

void api_abandonAbnormalAwakeByApp(void)
{
	requestAbnormalAwakeByApp(false);
}



void api_duckAudio(void)
{
	duckAudio(500,20);
}

void api_unduckAudio(void)
{
	unduckAudio(500);
}

void audioFocusChanged(ama_audioDeviceType_t audioDevice,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID)
{
	std::cout << "audioFocusChanged(audioDevice:" << audioDevice << "newFocusSta:" << newFocusSta << "applyStreamID:"<< applyStreamID << std::endl;
}

static AudioStream* spTestMusicStream = NULL;
void test_createStream(){

	std::cout << "call createAudioStream()" << std::endl;
	Mock(0x1007);
	spTestMusicStream = createAudioStream(E_STREAM_MUSIC);
}

void test_requestAudioFocus(void)
{
	if(NULL==spTestMusicStream){
		std::cout << "error:you havn't create audioStream yet!" << std::endl;
		return;
	}

	std::cout << "call streamID()" << std::endl;
	ama_streamID_t streamID = spTestMusicStream->streamID();
	std::cout << "return streamID:"<< streamID << std::endl;

	std::cout << "call streamIDStr()" << std::endl;
	string streamStr = spTestMusicStream->streamIDStr();
	std::cout << "return streamStr:"<< streamStr << std::endl;

	std::cout << "call requestAudioFocus()" << std::endl;
	ama_focusSta_t focusSta = spTestMusicStream->requestAudioFocus(audioFocusChanged);
	std::cout << "return focusSta:"<< focusSta << std::endl;
	sleep(2);

	std::cout << "call getFocusState()" << std::endl;
	focusSta = spTestMusicStream->getFocusState(E_IVI_SPEAKER);
	std::cout << "return focusSta:"<< focusSta << std::endl;

	sleep(2);

	std::cout << "call requestAudioFocus()" << std::endl;
	focusSta = spTestMusicStream->requestAudioFocus(audioFocusChanged);
	std::cout << "return focusSta:"<< focusSta << std::endl;
	sleep(2);

	std::cout << "call setStreamMuteStateSync(true)" << std::endl;
	spTestMusicStream->setStreamMuteState(true);

	sleep(2);

	std::cout << "call setStreamMuteStateSync(false)" << std::endl;
	spTestMusicStream->setStreamMuteState(false);
}

void test_abandonAudioFocus(void)
{
	if(NULL==spTestMusicStream){
		std::cout << "error:you havn't create audioStream yet!" << std::endl;
		return;
	}

	std::cout << "call abandonAudioFocus()" << std::endl;
	ama_focusSta_t focusSta = spTestMusicStream->abandonAudioFocus();
	std::cout << "return focusSta:"<< focusSta << std::endl;

	sleep(2);

	std::cout << "call abandonAudioFocus()" << std::endl;
	focusSta = spTestMusicStream->abandonAudioFocus();
	std::cout << "return focusSta:"<< focusSta << std::endl;
}


void dontPopApp(void)
{
    uint32_t ampid = 0;
    try
    {
        ampid = std::stoul(args.at(1), 0, 0);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    auto originalAMPID = GetAMPID();
    Mock(ampid);

    DontPopMe();

    Mock(originalAMPID);
}

void keepScreenOn(void)
{
    uint32_t seatID = 0;
    bool isAlwaysOn = true;
    try
    {
        seatID = std::stoul(args.at(1), 0, 0);
        if(args.size()>=3)
            isAlwaysOn = std::stoul(args.at(2), 0, 0)==0?false:true;
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    KeepScreenOn(static_cast<E_SEAT>(seatID), isAlwaysOn);
}

void api_requestPowerGoodbye(void)
{
	requestPowerGoodbye();
}

void test_appDB(void)
{
	constructorMap setMap;
	setMap["testKey1"] = "testDefValue1";
	setMap["testKey2"] = "testDefValue2";
	setMap["testKey3"] = "1";
	setMap["testKey4"] = "1";

	keyValueMap getMap;
	int ret;

	AppDatabase musicDB = AppDatabase("music",setMap);

	int getIntValue;
	ret = musicDB.getValueOfKey("testKey3",getIntValue);
	if(E_DB_OK!=ret){
		std::cout << "getValueOfKey(testKey3,getIntValue) failed"<< std::endl;
	}else{
		std::cout << "getIntValue:"<< getIntValue << std::endl;
	}

	bool getBoolValue;
	ret = musicDB.getValueOfKey("testKey4",getBoolValue);
	if(E_DB_OK!=ret){
		std::cout << "getValueOfKey(testKey3,getIntValue) failed"<< std::endl;
	}else{
		std::cout << "getBoolValue:"<< getBoolValue << std::endl;
	}

	musicDB.getAllKeyValue(getMap);
	for(keyValueMap::iterator it=getMap.begin(); it!=getMap.end(); ++it){
		std::cout << "key:"  << it->first << std::endl;
		std::cout << "value:"  << it->second << std::endl;

	}

	ret = musicDB.setValueOfKey("testKey1",string("testValue1"));
	if(E_DB_OK!=ret){
		std::cout << "setValueOfKey(testKey1,testValue1) failed"<< std::endl;
	}

	ret = musicDB.setValueOfKey("testKey2","testValue2");
	if(E_DB_OK!=ret){
		std::cout << "setValueOfKey(testKey2,testValue2) failed"<< std::endl;
	}

	ret = musicDB.setValueOfKey("testKey3",3);
	if(E_DB_OK!=ret){
		std::cout << "setValueOfKey(testKey2,testValue2) failed"<< std::endl;
	}

	ret = musicDB.setValueOfKey("testKey4",false);
	if(E_DB_OK!=ret){
		std::cout << "setValueOfKey(testKey2,testValue2) failed"<< std::endl;
	}

	musicDB.getAllKeyValue(getMap);
	for(keyValueMap::iterator it=getMap.begin(); it!=getMap.end(); ++it){
		std::cout << "key:"  << it->first << std::endl;
		std::cout << "value:"  << it->second << std::endl;
	}

	// musicDB.resetDefValueForAllKey();
	// musicDB.getAllKeyValue(getMap);
	// for(keyValueMap::iterator it=getMap.begin(); it!=getMap.end(); ++it){
	// 	std::cout << "key:"  << it->first << std::endl;
	// 	std::cout << "value:"  << it->second << std::endl;
	//
	// }
}


void test_appDBCache(void)
{
	constructorMap setMap;
	setMap["testKey1"] = "testDefValue1";
	setMap["testKey2"] = "testDefValue2";
	setMap["testKey3"] = "1";
	setMap["testKey4"] = "1";

	keyValueMap getMap;
	int ret;

	AppDatabase musicDB = AppDatabase("music",setMap,true);

	int getIntValue;
	ret = musicDB.getValueOfKey("testKey3",getIntValue);
	if(E_DB_OK!=ret){
		std::cout << "getValueOfKey(testKey3,getIntValue) failed"<< std::endl;
	}else{
		std::cout << "getIntValue:"<< getIntValue << std::endl;
	}

	bool getBoolValue;
	ret = musicDB.getValueOfKey("testKey4",getBoolValue);
	if(E_DB_OK!=ret){
		std::cout << "getValueOfKey(testKey3,getIntValue) failed"<< std::endl;
	}else{
		std::cout << "getBoolValue:"<< getBoolValue << std::endl;
	}

	musicDB.getAllKeyValue(getMap);
	for(keyValueMap::iterator it=getMap.begin(); it!=getMap.end(); ++it){
		std::cout << "key:"  << it->first << std::endl;
		std::cout << "value:"  << it->second << std::endl;

	}

	ret = musicDB.setValueOfKey("testKey1",string("testValue1"));
	if(E_DB_OK!=ret){
		std::cout << "setValueOfKey(testKey1,testValue1) failed"<< std::endl;
	}

	ret = musicDB.setValueOfKey("testKey2","testValue2");
	if(E_DB_OK!=ret){
		std::cout << "setValueOfKey(testKey2,testValue2) failed"<< std::endl;
	}

	ret = musicDB.setValueOfKey("testKey3",3);
	if(E_DB_OK!=ret){
		std::cout << "setValueOfKey(testKey2,testValue2) failed"<< std::endl;
	}

	ret = musicDB.setValueOfKey("testKey4",false);
	if(E_DB_OK!=ret){
		std::cout << "setValueOfKey(testKey2,testValue2) failed"<< std::endl;
	}

	musicDB.getAllKeyValue(getMap);
	for(keyValueMap::iterator it=getMap.begin(); it!=getMap.end(); ++it){
		std::cout << "key:"  << it->first << std::endl;
		std::cout << "value:"  << it->second << std::endl;
	}

	musicDB.commitCacheIntoDB();

	musicDB.resetDefValueForAllKey();
	musicDB.getAllKeyValue(getMap);
	for(keyValueMap::iterator it=getMap.begin(); it!=getMap.end(); ++it){
		std::cout << "key:"  << it->first << std::endl;
		std::cout << "value:"  << it->second << std::endl;

	}
}

void test_appDBLarge(){
	constructorMap setMap;
	for(int i=0;i<100;i++){
		stringstream ss;
		ss<<i;
		string str;
		ss>>str;
		setMap[str] = str;
	}

	keyValueMap getMap;
	int ret;

	AppDatabase musicDB = AppDatabase("test",setMap,true);

	musicDB.getAllKeyValue(getMap);
	for(keyValueMap::iterator it=getMap.begin(); it!=getMap.end(); ++it){
		std::cout << "key:"  << it->first << std::endl;
		std::cout << "value:"  << it->second << std::endl;

	}

	for(int i=0;i<100;i++){
		stringstream ss;
		ss<<i;
		string str;
		ss>>str;
		musicDB.setValueOfKey(str,"1000");
	}

	musicDB.getAllKeyValue(getMap);
	for(keyValueMap::iterator it=getMap.begin(); it!=getMap.end(); ++it){
		std::cout << "key:"  << it->first << std::endl;
		std::cout << "value:"  << it->second << std::endl;
	}

	musicDB.commitCacheIntoDB();

	musicDB.resetDefValueForAllKey();
	musicDB.getAllKeyValue(getMap);
	for(keyValueMap::iterator it=getMap.begin(); it!=getMap.end(); ++it){
		std::cout << "key:"  << it->first << std::endl;
		std::cout << "value:"  << it->second << std::endl;

	}
}

static void appStatusChangedCB(const AMPID ampid,const AppStatus appSta)
{
	std::cout << "ampid:" << ampid << std::endl;
	printf("appSta:%x \n",appSta);
	if(GetAMPID("com.hs7.vr.scr1")==ampid){
		if(!IS_VISIBLE(appSta)){
			std::cout << "vr is invisible" << std::endl;
		}else{
			std::cout << "vr is visible" << std::endl;
		}
	}
}

void test_appState(void)
{
	// ListenToAppStateNotify(appStatusChangedCB);
	//
	// AppStatus appSta = GetAppState(GetAMPID("com.hs7.vr.scr1"));
	// if(IS_VISIBLE(appSta)){
	// 	std::cout << "vr is visible" << std::endl;
	// }else{
	// 	std::cout << "vr is invisible" << std::endl;
	// }

	std::list<shared_ptr<AppAudioStream>> audioList;

	for(int i=0;i<100;i++){
		shared_ptr<AppAudioStream> pAudioStream = std::make_shared<AppAudioStream>(E_STREAM_MUSIC);
		std::cout << "pAudioStream->streamIDStr()" << pAudioStream->streamIDStr() << std::endl;
		audioList.push_back(pAudioStream);
	}
}

void api_startPlayAVMAlertSound(void)
{
	startPlayAVMAlertSound();
}

void api_stopPlayAVMAlertSound(void)
{
	stopPlayAVMAlertSound();
}


void api_KeepAppAlive(void)
{
    uint32_t ampid = 0;

    try
    {
        ampid = std::stoul(args.at(2), 0, 0);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    KeepAppAlive(args.at(1), ampid);

}

void test_radarSound(void)
{
	for(int i=0;i<5;i++){
		std::cout << "call reqToPlayRadarSound(0x1,0x1111)" << std::endl;
		reqToPlayRadarSound(0x1,0x1111);
		std::cout << "sleep 100ms" << std::endl;
		usleep(100*1000);

		std::cout << "call reqToPlayRadarSound(0x2,0x1111)" << std::endl;
		reqToPlayRadarSound(0x2,0x1111);
		std::cout << "sleep 100ms" << std::endl;
		usleep(100*1000);

		std::cout << "call reqToPlayRadarSound(0x4,0x1111)" << std::endl;
		reqToPlayRadarSound(0x4,0x1111);
		std::cout << "sleep 100ms" << std::endl;
		usleep(100*1000);
	}

	// for(int i=0;i<10;i++){
	// 	std::cout << "call reqToPlayRadarSound(0x1,0x1111)" << std::endl;
	// 	reqToPlayRadarSound(0x1,0x1111);
	// 	usleep(100*1000);
	// }
	//
	// for(int i=0;i<10;i++){
	// 	std::cout << "call reqToPlayRadarSound(0x2,0x1111)" << std::endl;
	// 	reqToPlayRadarSound(0x2,0x1111);
	// 	sleep(1);
	// }
	//
	// for(int i=0;i<10;i++){
	// 	std::cout << "call reqToPlayRadarSound(0x4,0x1111)" << std::endl;
	// 	reqToPlayRadarSound(0x4,0x1111);
	// 	sleep(1);
	// }

	// std::cout << "call reqToPlayRadarSound(0x5,0x1111)" << std::endl;
	// reqToPlayRadarSound(0x5,0x1111);
	// sleep(1);
	//
	// std::cout << "call reqToPlayRadarSound(0x6,0x1111)" << std::endl;
	// reqToPlayRadarSound(0x6,0x1111);
	// sleep(1);

	std::cout << "call reqToPlayRadarSound(0x0,0x1111)" << std::endl;
	reqToPlayRadarSound(0x0,0x1111);
	sleep(1);
}

void test_allSound(void)
{
	std::cout << "call playSoftKeySound(E_IVI_SPEAKER)" << std::endl;
	playSoftKeySound(E_IVI_SPEAKER);
	sleep(2);

	std::cout << "call playPromptSound(E_IVI_SPEAKER)" << std::endl;
	playPromptSound(E_IVI_SPEAKER);
	sleep(2);

	std::cout << "call playBackdoorPromptSound()" << std::endl;
	playBackdoorPromptSound();
	sleep(2);

	std::cout << "call reqToPlayRadarSound(0x1,0x1111)" << std::endl;
	reqToPlayRadarSound(0x1,0x1111);
	sleep(5);

	std::cout << "call reqToPlayRadarSound(0x2,0x1111)" << std::endl;
	reqToPlayRadarSound(0x2,0x1111);
	sleep(5);

	std::cout << "call reqToPlayRadarSound(0x4,0x1111)" << std::endl;
	reqToPlayRadarSound(0x4,0x1111);
	sleep(5);

	std::cout << "call reqToPlayRadarSound(0x5,0x1111)" << std::endl;
	reqToPlayRadarSound(0x5,0x1111);
	sleep(2);

	std::cout << "call reqToPlayRadarSound(0x6,0x1111)" << std::endl;
	reqToPlayRadarSound(0x6,0x1111);
	sleep(2);

	std::cout << "call reqToPlayRadarSound(0x0,0x1111)" << std::endl;
	reqToPlayRadarSound(0x0,0x1111);
	sleep(1);

	std::cout << "call startPlayAVMAlertSound()" << std::endl;
	startPlayAVMAlertSound();
	sleep(10);
	std::cout << "call stopPlayAVMAlertSound()" << std::endl;
	stopPlayAVMAlertSound();
}

void api_setVolume(void)
{
	uint32_t volumeType = (uint32_t)(ama_setType_e::E_Key_Max);
	uint32_t volume = 0;
	try{
		volumeType = std::stoul(args.at(1), 0, 0);
		volume = std::stoul(args.at(2), 0, 0);
	}catch (std::out_of_range & e){
		std::cerr << "argument error " << std::endl;
	}

	setVolumeByAppManager((ama_setType_e)volumeType, volume, false);
}