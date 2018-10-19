/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef DBHANDLER_H_
#define DBHANDLER_H_

#include "Event.h"
#include "Events.h"

#include "SqlRecord.h"
#include "SqlDatabase.h"

#include <mutex>
#include <memory>
#include <string>

using namespace sql;
using namespace std;

namespace ama
{

class DBHandler
{
public:
    static DBHandler* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static DBHandler instance;
        return &instance;
    }
    ~DBHandler();
    void saveEventsAs(Events *pEvents,std::string filePath) throw(ama_exceptionType_e);
    void getEventsFrom(Events *pEvents,std::string filePath) throw(ama_exceptionType_e);
private:
    DBHandler();
    void getRecordFromEvent(Record &record,shared_ptr<Event> pEvent);
    shared_ptr<Event> getEventFromRecord(Record *pRecord);

    sql::Database mDB;
};//end DBHandler

}//end namespace ama

#endif //DBHANDLER_H_
