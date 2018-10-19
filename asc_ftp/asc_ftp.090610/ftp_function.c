/*********************************************************************
*
*	Copyright (C), 1995-2003, Si-Tech Information Technology Ltd.
*
**********************************************************************
*
*	File_name:	ftp_function.c
*	Author:		张奇伟
*	Version:	版本（V1.00） 
*	Date:		2003/11/15
*	Description:
*		本文件中描述的函数包含所有与FTP协议会话相关的操作。
*	此处的所有函数直接被主函数调用。基于底层的传输控制函数不在
*	此处实现。
*
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "ftp_function.h"

/*********************************************************************
*	获取服务器所支持的传输模式
*/
static int get_support_info
(
int ,					/*socket句柄*/
struct STRU_SERVER_INFO *,		/*服务器配置信息*/
struct STRU_SUPPORT_ARGUMENTS *,	/*服务器支持的命令信息*/
char *					/*错误代码*/
);

/*********************************************************************
*	检查目录配置
*/
static int check_directory
(
struct STRU_FILE_SYSTEM_INFO 
);

/*********************************************************************
*	生成PORT所需信息
*/
static int Retr_PORT_Info
(
char *,		/*IP字符信息*/
unsigned int ,	/*端口信息*/
char *,		/*输出PORT信息*/
char *		/*错误代码*/
);

extern int init_environment(char *password_file,int u_len,int p_len,char *config_file,char *user_name,char *password_val,int *sockfd,struct STRU_FILE_SYSTEM_INFO *stru_file_info,struct STRU_SERVER_INFO *stru_server_info,struct STRU_SUPPORT_ARGUMENTS *stru_support_info,char *err_code)
{
	char p_command[FUN_CMD_LINE_LEN_MAX]={
		0	};
	char msg_info[FUN_RET_LINE_LEN_MAX]={
		0	};
	int i;
	/*读取密码文件的用户名和密码*/
	if(!get_pwd_info(password_file,FUN_KEY_VALUE,user_name,u_len,password_val,p_len,err_code))
	{
		return 0;
	}
	/*读取配置文件信息*/
	if(!init_config_info(config_file,stru_file_info,stru_server_info,err_code))
	{
		return 0;
	}
	for(i=0;i<stru_server_info->file_sys_group_count;i++)
	{
		if(!check_directory(stru_file_info[i]))/*判断上传还是下载*/
		{
			fprintf(stderr,"File group%02d's directorys config error!\n",i);
			exit(1);
		}
	}
	/*建立制定ip的socket连接*/
	if(!make_connection(sockfd,stru_server_info->host_ip,stru_server_info->host_port,err_code))
	{
		return 0;
	}
	if(!get_blk_msg(*sockfd,err_code))/*读取i/o口的信息*/
	{
		return 0;
	}
	memset(p_command,0,FUN_CMD_LINE_LEN_MAX);
	sprintf(p_command,"USER %s\r\n",user_name);
	if(!send_command(*sockfd,p_command,err_code))
	{
		return 0;
	}
	if(!get_blk_msg(*sockfd,err_code))
	{
		return 0;
	}
	memset(p_command,0,FUN_CMD_LINE_LEN_MAX);
	sprintf(p_command,"PASS %s\r\n",password_val);
	if(!send_command(*sockfd,p_command,err_code))
	{
		return 0;
	}
	if(!get_info_msg(*sockfd,msg_info,err_code))
	{
		return 0;
	}
	if(0==strncmp(msg_info,FUN_FTP_TAG_PASS_ERR,4))
	{
		close(*sockfd);
		return 0;
	}
	if(!get_support_info(*sockfd,stru_server_info,stru_support_info,err_code))
	{
		return 0;
	}
#ifdef DEBUG_FUN_INIT_ENVIRONMENT
	fprintf(stderr,"support_pasv=[%d]	support_size=[%d]	support_resume=[%d]\n",stru_support_info->Is_support_pasv,stru_support_info->Is_support_size,stru_support_info->Is_support_resume);
#endif
	return 1;
}

static int check_directory(struct STRU_FILE_SYSTEM_INFO stru_file_info)
{
	struct stat st_dir1;
	struct stat st_dir2;

	if(0==strcmp(FUN_PUB_CONF_DOWN,stru_file_info.trans_file_direction))
	{
		st_dir1.st_dev=0;
		st_dir2.st_dev=0;
		stat(stru_file_info.dst_directory,&st_dir1);
		stat(stru_file_info.tmp_directory,&st_dir2);
		if(st_dir1.st_dev!=st_dir2.st_dev)
		{
			return 0;
		}
	}
	else
	{
		st_dir1.st_dev=0;
		st_dir2.st_dev=0;
		stat(stru_file_info.src_directory,&st_dir1);
		stat(stru_file_info.bak_directory,&st_dir2);
		if(st_dir1.st_dev!=st_dir2.st_dev)
		{
			return 0;
		}
	}
	st_dir1.st_dev=0;
	st_dir2.st_dev=0;
	stat(stru_file_info.log_directory,&st_dir1);
	stat(stru_file_info.log_tmp_directory,&st_dir2);
	if(st_dir1.st_dev!=st_dir2.st_dev)
	{
		return 0;
	}
	return 1;
}

