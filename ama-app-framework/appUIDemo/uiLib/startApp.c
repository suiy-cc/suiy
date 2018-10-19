/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <ilm_common.h>
#include <ilm_client.h>
#include <ilm_control.h>

#include "hmi.h"
#include "common.h"

LOG_DECLARE();

static int _do_start_execv(char *const argv[])
{
    LOGI("%s\n", __FUNCTION__);

    int ret = -1;
    int pid = -1;

    if(access(argv[0], F_OK) != 0) {
    	return -1;
    }

    pid = fork();

    LOGD("pid=%d\n", pid);

    if( pid > 0 ){

    }
    else if( pid == 0 ){

        {
            int i=0;
            LOGD("binary = %s\n", argv[0]);
            while(argv[i] != NULL) {
                LOGD("argv[%d] = %s\n", i, argv[i]);
                i++;
            }
        }

        execv(argv[0], argv);

        LOGE("execv : %s for %s", strerror(errno), argv[0]);

    }
    else{
        LOGE( "fork failed\n");
    }

    LOGD("%d quit.\n", pid);

    return ret;
}

static int read_to_buf(const char *filename, void *buf, int size)
{
    int fd;
    /* open_read_close() would do two reads, checking for EOF.
     * When you have 10000 /proc/$NUM/stat to read, it isn't desirable */
    ssize_t ret = -1;
    fd = open(filename, O_RDONLY);
    if (fd >= 0) {
        ret = read(fd, buf, size-1);
        close(fd);
    }
    ((char *)buf)[ret > 0 ? ret : 0] = '\0';

    if(((char *)buf)[ret > 0 ? ret-1 : 0] == '\n') {
        ((char *)buf)[ret > 0 ? ret-1 : 0] = '\0';
    }
    return ret;
}

void do_start_execv(const char* bin, unsigned int surfaceID, unsigned int layerId, int width, int height)
{
    LOG_INIT();

    LOGI("%s\n", __FUNCTION__);

    char cmd[1024];

    sprintf(cmd, "ps | grep \"/usr/share/ilm/testILM/\" | grep %u > /tmp/do_start_execv.process.check", surfaceID);
    system(cmd);

    char buff[10240];
    read_to_buf("/tmp/do_start_execv.process.check", buff, 10240);

    //LOGI("read from /tmp/bar.process.check : %s\n", buff);

    if(strstr(buff, bin) == NULL) {

        char id[10], w[10], h[10];
        sprintf(id, "%u", surfaceID);
        sprintf(w, "%d", width);
        sprintf(h, "%d", height);

        char * const argv[] = {
            [0] = (char*)bin,
            [1] = id,
            [2] = w,
            [3] = h,
            [4] = NULL
        };

        _do_start_execv(argv);
    }
    else {
        LOGI("surfaceSetVisibility(0x%08X) : ILM_TRUE\n", surfaceID);
        ilmErrorTypes err = ILM_SUCCESS;
        err = ilm_surfaceSetVisibility(surfaceID, ILM_FALSE);
        if(err != ILM_SUCCESS) {
            LOGE("surfaceSetVisibility(0x%08X) : %s\n", surfaceID, ILM_ERROR_STRING(err));
        }
        err = ilm_commitChanges();
        if(err != ILM_SUCCESS) {
            LOGE("commitChanges() : %s\n", ILM_ERROR_STRING(err));
        }

        err = ilm_layerRemoveSurface(layerId, surfaceID);
        if(err != ILM_SUCCESS) {
            LOGE("layerRemoveSurface(0x%08X) : %s\n", surfaceID, ILM_ERROR_STRING(err));
        }
        err = ilm_layerAddSurface(layerId, surfaceID);
        if(err != ILM_SUCCESS) {
            LOGE("layerAddSurface(0x%08X) : %s\n", surfaceID, ILM_ERROR_STRING(err));
        }
        err = ilm_surfaceSetVisibility(surfaceID, ILM_TRUE);
        if(err != ILM_SUCCESS) {
            LOGE("surfaceSetVisibility(0x%08X) : %s\n", surfaceID, ILM_ERROR_STRING(err));
        }
        err = ilm_commitChanges();
        if(err != ILM_SUCCESS) {
            LOGE("commitChanges() : %s\n", ILM_ERROR_STRING(err));
        }
    }

   unlink("/tmp/do_start_execv.process.check");

   LOG_UNINIT();
}
