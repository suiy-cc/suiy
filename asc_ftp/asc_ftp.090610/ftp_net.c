/*********************************************************************
*
*	Copyright (C), 1995-2003, Si-Tech Information Technology Ltd.
*
**********************************************************************
*
*	File_name:	ftp_net.c
*	Author:		张奇伟
*	Version:	版本（V1.00） 
*	Date:		2003/11/15
*	Description:
*		本文件封装了所有底层的传输函数
*
**********************************************************************/

#include <stdio.h>
#include <unistd.h>
#ifdef LINUX
#include <sys/select.h>
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "ftp_net.h"

extern int make_connection(int *sockfd,char *cip,unsigned int iport,char *err_code)
{
	struct sockaddr_in serveradd;
	struct protoent *transeproto;
	char err_affix[1000]={
		0	};

	serveradd.sin_family = PF_INET;
	serveradd.sin_port = htons(iport);
	if(inet_aton(cip,&serveradd.sin_addr) <= 0)
	{
		sprintf(err_affix,"associated with [%s]",cip);
		deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_IP_TRAN,err_affix,err_code);
		return 0;
	};
	transeproto = getprotobyname("tcp");
	if((*sockfd = socket(PF_INET,SOCK_STREAM,transeproto->p_proto)) <= 0)
	{
		deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_SOCKET,"n/a",err_code);
		return 0;
	};
	if(connect(*sockfd,(struct sockaddr *)(&serveradd),sizeof(struct sockaddr))<0 )
	{
		deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_CONN,"n/a",err_code);
		return 0;
	};
	return 1;
}

extern int send_command(int sockfd,char *msg_command,char *err_code)
{
	int msg_length;
	int sent_length;
	int iret;
	fd_set write_fds;
	struct timeval timeout;

	timeout.tv_sec=NET_TIME_OUT;
	timeout.tv_usec=0;
	sent_length=0;
	msg_length=strlen(msg_command);
	errno=0;

	while(sent_length<msg_length)
	{
		FD_ZERO(&write_fds);
		FD_SET(sockfd,&write_fds);
		switch(select(sockfd+1,NULL,(fd_set *)&write_fds,NULL,&timeout))
		{
		case -1:
			deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_SELECT,"n/a",err_code);
			close(sockfd);
			return 0;
		case 0:
			deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_TIMEOUT,"n/a",err_code);
			close(sockfd);
			return 0;
		default:
			if(FD_ISSET(sockfd,&write_fds))
			{
				iret=send(sockfd,msg_command,msg_length,0);
				sent_length+=iret;
				if(-1==iret)
				{
					deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_SEND,"n/a",err_code);
					close(sockfd);
					return 0;
				}
			}
		}
	}
	return 1;
}

extern int get_line_msg(int sockfd,char *ret_msg,char *err_code)
{
	int get_count;
	int exit_do;
	fd_set read_fds;
	struct timeval timeout;

	timeout.tv_sec=NET_TIME_OUT;
	timeout.tv_usec=0;
	get_count=0;
	exit_do=1;

	while(exit_do)
	{
		FD_ZERO(&read_fds);/*清除与read_fds关联的所有句柄*/
		FD_SET(sockfd,&read_fds);/*建立sockfd与read_fds的联系*/
		switch(select(sockfd+1,&read_fds,NULL,NULL,&timeout))
		{
		case -1:
			deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_SELECT,"n/a",err_code);
			close(sockfd);
			return 0;
		case 0:
			deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_TIMEOUT,"n/a",err_code);
			close(sockfd);
			return 0;
		default:
			if(FD_ISSET(sockfd,&read_fds))/*判断与read_fds关联的sockfd是否可读写，> 0 表示可读*/
			{
				if(-1==read(sockfd,ret_msg+get_count,1))
				{
					deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_READ,"n/a",err_code);
					close(sockfd);
					return 0;
				}
				if('\n'==ret_msg[get_count])
				{
					ret_msg[get_count+1]=0;
					exit_do=0;
				}
				get_count++;
			}
		}
	}
	return 1;
}