static int get_support_info(int sockfd,struct STRU_SERVER_INFO *stru_server_info,struct STRU_SUPPORT_ARGUMENTS *stru_support_info,char *err_code)
{
	char p_command[FUN_CMD_LINE_LEN_MAX];
	char msg_info[FUN_RET_LINE_LEN_MAX];

	if(FUN_PUB_CONF_AUTO==stru_server_info->pasv_support)
	{
		sprintf(p_command,"PASV\r\n");
		if(!send_command(sockfd,p_command,err_code))
		{
			return 0;
		}
		memset(msg_info,0,FUN_RET_LINE_LEN_MAX);
		if(!get_info_msg(sockfd,msg_info,err_code))
		{
			return 0;
		}
		if(0==strncmp(msg_info,FUN_FTP_TAG_PASV_OK,4))
		{
			stru_support_info->Is_support_pasv=1;
		}
		else
		{
			stru_support_info->Is_support_pasv=0;
		}
	}
	else if(FUN_PUB_CONF_YES==stru_server_info->pasv_support)
	{
		stru_support_info->Is_support_pasv=1;
	}
	else
	{
		stru_support_info->Is_support_pasv=0;
	}

	if(FUN_PUB_CONF_AUTO==stru_server_info->size_support)
	{
		sprintf(p_command,"SIZE\r\n");
		if(!send_command(sockfd,p_command,err_code))
		{
			return 0;
		}
		memset(msg_info,0,FUN_RET_LINE_LEN_MAX);
		if(!get_info_msg(sockfd,msg_info,err_code))
		{
			return 0;
		}
		if(0==strncmp(msg_info,FUN_FTP_TAG_SIZE_OK,4))
		{
			stru_support_info->Is_support_size=1;
		}
		else
		{
			stru_support_info->Is_support_size=0;
		}
	}
	else if(FUN_PUB_CONF_YES==stru_server_info->size_support)
	{
		stru_support_info->Is_support_size=1;
	}
	else
	{
		stru_support_info->Is_support_size=0;
	}

	if(FUN_PUB_CONF_AUTO==stru_server_info->resume_support)
	{
		sprintf(p_command,"REST 100\r\n");
		if(!send_command(sockfd,p_command,err_code))
		{
			return 0;
		}
		memset(msg_info,0,FUN_RET_LINE_LEN_MAX);
		if(!get_info_msg(sockfd,msg_info,err_code))
		{
			return 0;
		}
		if(0==strncmp(msg_info,FUN_FTP_TAG_RESUME_OK,4))
		{
			stru_support_info->Is_support_resume=1;
		}
		else
		{
			stru_support_info->Is_support_resume=0;
		}
		sprintf(p_command,"REST 0\r\n");
		if(!send_command(sockfd,p_command,err_code))
		{
			return 0;
		}
		memset(msg_info,0,FUN_RET_LINE_LEN_MAX);
		if(!get_info_msg(sockfd,msg_info,err_code))
		{
			return 0;
		}
	}
	else if(FUN_PUB_CONF_YES==stru_server_info->resume_support)
	{
		stru_support_info->Is_support_resume=1;
	}
	else
	{
		stru_support_info->Is_support_resume=0;
	}
	return 1;
}

static int Retr_PORT_Info(char *in_cip,unsigned int in_port,char *out_info,char *err_code)
{
	char p_ip[4][4]={
		0	};
	char p_in_cip[20]={
		0	};
	int p_port_head;
	int p_port_tail;
	char *p_tmp;

	char err_affix[1000]={
		0	};

	sprintf(p_in_cip,"%s",in_cip);

	if(NULL==(p_tmp=strrchr(p_in_cip,'.')))
	{
		sprintf(err_affix,"associated with [%s]",in_cip);
		deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_PORT_INFO,err_affix,err_code);
		return 0;
	}
	sprintf(p_ip[3],"%s",p_tmp+1);
	p_tmp[0]=0;
	if(NULL==(p_tmp=strrchr(p_in_cip,'.')))
	{
		sprintf(err_affix,"associated with [%s]",in_cip);
		deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_PORT_INFO,err_affix,err_code);
		return 0;
	}
	sprintf(p_ip[2],"%s",p_tmp+1);
	p_tmp[0]=0;
	if(NULL==(p_tmp=strrchr(p_in_cip,'.')))
	{
		sprintf(err_affix,"associated with [%s]",in_cip);
		deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_PORT_INFO,err_affix,err_code);
		return 0;
	}
	sprintf(p_ip[1],"%s",p_tmp+1);
	p_tmp[0]=0;
	sprintf(p_ip[0],"%s",p_in_cip);
	p_port_head=in_port/256;
	p_port_tail=in_port%256;
	sprintf(out_info,"%s,%s,%s,%s,%d,%d",p_ip[0],p_ip[1],p_ip[2],p_ip[3],p_port_head,p_port_tail);
#ifdef DEBUG_FUN_RETR_PORT_INFO
	fprintf(stderr,"Port_info=[%s]\n",out_info);
#endif
	return 1;
}

extern int get_info_msg(int sockfd,char *ret_msg,char *err_code)
{
#ifdef DEBUG_FUN_INFO_MSG
	fprintf(stderr,"====== get_info_msg ======\n");
#endif
	if(0==get_line_msg(sockfd,ret_msg,err_code))
	{
		return 0;
	}
#ifdef DEBUG_FUN_INFO_MSG
	fprintf(stderr,"[%s]\n",ret_msg);
#endif
	return 1;
}

