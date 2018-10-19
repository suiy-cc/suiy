/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef DLTWRAPPER_H_
#define DLTWRAPPER_H_

#include <string>
#include <pthread.h>
#include <memory>

class LogContext
{
public:
    LogContext();
    virtual ~LogContext();

    void* GetDltContext();

protected:
    friend class CDltWrapper;
    void SetRegistered(bool isRegistered);
    bool IsRegistered()const;

private:
    std::shared_ptr<void> dltContext;
    bool isRegistered;
};

class LogContextData
{
public:
    LogContextData();
    virtual ~LogContextData();

    void* GetDltContextData();

private:
    std::shared_ptr<void> dltContextData;
};

#define LOG_DECLARE_CONTEXT(CONTEXT) \
    LogContext CONTEXT;

typedef enum
{
    LOG_DEFT    =             -1,   /**< Default log level */
    LOG_OFF_    =           0x00,   /**< Log level off */
    LOG_FATL    =           0x01,   /**< fatal system error */
    LOG_ERRO    =           0x02,   /**< error with impact to correct functionality */
    LOG_WARN    =           0x03,   /**< warning, correct behaviour could not be ensured */
    LOG_INFM    =           0x04,   /**< informational */
    LOG_DEBG    =           0x05,   /**< debug  */
    LOG_VBOS    =           0x06,   /**< highest grade of information */
    LOG_MAX_                        /**< maximum value, used for range check */
}LogLevelType;

/**
 * Wraps around the dlt. This class is instantiated as a singleton and offers a default
 * context (maincontext) that is registered to log to.
 * Logging under the default context can simply be done with the logInfo/logError templates with up to 10 values at a time.
 * For logging with a different context, you can use the log template. First register a context with registerContext.
 */
class CDltWrapper
{
public:
    static CDltWrapper* instance(const bool enableNoDLTDebug = false);
    void registerApp(const char *appid, const char * description);
    void registerContext(LogContext& handle, const char *contextid, const char * description);
    void unregisterContext(LogContext& handle);

    bool init(LogLevelType loglevel, LogContext *context = NULL);
    bool initUI(LogLevelType loglevel);
    void send();
    void append(const int8_t value);
    void append(const uint8_t value);
    void append(const int16_t value);
    void append(const uint16_t value);
    void append(const int32_t value);
    void append(const uint32_t value);
    void append(const uint64_t value);
    void append(const int64_t value);
    void append(const unsigned long long value);
    void append(const float value);
    void append(const double value);
    void append(const char*& value);
    void append(const std::string& value);
    void append(const bool value);

    LogContext* getUILogContext(){return &mUIContext;};

    ~CDltWrapper();
private:
    CDltWrapper(const bool isOutputLog2File); //is private because of singleton pattern

    LogContext mDltContext; //!< the default context
    LogContextData mDltContextData; //!< contextdata
    LogContext mUIContext;
    static CDltWrapper* mpDLTWrapper; //!< pointer to the wrapper instance
    static pthread_mutex_t mMutex;

};

/**
 * returns the instance of the CDltWrapper
 * @return
 */
inline CDltWrapper* getWrapper()
{
    return (CDltWrapper::instance(1));
}

void dltEnableVerboseMode(void);

void dltDisableVerboseMode(void);

#endif /* DLTWRAPPER_H_ */
