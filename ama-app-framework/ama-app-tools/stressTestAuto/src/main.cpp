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

#include "tools.h"
#include "appSDKTestCase.h"
#include "appSDKtestInterface.h"

#include "TestAPI.h"

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
void testAllAPI(void);
void testAppSwitch(void);
void testPopupAPI(void);
void testRadomAppStartStop(void);
void testAppStartStop(void);
void api_Mock(void);


std::vector<Command> commandList = {
    {"-h",     		            help,          		    "-h"},
	{"help",     		        help,          		    "help"},
	{"quit",     		        quit,          		    "quit"},
	{"api_Mock",   		    	api_Mock,     			"api_Mock    <AMPID>"},
	{"testAllAPI",   		    testAllAPI,     		"testAllAPI    <test count>"},
	{"testAppSwitch",   		testAppSwitch,     		"testAppSwitch    <test count>"},
	{"testRadomAppStartStop",   testRadomAppStartStop,  "testRadomAppStartStop    <test count> <interval time(ms)>"},
	{"testAppStartStop",   		testAppStartStop,     	"testAppStartStop    <ampid> <test count> <interval time(ms)>"},
	{"testPopupAPI",   			testPopupAPI,     		"testPopupAPI    <test count>"},
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


int main(int argc, char* argv[])
{
	//block console output
    std::fstream outFile("/tmp/stressTestAutoOutput.txt");
    auto stdOut = std::cout.rdbuf(outFile.rdbuf());

    // initialize AppSDK
    appSDKTestInit();

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


void testAllAPI(void)
{
	int pidOfAppMgr = 0;
	int testCaseNum = 0;
	int isAppMgrExist = 0;

	int testCnt = 0;
    try{
        testCnt = std::stoul(args.at(1), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

	pidOfAppMgr = getAppMgrPid_t();
	std::cout << "pid of ama-app-manaer is " << pidOfAppMgr <<  std::endl;

	loadXXXModuleTestCase_t(E_LOAD_ALL_MODULE_TEST_CASE);

	testCaseNum = getAppSDKTestCaseNum_t();

	std::cout << "start all apps first!" <<  std::endl;
	startAllApp();

	std::cout << "test case num is " << testCaseNum <<  std::endl;
	std::cout << "call every test case first!" <<  std::endl;
	int caseIndex = 0;
	while(caseIndex < testCaseNum)
	{
	    runAppSDKTestCaseByIndex_t(caseIndex);
	    caseIndex = caseIndex + 1;
	}

	int runRandomTime = 0;
	int ret = 0;
	while(runRandomTime < testCnt)
	{
	    caseIndex = getRand(0, (testCaseNum-1));
		std::cout << "test rand caseIndex:" << caseIndex << std::endl;
		if(!runAppSDKTestCaseByIndex_t(caseIndex)){
			std::cout << "ama-app-manager crash! " <<  std::endl;
			break;
		}
	    runRandomTime = runRandomTime + 1;
	}

	std::cout << "end of test : " << runRandomTime <<  std::endl;
}

void testAppSwitch(void)
{
	int pidOfAppMgr = 0;

	int testCnt = 0;
    try{
        testCnt = std::stoul(args.at(1), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

	pidOfAppMgr = getAppMgrPid_t();
	std::cout << "pid of ama-app-manaer is " << pidOfAppMgr <<  std::endl;

	if(!testCase_StartEVApp(testCnt)){
		std::cout << "ama-app-manager crash! " <<  std::endl;
	}

	std::cout << "end of test : " <<  std::endl;
}

void testRadomAppStartStop(void)
{
	int testCnt = 0;
	int intervalTime = 1000;
    try{
        testCnt = std::stoul(args.at(1), 0, 0);
		intervalTime = std::stoul(args.at(2), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

	int pidOfAppMgr = getAppMgrPid_t();
	std::cout << "pid of ama-app-manaer is " << pidOfAppMgr <<  std::endl;

	if(!testCase_StartStopRadomApp(testCnt,intervalTime)){
		std::cout << "ama-app-manager crash! " <<  std::endl;
	}

	std::cout << "end of test : " <<  std::endl;
}

void testAppStartStop(void)
{
	int ampid = 0;
	int testCnt = 0;
	int intervalTime = 1000;
    try{
		ampid = std::stoul(args.at(1), 0, 0);
        testCnt = std::stoul(args.at(2), 0, 0);
		intervalTime = std::stoul(args.at(3), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

	int pidOfAppMgr = getAppMgrPid_t();
	std::cout << "pid of ama-app-manaer is " << pidOfAppMgr <<  std::endl;

	if(!testCase_StartStopApp(ampid,testCnt,intervalTime)){
		std::cout << "ama-app-manager crash! " <<  std::endl;
	}

	std::cout << "end of test : " <<  std::endl;
}

void testPopupAPI(void)
{
	int pidOfAppMgr = 0;
	int testCaseNum = 0;
	int isAppMgrExist = 0;

	int testCnt = 0;
    try{
        testCnt = std::stoul(args.at(1), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

	pidOfAppMgr = getAppMgrPid_t();
	std::cout << "pid of ama-app-manaer is " << pidOfAppMgr <<  std::endl;

	loadOnlyPopupAPITestCase();

	testCaseNum = getAppSDKTestCaseNum_t();

	std::cout << "test all type popup " <<  std::endl;
	testCase_psShowAllTypePopup();

	std::cout << "test case num is " << testCaseNum <<  std::endl;
	std::cout << "call every test case first!" <<  std::endl;
	int caseIndex = 0;
	while(caseIndex < testCaseNum)
	{
	    runAppSDKTestCaseByIndex_t(caseIndex);
	    caseIndex = caseIndex + 1;
	}

	int runRandomTime = 0;
	int ret = 0;
	while(runRandomTime < testCnt)
	{
	    caseIndex = getRand(0, (testCaseNum-1));
		std::cout << "test rand caseIndex:" << caseIndex << std::endl;
		if(!runAppSDKTestCaseByIndex_t(caseIndex)){
			std::cout << "ama-app-manager crash! " <<  std::endl;
			break;
		}
	    runRandomTime = runRandomTime + 1;
	}

	std::cout << "end of test : " << runRandomTime <<  std::endl;
}

void api_Mock(void)
{
	int ampid = 0;
    try{
        ampid = std::stoul(args.at(1), 0, 0);
    }catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }

	Mock(ampid);
}