extern int get_blk_msg(int sockfd,char *err_code)
{
	char buff[FUN_RET_LINE_LEN_MAX];
	char head_tag[5];
	char test_tag[5];
	char err_affix[1000]={
		0	};

#ifdef DEBUG_FUN_BLK_MSG
	fprintf(stderr,"====== get_blk_msg ======\n");
#endif

	if(0==get_line_msg(sockfd,buff,err_code))
	{
		return 0;
	}
#ifdef DEBUG_FUN_BLK_MSG
	fprintf(stderr,"[%s]\n",buff);
#endif
	strncpy(head_tag,buff,4);
	head_tag[4]=0;
	if('0'>head_tag[0] || '9'<head_tag[0])
	{
		sprintf(err_affix,"associated with [%s]",head_tag);
		deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_MSG_TAG,err_affix,err_code);
		close(sockfd);
		return 0;
	}
	if('0'>head_tag[1] || '9'<head_tag[1])
	{
		sprintf(err_affix,"associated with [%s]",head_tag);
		deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_MSG_TAG,err_affix,err_code);
		close(sockfd);
		return 0;
	}
	if('0'>head_tag[2] || '9'<head_tag[2])
	{
		sprintf(err_affix,"associated with [%s]",head_tag);
		deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_MSG_TAG,err_affix,err_code);
		close(sockfd);
		return 0;
	}
	if('-'==head_tag[3])
	{
		do
		{
			if(0==get_line_msg(sockfd,buff,err_code))
			{
				return 0;
			}
#ifdef DEBUG_FUN_BLK_MSG
			fprintf(stderr,"[%s]\n",buff);
#endif
			strncpy(test_tag,buff,4);
			test_tag[4]=0;
		}while(!(' '==test_tag[3] && 0==strncmp(head_tag,test_tag,3)));
	}
	return 1;
}

extern int Retr_PASV_Info(char *pasv_info,char *ip_info,unsigned int *port_info,char *err_code)
{
	char info[30];
	char *curr;
	int h1,h2,h3,h4,p1,p2;
	char err_affix[1000]={
		0	};

#ifdef DEBUG_FUN_PASV_INFO
	fprintf(stderr,"====== Retr_PASV_Info ======\n");
#endif

	if(0==strncmp(pasv_info,FUN_FTP_TAG_PASV_OK,4))
	{
		memset(info,0,30);
		if(NULL==(curr=strchr(pasv_info,'(')))
		{
			sprintf(err_affix,"associated with [%s]",pasv_info);
			deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_PASV_INFO,err_affix,err_code);
			return 0;
		}
		sprintf(info,"%s",curr+1);
		if(NULL==(curr=strchr(info,')')))
		{
			sprintf(err_affix,"associated with [%s]",pasv_info);
			deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_PASV_INFO,err_affix,err_code);
			return 0;
		}
		curr[0]=0;
		sscanf(info,"%d,%d,%d,%d,%d,%d",&h1,&h2,&h3,&h4,&p1,&p2);
		sprintf(ip_info,"%d.%d.%d.%d",h1,h2,h3,h4);
		*port_info = p1*256+p2;
#ifdef DEBUG_FUN_PASV_INFO
		fprintf(stderr,"[%d],[%d],[%d],[%d],[%d],[%d]\n",h1,h2,h3,h4,p1,p2);
		fprintf(stderr,"[%s],[%d]\n",ip_info,*port_info);
#endif
	}
	else
	{
		sprintf(err_affix,"associated with [%s]",pasv_info);
		deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_PASV_INFO,err_affix,err_code);
		return 0;
	}
	return 1;
}

extern int diff_tow_file(char *file1,unsigned long file_size,char *err_code)
{
	struct stat file_stat;
	stat(file1,&file_stat);
	char err_affix[1000]={
		0	};

	if(file_size!=file_stat.st_size)
	{
		sprintf(err_affix,"associated with [%ld/%ld]",file_size,file_stat.st_size);
		deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_NOT_SAME,err_affix,err_code);
		return 0;
	}
	return 1;
}

