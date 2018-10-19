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

using namespace std;

#ifndef WIN32
#include <unistd.h>
#endif

#include <ama_types.h>
#include <CommonAPI/CommonAPI.hpp>
#include <v0_1/org/neusoft/AppMgrProxy.hpp>

#include "CAPI/CAPINotifyEnum.h"
#include "COMMON/StringOperation.h"


using namespace v0_1::org::neusoft;

std::shared_ptr<v0_1::org::neusoft::AppMgrProxy<>> appManager;

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

void OnAppStatusChangeNotify(unsigned int app, unsigned int status)
{
    std::cout << "app: 0x" << std::hex << app << " visibility: " << IS_VISIBLE(status) << " availability: " << IS_AVAILABLE(status) << std::endl;
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
void lightUpScreen(void);
void setUnderLayerCenter(void);
void isUnderLayerCenter(void);
void getAppPKGInfo(void);
void getInstalledApp(void);
void getAppStartupScreen(void);
void requestAudioFocus(void);
void abandonAudioFocus(void);
void getCurrentSource(void);
void setCurrentSource(void);
void getLastSource(void);
void setTitlebarVisibility(void);
void getTitlebarVisibility(void);
void dropDownTitlebar(void);
void getTitlebarDropDownState(void);
void markTitlebarDropDown(void);
void dontPopApp(void);
void addExtraSurface(void);
void getScreenSize(void);

std::shared_ptr<AppMgrProxy<>> myProxy;
std::vector<Command> commandList = {
    {"-h",     		            help,          		    "-h"},
	{"help",     		        help,          		    "help"},
	{"quit",     		        quit,          		    "quit"},
	{"intent",   		        intentReq,     		    "intent    <Item ID> [<action> <data>] [<quiet mode(1/0)>]"},
    {"start",    		        startReq,      		    "start     <AMPID> "},
	{"stop",     		        stopReq,       		    "stop      <AMPID> "},
	{"hide",     		        hideReq,       		    "hide      <AMPID> "},
    {"getAMPID", 		        getAMPID,      		    "getAMPID"},
	{"debug", 			        debug,      		    "debug <debugCmd>"},
	{"showDebugCmdList",        showDebugCmdList,       "showDebugCmdList description:show all debug command"},
    {"screenShare",             screenShare,            "screenShare <source app> <destination screen> [<isShowMoveAnimation(1/0)>]"},
    {"stopScreenShare",         stopScreenShare,        "stopScreenShare <source app>"},
    {"getShareApp",             getShareApp,            "getShareApp <seat id>"},
    {"enableTouchApp",          enableTouchApp,         "enableTouchApp <AMPID> <isTouchable(1/0)>"},
    {"enableChildSafetyLock",   enableChildSafetyLock,  "enableChildSafetyLock <seat id> <isEnable(1/0)> [<png path to show>]"},
    {"enableTouchScreen",       enableTouchScreen,      "enableTouchScreen <seat id> <isTouchable(1/0)> [<png path to show>]"},
	{"getLastMediaApp", 		getLastMediaApp,      	"getLastMediaApp <audioDeviceID>"},
    {"lightUpScreen", 			lightUpScreen,      	"lightUpScreen <screen id(2 3)>"},
    {"setUnderLayerCenter", 	setUnderLayerCenter,    "setUnderLayerCenter <AMPID> <isCenter(1/0)>"},
    {"isUnderLayerCenter", 		isUnderLayerCenter,     "isUnderLayerCenter <AMPID>"},
    {"getAppPKGInfo",           getAppPKGInfo,          "getAppPKGInfo <AMPID>"},
    {"getInstalledApp",         getInstalledApp,        "getInstalledApp <home AMPID>"},
    {"getAppStartupScreen",     getAppStartupScreen,    "getAppStartupScreen <AMPID>"},
	{"requestAudioFocus",       requestAudioFocus,      "requestAudioFocus <connectID>"},
	{"abandonAudioFocus",       abandonAudioFocus,      "abandonAudioFocus <connectID>"},
    {"getCurrentSource",        getCurrentSource,       "getCurrentSource"},
    {"setCurrentSource",        setCurrentSource,       "setCurrentSource <count> <ItemID~action~data~isQuiet> [...]"},
    {"getLastSource",           getLastSource,          "getLastSource"},
    {"setTitlebarVisibility",   setTitlebarVisibility,  "setTitlebarVisibility <AMPID> <visibility(0/1)> [<style>]"},
    {"getTitlebarVisibility",   getTitlebarVisibility,  "getTitlebarVisibility <AMPID>"},
    {"dropDownTitlebar",        dropDownTitlebar,       "dropDownTitlebar <requester's AMPID> <isDropDown(1/0)> [<style>]"},
    {"getTitlebarDropDownState",getTitlebarDropDownState,"getTitlebarDropDownState <requester's AMPID>"},
    {"markTitlebarDropDown",    markTitlebarDropDown,   "markTitlebarDropDown <titlebar's AMPID>"},
    {"dontPopApp",              dontPopApp,             "dontPopApp <AMPID>"},
    {"addExtraSurface",         addExtraSurface,        "addExtraSurface <surface ID> <layer ID>"},
    {"getScreenSize",           getScreenSize,          "getScreenSize <seat ID>"},
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
    CommonAPI::Runtime::setProperty("LogContext", "APLC");
    CommonAPI::Runtime::setProperty("LibraryBase", "AppMgr");

    string domain = "";
    string instance = "";
    string connection = "";

    // initialize CAPI
    shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
    appManager = runtime->buildProxy<::v0_1::org::neusoft::AppMgrProxy>(domain,instance, connection);
    std::cout << "connecting CAPI server..." << std::endl;
    while (!appManager->isAvailable())
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::cout << "connecting CAPI server OK" << std::endl;

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

    std::cout << "subscribing notify..." << std::endl;
//    appManager->getInitializeNotifyEvent().subscribe(OnInitializeNotify);
//    appManager->getStopNotifyEvent().subscribe(OnStopNotify);
//    appManager->getShowNotifyEvent().subscribe(OnShowNotify);
//    appManager->getHideNotifyEvent().subscribe(OnHideNotify);
//    appManager->getIntentNotifyEvent().subscribe(OnIntentNotify);
    std::cout << "notify's subscribed." << std::endl;

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

    // this synchronized invoke is for waiting for last asynchronized invoke in ExecuteCMD().
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(1000);
    appManager->None(callStatus, &callInfo);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        std::cerr << "synchronize failure!" << std::endl;

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

    // start app
    CommonAPI::CallStatus callStatus;
    appManager->StartAppReq(itemID, action, data, isQuiet, callStatus);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "appStartReq:Remote call failed!\n";
        return ;
    }
}