extern int get_data_with_proper_size(int sockfd,unsigned long data_size,char *ret_data,unsigned long *get_size,char *err_code)
{
	unsigned long get_count;
	long ret;
	fd_set fd_rds;
	struct timeval timeout;

	timeout.tv_sec=NET_TIME_OUT;
	timeout.tv_usec=0;
	get_count=0;

	while(get_count<data_size)
	{
		FD_ZERO(&fd_rds);
		FD_SET(sockfd,&fd_rds);
		switch(select(sockfd+1,&fd_rds,NULL,NULL,&timeout))
		{
		case -1:
			deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_SELECT,"n/a",err_code);
			close(sockfd);
			return 0;
		case 0:
			deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_TIMEOUT,"n/a",err_code);
			close(sockfd);
			return 0;
		default:
			if(FD_ISSET(sockfd,&fd_rds))
			{
				if(-1==(ret=read(sockfd,ret_data+get_count,data_size-get_count)))
				{
					deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_READ,"n/a",err_code);
					close(sockfd);
					return 0;
				}
				if(0==ret)
				{
					*get_size=get_count;
					close(sockfd);
					return -1;	/*-1表示因连接断开而终止*/
				}
				get_count+=ret;
			}
		}
	}
	*get_size=get_count;
	return 1;
}

extern int put_data_with_proper_size(int sockfd,long data_size,char *put_data,char *err_code)
{
	unsigned long put_count;
	long ret;
	fd_set fd_wrs;
	struct timeval timeout;

	timeout.tv_sec=NET_TIME_OUT;
	timeout.tv_usec=0;
	put_count=0;
	while(put_count<data_size)
	{
		FD_ZERO(&fd_wrs);
		FD_SET(sockfd,&fd_wrs);
		switch(select(sockfd+1,NULL,&fd_wrs,NULL,&timeout))
		{
		case -1:
			deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_SELECT,"n/a",err_code);
			close(sockfd);
			return 0;
		case 0:
			deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_TIMEOUT,"n/a",err_code);
			close(sockfd);
			return 0;
		default:
			if(FD_ISSET(sockfd,&fd_wrs))
			{
				if(0>=(ret=write(sockfd,put_data+put_count,data_size-put_count)))
				{
					deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_WRITE,"n/a",err_code);
					close(sockfd);
					return 0;
				}
				put_count+=ret;
			}
		}
	}
	return 1;
}

extern int make_socket_server(int *out_socket,unsigned int *out_port,char *err_code)
{
	struct sockaddr_in local_server;
	struct protoent *transeproto;
	int addr_len;

	transeproto = getprotobyname("tcp");
	if((*out_socket = socket(PF_INET,SOCK_STREAM,transeproto->p_proto)) <= 0)
	{
		deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_SOCKET,"n/a",err_code);
		return 0;
	};
	memset(&local_server,0,sizeof(struct sockaddr_in));
	local_server.sin_family=PF_INET;
	local_server.sin_addr.s_addr=INADDR_ANY;
	if(-1==bind(*out_socket,(struct sockaddr *)(&local_server),sizeof(local_server)))
	{
		deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_BIND,"n/a",err_code);
		return 0;
	}
	addr_len=sizeof(local_server);
	if(-1==getsockname(*out_socket,(struct sockaddr *)(&local_server),&addr_len))
	{
		deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_BIND,"n/a",err_code);
		close(*out_socket);
		return 0;
	}
	*out_port=ntohs(local_server.sin_port);
	if(-1==listen(*out_socket,5))
	{
		deal_run_err_msg("warn",__FILE__,__LINE__,NET_ERR_BIND,"n/a",err_code);
		close(*out_socket);
		return 0;
	}
	return 1;
}
