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

#include "InputEvent.h"
#include "Player.h"
#include "DBHandler.h"

#include "AppSDK.h"

using namespace std;
using namespace ama;

static TouchEvents sTmpTouchEvents;
static TestEvents sTmpTestEvents;

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
void recordTouch(void);
void recordTestCase(void);
void recordTestCaseByFilter(void);
void playTouch(void);
void checkTestCase(void);
void playTouchByFile(void);
void playTouchByFileLoop(void);
void checkTestCaseByFile(void);
void print(void);
void save(void);
void stopRecord(void);
void stopPlay(void);


std::vector<Command> commandList = {
    {"-h",     		            help,          		    "-h"},
	{"help",     		        help,          		    "help"},
	{"quit",     		        quit,          		    "quit"},
    {"showTouchEventDevice",    showTouchEventDevice,   "showTouchEventDevice"},
    {"startPrintInputEvent",    startPrintInputEvent,   "startPrintInputEvent"},
    {"stopPrintInputEvent",     stopPrintInputEvent,    "stopPrintInputEvent"},
	{"recordTouch",    			recordTouch,   			"recordTouch"},
	{"recordTestCase",    		recordTestCase,   		"recordTestCase"},
	{"recordTestCaseByFilter",  recordTestCaseByFilter, "recordTestCaseByFilter"},
	{"playTouch",    			playTouch,   			"playTouch"},
	{"checkTestCase",    		checkTestCase,   		"checkTestCase"},
	{"playTouchByFile",    		playTouchByFile,   		"playTouchByFile"},
	{"playTouchByFileLoop",    	playTouchByFileLoop,   	"playTouchByFileLoop"},
	{"checkTestCaseByFile",    	checkTestCaseByFile,   	"checkTestCaseByFile"},
	{"print",    				print,   				"print"},
	{"save",    				save,   				"save"},
	{"stopRecord",    			stopRecord,   			"stopRecord"},
	{"stopPlay",    			stopPlay,   			"stopPlay"},
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



// void playRecordLoop(void)
// {
// 	try{
// 		std::string filePath = args.at(1);
// 		int playCnt = std::stol(args.at(2), 0, 0);
// 		std::cout << "record loading ..." << std::endl;
// 		DBHandler::GetInstance()->getTouchEventsFrom(sTmpTouchEvents,filePath);
// 		std::cout << "record loaded !" << std::endl;
// 		std::cout << "playing ..." << std::endl;
// 		for(int i=0;i<playCnt;i++){
// 			Player::GetInstance()->playSync(sTmpTouchEvents);
// 		}
// 		std::cout << "playing over !" << std::endl;
// 	}catch (std::out_of_range & e){
//         std::cerr << "argument error " << std::endl;
//     }catch(ama_exceptionType_e e){
// 		printException(e);
// 	}
// }

void recordTouch(void)
{
	try{
		Player::GetInstance()->recordTouch();
	}catch(ama_exceptionType_e e){
		printException(e);
	}
}

void recordTestCase(void)
{
	try{
		Player::GetInstance()->recordTestCase();
	}catch(ama_exceptionType_e e){
		printException(e);
	}
}

void recordTestCaseByFilter(void)
{
	try{
		std::string filter = args.at(1);
		Player::GetInstance()->recordTestCase(filter);
	}catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }catch(ama_exceptionType_e e){
		printException(e);
	}
}

void playTouch(void)
{
	try{
		Player::GetInstance()->playTouch();
	}catch(ama_exceptionType_e e){
		printException(e);
	}
}

void checkTestCase(void)
{
	try{
		Player::GetInstance()->playTestCase();
	}catch(ama_exceptionType_e e){
		printException(e);
	}
}

void playTouchByFile(void)
{
	try{
		std::string file = args.at(1);
		Player::GetInstance()->playTouch(file);
	}catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }catch(ama_exceptionType_e e){
		printException(e);
	}
}

void playTouchByFileLoop(void)
{
	try{
		std::string file = args.at(1);
		int playCnt = std::stoul(args.at(2), 0, 0);
		Player::GetInstance()->playTouch(file,playCnt);
	}catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }catch(ama_exceptionType_e e){
		printException(e);
	}
}

void checkTestCaseByFile(void)
{
	try{
		std::string file = args.at(1);
		Player::GetInstance()->playTestCase(file);
	}catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }catch(ama_exceptionType_e e){
		printException(e);
	}
}

void print(void)
{

}

void save(void)
{
	try{
		std::string filePath = args.at(1);
		Player::GetInstance()->save(filePath);
	}catch (std::out_of_range & e){
        std::cerr << "argument error " << std::endl;
    }catch(ama_exceptionType_e e){
		printException(e);
	}
}

void stopRecord(void)
{
	try{
		Player::GetInstance()->stopRecord();
	}catch(ama_exceptionType_e e){
		printException(e);
	}
}

void stopPlay(void)
{
	try{
		Player::GetInstance()->stopPlay();
	}catch(ama_exceptionType_e e){
		printException(e);
	}
}

void printException(ama_exceptionType_e e)
{
	switch(e){
	case E_RECORDING:
		std::cout << "is recording..." << std::endl;
		break;
	case E_NOT_RECORDING:
		std::cout << "isn't recording !" << std::endl;
		break;
	case E_NO_RECORD:
		std::cout << "have't record !" << std::endl;
		break;
	case E_PLAYING:
		std::cout << "is playing ..." << std::endl;
		break;
	case E_NOT_PLAYING:
		std::cout << "is't playing" << std::endl;
		break;
	case E_IS_OPERATING:
		std::cout << "is operating now!" << std::endl;
		break;
	case E_NOT_OPERATING:
		std::cout << "is't operating now!" << std::endl;
		break;
	case E_NO_EVENTS:
		std::cout << "have't events" << std::endl;
		break;
	case E_LOAD_FILE_FAILED:
		std::cout << "load file failed!" << std::endl;
		break;
	case E_SAVE_FILE_FAILED:
		std::cout << "save file failed!" << std::endl;
		break;
	case E_NO_OPERAT_THING:
		std::cout << "there is nothing to be operate" << std::endl;
		break;
	default:
		std::cout << "exception is not defined !" << std::endl;
		break;
	}
}