void startReq(void)
{
    // get AMPID
	unsigned int ampid = 0;
    try
    {
    	ampid = std::stoi(args.at(1), 0, 0);
	}
	catch (std::out_of_range & e)
	{
		std::cerr << "argument error " << std::endl;
	}

    // get item ID
    CommonAPI::CallStatus callStatus;
    AppMgr::AppProperties properties;
    CommonAPI::CallInfo callInfo(1000); // wait forever until server return
    appManager->GetAppPropertiesReq(ampid, callStatus, properties, &callInfo);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        std::cerr << "failed calling GetAppPropertiesReq()!" << std::endl;
        return;
    }

    // start app
	appManager->StartAppReq(properties.getItemID(), "", "", false, callStatus);
	if (callStatus != CommonAPI::CallStatus::SUCCESS) {
		std::cerr << "appStartReq:Remote call failed!\n";
		return ;
	}
}

void stopReq(void)
{
	CommonAPI::CallStatus callStatus;
	int surfaceId;
    try
    {
    	surfaceId = std::stoi(args.at(1), 0, 0);
	}
	catch (std::out_of_range & e)
	{
		std::cerr << "argument error " << std::endl;
	}
	appManager->StopAppReq(surfaceId, callStatus);
	if (callStatus != CommonAPI::CallStatus::SUCCESS) {
		std::cerr << "appStartReq:Remote call failed!\n";
		return ;
	}
}

void hideReq(void)
{
	CommonAPI::CallStatus callStatus;
	int surfaceId;
    try
    {
    	surfaceId = std::stoi(args.at(1), 0, 0);
	}
	catch (std::out_of_range & e)
	{
		std::cerr << "argument error " << std::endl;
	}
	appManager->HideAppReq(surfaceId, callStatus);
	if (callStatus != CommonAPI::CallStatus::SUCCESS) {
		std::cerr << "appStartReq:Remote call failed!\n";
		return ;
	}
}

void getAMPID()
{
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);
    unsigned int ampid = 0;
    appManager->GetAnonymousAMPIDReq(callStatus, ampid, &callInfo);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "GetAnonymousAMPIDReq:Remote call failed!\n";
        return ;
    }
    std::cout << "anonymous AMPID = " << std::hex << ampid << std::endl;
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

