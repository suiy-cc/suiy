/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file HandleManagement.h
/// @brief contains declaration of handle management API
/// 
/// Created by zs on 2017/02/23.
///

#ifndef HANDLEMANAGEMENT_H
#define HANDLEMANAGEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned int GetMinAvailableHandle();

void CreateHandle(unsigned int handle);

bool IsHandleExists(unsigned int handle);

void AssociateObject2Handle(unsigned int handle, void* obj);

void* GetObjectFromHandle(unsigned int handle);

void DeleteHandle(unsigned int handle);

#ifdef __cplusplus
};
#endif

#ifdef __cplusplus

#include <memory.h>
#include <map>

#include <COMMON/ThreadSafeValueKeeper.h>

template<class ObjectType>
class HandleManager{
public:
    typedef std::map<unsigned int, ObjectType> HandleMap;

public:
    unsigned int GetMinAvailableHandle();
    void CreateHandle(unsigned int handle);
    bool IsHandleExists(unsigned int handle)const;
    void AssociateObject2Handle(unsigned int handle, const ObjectType& obj);
    ObjectType GetObjectFromHandle(unsigned int handle)const;
    void DeleteHandle(unsigned int handle);
    void Iterate(std::function<void(unsigned int, ObjectType& )> callback);
    void IterateConst(std::function<void(unsigned int, const ObjectType& )> callback)const;
    unsigned int GetSize()const;
private:
    ThreadSafeValueKeeper<HandleMap> handles;
};

template<class ObjectType>
unsigned int HandleManager<ObjectType>::GetMinAvailableHandle()
{
    unsigned int minAvailableHandle = 0;

    handles.Operate([&minAvailableHandle](HandleMap & handleSet){
        if(handleSet.size()==0)
            minAvailableHandle = 1;
        else if(handleSet.size()==handleSet.rbegin()->first)
            minAvailableHandle = handleSet.rbegin()->first + 1;
        else
        {
            unsigned int maxUINT = 0;
            memset(&maxUINT, 0xff, sizeof(maxUINT));
            for(unsigned int i = 1; i < maxUINT; ++i)
                if(handleSet.find(i)==handleSet.end())
                {
                    minAvailableHandle = i;
                    break;
                }
        }

        if(minAvailableHandle)
            handleSet.insert(std::make_pair(minAvailableHandle, ObjectType()));
    });

    return minAvailableHandle;
}

template<class ObjectType>
void HandleManager<ObjectType>::CreateHandle(unsigned int handle)
{
    handles.Operate([handle](HandleMap & handleSet){
        handleSet.insert(std::make_pair(handle, ObjectType()));
    });
}

template<class ObjectType>
bool HandleManager<ObjectType>::IsHandleExists(unsigned int handle)const
{
    bool isHandleExists = false;
    handles.OperateConst([&isHandleExists, handle](const HandleMap & handleSet){
        isHandleExists = handleSet.find(handle)!=handleSet.end();
    });

    return isHandleExists;
}

template<class ObjectType>
void HandleManager<ObjectType>::AssociateObject2Handle(unsigned int handle, const ObjectType& obj)
{
    handles.Operate([handle, &obj](HandleMap & handleSet){
        handleSet[handle] = obj;
    });
}

template<class ObjectType>
ObjectType HandleManager<ObjectType>::GetObjectFromHandle(unsigned int handle)const
{
    ObjectType object;
    handles.OperateConst([handle, &object](const HandleMap & handleSet){
        if(handleSet.find(handle)!=handleSet.end())
            object = handleSet.at(handle);
    });

    return object;
}

template<class ObjectType>
void HandleManager<ObjectType>::DeleteHandle(unsigned int handle)
{
    handles.Operate([handle](HandleMap & handleSet){
        handleSet.erase(handle);
    });
}

template<class ObjectType>
void HandleManager<ObjectType>::Iterate(std::function<void(unsigned int, ObjectType& )> callback)
{
    handles.Operate([&callback](HandleMap & handleSet){
        for(auto& handle: handleSet)
            callback(handle.first, handle.second);
    });
}

template<class ObjectType>
void HandleManager<ObjectType>::IterateConst(std::function<void(unsigned int, const ObjectType& )> callback)const
{
    handles.OperateConst([&callback](const HandleMap & handleSet){
        for(auto& handle: handleSet)
            callback(handle.first, handle.second);
    });
}

template<class ObjectType>
unsigned int HandleManager<ObjectType>::GetSize()const
{
    return handles.GetValue().size();
}

#endif

#endif // HANDLEMANAGEMENT_H