extern int get_file_list(int p_socket,struct STRU_SUPPORT_ARGUMENTS *stru_support,char *p_ip_local,char *src_dir,char *list_file_name,char *err_code)
{
	char buff[FUN_RET_BUFF_LEN_MAX];
	char p_command[FUN_CMD_LINE_LEN_MAX];
	char p_ip[20];
	char msg_info[FUN_RET_LINE_LEN_MAX];
	char p_port_info[100]={
		0	};
	int p_socket_data;
	int p_socket_listen;
	unsigned int p_port;
	unsigned int p_port_local;
	int ret_code;
	int brk_flag;
	int p_remote_addr_len;
	unsigned long ret_len;
	FILE *fp;
	struct sockaddr_in p_remote_addr;
	char err_affix[1000]={
		0	};

#ifdef DEBUG_FUN_FILE_LIST
	fprintf(stderr,"====== get_file_list ======\n");
#endif

	sprintf(p_command,"CWD %s/\r\n",src_dir);
	if(!send_command(p_socket,p_command,err_code))
	{
		return 0;
	}

	if(!get_blk_msg(p_socket,err_code))
	{
		return 0;
	}

	if(stru_support->Is_support_pasv)
	{
		sprintf(p_command,"PASV\r\n");
		if(!send_command(p_socket,p_command,err_code))
		{
			return 0;
		}
		memset(msg_info,0,FUN_RET_LINE_LEN_MAX);
		if(!get_info_msg(p_socket,msg_info,err_code))
		{
			return 0;
		}

		if(!Retr_PASV_Info(msg_info,p_ip,&p_port,err_code))
		{
			close(p_socket);
			return 0;
		}

		sprintf(p_command,"LIST\r\n");
		if(!send_command(p_socket,p_command,err_code))
		{
			return 0;
		}

		if(!make_connection(&p_socket_data,p_ip,p_port,err_code))
		{
			close(p_socket);
			return 0;
		}
		if(NULL==(fp=fopen(list_file_name,"wb")))
		{
			sprintf(err_affix,"associated with [%s]",list_file_name);
			deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_FOPEN,err_affix,err_code);
			close(p_socket);
			close(p_socket_data);
			return 0;
		}

		brk_flag=1;
		while(brk_flag)
		{
			ret_code=0;
			memset(buff,0,FUN_RET_BUFF_LEN_MAX);
			ret_code=get_data_with_proper_size(p_socket_data,FUN_RET_BUFF_LEN_MAX-1,buff,&ret_len,err_code);
			if(1!=ret_code)
			{
				if(-1 /*ERR_CONN_CLOSE*/ == ret_code)
				{
					if(!get_blk_msg(p_socket,err_code))
					{
						fclose(fp);
						return 0;
					}
					memset(msg_info,0,FUN_RET_LINE_LEN_MAX);
					if(!get_info_msg(p_socket,msg_info,err_code))
					{
						fclose(fp);
						return 0;
					}
					if(0==strncmp(msg_info,"226 ",4))
					{
						if(ret_len!=fwrite(buff,1,ret_len,fp))
						{
							sprintf(err_affix,"associated with [%s]",list_file_name);
							deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_F_IO,err_affix,err_code);
							fclose(fp);
							close(p_socket);
							return 0;
						}
						brk_flag=0;
					}
					else
					{
						sprintf(err_affix,"associated with [%s]",list_file_name);
						deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_MSG_TAG,err_affix,err_code);
						fclose(fp);
						close(p_socket);
						return 0;
					}
				}
				else
				{
					fclose(fp);
					close(p_socket);
					return 0;
				}
			}
			else
			{
				if((size_t)(FUN_RET_BUFF_LEN_MAX-1)!=fwrite(buff,1,FUN_RET_BUFF_LEN_MAX-1,fp))
				{
					sprintf(err_affix,"associated with [%s]",list_file_name);
					deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_F_IO,err_affix,err_code);
					fclose(fp);
					close(p_socket_data);
					close(p_socket);
					return 0;
				}
			}
		}
		fclose(fp);
		close(p_socket_data);
	}
	else
	{
		if(!make_socket_server(&p_socket_listen,&p_port_local,err_code))
		{
			close(p_socket);
			return 0;
		}
		if(!Retr_PORT_Info(p_ip_local,p_port_local,p_port_info,err_code))
		{
			close(p_socket);
			close(p_socket_listen);
			return 0;
		}
		sprintf(p_command,"PORT %s\r\n",p_port_info);
		if(!send_command(p_socket,p_command,err_code))
		{
			close(p_socket_listen);
			return 0;
		}
		memset(msg_info,0,FUN_RET_LINE_LEN_MAX);
		if(!get_info_msg(p_socket,msg_info,err_code))
		{
			close(p_socket_listen);
			return 0;
		}

		sprintf(p_command,"LIST\r\n");
		if(!send_command(p_socket,p_command,err_code))
		{
			close(p_socket_listen);
			return 0;
		}
		memset(&p_remote_addr,0,sizeof(struct sockaddr_in));
		p_remote_addr_len=sizeof(struct sockaddr_in);
		if(-1==(p_socket_data=accept(p_socket_listen,(struct sockaddr *)(&p_remote_addr),&p_remote_addr_len)))
		{
			close(p_socket);
			close(p_socket_listen);
			return 0;
		}

		if(NULL==(fp=fopen(list_file_name,"wb")))
		{
			sprintf(err_affix,"associated with [%s]",list_file_name);
			deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_FOPEN,err_affix,err_code);
			close(p_socket);
			close(p_socket_listen);
			close(p_socket_data);
			return 0;
		}

		brk_flag=1;
		while(brk_flag)
		{
			ret_code=0;
			memset(buff,0,FUN_RET_BUFF_LEN_MAX);
			ret_code=get_data_with_proper_size(p_socket_data,FUN_RET_BUFF_LEN_MAX-1,buff,&ret_len,err_code);
			if(1!=ret_code)
			{
				if(-1 /*ERR_CONN_CLOSE*/ == ret_code)
				{
					if(!get_blk_msg(p_socket,err_code))
					{
						fclose(fp);
						close(p_socket_listen);
						return 0;
					}
					memset(msg_info,0,FUN_RET_LINE_LEN_MAX);
					if(!get_info_msg(p_socket,msg_info,err_code))
					{
						fclose(fp);
						close(p_socket_listen);
						return 0;
					}
					if(0==strncmp(msg_info,"226 ",4))
					{
						if(ret_len!=fwrite(buff,1,ret_len,fp))
						{
							sprintf(err_affix,"associated with [%s]",list_file_name);
							deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_F_IO,err_affix,err_code);
							fclose(fp);
							close(p_socket_listen);
							close(p_socket);
							return 0;
						}
						brk_flag=0;
					}
					else
					{
						sprintf(err_affix,"associated with [%s]",list_file_name);
						deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_MSG_TAG,err_affix,err_code);
						fclose(fp);
						close(p_socket);
						close(p_socket_listen);
						return 0;
					}
				}
				else
				{
					fclose(fp);
					close(p_socket);
					close(p_socket_listen);
					return 0;
				}
			}
			else
			{
				if((size_t)(FUN_RET_BUFF_LEN_MAX-1)!=fwrite(buff,1,FUN_RET_BUFF_LEN_MAX-1,fp))
				{
					sprintf(err_affix,"associated with [%s]",list_file_name);
					deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_F_IO,err_affix,err_code);
					fclose(fp);
					close(p_socket_data);
					close(p_socket_listen);
					close(p_socket);
					return 0;
				}
			}
		}
		fclose(fp);
		close(p_socket_data);
		close(p_socket_listen);
	}
	return 1;
}

