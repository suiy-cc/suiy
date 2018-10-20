/* Copyright (C) 2015 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <iostream>
#include <string>

#ifndef WIN32
#include <unistd.h>
#endif

#include <CommonAPI/CommonAPI.hpp>
#include <v0_1/APP_NAME/HelloWorldProxy.hpp>

using namespace v0_1::APP_NAME;

void myAttrXCallback(const CommonAPI::CallStatus& callStatus, const int32_t& val) 
{
	std::cout << "Receive callback for Attribute x: " << val << std::endl;
}

int main() {
	/*  LogContext：如果使用DLT作为LOG输出手段，则本参数生效。               */ 
	/* 本参数用于设置DLT的LOG输出时使用的Context值，是一个最大4字节的字符串。*/
	/* 如果不进行设置，默认值是"CAPI"。                                      */
	CommonAPI::Runtime::setProperty("LogContext", "DEMC");
	/* LibraryBase：用于设置binding-library的名称。                     */
	/* 例如设置本参数为Abc并指定Binding类型为"dbus"                     */
	/* 则CommonAPI自动寻找一个叫做"libAbc-dbus.so"的库文件进行载入      */
	/* 如下定义则自动寻找一个叫做"libAPP_NAME-dbus.so"的库文件          */
	CommonAPI::Runtime::setProperty("LibraryBase", "APP_NAME");

	/* 空 */
	std::string domain = "";
    std::string instance = "";
    std::string connection = "";

	/* 初始化 */
	std::shared_ptr < CommonAPI::Runtime > runtime = CommonAPI::Runtime::get();
	std::shared_ptr<HelloWorldProxy<>> myProxy = runtime->buildProxy<HelloWorldProxy>(domain,instance, connection);

    std::cout << "Checking availability!" << std::endl;
    while (!myProxy->isAvailable())
    	usleep(10);
    std::cout << "Available..." << std::endl;
    
    const std::string name = "Hello_World";//sayHello的para1 
    CommonAPI::CallStatus callStatus;//sayHello的para2
    std::string returnMessage;//sayHello的para3
    CommonAPI::CallInfo info(1000);//sayHello的para4
    info.sender_ = 1234;

	/* 1st: method sayHello */
	myProxy->sayHello(name, callStatus, returnMessage, &info);
	if (callStatus != CommonAPI::CallStatus::SUCCESS) {
		std::cerr << "sayHello:Remote call failed!\n";
		return -1;
	}
	info.timeout_ = info.timeout_ + 1000;
	std::cout << "sayHello Got message: '" << returnMessage << "'\n";
	std::this_thread::sleep_for(std::chrono::seconds(1));

	/* 2nd:method tell fireAndForget */
	myProxy->tell(name, callStatus);
	if (callStatus != CommonAPI::CallStatus::SUCCESS) {
		std::cerr << "tell:Remote call failed!\n";
		return -1;
	}

    /* 3rd:broadcast reply selective */
	myProxy->getReplySelectiveEvent().subscribe(
                    [&](const std::string& reply) {
                        std::cout << "getReplySelectiveEvent: " << reply << std::endl;
                    });

	/* 4th:broadcast yell */
    myProxy->getYellEvent().subscribe([&](const std::string& val) {
        std::cout << "getYellEvent: " << val << std::endl;
    });

	/* 5th:attribute Int32 x */
	int32_t value = 33;
	std::cout << "Getting attribute value: " << value << std::endl;
	myProxy->getXAttribute().getValue(callStatus, value, &info);
	if (callStatus != CommonAPI::CallStatus::SUCCESS) {
		std::cerr << "Remote call A failed!\n";
		return -1;
	}
	std::cout << "Got attribute value: " << value << std::endl;

	// Subscribe for receiving values
	myProxy->getXAttribute().getChangedEvent().subscribe([&](const int32_t& val){
	std::cout << "getXAttribute Received change message: " << val << std::endl;});

	/* 可执行 */
	#if 0
	int32_t gValueForX = 12345;
	std::cout << "Setting attribute value: " << gValueForX << std::endl;
	myProxy->getXAttribute().setValueAsync(gValueForX , myAttrXCallback);
	std::cout << "Set attribute value: " << gValueForX << std::endl;

	//static int32_t val = 0;
	//myProxy->getXAttribute().setValueAsync(val++ , myAttrXCallback);
	#endif
	while(true)
	{
		myProxy->sayHello(name, callStatus, returnMessage, &info);
		if (callStatus != CommonAPI::CallStatus::SUCCESS) {
			std::cerr << "sayHello:Remote call failed!\n";
			return -1;
		}
		info.timeout_ = info.timeout_ + 1000;
		std::cout << "sayHello Got message: '" << returnMessage << "'\n";
		std::this_thread::sleep_for(std::chrono::seconds(20));	

		myProxy->tell(name, callStatus);
		if (callStatus != CommonAPI::CallStatus::SUCCESS) {
			std::cerr << "tell:Remote call failed!\n";
			return -1;
		}
	}

    return 0;
}
