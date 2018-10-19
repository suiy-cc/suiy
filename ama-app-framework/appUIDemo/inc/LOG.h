/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef __LOG_H__
#define __LOG_H__

#ifndef LOG_TAG
#define LOG_TAG ""
#endif

#ifndef TRUE
#define TRUE (~0)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#ifdef DLT_2_14

    #include <dlt/dlt.h>

    #define _LOG(c,p,...)  \
    { \
        static char _strbuf[1024]; \
        snprintf(_strbuf, 1024, ##__VA_ARGS__); \
        DLT_LOG(c, p, DLT_STRING(_strbuf)); \
    }

    #define LOGI(fmt,...) _LOG(myContext, DLT_LOG_INFO, fmt, ##__VA_ARGS__)
    #define LOGD(fmt,...) _LOG(myContext, DLT_LOG_DEBUG, fmt, ##__VA_ARGS__)
    #define LOGW(fmt,...) _LOG(myContext, DLT_LOG_WARN, fmt, ##__VA_ARGS__)
    #define LOGE(fmt,...) _LOG(myContext, DLT_LOG_ERROR, fmt, ##__VA_ARGS__)

    DLT_IMPORT_CONTEXT(myContext);

    #define LOG_INIT() {\
        DLT_REGISTER_APP(LOG_TAG, LOG_TAG" for Logging");\
        DLT_REGISTER_CONTEXT(myContext, LOG_TAG, LOG_TAG" Context for Logging");\
    }

    #define LOG_DECLARE() DLT_DECLARE_CONTEXT(myContext);

    #define LOG_UNINIT() {\
        DLT_UNREGISTER_CONTEXT(myContext);\
        DLT_UNREGISTER_APP();\
    }

#else

    #include <stdio.h>

    #define LOGI(fmt,...) printf("["LOG_TAG"](%6d) Info : " fmt, __LINE__, ##__VA_ARGS__)
    #define LOGD(fmt,...) printf("["LOG_TAG"](%6d) Debug : " fmt, __LINE__, ##__VA_ARGS__)
    #define LOGW(fmt,...) printf("["LOG_TAG"](%6d) Warning : " fmt, __LINE__, ##__VA_ARGS__)
    #define LOGE(fmt,...) fprintf(stderr, "["LOG_TAG"](%6d) Error : " fmt, __LINE__, ##__VA_ARGS__)

    #define LOG_INIT()
    #define LOG_DECLARE()
    #define LOG_UNINIT()

#endif

#define LOGF(fmt,...) LOGI("%s(%d):%s : " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define CHECK_IF_FAIL(c) \
    {\
        if(!(c)) {\
            LOGE("%s(%d): warning '%s' fail\n", __FILE__, __LINE__, #c);\
            return;\
        }\
    }

#define CHECK_IF_FAIL2(c, r) \
    {\
        if(!(c)) {\
            LOGE("%s(%d): warning '%s' fail\n", __FILE__, __LINE__, #c);\
            return r;\
        }\
    }


#endif
