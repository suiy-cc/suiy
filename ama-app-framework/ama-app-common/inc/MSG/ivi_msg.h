/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 *
 * ivi_msg -- A simple wrapper for message send/receive. 
 *
 * author:
 * 		songshzh@neusoft.com
 *
 */

#ifndef IVI_MSG_H
#define IVI_MSG_H

#ifdef __cplusplus
extern "C" {
#endif	

struct msgbuf {
	long mtype;       /* message type, must be > 0 */
    char mtext[1];    /* message data */
};

#define MSG_MAX_SIZE	4096

/* buf: point to message lib internal memory, should copy out before callback return. */
typedef void (*ivi_msg_callback_t)(void *buf, int buf_size);

extern int ivi_msg_send(char *key, void *buf, int buf_size);
extern int ivi_msg_start_recv(char *key, ivi_msg_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif /* IVI_MSG_H */