void debug(void)
{
	// get debug command
    std::string debugCmdStr;
    try
    {
        debugCmdStr = args.at(1);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

	int debugCmd;
	std::map<std::string, E_DEBUG_CMD>::iterator it;
	it =  DebugCmdMap.find(debugCmdStr);
	if(it != DebugCmdMap.end()){
		debugCmd = DebugCmdMap[debugCmdStr];
		// send dubug command
	    CommonAPI::CallStatus callStatus;
	    appManager->DebugReq(debugCmd, callStatus);
	    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
	        std::cerr << "DebugReq:Remote call failed!\n";
	        return ;
	    }
	}else{
		std::cerr << "debugCmd is not defined!\n";
	}


}

void screenShare(void)
{
    CommonAPI::CallStatus callStatus;
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
    appManager->StartShareAppReq(app, seat, isMove, callStatus);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "StartShareAppReq:Remote call failed!\n";
        return ;
    }
}

void stopScreenShare(void)
{
    CommonAPI::CallStatus callStatus;
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
    appManager->StopShareAppReq(app, seat, callStatus);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "StopShareAppReq:Remote call failed!\n";
        return ;
    }
}

void getShareApp(void)
{
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(1000);
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
    appManager->GetCurrentShareReq(seat, callStatus, app, &callInfo);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "StopShareAppReq:Remote call failed!\n";
        return ;
    }

    std::cout << "seat: " << seat << " sharing app: 0x" << std::hex << app << std::endl;
}

void enableTouchApp(void)
{
    CommonAPI::CallStatus callStatus;
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
    appManager->EnableTouchAppReq(app, enable, callStatus);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "EnableTouchAppReq:Remote call failed!\n";
        return ;
    }
}

void enableChildSafetyLock(void)
{
    CommonAPI::CallStatus callStatus;
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
    appManager->EnableChildSafetyLockReq(seat, enable, pngPath, callStatus);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "EnableChildSafetyLockReq:Remote call failed!\n";
        return ;
    }
}

void enableTouchScreen(void)
{
    CommonAPI::CallStatus callStatus;
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
    appManager->EnableTouchScreenReq(seat, enable, pngPath, callStatus);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "EnableTouchScreenReq:Remote call failed!\n";
        return ;
    }
}

void getLastMediaApp()
{
	unsigned int deviceID = 0;
    try
    {
        deviceID = std::stoi(args.at(1), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

	CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);
    unsigned int ampid = 0;
    appManager->getLastMediaAppReq(deviceID,callStatus, ampid, &callInfo);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "GetAnonymousAMPIDReq:Remote call failed!\n";
        return ;
    }
    std::cout << "lastMedia app = " << std::hex << ampid << std::endl;
}