extern int get_retr_file_info(char *ora_rec,struct STRU_FILE_SYSTEM_INFO p_stru_file_info,char *src_file,char *tmp_file,char *dst_file,char *err_code)
{
	char *p;
	char err_affix[1000]={
		0	};

#ifdef DEBUG_FUN_RETR_FILE_INFO
	fprintf(stderr,"====== get_retr_file_info ======\n");
	fprintf(stderr,"[%s]\n",ora_rec);
#endif

	if('-'==ora_rec[0])
	{
		p=strrchr(ora_rec,' ');
		p++;
		sprintf(src_file,"%s",p);
		sprintf(dst_file,"%s/%s%s%s",p_stru_file_info.dst_directory,p_stru_file_info.file_prefix,p,p_stru_file_info.file_postfix);
		sprintf(tmp_file,"%s/%s%s%s",p_stru_file_info.tmp_directory,p_stru_file_info.file_prefix,p,p_stru_file_info.file_postfix);
	}
	else
	{
		sprintf(err_affix,"associated with [%s]",ora_rec);
		deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_NOT_FILE,err_affix,err_code);
		return 0;
	}
	return 1;
}

extern int Identify_file_name(char *file_name,char *include_flag,char *exclude_flag)
{
	return File_is_ok(file_name,include_flag,exclude_flag);
}

extern int get_file_size(int sockfd,int Is_support_size,char *info_str,char *file_name,unsigned long *file_size,char *err_code)
{
	char p_ret_msg[FUN_RET_LINE_LEN_MAX]={
		0	};
	char p_command[FUN_CMD_LINE_LEN_MAX]={
		0	};
	char p_size[100]={
		0	};
	int num_count;
	int sp_count;
	char *p_info;
	char pre_char;

#ifdef DEBUG_FUN_GET_FILE_SIZE
	fprintf(stderr,"====== get_file_size ======\n");
#endif

	if(Is_support_size)
	{
		if(!send_command(sockfd,"TYPE I\r\n",err_code))
		{
			return 0;
		}
		if(!get_blk_msg(sockfd,err_code))
		{
			return 0;
		}
		sprintf(p_command,"SIZE %s\r\n",file_name);
		if(!send_command(sockfd,p_command,err_code))
		{
			return 0;
		}
		memset(p_ret_msg,0,FUN_RET_LINE_LEN_MAX);
		if(!get_info_msg(sockfd,p_ret_msg,err_code))
		{
			return 0;
		}
		*file_size=atol(p_ret_msg+4);
	}
	else
	{
		sp_count=0;
		p_info=info_str;
		pre_char=0;
		num_count=0;
		while(p_info[0])
		{
			if(pre_char!=p_info[0])
			{
				if(' '==p_info[0] || '\t'==p_info[0])
				{
					sp_count++;
				}
				pre_char=p_info[0];
			}
			if(4==sp_count)
			{
				p_size[num_count]=p_info[0];
				num_count++;
			}
			if(5==sp_count)
			{
				break;
			}
			p_info++;
		}
		*file_size=atol(p_size);
	}
	return 1;
}

