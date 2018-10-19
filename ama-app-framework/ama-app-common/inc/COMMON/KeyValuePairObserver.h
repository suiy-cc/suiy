/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file KeyValuePairObserver.h
/// @brief contains the definition of class KeyValuePairObserver
/// 
/// Created by zs on 2017/08/10.
///
/// this file contains the definition of class KeyValuePairObserver
/// 

#ifndef KEYVALUEPAIROBSERVER_H
#define KEYVALUEPAIROBSERVER_H

#include <map>
#include <COMMON/Broadcaster.h>
#include <COMMON/ThreadSafeValueKeeper.h>

template <class keyType, class valueType>
class KeyValuePairObserver
{
    // private types:

    // public types:
public:
    typedef std::map<keyType, valueType> KVMap;
    enum E_EVENT{
        E_EVENT_NONE = 0,

        E_EVENT_VALUE_CHANGED = 1,
        E_EVENT_KEY_CREATED = 2,
        E_EVENT_KEY_DELETED = 4,

        E_EVENT_MAX, // don't use this one, it's for "E_EVENT_ALL"'s generation

        E_EVENT_ALL = (E_EVENT_MAX-1)*2-1, // stands for all of the available options
    };
    typedef void(*ObserverType)(const keyType& key, const valueType& value, E_EVENT event);

    // public interfaces:
public:
    void AddObserver(const keyType& key, ObserverType observer, E_EVENT event = E_EVENT_VALUE_CHANGED);
    void DeleteObserver(const keyType& key, ObserverType observer, E_EVENT event = E_EVENT_VALUE_CHANGED);

    void AddKey(const keyType& key, const valueType& value = valueType());
    void DeleteKey(const keyType& key);
    bool IsKeyExisting(const keyType& key)const;
    valueType GetValue(const keyType &key)const;
    void ModifyValue(const keyType& key, const valueType& value);
    void Clear();
    bool Empty()const;
    typename KeyValuePairObserver<keyType, valueType>::KVMap::size_type Size()const;
    typename KeyValuePairObserver<keyType, valueType>::KVMap Copy()const;

protected:
    void Broadcast(const keyType &key, const valueType &value, KeyValuePairObserver::E_EVENT event);

    // private members:
private:
    typedef Broadcaster<ObserverType> BroadcasterType;
    typedef std::map<keyType, std::map<E_EVENT, BroadcasterType>> KBMap;
    ThreadSafeValueKeeper<KVMap> kvMap;
    ThreadSafeValueKeeper<KBMap> kbMap;
};

template <class keyType, class valueType>
void KeyValuePairObserver<keyType, valueType>::AddObserver(const keyType& key, ObserverType observer, E_EVENT event)
{
    kbMap.Operate([&](KBMap& bMap){
        if(event&E_EVENT_VALUE_CHANGED)
            bMap[key][E_EVENT_VALUE_CHANGED].Register(observer);
        if(event&E_EVENT_KEY_CREATED)
            bMap[key][E_EVENT_KEY_CREATED].Register(observer);
        if(event&E_EVENT_KEY_DELETED)
            bMap[key][E_EVENT_KEY_DELETED].Register(observer);
    });
}

template <class keyType, class valueType>
void KeyValuePairObserver<keyType, valueType>::DeleteObserver(const keyType& key, ObserverType observer, E_EVENT event)
{
    kbMap.Operate([&](KBMap& bMap){
        if(event&E_EVENT_VALUE_CHANGED)
            bMap[key][E_EVENT_VALUE_CHANGED].Unregister(observer);
        if(event&E_EVENT_KEY_CREATED)
            bMap[key][E_EVENT_KEY_CREATED].Unregister(observer);
        if(event&E_EVENT_KEY_DELETED)
            bMap[key][E_EVENT_KEY_DELETED].Unregister(observer);
    });
}

template <class keyType, class valueType>
void KeyValuePairObserver<keyType, valueType>::AddKey(const keyType& key, const valueType& value)
{
    kvMap.Operate([&](KVMap& vMap){
        vMap[key] = value;
        Broadcast(key, value, E_EVENT_KEY_CREATED);
    });
}

template <class keyType, class valueType>
void KeyValuePairObserver<keyType, valueType>::DeleteKey(const keyType& key)
{
    kvMap.Operate([&](KVMap& vMap){
        vMap.erase(key);
        Broadcast(key, valueType(), E_EVENT_KEY_DELETED);
    });
}

template <class keyType, class valueType>
bool KeyValuePairObserver<keyType, valueType>::IsKeyExisting(const keyType& key)const
{
    bool rtv = false;
    kvMap.OperateConst([&](const KVMap& vMap){
        rtv = vMap.find(key)!=vMap.end();
    });

    return rtv;
}

template <class keyType, class valueType>
valueType KeyValuePairObserver<keyType, valueType>::GetValue(const keyType &key)const
{
    valueType rtv;
    kvMap.OperateConst([&](const KVMap& vMap){
        if(vMap.find(key)==vMap.end())
            return;

        rtv = vMap.at(key);
    });

    return rtv;
}

template <class keyType, class valueType>
void KeyValuePairObserver<keyType, valueType>::ModifyValue(const keyType& key, const valueType& value)
{
    kvMap.Operate([&](KVMap& vMap){
        if(vMap.find(key)==vMap.end())
            return;
        vMap[key] = value;
        Broadcast(key, value, E_EVENT_VALUE_CHANGED);

    });
}

template <class keyType, class valueType>
void KeyValuePairObserver<keyType, valueType>::Clear()
{
    kvMap.Operate([&](KVMap& vMap){
        vMap.clear();
    });
}

template <class keyType, class valueType>
bool KeyValuePairObserver<keyType, valueType>::Empty()const
{
    bool rtv = true;
    kvMap.OperateConst([&](const KVMap& vMap){
        rtv = vMap.empty();
    });

    return rtv;
}

template <class keyType, class valueType>
typename KeyValuePairObserver<keyType, valueType>::KVMap::size_type KeyValuePairObserver<keyType, valueType>::Size()const
{
    typename KeyValuePairObserver<keyType, valueType>::KVMap::size_type rtv = 0;
    kvMap.OperateConst([&](const typename KeyValuePairObserver<keyType, valueType>::KVMap& vMap){
        rtv = vMap.size();
    });

    return rtv;
}

template <class keyType, class valueType>
typename KeyValuePairObserver<keyType, valueType>::KVMap KeyValuePairObserver<keyType, valueType>::Copy()const
{
    typename KeyValuePairObserver<keyType, valueType>::KVMap rtv;
    kvMap.OperateConst([&](const typename KeyValuePairObserver<keyType, valueType>::KVMap& vMap){
        rtv = vMap;
    });

    return std::move(rtv);
}

template <class keyType, class valueType>
void KeyValuePairObserver<keyType, valueType>::Broadcast(const keyType &key, const valueType &value, KeyValuePairObserver::E_EVENT event)
{
    kbMap.Operate([&](KeyValuePairObserver<keyType, valueType>::KBMap& bMap){
        if(bMap.find(key)==bMap.end())
            return;
        if(bMap[key].find(event)==bMap[key].end())
            return;

        bMap[key][event].NotifyAll([&](KeyValuePairObserver::ObserverType observer){
            observer(key, value, event);
        });
    });
}

#endif // KEYVALUEPAIROBSERVER_H