void lightUpScreen()
{
    CommonAPI::CallStatus callStatus;
    unsigned int seat = 0;
    try{
        seat = std::stoul(args.at(1), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

    appManager->LightUpScreenReq(seat, callStatus);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "lightUpScreen:Remote call failed!\n";
        return ;
    }
}

void setUnderLayerCenter(void)
{
    CommonAPI::CallStatus callStatus;
    unsigned int ampid = 0;
    unsigned int isCenter = 0;
    try{
        ampid = std::stoul(args.at(1), 0, 0);
        isCenter = std::stoul(args.at(2), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

    appManager->SetUnderLayerCenterReq(ampid, isCenter, callStatus);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "setUnderLayerCenter:Remote call failed!\n";
        return ;
    }
}

void isUnderLayerCenter(void)
{
    unsigned int ampid = 0;
    bool isCenter = false;
    try
    {
        ampid = std::stoi(args.at(1), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);
    appManager->IsUnderLayerCenterReq(ampid,callStatus, isCenter, &callInfo);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "isUnderLayerCenter:Remote call failed!\n";
        return ;
    }
    std::cout << "app 0x" << std::hex << ampid << "'s under-layer is " << (isCenter?"center":"not center") << std::endl;
}

void getAppPKGInfo(void)
{
    unsigned int ampid = 0;
    try
    {
        ampid = std::stoi(args.at(1), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

    v0_1::org::neusoft::AppMgr::AppProperties appProperties;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);
    appManager->GetAppPropertiesReq(ampid, callStatus, appProperties, &callInfo);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "getAppPKGInfo:Remote call failed!\n";
        return ;
    }
    std::cout << "app 0x" << std::hex << ampid << "'s status: " << std::endl;
    std::cout << "ItemID    = " << appProperties.getItemID() << std::endl;
    std::cout << "AppName   = " << appProperties.getAppName() << std::endl;
    std::cout << "AppType   = " << appProperties.getAppType() << std::endl;
    std::cout << "BIcon     = " << appProperties.getBigIconPath() << std::endl;
    std::cout << "SIcon     = " << appProperties.getSmallIconPath() << std::endl;
}

void getInstalledApp(void)
{
    unsigned int ampid = 0;
    try
    {
        ampid = std::stoi(args.at(1), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

    std::vector<unsigned int> appList;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);
    appManager->GetInstalledAppReq(ampid, callStatus, appList, &callInfo);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "getInstalledApp:Remote call failed!\n";
        return ;
    }
    std::cout << "seat " << GET_SEAT(ampid) << "'s installed app: " << std::endl;
    for(auto& app: appList)
        std::cout << "app AMPID = 0x" << std::hex << app << std::endl;
}

void getAppStartupScreen(void)
{
    unsigned int ampid = 0;
    try
    {
        ampid = std::stoi(args.at(1), 0, 0);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    bool isShowStartupScreen = true;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);
    appManager->GetStartupScreenAvailibilityReq(ampid, callStatus, isShowStartupScreen, &callInfo);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "getAppStartupScreen:Remote call failed!\n";
        return ;
    }
    std::cout << "app 0x" << std::hex << ampid << "'s startup screen: " << std::boolalpha << isShowStartupScreen << std::endl;
}

void requestAudioFocus(void)
{
	unsigned int connectID = 0;
    try
    {
        connectID = std::stoi(args.at(1), 0, 0);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

	CommonAPI::CallStatus callStatus;
    appManager->requestAudioDeviceFocusReq(connectID, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
		std::cerr << "requestAudioDeviceFocusReq:Remote call failed!\n";
        return ;
    }

	std::cout << "request audio focus for connectID:" << std::hex << connectID << std::endl;
}

void abandonAudioFocus(void)
{
	unsigned int connectID = 0;
    try
    {
        connectID = std::stoi(args.at(1), 0, 0);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

	CommonAPI::CallStatus callStatus;
    appManager->releaseAudioDeviceFocusReq(connectID, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
		std::cerr << "releaseAudioDeviceFocusReq:Remote call failed!\n";
        return ;
    }

	std::cout << "abandon audio focus for connectID:" << std::hex << connectID << std::endl;
}

void getCurrentSource()
{
    v0_1::org::neusoft::AppMgr::IntentInfoArray array;

    CommonAPI::CallStatus callStatus;
    appManager->GetLastSourceReq(callStatus, array);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        std::cerr << "getCurrentSource:Remote call failed!" << std::endl;
        return ;
    }
    else
    {
        std::cout << "current source list:" << std::endl;
        for(auto& i: array)
            std::cout << i.getItemID() << "~" << i.getAction() << "~" << i.getData() << "~" << i.getIsQuiet() << std::endl;
    }
}

void setCurrentSource()
{
    unsigned int count = 0;
    v0_1::org::neusoft::AppMgr::IntentInfoArray array;
    try
    {
        count = std::stoi(args.at(1), 0, 0);
        for(int i=0; i < count; ++i)
        {
            auto record = args.at(2+i);
            auto infos = split(record, "~");
            if(infos.size()<4)
            {
                std::cerr << "infos.size() = " << infos.size() << std::endl;
                for(int i = 0; i< infos.size(); ++i)
                    std::cerr << "infos[" << i << "] = "<< infos[i] << std::endl;

                throw std::out_of_range("~");
            }
            array.push_back(v0_1::org::neusoft::AppMgr::IntentInfo(infos[0], infos[1], infos[2], infos[3]=="0"?false:true));
        }
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    CommonAPI::CallStatus callStatus;
    appManager->SetLastSourceReq(array, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        std::cerr << "setCurrentSource:Remote call failed!\n";
        return ;
    }
}

void getLastSource(void)
{
    v0_1::org::neusoft::AppMgr::IntentInfoArray array;

    CommonAPI::CallStatus callStatus;
    appManager->GetRawLastSourceReq(callStatus, array);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        std::cerr << "getLastSource:Remote call failed!" << std::endl;
        return ;
    }
    else
    {
        std::cout << "last source list:" << std::endl;
        for(auto& i: array)
            std::cout << i.getItemID() << "~" << i.getAction() << "~" << i.getData() << "~" << i.getIsQuiet() << std::endl;
    }
}

void setTitlebarVisibility()
{
    unsigned int app = 0;
    bool isVisible = true;
    int style = 0;
    try
    {
        app = std::stoi(args.at(1), 0, 0);
        isVisible = std::stoi(args.at(2), 0, 0);
        if(args.size() >= 4)
            style = std::stoi(args.at(3), 0, 0);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    CommonAPI::CallStatus callStatus;
    appManager->SetTitlebarVisibilityReq(app, isVisible, style, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        std::cerr << "setTitlebarVisibility:Remote call failed!\n";
        return ;
    }
}

void GeneralNotify(uint32_t func, const std::vector<uint64_t>& u64Args, const std::vector<std::string>& stringArgs)
{
    if(func==E_CAPI_NOTIFY_TITLEBARVISIBILITYSWITCH)
    {
        std::cout << "E_CAPI_NOTIFY_TITLEBARVISIBILITYSWITCH 0x" << std::hex << u64Args[0] << std::dec << " " << u64Args[1] << " " << static_cast<int>(u64Args[2]) << std::endl;
    }
}

void getTitlebarVisibility(void)
{
    unsigned int ampid = 0;
    try
    {
        ampid = std::stoi(args.at(1), 0, 0);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    bool isVisible = true;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);
    appManager->GetTitlebarVisibilityReq(ampid, callStatus, isVisible, &callInfo);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "getTitlebarVisibility:Remote call failed!\n";
        return ;
    }
    std::cout << "titlebar 0x" << std::hex << ampid << "'s visibility: " << std::boolalpha << isVisible << std::endl;

    appManager->getGeneralNotifyEvent().subscribe(GeneralNotify);
}

void dropDownTitlebar(void)
{
    unsigned int app = 0;
    bool isDropDown = true;
    unsigned int style = 0;
    try
    {
        app = std::stoi(args.at(1), 0, 0);
        isDropDown = std::stoi(args.at(2), 0, 0);
        if(args.size() >= 4)
            style = std::stoi(args.at(3), 0, 0);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    CommonAPI::CallStatus callStatus;
    appManager->DropDownTitlebarReq(app, isDropDown, style, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        std::cerr << "dropDownTitlebar:Remote call failed!\n";
        return ;
    }
}

void getTitlebarDropDownState(void)
{
    unsigned int ampid = 0;
    try
    {
        ampid = std::stoi(args.at(1), 0, 0);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    bool isDropDown = true;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);
    appManager->GetTitlebarDropDownStateReq(ampid, callStatus, isDropDown, &callInfo);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "getTitlebarDropDownState:Remote call failed!\n";
        return ;
    }
    std::cout << "titlebar 0x" << std::hex << ampid << "'s drop-down state: " << std::boolalpha << isDropDown << std::endl;
}

void markTitlebarDropDown(void)
{
    unsigned int titlebar = 0;
    bool isDropDown = true;
    try
    {
        titlebar = std::stoi(args.at(1), 0, 0);
        isDropDown = std::stoi(args.at(2), 0, 0);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    CommonAPI::CallStatus callStatus;
    appManager->MarkTitlebarDropDownStateReq(titlebar, isDropDown, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        std::cerr << "markTitlebarDropDown:Remote call failed!\n";
        return ;
    }
}

void dontPopApp(void)
{
    unsigned int app = 0;
    try
    {
        app = std::stoi(args.at(1), 0, 0);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    CommonAPI::CallStatus callStatus;
    appManager->RemoveAppFromZOrderReq(app, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        std::cerr << "dontPopApp: Remote call failed!" << std::endl;
        return ;
    }
}

void addExtraSurface(void)
{
    unsigned int surface = ~0;
    unsigned int layer = ~0;

    try
    {
        surface = std::stoi(args.at(1), 0, 0);
        layer = std::stoi(args.at(2), 0, 0);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);
    appManager->AddExtraSurfaceReq(surface, layer, callStatus, &callInfo);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        std::cerr << "addExtraSurface:Remote call failed!\n";
        return ;
    }
}

void getScreenSize(void)
{
    unsigned int seat = 0;
    unsigned int width = 0;
    unsigned int height = 0;

    try
    {
        seat = std::stoi(args.at(1), 0, 0);
    }
    catch (std::out_of_range & e)
    {
        std::cerr << "argument error " << std::endl;
    }

    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);
    appManager->GetScreenSizeReq(seat, callStatus, width, height, &callInfo);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        std::cerr << "getScreenSize:Remote call failed!\n";
        return ;
    }

    std::cout << "width: " << width << " height: " << height << std::endl;
}