extern int get_bin_file(int sockfd,struct STRU_SUPPORT_ARGUMENTS *stru_support,char *p_ip_local,char *info,char *file_name,char *dest_file,char *tmp_file,unsigned long *file_size,char *err_code)
{
	char p_ret_msg[FUN_RET_LINE_LEN_MAX]={
		0	};
	char buff[FUN_RET_BUFF_LEN_MAX];
	char p_command[500]={
		0	};
	char p_ip[20]={
		0	};
	char p_port_info[100]={
		0	};
	unsigned int p_port;
	unsigned int p_port_local;
	int p_socket_data;
	int p_socket_listen;
	int ret_code;
	int brk_flag;
	int p_remote_addr_len;
	unsigned long ret_len;
	unsigned long p_file_size;
	FILE *fp;
	struct sockaddr_in p_remote_addr;
	char err_affix[1000]={
		0	};

#ifdef DEBUG_FUN_GET_BIN_FILE
	fprintf(stderr,"====== get_bin_file ======\n");
#endif

	if(!send_command(sockfd,"TYPE I\r\n",err_code))
	{
		return 0;
	}
	if(!get_blk_msg(sockfd,err_code))
	{
		return 0;
	}
	if(!get_file_size(sockfd,stru_support->Is_support_size,info,file_name,&p_file_size,err_code))
	{
		return 0;
	}
	if(stru_support->Is_support_pasv)
	{
		if(!send_command(sockfd,"PASV\r\n",err_code))
		{
			return 0;
		}
		memset(p_ret_msg,0,FUN_RET_LINE_LEN_MAX);
		if(!get_info_msg(sockfd,p_ret_msg,err_code))
		{
			return 0;
		}
		if(!Retr_PASV_Info(p_ret_msg,p_ip,&p_port,err_code))
		{
			close(sockfd);
			return 0;
		}
		sprintf(p_command,"RETR %s\r\n",file_name);
		if(!send_command(sockfd,p_command,err_code))
		{
			return 0;
		}
		if(!make_connection(&p_socket_data,p_ip,p_port,err_code))
		{
			close(sockfd);
			return 0;
		}
		if(NULL==(fp=fopen(tmp_file,"wb")))
		{
			sprintf(err_affix,"associated with [%s]",tmp_file);
			deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_FOPEN,err_affix,err_code);
			close(p_socket_data);
			close(sockfd);
			return 0;
		}
		*file_size=p_file_size;
		brk_flag=1;
		while(brk_flag)
		{
			ret_code=0;
			memset(buff,0,FUN_RET_BUFF_LEN_MAX);
			ret_code=get_data_with_proper_size(p_socket_data,FUN_RET_BUFF_LEN_MAX,buff,&ret_len,err_code);
			if(1!=ret_code)
			{
				if(-1 /*ERR_CONN_CLOSE*/ == ret_code)
				{
					if(!get_blk_msg(sockfd,err_code))
					{
						fclose(fp);
						return 0;
					}
					memset(p_ret_msg,0,FUN_RET_LINE_LEN_MAX);
					if(!get_info_msg(sockfd,p_ret_msg,err_code))
					{
						fclose(fp);
						return 0;
					}
					if(0==strncmp(p_ret_msg,FUN_FTP_TAG_TRANS_OK,4))
					{
						if(ret_len!=fwrite(buff,1,ret_len,fp))
						{
							sprintf(err_affix,"associated with [%s]",tmp_file);
							deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_F_IO,err_affix,err_code);
							close(sockfd);
							fclose(fp);
							return 0;
						}
						brk_flag=0;
					}
					else
					{
						sprintf(err_affix,"associated with [%s]",p_ret_msg);
						deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_MSG_TAG,err_affix,err_code);
						fclose(fp);
						close(sockfd);
						return 0;
					}
				}
				else
				{
					fclose(fp);
					close(sockfd);
					return 0;
				}
			}
			else
			{
				if((size_t)(FUN_RET_BUFF_LEN_MAX)!=fwrite(buff,1,FUN_RET_BUFF_LEN_MAX,fp))
				{
					sprintf(err_affix,"associated with [%s]",tmp_file);
					deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_F_IO,err_affix,err_code);
					fclose(fp);
					close(p_socket_data);
					close(sockfd);
					return 0;
				}
			}
		}
		fclose(fp);
		close(p_socket_data);
	}
	else
	{
		if(!make_socket_server(&p_socket_listen,&p_port_local,err_code))
		{
			close(sockfd);
			return 0;
		}
		if(!Retr_PORT_Info(p_ip_local,p_port_local,p_port_info,err_code))
		{
			close(sockfd);
			close(p_socket_listen);
			return 0;
		}
		sprintf(p_command,"PORT %s\r\n",p_port_info);
		if(!send_command(sockfd,p_command,err_code))
		{
			close(p_socket_listen);
			return 0;
		}
		memset(p_ret_msg,0,FUN_RET_LINE_LEN_MAX);
		if(!get_info_msg(sockfd,p_ret_msg,err_code))
		{
			close(p_socket_listen);
			return 0;
		}
		sprintf(p_command,"RETR %s\r\n",file_name);
		if(!send_command(sockfd,p_command,err_code))
		{
			close(p_socket_listen);
			return 0;
		}
		memset(&p_remote_addr,0,sizeof(struct sockaddr_in));
		p_remote_addr_len=sizeof(struct sockaddr_in);
		if(-1==(p_socket_data=accept(p_socket_listen,(struct sockaddr *)(&p_remote_addr),&p_remote_addr_len)))
		{
			close(sockfd);
			close(p_socket_listen);
			return 0;
		}
		if(NULL==(fp=fopen(tmp_file,"wb")))
		{
			sprintf(err_affix,"associated with [%s]",tmp_file);
			deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_FOPEN,err_affix,err_code);
			close(p_socket_data);
			close(p_socket_listen);
			close(sockfd);
			return 0;
		}
		*file_size=p_file_size;
		brk_flag=1;
		while(brk_flag)
		{
			ret_code=0;
			memset(buff,0,FUN_RET_BUFF_LEN_MAX);
			ret_code=get_data_with_proper_size(p_socket_data,FUN_RET_BUFF_LEN_MAX,buff,&ret_len,err_code);
			if(1!=ret_code)
			{
				if(-1 /*ERR_CONN_CLOSE*/ == ret_code)
				{
					if(!get_blk_msg(sockfd,err_code))
					{
						fclose(fp);
						close(p_socket_listen);
						return 0;
					}
					memset(p_ret_msg,0,FUN_RET_LINE_LEN_MAX);
					if(!get_info_msg(sockfd,p_ret_msg,err_code))
					{
						fclose(fp);
						close(p_socket_listen);
						return 0;
					}
					if(0==strncmp(p_ret_msg,FUN_FTP_TAG_TRANS_OK,4))
					{
						if(ret_len!=fwrite(buff,1,ret_len,fp))
						{
							sprintf(err_affix,"associated with [%s]",tmp_file);
							deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_F_IO,err_affix,err_code);
							close(sockfd);
							close(p_socket_listen);
							fclose(fp);
							return 0;
						}
						brk_flag=0;
					}
					else
					{
						sprintf(err_affix,"associated with [%s]",p_ret_msg);
						deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_MSG_TAG,err_affix,err_code);
						fclose(fp);
						close(sockfd);
						close(p_socket_listen);
						return 0;
					}
				}
				else
				{
					fclose(fp);
					close(p_socket_listen);
					close(sockfd);
					return 0;
				}
			}
			else
			{
				if((size_t)(FUN_RET_BUFF_LEN_MAX)!=fwrite(buff,1,FUN_RET_BUFF_LEN_MAX,fp))
				{
					sprintf(err_affix,"associated with [%s]",tmp_file);
					deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_F_IO,err_affix,err_code);
					fclose(fp);
					close(p_socket_data);
					close(p_socket_listen);
					close(sockfd);
					return 0;
				}
			}
		}
		fclose(fp);
		close(p_socket_data);
		close(p_socket_listen);
	}
	if(!diff_tow_file(tmp_file,p_file_size,err_code))
	{
		close(sockfd);
		return 0;
	}
	if(-1==rename(tmp_file,dest_file))
	{
		sprintf(err_affix,"associated with [%s]",dest_file);
		deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_RENAME,err_affix,err_code);
		close(sockfd);
		return 0;
	}
	return 1;
}

