/* Copyright (C) 2015 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HELLOWORLDSTUBIMPL_H_
#define HELLOWORLDSTUBIMPL_H_

#include <vector>

#include <CommonAPI/CommonAPI.hpp>
#include <v0_1/APP_NAME/HelloWorldStubDefault.hpp>

class HelloWorldStubImpl: public v0_1::APP_NAME::HelloWorldStubDefault {

public:
    HelloWorldStubImpl();
    virtual ~HelloWorldStubImpl();

    virtual void sayHello(const std::shared_ptr<CommonAPI::ClientId> _client, std::string _name, sayHelloReply_t _return);
	virtual void tell(const std::shared_ptr<CommonAPI::ClientId> _client, std::string _name);
	virtual bool onReplySelectiveSubscriptionRequested(const std::shared_ptr<CommonAPI::ClientId> _client);
	std::shared_ptr<CommonAPI::ClientIdList>& get_clientIds()
	{
		return clientIds_;
	};
	virtual const int32_t& getXAttribute();

private:
	std::shared_ptr<CommonAPI::ClientIdList>  clientIds_;
};

#endif /* E01HELLOWORLDSTUBIMPL_H_ */
