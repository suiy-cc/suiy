/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "DBHandler.h"

#include "SqlCommon.h"
#include "SqlField.h"
#include "SqlTable.h"

namespace ama
{

const string TouchTableNameInDB = "TouchEventsTable";
const string TestTableNameInDB = "TestEventsTable";

//define table structure
Field keyValueTableDef[] =
{
    Field(FIELD_KEY),
    Field("eventFlag", type_int, flag_not_null),
    Field("timeStamp", type_int, flag_not_null),
    //MarkEvent
    Field("markType", type_int),
    //TouchEvent
    Field("eventType", type_int),
    Field("eventCode", type_int),
    Field("eventValue", type_int),
    //TestEvent
    Field("funDesStr", type_text),
    Field("dataStr", type_text),
    Field(DEFINITION_END),
};

DBHandler::DBHandler()
{

}

DBHandler::~DBHandler()
{

}

void DBHandler::saveEventsAs(Events *pEvents,std::string filePath) throw(ama_exceptionType_e)
{
    try{
        mDB.open(filePath);
        mDB.transactionBegin();

        string tableName;
        if(typeid(*pEvents)==typeid(TouchEvents)){
            tableName = TouchTableNameInDB;
        }else if(typeid(*pEvents)==typeid(TestEvents)){
            tableName = TestTableNameInDB;
        }else{
            printf("events in not defined! \n");
        }

        Table iTable(mDB.getHandle(), tableName, keyValueTableDef);

        if (iTable.exists()){
            iTable.remove();
        }

        iTable.create();
        iTable.open();

        for(std::vector<shared_ptr<Event>>::iterator it=pEvents->mEventVector.begin();it!=pEvents->mEventVector.end();it++){
            Record record(iTable.fields());
            getRecordFromEvent(record,*it);
            iTable.addRecord(&record);
        }

        mDB.transactionCommit();
    }catch (Exception e) {
        printf("errMsg:%s \n",e.msg().c_str());
        throw E_SAVE_FILE_FAILED;
    }
}

void DBHandler::getEventsFrom(Events *pEvents,std::string filePath) throw(ama_exceptionType_e)
{
    try{
        mDB.open(filePath);
        mDB.transactionBegin();
        string tableName;
        if(typeid(*pEvents)==typeid(TouchEvents)){
            tableName = TouchTableNameInDB;
        }else if(typeid(*pEvents)==typeid(TestEvents)){
            tableName = TestTableNameInDB;
        }else{
            printf("events in not defined! \n");
        }

        Table iTable(mDB.getHandle(), tableName, keyValueTableDef);
        iTable.open();
        pEvents->clear();
        for(int recordIndex=0;recordIndex<iTable.recordCount();recordIndex++){
            pEvents->addEvent(getEventFromRecord(iTable.getRecord(recordIndex)));
        }
        mDB.transactionCommit();
    }catch (Exception e) {
        printf("errMsg:%s \n",e.msg().c_str());
        throw E_LOAD_FILE_FAILED;
    }
}

void DBHandler::getRecordFromEvent(Record &record,shared_ptr<Event> pEvent)
{
    record.setInteger("eventFlag", pEvent->mEventFlag);
    record.setInteger("timeStamp", pEvent->mTimeStamp);
    switch(pEvent->getEventFlag()){
    case E_FLAG_NULL:
        break;
    case E_FLAG_MARK:
        record.setInteger("markType", dynamic_pointer_cast<MarkEvent>(pEvent)->mMaskType);
        break;
    case E_FLAG_TOUCH:
        record.setInteger("eventType", dynamic_pointer_cast<TouchEvent>(pEvent)->mTouchEvent.type);
        record.setInteger("eventCode", dynamic_pointer_cast<TouchEvent>(pEvent)->mTouchEvent.code);
        record.setInteger("eventValue", dynamic_pointer_cast<TouchEvent>(pEvent)->mTouchEvent.value);
        break;
    case E_FLAG_TEST:
        record.setString("funDesStr", dynamic_pointer_cast<TestEvent>(pEvent)->mFunDescriptionStr);
        record.setString("dataStr", dynamic_pointer_cast<TestEvent>(pEvent)->mDataStr);
        break;
    default:
        printf("event flag is not defined! \n");
        break;
    }

}

shared_ptr<Event> DBHandler::getEventFromRecord(Record *pRecord)
{
    shared_ptr<Event> pEvent = std::make_shared<Event>();
    shared_ptr<MarkEvent> pMarkEvent = std::make_shared<MarkEvent>();
    shared_ptr<TouchEvent> pTouchEvent = std::make_shared<TouchEvent>();
    shared_ptr<TestEvent> pTestEvent = std::make_shared<TestEvent>();

    switch(static_cast<ama_EventFlag_e>(pRecord->getValue("eventFlag")->asInteger())){
    case E_FLAG_NULL:
        return pEvent;
    case E_FLAG_MARK:
        dynamic_pointer_cast<Event>(pMarkEvent)->mEventFlag = static_cast<ama_EventFlag_e>(pRecord->getValue("eventFlag")->asInteger());
        dynamic_pointer_cast<Event>(pMarkEvent)->mTimeStamp = pRecord->getValue("timeStamp")->asInteger();
        pMarkEvent->mMaskType = static_cast<ama_MarkType_e>(pRecord->getValue("markType")->asInteger());
        return dynamic_pointer_cast<Event>(pMarkEvent);
    case E_FLAG_TOUCH:
        dynamic_pointer_cast<Event>(pTouchEvent)->mEventFlag = static_cast<ama_EventFlag_e>(pRecord->getValue("eventFlag")->asInteger());
        dynamic_pointer_cast<Event>(pTouchEvent)->mTimeStamp = pRecord->getValue("timeStamp")->asInteger();
        pTouchEvent->mTouchEvent.type = pRecord->getValue("eventType")->asInteger();
        pTouchEvent->mTouchEvent.code = pRecord->getValue("eventCode")->asInteger();
        pTouchEvent->mTouchEvent.value = pRecord->getValue("eventValue")->asInteger();
        return dynamic_pointer_cast<Event>(pTouchEvent);
    case E_FLAG_TEST:
        dynamic_pointer_cast<Event>(pTestEvent)->mEventFlag = static_cast<ama_EventFlag_e>(pRecord->getValue("eventFlag")->asInteger());
        dynamic_pointer_cast<Event>(pTestEvent)->mTimeStamp = pRecord->getValue("timeStamp")->asInteger();
        pTestEvent->mFunDescriptionStr = pRecord->getValue("funDesStr")->asString();
        pTestEvent->mDataStr = pRecord->getValue("dataStr")->asString();
        return dynamic_pointer_cast<Event>(pTestEvent);
    default:
        printf("event flag is not defined! \n");
        return pEvent;
    }
}

}//end namespace ama
