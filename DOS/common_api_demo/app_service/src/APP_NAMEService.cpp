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
	/*  LogContext�����ʹ��DLT��ΪLOG����ֶΣ��򱾲�����Ч��               */ 
	/* ��������������DLT��LOG���ʱʹ�õ�Contextֵ����һ�����4�ֽڵ��ַ�����*/
	/* ������������ã�Ĭ��ֵ��"CAPI"��                                      */
	CommonAPI::Runtime::setProperty("LogContext", "DEMS");
	/* LibraryBase����������binding-library�����ơ�                     */
	/* �������ñ�����ΪAbc��ָ��Binding����Ϊ"dbus"                     */
	/* ��CommonAPI�Զ�Ѱ��һ������"libAbc-dbus.so"�Ŀ��ļ���������      */
	/* ���¶������Զ�Ѱ��һ������"libAPP_NAME-dbus.so"�Ŀ��ļ�          */
	CommonAPI::Runtime::setProperty("LibraryBase", "APP_NAME");

    /* �� */
	std::string domain = "";
	std::string instance = "";
	std::string connection = "";

	/* ��ʼ�� */
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

	/* ��ִ�� */
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
