/* Copyright (C) 2015 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <iostream>
#include <thread>

#include <CommonAPI/CommonAPI.hpp>
#include "APP_NAMEStubImpl.hpp"

using namespace std;

int main() {
	/*  LogContext：如果使用DLT作为LOG输出手段，则本参数生效。               */ 
	/* 本参数用于设置DLT的LOG输出时使用的Context值，是一个最大4字节的字符串。*/
	/* 如果不进行设置，默认值是"CAPI"。                                      */
	CommonAPI::Runtime::setProperty("LogContext", "DEMS");
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
	std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
	std::shared_ptr<HelloWorldStubImpl> myService = std::make_shared<HelloWorldStubImpl>();
    bool successfullyRegistered = runtime->registerService(domain, instance, myService, connection);

	while (!successfullyRegistered) {
		std::cout << "Register Service failed, trying again in 100 milliseconds..." << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		successfullyRegistered = runtime->registerService(domain, instance, myService, connection);
	}

	std::cout << "Successfully Registered Service!" << std::endl;
	std::cout << "Waiting for calls... (Abort with CTRL+C)" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
	
	/* 1st: method sayHello */
	/* APP_NAMEStubImpl.cpp */

	/* 2nd:method tell fireAndForget */
	/* APP_NAMEStubImpl.cpp */

	/* 5th:attribute Int32 x */
	int32_t value = 99;
	std::cout << "Setting attribute value: " << value << std::endl;
	myService->setXAttribute(value);
	//std::cout << "Set attribute value: " << value << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(20));

	/* 可执行 */
	#if 0
	std::cout << "Getting attribute value: " << value << std::endl;
	value=myService->getXAttribute();
	std::cout << "Got attribute value: " << value << std::endl;
	#endif

    int i=1;
	while (true) {
        /* 3rd:broadcast reply selective */
		std::shared_ptr<CommonAPI::ClientIdList>  clientIds= myService->get_clientIds();
		#if 0
		for (auto iter = clientIds->begin(); iter != clientIds->end(); ++iter) {
			std::cout << "myService->get_clientIds(): "  << *iter << std::endl;
		}
		#endif
		myService->fireReplySelective("reply fire -> -> ->",clientIds);
		std::this_thread::sleep_for(std::chrono::seconds(5));

		/* 4th:broadcast yell */
		myService->fireYellEvent("yell fire -> -> ->");
		std::this_thread::sleep_for(std::chrono::seconds(5));

		std::cout << "Test: " << i << std::endl;
		static int32_t val = 0;
		myService->setXAttribute(val++);
		i++;		
    }

    return 0;
}
