/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include <iostream>
#include <new>

#include <stdlib.h>
#include <string.h>

#include "ama_mem.h"

#ifdef _DEBUG_MEMORY_

    #include <sys/types.h>
    #include <unistd.h>

    #include <glib.h>

    typedef struct {
        std::size_t size;
        char *file;
        int line;
    } debug_record_t;

    static GHashTable *hash = NULL;
    static std::size_t summary = 0;

    G_LOCK_DEFINE(hash);

#endif/* _DEBUG_MEMORY_ ->Line 17*/

// C

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _DEBUG_MEMORY_
void debug_malloc_insert(void* p, size_t size, const char* file, int line)
{
    if(hash == NULL) {
        hash = g_hash_table_new (g_direct_hash, g_direct_equal);
        // g_mutex_init (&mutex);
    }


    debug_record_t *r = (debug_record_t*)malloc(sizeof(debug_record_t));
    if(r != NULL) {
        r->size = size;
        r->file = strdup(basename((char*)file));
        r->line = line;
        G_LOCK(hash);
        g_hash_table_insert(hash, p, r);
        G_UNLOCK(hash);
    }
}

void debug_malloc_remove(void* p)
{
    if(hash != NULL) {
        G_LOCK(hash);
        debug_record_t *r = (debug_record_t*)g_hash_table_lookup(hash, p);
        if(r != NULL) {
            if(r->file != NULL) {
                free(r->file);
            }
            free(r);
        }
        g_hash_table_remove(hash, p);
        G_UNLOCK(hash);
    }
}


void *_ama_malloc(size_t size, const char* file, int line)
{
    void *p = malloc (size);

#ifdef _DEBUG_MEMORY_
    debug_malloc_insert(p, size, file, line);
    _DEBUG_MEMORY_LOG("[%5d][%s:%d]malloc %p : %d\n", getpid(), ::basename((char*)file), line, p, size);
#endif

    return p;
}

void ama_free(void *ptr)
{

#ifdef _DEBUG_MEMORY_
    debug_malloc_remove(ptr);
    _DEBUG_MEMORY_LOG("[%5d]free %p\n", getpid(), ptr);
#endif

    free(ptr);
}

void *_ama_calloc(size_t nmemb, size_t size, const char* file, int line)
{
    void *p = calloc(nmemb, size);

#ifdef _DEBUG_MEMORY_
    debug_malloc_insert(p, size, file, line);
    _DEBUG_MEMORY_LOG("[%5d][%s:%d]calloc %p : %d\n", getpid(), ::basename((char*)file), line, p, size);
#endif

    return p;
}

void *_ama_realloc(void *ptr, size_t size, const char* file, int line)
{
    void *p = realloc(ptr, size);

#ifdef _DEBUG_MEMORY_

    if(ptr == NULL) { // equivalent to ama_malloc(size)
        debug_malloc_insert(p, size, file, line);
    }
    else if(size == 0) { // equivalent to ama_free(ptr)
        debug_malloc_remove(ptr);
    }
    else { // real realloc
        debug_malloc_remove(ptr);
        debug_malloc_insert(p, size, file, line);
    }

    _DEBUG_MEMORY_LOG("[%5d][%s:%d]realloc %p : %d\n", getpid(), ::basename((char*)file), line, p, size);
#endif

    return p;
}

#endif/* _DEBUG_MEMORY_ ->Line 43*/

#ifdef __cplusplus
}
#endif

// C++

#ifdef _DEBUG_MEMORY_

static void
_summary (gpointer key,
           gpointer value,
           gpointer user_data)
{
    debug_record_t *r = (debug_record_t*)value;
    if(r != NULL) {
        summary += r->size;
    }
}

static void* func_data = NULL;

static void
_details (gpointer key,
           gpointer value,
           gpointer user_data)
{
    debug_record_t *r = (debug_record_t*)value;
    debug_malloc_detail_func func = (debug_malloc_detail_func)user_data;
    if(r != NULL && func != NULL) {
        func(r->file, r->line, r->size, func_data);
    }
}

#endif/* _DEBUG_MEMORY_ ->Line 146*/

size_t debug_malloc_summary(void)
{
#ifdef _DEBUG_MEMORY_
    {
        summary = 0;
        G_LOCK(hash);
        g_hash_table_foreach(hash, _summary, NULL);
        G_UNLOCK(hash);
        return summary;
    }
#else
    {
        //do nothing
        return 0;
    }
#endif/* _DEBUG_MEMORY_ ->Line 186*/
}

void debug_malloc_summary_details(debug_malloc_detail_func func, void* user_data)
{
#ifdef _DEBUG_MEMORY_
    {
        func_data = user_data;
        G_LOCK(hash);
        g_hash_table_foreach(hash, _details, (gpointer)func);
        G_UNLOCK(hash);
    }
#else
    {
        //do nothing
        return;
    }
#endif/* _DEBUG_MEMORY_ ->Line 204*/

}
