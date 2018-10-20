/* Copyright (C) 2015 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "APP_NAMEStubImpl.hpp"

HelloWorldStubImpl::HelloWorldStubImpl() {
	clientIds_ = std::make_shared<CommonAPI::ClientIdList>();
}

HelloWorldStubImpl::~HelloWorldStubImpl() {
}

void HelloWorldStubImpl::sayHello(const std::shared_ptr<CommonAPI::ClientId> _client,
		std::string _name,
		sayHelloReply_t _reply) {

	std::stringstream messageStream;

    messageStream << "messageStream:" << _name << "!";

    _reply(messageStream.str());
};

void HelloWorldStubImpl::tell(const std::shared_ptr<CommonAPI::ClientId> _client, std::string _name)
{
    std::cout << "Tell('" << _name << "')" << "\n";
};

bool HelloWorldStubImpl::onReplySelectiveSubscriptionRequested(const std::shared_ptr<CommonAPI::ClientId> _client)
{
	// clientIds_->insert(_client);
	#if 0
	for (auto iter = clientIds_->begin(); iter != clientIds_->end(); ++iter) {
		std::cout << *iter << std::endl;
	}
	#endif
	return true;
}

const int32_t& HelloWorldStubImpl::getXAttribute() {
	const int32_t& val_get = HelloWorldStubDefault::getXAttribute();
    //std::cout << "getXAttribute: " << val_get << std::endl;
	return val_get;
}



