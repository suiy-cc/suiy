/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef __AMA_MEM_H__
#define __AMA_MEM_H__

#ifdef __cplusplus
    #include <new>
#endif

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////    PUBLIC     /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _DEBUG_MEMORY_

    #define ama_malloc(s)     _ama_malloc((s), __FILE__, __LINE__)
    #define ama_calloc(n, s)  _ama_calloc((n), (s), __FILE__, __LINE__)
    #define ama_realloc(p, s) _ama_realloc((p), (s), __FILE__, __LINE__)

    extern void ama_free(void *ptr);

#else

    #define ama_malloc  malloc
    #define ama_calloc  calloc
    #define ama_realloc realloc
    #define ama_free    free

#endif

typedef void (*debug_malloc_detail_func)(
    const char* file, int line, size_t size, void* user_data);

extern size_t debug_malloc_summary(void);
extern void debug_malloc_summary_details(
                                debug_malloc_detail_func, void*);


#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////    PRIVATE    /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG_MEMORY_

    #define _DEBUG_MEMORY_LOG(fmt, ...)
    // #define _DEBUG_MEMORY_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__);

    #ifdef __cplusplus
    extern "C" {
    #endif

        extern void *_ama_malloc(size_t size,
                                    const char* file, int line);
        extern void *_ama_calloc(size_t nmemb,
                                    size_t size, const char* file, int line);
        extern void *_ama_realloc(void *ptr,
                                    size_t size, const char* file, int line);

        extern void debug_malloc_insert(void* p,
                                    size_t size, const char* file, int line);
        extern void debug_malloc_remove(void* p);

    #ifdef __cplusplus
    }
    #endif

    #ifdef __cplusplus

        inline void* operator new (std::size_t size,
                                    const char* file, int line)
        {
            void *p = malloc (size);

            debug_malloc_insert(p, size, file, line);

            _DEBUG_MEMORY_LOG("[%5d][%s:%d]new %p : %d\n",
                getpid(), file, line, p, size);
            return p;
        }

        inline void* operator new[] (std::size_t size,
                                    const char* file, int line)
        {
            void *p = operator new (size, file, line);
            return p;
        }

        inline void operator delete (void* ptr)
        {
            debug_malloc_remove(ptr);
            _DEBUG_MEMORY_LOG("[%5d]delete %p\n", getpid(), ptr);

            free (ptr);
        }

        inline void operator delete[] (void* ptr)
        {
           operator delete (ptr);
        }

        #define new new(__FILE__, __LINE__)

    #endif /* __cplusplus */

#endif /* _DEBUG_MEMORY_ */

#endif /* __AMA_MEM_H__ */