extern int delete_file(int p_socket,char *file_name,char *err_code)
{
	char p_ret_msg[FUN_RET_LINE_LEN_MAX]={
		0	};
	char p_command[FUN_CMD_LINE_LEN_MAX]={
		0	};
	char err_affix[1000]={
		0	};

#ifdef DEBUG_FUN_DELETE_FILE
	fprintf(stderr,"====== delete_file ======\n");
#endif

	memset(p_command,0,FUN_CMD_LINE_LEN_MAX);
	sprintf(p_command,"DELE %s\r\n",file_name);
	if(!send_command(p_socket,p_command,err_code))
	{
		return 0;
	}
	memset(p_ret_msg,0,FUN_RET_LINE_LEN_MAX);
	if(!get_info_msg(p_socket,p_ret_msg,err_code))
	{
		return 0;
	}
	if(0!=strncmp(FUN_FTP_TAG_CMD_OK,p_ret_msg,4))
	{
		sprintf(err_affix,"associated with [%s]",p_ret_msg);
		deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_DELETE_FILE,err_affix,err_code);
		close(p_socket);
		return 0;
	}
	return 1;
}

extern int put_bin_file(int sockfd,struct STRU_SUPPORT_ARGUMENTS *stru_support,char *p_ip_local,char *src_file,char *dst_file,char *dst_tmp_file,unsigned long src_file_size,char *err_code)
{
	char p_ret_msg[FUN_RET_LINE_LEN_MAX]={
		0	};
	char buff[FUN_RET_BUFF_LEN_MAX];
	char p_command[500]={
		0	};
	char p_ip[20]={
		0	};
	char p_port_info[100]={
		0	};
	unsigned int p_port;
	unsigned int p_port_local;
	int p_socket_data;
	int p_socket_listen;
	int p_remote_addr_len;
	long read_len;
	unsigned long p_file_size;
	FILE *fp;
	struct sockaddr_in p_remote_addr;
	char err_affix[1000]={
		0	};

#ifdef DEBUG_FUN_PUT_BIN_FILE
	fprintf(stderr,"====== put_bin_file ======\n");
#endif

	if(!send_command(sockfd,"TYPE I\r\n",err_code))
	{
		return 0;
	}
	if(!get_blk_msg(sockfd,err_code))
	{
		return 0;
	}
	if(stru_support->Is_support_pasv)
	{
		if(!send_command(sockfd,"PASV\r\n",err_code))
		{
			return 0;
		}
		memset(p_ret_msg,0,FUN_RET_LINE_LEN_MAX);
		if(!get_info_msg(sockfd,p_ret_msg,err_code))
		{
			return 0;
		}
		if(!Retr_PASV_Info(p_ret_msg,p_ip,&p_port,err_code))
		{
			close(sockfd);
			return 0;
		}
		sprintf(p_command,"STOR %s\r\n",dst_tmp_file);
		if(!send_command(sockfd,p_command,err_code))
		{
			return 0;
		}
		if(!make_connection(&p_socket_data,p_ip,p_port,err_code))
		{
			close(sockfd);
			return 0;
		}
		if(NULL==(fp=fopen(src_file,"rb")))
		{
			sprintf(err_affix,"associated with [%s]",src_file);
			deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_FOPEN,err_affix,err_code);
			close(p_socket_data);
			close(sockfd);
			return 0;
		}
		p_file_size=0;
		while(!feof(fp))
		{
			if(0>=(read_len=fread(buff,1,FUN_RET_BUFF_LEN_MAX,fp)))
			{
				if(ferror(fp))
				{
					sprintf(err_affix,"associated with [%s]",src_file);
					deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_F_IO,err_affix,err_code);
					close(p_socket_data);
					close(sockfd);
					fclose(fp);
					return 0;
				}
				else
				{
					continue;
				}
			}
			if(put_data_with_proper_size(p_socket_data,read_len,buff,err_code))
			{
				p_file_size += read_len;
			}
			else
			{
				close(sockfd);
				fclose(fp);
				return 0;
			}
		}
		fclose(fp);
		close(p_socket_data);
		if(!get_blk_msg(sockfd,err_code))
		{
			return 0;
		}
		memset(p_ret_msg,0,FUN_RET_LINE_LEN_MAX);
		if(!get_info_msg(sockfd,p_ret_msg,err_code))
		{
			return 0;
		}
		if(0!=strncmp(p_ret_msg,FUN_FTP_TAG_TRANS_OK,4))
		{
			sprintf(err_affix,"associated with [%s]",p_ret_msg);
			deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_MSG_TAG,err_affix,err_code);
			close(sockfd);
			return 0;
		}
	}
	else
	{
		if(!make_socket_server(&p_socket_listen,&p_port_local,err_code))
		{
			close(sockfd);
			return 0;
		}
		if(!Retr_PORT_Info(p_ip_local,p_port_local,p_port_info,err_code))
		{
			close(sockfd);
			close(p_socket_listen);
			return 0;
		}
		sprintf(p_command,"PORT %s\r\n",p_port_info);
		if(!send_command(sockfd,p_command,err_code))
		{
			close(p_socket_listen);
			return 0;
		}
		memset(p_ret_msg,0,FUN_RET_LINE_LEN_MAX);
		if(!get_info_msg(sockfd,p_ret_msg,err_code))
		{
			close(p_socket_listen);
			return 0;
		}
		sprintf(p_command,"STOR %s\r\n",dst_tmp_file);
		if(!send_command(sockfd,p_command,err_code))
		{
			close(p_socket_listen);
			return 0;
		}
		memset(&p_remote_addr,0,sizeof(struct sockaddr_in));
		p_remote_addr_len=sizeof(struct sockaddr_in);
		if(-1==(p_socket_data=accept(p_socket_listen,(struct sockaddr *)(&p_remote_addr),&p_remote_addr_len)))
		{
			close(sockfd);
			close(p_socket_listen);
			return 0;
		}

		if(NULL==(fp=fopen(src_file,"rb")))
		{
			sprintf(err_affix,"associated with [%s]",src_file);
			deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_FOPEN,err_affix,err_code);
			close(p_socket_listen);
			close(p_socket_data);
			close(sockfd);
			return 0;
		}
		p_file_size=0;
		while(!feof(fp))
		{
			if(0>=(read_len=fread(buff,1,FUN_RET_BUFF_LEN_MAX,fp)))
			{
				if(ferror(fp))
				{
					sprintf(err_affix,"associated with [%s]",src_file);
					deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_F_IO,err_affix,err_code);
					close(p_socket_data);
					close(p_socket_listen);
					close(sockfd);
					fclose(fp);
					return 0;
				}
				else
				{
					continue;
				}
			}
			if(put_data_with_proper_size(p_socket_data,read_len,buff,err_code))
			{
				p_file_size += read_len;
			}
			else
			{
				close(sockfd);
				close(p_socket_listen);
				fclose(fp);
				return 0;
			}
		}
		fclose(fp);
		close(p_socket_data);
		close(p_socket_listen);
		if(!get_blk_msg(sockfd,err_code))
		{
			return 0;
		}
		memset(p_ret_msg,0,FUN_RET_LINE_LEN_MAX);
		if(!get_info_msg(sockfd,p_ret_msg,err_code))
		{
			return 0;
		}
		if(0!=strncmp(p_ret_msg,FUN_FTP_TAG_TRANS_OK,4))
		{
			sprintf(err_affix,"associated with [%s]",p_ret_msg);
			deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_MSG_TAG,err_affix,err_code);
			close(sockfd);
			return 0;
		}
	}
	if(p_file_size!=src_file_size)
	{
		sprintf(err_affix,"associated with [%s]",src_file);
		deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_VERIFY_FILE,err_affix,err_code);
		close(sockfd);
		return 0;
	}
	/*******************************************
	*	移动上传到服务器上的文件
	*/
	if(!move_remote_file(sockfd,dst_tmp_file,dst_file,err_code))
	{
		return 0;
	}
	return 1;
}

