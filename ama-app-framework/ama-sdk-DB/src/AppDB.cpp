/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "AppDB.h"
#include "AppDBImpl.h"

// AppDatabase::AppDatabase(const string nameOfDB,const constructorMap map)
// :mName(nameOfDB)
// ,pAppDBImpl(nullptr)
// {
//     pAppDBImpl = std::make_shared<AppDatabaseImpl>(nameOfDB,map,false);
// }

AppDatabase::AppDatabase(const string nameOfDB,const constructorMap map,const bool isUseCache)
:mName(nameOfDB)
,pAppDBImpl(nullptr)
{
    pAppDBImpl = std::make_shared<AppDatabaseImpl>(nameOfDB,map,isUseCache);
}

AppDatabase::~AppDatabase()
{

}

ama_DB_Error_e AppDatabase::init(void)
{
    if(pAppDBImpl){
        return pAppDBImpl->init();
    }else{
        return E_DB_NULL_PTR;
    }
}

bool AppDatabase::isKeyExist(const string key)
{
    if(pAppDBImpl){
        return pAppDBImpl->isKeyExist(key);
    }else{
        return E_DB_NULL_PTR;
    }
}

ama_DB_Error_e AppDatabase::getValueOfKey(const string key,string &value)
{
    if(pAppDBImpl){
        return pAppDBImpl->getValueOfKey(key,value);
    }else{
        return E_DB_NULL_PTR;
    }
}

ama_DB_Error_e AppDatabase::getValueOfKey(const string key,int &value)
{
    if(pAppDBImpl){
        return pAppDBImpl->getValueOfKey(key,value);
    }else{
        return E_DB_NULL_PTR;
    }
}

ama_DB_Error_e AppDatabase::getValueOfKey(const string key,bool &value)
{
    if(pAppDBImpl){
        return pAppDBImpl->getValueOfKey(key,value);
    }else{
        return E_DB_NULL_PTR;
    }
}

ama_DB_Error_e AppDatabase::getDefValueOfKey(const string key,string &defValue)
{
    if(pAppDBImpl){
        return pAppDBImpl->getDefValueOfKey(key,defValue);
    }else{
        return E_DB_NULL_PTR;
    }
}

ama_DB_Error_e AppDatabase::getDefValueOfKey(const string key,int &defValue)
{
    if(pAppDBImpl){
        return pAppDBImpl->getDefValueOfKey(key,defValue);
    }else{
        return E_DB_NULL_PTR;
    }
}

ama_DB_Error_e AppDatabase::getDefValueOfKey(const string key,bool &defValue)
{
    if(pAppDBImpl){
        return pAppDBImpl->getDefValueOfKey(key,defValue);
    }else{
        return E_DB_NULL_PTR;
    }
}

ama_DB_Error_e AppDatabase::setValueOfKey(const string key,const string value)
{
    if(pAppDBImpl){
        return pAppDBImpl->setValueOfKey(key,value);
    }else{
        return E_DB_NULL_PTR;
    }
}

ama_DB_Error_e AppDatabase::setValueOfKey(const string key,const char* value)
{
    string valueStr = value;
    if(pAppDBImpl){
        return pAppDBImpl->setValueOfKey(key,valueStr);
    }else{
        return E_DB_NULL_PTR;
    }
}

ama_DB_Error_e AppDatabase::setValueOfKey(const string key,const int value)
{
    if(pAppDBImpl){
        return pAppDBImpl->setValueOfKey(key,value);
    }else{
        return E_DB_NULL_PTR;
    }
}

ama_DB_Error_e AppDatabase::setValueOfKey(const string key,const bool value)
{
    if(pAppDBImpl){
        return pAppDBImpl->setValueOfKey(key,value);
    }else{
        return E_DB_NULL_PTR;
    }
}

ama_DB_Error_e AppDatabase::getAllKeyValue(keyValueMap &map)
{
    if(pAppDBImpl){
        return pAppDBImpl->getAllKeyValue(map);
    }else{
        return E_DB_NULL_PTR;
    }
}

ama_DB_Error_e AppDatabase::resetDefValueForAllKey(void)
{
    if(pAppDBImpl){
        return pAppDBImpl->resetDefValueForAllKey();
    }else{
        return E_DB_NULL_PTR;
    }
}

ama_DB_Error_e AppDatabase::commitCacheIntoDB(void)
{
    if(pAppDBImpl){
        return pAppDBImpl->commitCacheIntoDB();
    }else{
        return E_DB_NULL_PTR;
    }
}


AppDatabaseEx::AppDatabaseEx()
    : pAppDBImpl(nullptr)
{

}

AppDatabaseEx::~AppDatabaseEx()
{

}

ama_DB_Error_e AppDatabaseEx::connectDB(const string nameOfDB)
{
    pAppDBImpl = std::make_shared<AppDatabaseImpl>(nameOfDB);
    if (pAppDBImpl == nullptr)
    {
        return ama_DB_Error_e::E_DB_NULL_PTR;
    }

    if (pAppDBImpl->connectDB() == false)
    {
        return ama_DB_Error_e::E_DB_NULL_PTR;
    }

    return ama_DB_Error_e::E_DB_OK;
}

ama_DB_Error_e AppDatabaseEx::getValueOfKey(const string key,string &value)
{
    if(pAppDBImpl){
        return pAppDBImpl->getValueOfKey(key,value);
    }else{
        return E_DB_NULL_PTR;
    }
}

ama_DB_Error_e AppDatabaseEx::getValueOfKey(const string key,int &value)
{
    if(pAppDBImpl){
        return pAppDBImpl->getValueOfKey(key,value);
    }else{
        return E_DB_NULL_PTR;
    }
}

ama_DB_Error_e AppDatabaseEx::getValueOfKey(const string key,bool &value)
{
    if(pAppDBImpl){
        return pAppDBImpl->getValueOfKey(key,value);
    }else{
        return E_DB_NULL_PTR;
    }
}

