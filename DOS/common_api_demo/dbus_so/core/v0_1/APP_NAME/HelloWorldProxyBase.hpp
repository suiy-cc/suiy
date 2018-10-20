/*
* This file was generated by the CommonAPI Generators.
* Used org.genivi.commonapi.core 3.1.3.v201507280808.
* Used org.franca.core 0.9.1.201412191134.
*
* This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
* If a copy of the MPL was not distributed with this file, You can obtain one at
* http://mozilla.org/MPL/2.0/.
*/
#ifndef APP_NAME_Hello_World_PROXY_BASE_HPP_
#define APP_NAME_Hello_World_PROXY_BASE_HPP_

#include <v0_1/APP_NAME/HelloWorld.hpp>



#if !defined (COMMONAPI_INTERNAL_COMPILATION)
#define COMMONAPI_INTERNAL_COMPILATION
#endif

#include <vector>

#include <CommonAPI/Attribute.hpp>
#include <CommonAPI/Event.hpp>
#include <CommonAPI/SelectiveEvent.hpp>
#include <CommonAPI/Proxy.hpp>
#include <functional>
#include <future>

#undef COMMONAPI_INTERNAL_COMPILATION

namespace v0_1 {
namespace APP_NAME {

class HelloWorldProxyBase
    : virtual public CommonAPI::Proxy {
public:
    typedef CommonAPI::ObservableAttribute<int32_t> XAttribute;
    typedef CommonAPI::SelectiveEvent<std::string> ReplySelectiveEvent;
    typedef CommonAPI::Event<
        std::string
    > YellEvent;

    typedef std::function<void(const CommonAPI::CallStatus&, const std::string&)> SayHelloAsyncCallback;

    /**
     * description: 定义一个私有成员变量和一组对应的Get（客户端）/ Set（服务端）函数
     */
    virtual XAttribute& getXAttribute() = 0;

    /**
     * description: selective的广播，只有一个输出参数。注意，这里的输出是针对服务端来说的，也就是说，从服务端输出到客户端。从通常意义来讲，这是个输入参数。下同
     */
    virtual ReplySelectiveEvent& getReplySelectiveEvent() = 0;
    /**
     * description: 一个标准的广播接口，带有一个参数
     */
    virtual YellEvent& getYellEvent() = 0;

    /**
     * description: 这是一个标准的函数接口，拥有一个输入参数，一个输出参数
     */
    virtual void sayHello(const std::string &_name, CommonAPI::CallStatus &_internalCallStatus, std::string &_message, const CommonAPI::CallInfo *_info = nullptr) = 0;
    virtual std::future<CommonAPI::CallStatus> sayHelloAsync(const std::string &_name, SayHelloAsyncCallback _callback, const CommonAPI::CallInfo *_info = nullptr) = 0;
    /**
     * description: fireAndForget的函数，只有一个输入参数  note tell为函数名
     */
    /**
     * @invariant Fire And Forget
     */
    virtual void tell(const std::string &_name, CommonAPI::CallStatus &_internalCallStatus) = 0;
};

} // namespace APP_NAME
} // namespace v0_1

#endif // APP_NAME_Hello_World_PROXY_BASE_HPP_