extern int move_remote_file(int sockfd,char *src_file,char *dst_file,char *err_code)
{
	char p_ret_msg[FUN_RET_LINE_LEN_MAX]={
		0	};
	char p_command[500]={
		0	};
	char err_affix[1000]={
		0	};

	sprintf(p_command,"RNFR %s\r\n",src_file);
	if(!send_command(sockfd,p_command,err_code))
	{
		return 0;
	}
	memset(p_ret_msg,0,FUN_RET_LINE_LEN_MAX);
	if(!get_info_msg(sockfd,p_ret_msg,err_code))
	{
		return 0;
	}
	if(FUN_FTP_TAG_HEAD_RESUME!=p_ret_msg[0])
	{
		sprintf(err_affix,"associated with [%s]",p_ret_msg);
		deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_MSG_TAG,err_affix,err_code);
		close(sockfd);
		return 0;
	}
	sprintf(p_command,"RNTO %s\r\n",dst_file);
	if(!send_command(sockfd,p_command,err_code))
	{
		return 0;
	}
	memset(p_ret_msg,0,FUN_RET_LINE_LEN_MAX);
	if(!get_info_msg(sockfd,p_ret_msg,err_code))
	{
		return 0;
	}
	if(FUN_FTP_TAG_HEAD_OK!=p_ret_msg[0])
	{
		sprintf(err_affix,"associated with [%s]",p_ret_msg);
		deal_run_err_msg("warn",__FILE__,__LINE__,FUN_ERR_MSG_TAG,err_affix,err_code);
		close(sockfd);
		return 0;
	}
	return 1;
}

extern int get_running_flag(char *file_name,char *err_code)
{
	FILE *fp;
	char parm_val[80]={
		0	};
	if(NULL==(fp=fopen(file_name,"r")))
	{
		fprintf(stderr,"Error read config file\n");
		exit(1);
	}
	if(0==get_config_val(fp,PUB_SEG_TERMINAL,PUB_SEG_TERMINAL_PARAM_FLAG,parm_val,err_code))
	{
		fclose(fp);
		return 0;
	}
	if(0==strcmp(parm_val,FUN_PUB_CONF_STOP))
	{
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return 1;
}
