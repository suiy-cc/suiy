/*
* This file was generated by the CommonAPI Generators.
* Used org.genivi.commonapi.core 3.1.3.v201507280808.
* Used org.franca.core 0.9.1.201412191134.
*
* This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
* If a copy of the MPL was not distributed with this file, You can obtain one at
* http://mozilla.org/MPL/2.0/.
*/
#include <v0_1/APP_NAME/HelloWorldStubDefault.hpp>
#include <assert.h>

namespace v0_1 {
namespace APP_NAME {

HelloWorldStubDefault::HelloWorldStubDefault():
        remoteEventHandler_(this),
        interfaceVersion_(HelloWorld::getInterfaceVersion()) {
}

const CommonAPI::Version& HelloWorldStubDefault::getInterfaceVersion(std::shared_ptr<CommonAPI::ClientId> _client) {
    return interfaceVersion_;
}

HelloWorldStubRemoteEvent* HelloWorldStubDefault::initStubAdapter(const std::shared_ptr<HelloWorldStubAdapter> &_adapter) {
    CommonAPI::Stub<HelloWorldStubAdapter, HelloWorldStubRemoteEvent>::stubAdapter_ = _adapter;
    return &remoteEventHandler_;
}

const int32_t& HelloWorldStubDefault::getXAttribute() {
    return xAttributeValue_;
}

const int32_t& HelloWorldStubDefault::getXAttribute(const std::shared_ptr<CommonAPI::ClientId> _client) {
    return getXAttribute();
}

void HelloWorldStubDefault::setXAttribute(int32_t _value) {
    const bool valueChanged = trySetXAttribute(std::move(_value));
    if (valueChanged && CommonAPI::Stub<HelloWorldStubAdapter, HelloWorldStubRemoteEvent>::stubAdapter_ != NULL) {
         CommonAPI::Stub<HelloWorldStubAdapter, HelloWorldStubRemoteEvent>::stubAdapter_->fireXAttributeChanged(xAttributeValue_);
     }
}

bool HelloWorldStubDefault::trySetXAttribute(int32_t _value) {
    if (!validateXAttributeRequestedValue(_value))
        return false;

    const bool valueChanged = (xAttributeValue_ != _value);
    xAttributeValue_ = std::move(_value);
    return valueChanged;
}

bool HelloWorldStubDefault::validateXAttributeRequestedValue(const int32_t &_value) {
    return true;
}

void HelloWorldStubDefault::setXAttribute(const std::shared_ptr<CommonAPI::ClientId> _client, int32_t _value) {
    setXAttribute(_value);
}

void HelloWorldStubDefault::onRemoteXAttributeChanged() {
    // No operation in default
}

void HelloWorldStubDefault::RemoteEventHandler::onRemoteXAttributeChanged() {
	assert(defaultStub_ !=NULL);
    defaultStub_->onRemoteXAttributeChanged();
}

bool HelloWorldStubDefault::RemoteEventHandler::onRemoteSetXAttribute(int32_t _value) {
	assert(defaultStub_ !=NULL);
    return defaultStub_->trySetXAttribute(std::move(_value));
}

bool HelloWorldStubDefault::RemoteEventHandler::onRemoteSetXAttribute(const std::shared_ptr<CommonAPI::ClientId> _client, int32_t _value) {
    return onRemoteSetXAttribute(_value);
}


/**
 * description: 这是一个标准的函数接口，拥有一个输入参数，一个输出参数
 */
void HelloWorldStubDefault::sayHello(const std::shared_ptr<CommonAPI::ClientId> _client, std::string _name, sayHelloReply_t _reply) {
    std::string message = "";
    _reply(message);
}

/**
 * description: fireAndForget的函数，只有一个输入参数  note tell为函数名
 */
void HelloWorldStubDefault::tell(const std::shared_ptr<CommonAPI::ClientId> _client, std::string _name) {
}


/**
 * description: selective的广播，只有一个输出参数。注意，这里的输出是针对服务端来说的，也就是说，从服务端输出到客户端。从通常意义来讲，这是个输入参数。下同
 */
void HelloWorldStubDefault::fireReplySelective(const std::string &_name, const std::shared_ptr<CommonAPI::ClientIdList> _receivers) {
	assert((CommonAPI::Stub<HelloWorldStubAdapter, HelloWorldStubRemoteEvent>::stubAdapter_) !=NULL);
    CommonAPI::Stub<HelloWorldStubAdapter, HelloWorldStubRemoteEvent>::stubAdapter_->sendReplySelective(_name, _receivers);
}
void HelloWorldStubDefault::onReplySelectiveSubscriptionChanged(const std::shared_ptr<CommonAPI::ClientId> _client, const CommonAPI::SelectiveBroadcastSubscriptionEvent _event) {
    // No operation in default
}
bool HelloWorldStubDefault::onReplySelectiveSubscriptionRequested(const std::shared_ptr<CommonAPI::ClientId> _client) {
    // Accept in default
    return true;
}
std::shared_ptr<CommonAPI::ClientIdList> const HelloWorldStubDefault::getSubscribersForReplySelective() {
	assert((CommonAPI::Stub<HelloWorldStubAdapter, HelloWorldStubRemoteEvent>::stubAdapter_) !=NULL);
    return(CommonAPI::Stub<HelloWorldStubAdapter, HelloWorldStubRemoteEvent>::stubAdapter_->getSubscribersForReplySelective());
}

/**
 * description: 一个标准的广播接口，带有一个参数
 */
void HelloWorldStubDefault::fireYellEvent(const std::string &_name) {
	assert((CommonAPI::Stub<HelloWorldStubAdapter, HelloWorldStubRemoteEvent>::stubAdapter_) !=NULL);
    CommonAPI::Stub<HelloWorldStubAdapter, HelloWorldStubRemoteEvent>::stubAdapter_->fireYellEvent(_name);
}


HelloWorldStubDefault::RemoteEventHandler::RemoteEventHandler(HelloWorldStubDefault *_defaultStub)
    : 
      defaultStub_(_defaultStub) {
}

} // namespace APP_NAME
} // namespace v0_1