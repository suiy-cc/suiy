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
 
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include "MSG/ivi_msg.h"

struct msg_recv_arg_t{
	key_t key;
	ivi_msg_callback_t callback;
};

static key_t key_gen(char *key)
{
	key_t key_val = 0;
	int len = strlen(key);
	int i = 0;
	
	for(i = 0 ; i < len ; i++){
		key_val += key[i];
		key_val *= key[i];
	}
	
	return key_val;
}

int ivi_msg_send(char *key_str, void *buf, int buf_size)
{
	key_t key;
	int msg_id;
	int ret;
	struct msgbuf *msg_buf = NULL;;
	
	if (buf_size > MSG_MAX_SIZE){
		printf("buf_size more than %d", MSG_MAX_SIZE);
		return -1;
	}
	
	key = key_gen(key_str);
	
	msg_id = msgget(key, IPC_CREAT);
	if (msg_id == -1){
		printf("[%s]msgget failed, %m\n", __FUNCTION__);
		return -1;
	}
	
	printf("[%s]key :0x%x msg_id %d\n", __FUNCTION__, key, msg_id);
	
	msg_buf = malloc(buf_size + sizeof(struct msgbuf));
	msg_buf->mtype = 1;
	memcpy(msg_buf->mtext, (char *)buf, buf_size);
	
	ret = msgsnd(msg_id, msg_buf, buf_size, 0);
	free(msg_buf);
	if (ret == -1){
		printf("[%s]msgsnd failed, %m\n", __FUNCTION__);
		return -1;
	}
	
	return 0;
}

static void *msg_recv_task(void *arg)
{
	int msg_id;
	struct msg_recv_arg_t *msg_arg = arg;
	ivi_msg_callback_t callback;
	ssize_t ret_size;
	struct msgbuf *msg_buf = NULL;
	
	msg_id = msgget(msg_arg->key , IPC_CREAT);
	callback = msg_arg->callback;
	
	printf("[%s]key :0x%x msg_id %d\n", __FUNCTION__, msg_arg->key, msg_id);
	
	free(arg);
	
	msg_buf = malloc(MSG_MAX_SIZE + sizeof(struct msgbuf));
	
	for(;;){
		ret_size = msgrcv(msg_id, msg_buf, MSG_MAX_SIZE, 0, 0);
		if (ret_size == -1){
			printf("[%s]msgrcv failed %m\n", __FUNCTION__);
			sleep(1);
		}else{
			callback(msg_buf->mtext, ret_size);
		}
	}
	
	free(msg_buf);
	
	return NULL;
}

int ivi_msg_start_recv(char *key_str, ivi_msg_callback_t callback)
{
	int ret = 0;
	key_t key;
	struct msg_recv_arg_t *arg;
	pthread_attr_t  attr;
	pthread_t thread_id;
	
	key = key_gen(key_str);
	
	arg = malloc(sizeof(struct msg_recv_arg_t));
	
	arg->key = key;
	arg->callback = callback;
	
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
	pthread_attr_setschedpolicy( &attr, SCHED_OTHER);
	
	ret = pthread_create(&thread_id, &attr, msg_recv_task, arg);
	if (ret){
		printf("Task create failed\n");
		return -1;
	}
	
	return 0;
	
}


