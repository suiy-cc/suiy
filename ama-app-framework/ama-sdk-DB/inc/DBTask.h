/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */


#ifndef DBTASK_H_
#define DBTASK_H_

#include "Task.h"


using std::string;

enum DBTaskType{
    E_UPDATE_DB = 0x0,
};

class DBTask:
            public Task
{
public:
    DBTask(){}
    ~DBTask(){}
    void setKey(string key){
        mKey = key;
    }
    string getKey(void){
        return mKey;
    }

    void setValue(string value){
        mValue = value;
    }
    string getValue(void){
        return mValue;
    }

    virtual string getTaskTypeString()
	{
		switch(GetType()){
			case E_UPDATE_DB:    return "E_UPDATE_DB";
			default: return "ERROR TASK TYPE";
		}
	}
private:
    string mKey;
    string mValue;
};


#endif //ISTASK_H_
