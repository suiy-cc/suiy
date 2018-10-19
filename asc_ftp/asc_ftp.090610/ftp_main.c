/*********************************************************************
*
*	Copyright (C), 1995-2003, Si-Tech Information Technology Ltd.
*
**********************************************************************
*
*	File_name:	ftp_main.c
*	Author:		张奇伟
*	Version:	版本（V1.00） 
*	Date:		2003/11/15
*	Description:
*		本程序基于FTP(File Trasfer Protocol)协议(RFC 959)
*	实现两台主机间文件的可靠传输。程序通过配置文件灵活实现文
*	件的上传和下载。
*
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ftp_main.h"
/******************************************************
*	下传指定目录下的所有符合标准的文件
*/
static int down_load_directory
(
int ,				/*socket句柄*/
FILE * ,			/*运行日志句柄*/
struct STRU_FILE_SYSTEM_INFO ,	/*文件系统信息*/
struct STRU_SUPPORT_ARGUMENTS ,	/*系统支持信息*/
char *,				/*本地主机IP地址*/
char *,				/*目录文件名*/
char *				/*错误代码*/
);
/******************************************************
*	上传指定目录下的所有符合标准的文件
*/
static int up_load_directory
(
int ,				/*socket句柄*/
FILE *,				/*运行日志句柄*/
struct STRU_FILE_SYSTEM_INFO ,	/*文件系统信息*/
struct STRU_SUPPORT_ARGUMENTS ,	/*系统支持信息*/
char *,				/*本地主机IP地址*/
char *				/*错误代码*/
);

static void exit_moni(void)
{
	fprintf(stderr, "error = [%d], strerror = [%s]\n", errno, strerror(errno));
}

static void sig_moni(int signo)
{
	fprintf(stderr, "signo = [%d]\n", signo);
}

int main(int argc,char **argv)
{
	atexit(exit_moni);

	struct sigaction act, oact;

	act.sa_handler = sig_moni;
	act.sa_flags = SIG_BLOCK;
	sigemptyset(&act.sa_mask);
        sigaction(SIGABRT  , &act, &oact);
        sigaction(SIGALRM  , &act, &oact);
        sigaction(SIGBUS   , &act, &oact);
        sigaction(SIGCHLD  , &act, &oact);
        sigaction(SIGCONT  , &act, &oact);
        sigaction(SIGFPE   , &act, &oact);
        sigaction(SIGHUP   , &act, &oact);
        sigaction(SIGILL   , &act, &oact);
        sigaction(SIGINT   , &act, &oact);
        sigaction(SIGKILL  , &act, &oact);
        sigaction(SIGPIPE  , &act, &oact);
        sigaction(SIGQUIT  , &act, &oact);
        sigaction(SIGSEGV  , &act, &oact);
        sigaction(SIGSTOP  , &act, &oact);
        sigaction(SIGTERM  , &act, &oact);
        sigaction(SIGTSTP  , &act, &oact);
        sigaction(SIGTTIN  , &act, &oact);
        sigaction(SIGTTOU  , &act, &oact);
        sigaction(SIGUSR1  , &act, &oact);
        sigaction(SIGUSR2  , &act, &oact);
        sigaction(SIGPOLL  , &act, &oact);
        sigaction(SIGPROF  , &act, &oact);
        sigaction(SIGSYS   , &act, &oact);
        sigaction(SIGTRAP  , &act, &oact);
        sigaction(SIGURG   , &act, &oact);
        sigaction(SIGVTALRM, &act, &oact);
        sigaction(SIGXCPU  , &act, &oact);
        sigaction(SIGXFSZ  , &act, &oact);



	int p_socket;
	int i;
	int exit_flag;
	int reconn_flag;
	int pwd_flag;
	int has_flags;
	int iret;

	struct STRU_FILE_SYSTEM_INFO stru_file_info[99]={
		0	};
	struct STRU_SERVER_INFO stru_server_info={
		0	};
	struct STRU_SUPPORT_ARGUMENTS stru_support_info={
		0	};

	char err_code[MAIN_ERRCODES_LEN_MAX]={
		0	};
	char p_list_file[MAIN_FILE_LEN_MAX]={
		0	};
	char p_password_file[MAIN_FILE_LEN_MAX]={
		0	};
	char p_config_file[MAIN_FILE_LEN_MAX]={
		0	};
	char p_command[FUN_CMD_LINE_LEN_MAX]={
		0	};
	char p_uname[MAIN_USER_NAME_LEN_MAX]={
		0	};
	char p_pass[MAIN_PASSWORD_LEN_MAX]={
		0	};
	char p_log_file_name[FUN_PUB_DIR_LEN_MAX]={
		0	};
	char p_tmp_log_file_name[FUN_PUB_DIR_LEN_MAX]={
		0	};
	char p_pre_file_name[FUN_PUB_DIR_LEN_MAX]={
		0	};

	char *p_tmp;

	time_t tmp_time;
	time_t time_curr;
	time_t time_next;
	struct tm *tmp_tm1;
	struct tm tmp_tm;

	FILE *running_file_fp;

	pwd_flag=0;
	has_flags=0;
	while(-1!=(iret=getopt(argc,argv,"hvgu:p:f:")))
	{
		switch(iret)
		{
		case 'h':
			has_flags=1;
			fprintf(stderr,"\t****************************************\n");
			fprintf(stderr,"\t\t\tFTP传输程序\n\n");
			fprintf(stderr,"\t参数说明：\n");
			fprintf(stderr,"\t-h:	打印帮助信息\n");
			fprintf(stderr,"\t-v:	显示版本信息\n");
			fprintf(stderr,"\t-g:	本次调用结果是生成口令文件。默认为运行模式\n");
			fprintf(stderr,"\t-u:	用户名信息,在产生口令文件时有效\n");
			fprintf(stderr,"\t-p:	用户口令信息,在产生口令文件时有效\n");
			fprintf(stderr,"\t-f:	口令文件名,在产生口令文件时有效\n");
			fprintf(stderr,"\t运行模式:	调用方式为：进程名+口令文件+配置文件+临时文件+运行日志文件\n\n");
			fprintf(stderr,"\t----------------------------------------\n");
			exit(0);
		case 'v':
			has_flags=1;
			fprintf(stderr,"\t****************************************\n");
			fprintf(stderr,"\t\t\tFTP传输程序\n\n");
			fprintf(stderr,"\t版本号:	ver1.0\n");
			fprintf(stderr,"\t编译时间:	%s %s\n",__DATE__,__TIME__);
			fprintf(stderr,"\t----------------------------------------\n\n");
			exit(0);
		case 'u':
			has_flags=1;
			sprintf(p_uname,"%s",optarg);
			break;
		case 'p':
			has_flags=1;
			sprintf(p_pass,"%s",optarg);
			break;
		case 'f':
			has_flags=1;
			sprintf(p_password_file,"%s",optarg);
			break;
		case 'g':
			has_flags=1;
			pwd_flag=1;
			break;
		default:
			has_flags=1;
			fprintf(stderr,"请按照 %s -h 方式察看帮助信息！\n",argv[0]);
			exit(1);
		}
	}
	if(pwd_flag)
	{
		if(0==p_password_file[0])
		{
			fprintf(stderr,"生成口令文件必须存在-f参数！\n");
			exit(1);
		}
		if(0==p_uname[0])
		{
			fprintf(stderr,"生成口令文件必须存在-u参数！\n");
			exit(1);
		}
		if(0==p_pass[0])
		{
			fprintf(stderr,"生成口令文件必须存在-p参数！\n");
			exit(1);
		}
		if(!gen_pwd_file(p_password_file,MAIN_FUN_KEY_VALUE,p_uname,p_pass,err_code))
		{
			fprintf(stderr,"%s\n",err_code);
			exit(1);
		}
		exit(0);
	}
	if(5!=argc || has_flags)
	{
		fprintf(stderr,"请按照 %s -h 方式察看帮助信息！\n",argv[0]);
		exit(1);
	}
	/**************************************************
	*	打开运行日志
	*/
	if(NULL==(running_file_fp=fopen(argv[4],"a")))
	{
		fprintf(stderr,"运行日志文件打开错误！\n");
		exit(1);
	}
	setbuf(running_file_fp,NULL);/*关闭缓冲机制*/
	tmp_time=time(NULL);
	fprintf(running_file_fp,"FTP程序按照运行模式开始运行于：%s",ctime(&tmp_time));

	strcpy(p_config_file,argv[2]);
	strcpy(p_list_file,argv[3]);

	if(get_running_flag(p_config_file,err_code))
	{
		exit_flag=0;
		while(!exit_flag)
		{
			/**************************************************
			*	初始化运行环境
			*/
#ifdef DEBUG_MAIN_RUNNING
			fprintf(stderr,"Running:	Begin initializing configuartion file!\n");
#endif
			memset(p_uname,0,MAIN_USER_NAME_LEN_MAX);
			memset(p_pass,0,MAIN_PASSWORD_LEN_MAX);
			/*建立连接登陆远程主机，判断上传、下载，判断是否断点续传等*/
			if(!init_environment(argv[1],MAIN_USER_NAME_LEN_MAX,MAIN_PASSWORD_LEN_MAX,p_config_file,p_uname,p_pass,&p_socket,stru_file_info,&stru_server_info,&stru_support_info,err_code))
			{
				fprintf(running_file_fp,"%s",err_code);
				sleep(MAIN_RECONN_INTERVAL);
				continue;
			}
			time_next=(time(NULL)+MAIN_TIME_INTERVAL-1)/MAIN_TIME_INTERVAL*MAIN_TIME_INTERVAL;
			tmp_tm1=localtime(&time_next);
			tmp_tm=*tmp_tm1;
			reconn_flag=0;
			exit_flag=0;
			while(!(reconn_flag||exit_flag))
			{
				for(i=0;i<stru_server_info.file_sys_group_count;i++)
				{
					if(0==strcmp(FUN_PUB_CONF_UP,stru_file_info[i].trans_file_direction))
					{
						/***************************************************************
						*	上传文件部分
						*/
						if(!up_load_directory(p_socket,running_file_fp,stru_file_info[i],stru_support_info,stru_server_info.local_host_ip,err_code))
						{
							fprintf(running_file_fp,"%s",err_code);
							reconn_flag=1;
							break;
						}
					}
					else if(0==strcmp(FUN_PUB_CONF_DOWN,stru_file_info[i].trans_file_direction))
					{
						/***************************************************************
						*	下传文件部分
						*/
						if(!down_load_directory(p_socket,running_file_fp,stru_file_info[i],stru_support_info,stru_server_info.local_host_ip,p_list_file,err_code))
						{
							fprintf(running_file_fp,"%s",err_code);
							reconn_flag=1;
							break;
						}
					}
					else
					{
						fprintf(running_file_fp,"Transfer file direction error!\n");
						tmp_time=time(NULL);
						fprintf(running_file_fp,"FTP程序异常结束于：%s",ctime(&tmp_time));
						fclose(running_file_fp);
						exit(1);
					}
				}
				time_curr=time(NULL);
				if(time_next<time_curr)
				{
					for(i=0;i<stru_server_info.file_sys_group_count;i++)
					{
						memset(p_log_file_name,0,FUN_PUB_DIR_LEN_MAX);
						memset(p_tmp_log_file_name,0,FUN_PUB_DIR_LEN_MAX);
						if(0==strcmp(FUN_PUB_CONF_UP,stru_file_info[i].trans_file_direction))
						{
							p_tmp=strrchr(stru_file_info[i].dst_directory,'/');
						}
						else
						{
							p_tmp=strrchr(stru_file_info[i].src_directory,'/');
						}
						p_tmp++;
						sprintf(p_pre_file_name,"%s%s%02d%02d%02d%02d%02d.log",MAIN_FUN_LOG_FILE_HEAD,p_tmp,tmp_tm.tm_year-100,tmp_tm.tm_mon+1,tmp_tm.tm_mday,tmp_tm.tm_hour,tmp_tm.tm_min);
						sprintf(p_log_file_name,"%s/log%02d/%s",stru_file_info[i].log_directory,tmp_tm.tm_mday,p_pre_file_name);
						sprintf(p_tmp_log_file_name,"%s/tmp_file.%s.log",stru_file_info[i].log_tmp_directory,p_tmp);
						if(-1==rename(p_tmp_log_file_name,p_log_file_name))
						{
							fprintf(running_file_fp,"Rename [%s] to [%s] error!!\n",p_tmp_log_file_name,p_log_file_name);
							fclose(running_file_fp);
							exit(1);
						}
					}
					time_next=(time(NULL)+MAIN_TIME_INTERVAL-1)/MAIN_TIME_INTERVAL*MAIN_TIME_INTERVAL;
					tmp_tm1=localtime(&time_next);
					tmp_tm=*tmp_tm1;
				}
				if(!get_running_flag(p_config_file,err_code))
				{
					exit_flag=1;
				}
				sleep(MAIN_GRP_PROC_INTERVAL);
			}
			if(reconn_flag)
			{
				sleep(MAIN_RECONN_INTERVAL);
				continue;
			}
			if(exit_flag)
			{
				continue;
			}
			memset(p_command,0,FUN_CMD_LINE_LEN_MAX);
			sprintf(p_command,"QUIT\r\n");
			if(!send_command(p_socket,p_command,err_code))
			{
				fprintf(running_file_fp,"%s",err_code);
				sleep(MAIN_RECONN_INTERVAL);
				continue;
			}
			if(!get_blk_msg(p_socket,err_code))
			{
				fprintf(running_file_fp,"%s",err_code);
				sleep(MAIN_RECONN_INTERVAL);
				continue;
			}
			close(p_socket);
			sleep(MAIN_RECONN_INTERVAL);
		}
	}
	tmp_time=time(NULL);
	fprintf(running_file_fp,"FTP程序正常结束运行于：%s",ctime(&tmp_time));
	fclose(running_file_fp);

	return 0;/*主函数返回0表示正常退出。此处与函数返回值的定义相反*/
}

static int down_load_directory(int p_socket,FILE *running_file_fp,struct STRU_FILE_SYSTEM_INFO p_stru_file_info,struct STRU_SUPPORT_ARGUMENTS p_stru_support_info,char *p_local_ip,char *p_list_file,char *err_code)
{
	char p_tmp_log_file_name[FUN_PUB_DIR_LEN_MAX]={
		0	};
	char p_src_file_name[FUN_PUB_DIR_LEN_MAX]={
		0	};
	char p_src_file_name_ora[FUN_PUB_DIR_LEN_MAX]={
		0	};
	char p_dst_file_name[FUN_PUB_DIR_LEN_MAX]={
		0	};
	char p_bak_file_name[FUN_PUB_DIR_LEN_MAX]={
		0	};
	char p_tmp_file_name[FUN_PUB_DIR_LEN_MAX]={
		0	};
	char p_file_line_val[FUN_PUB_LENGTH_OF_LINE_MAX]={
		0	};
	char *p_tmp;
	char p_start_time[15]={
		0	};
	char p_end_time[15]={
		0	};

	unsigned long p_file_size;

	time_t tmp_time;
	struct tm *tmp_tm1;
	struct tm tmp_tm;
	FILE *list_file_fp;
	FILE *tmp_log_fp;

	memset(p_tmp_log_file_name,0,FUN_PUB_DIR_LEN_MAX);
	p_tmp=strrchr(p_stru_file_info.src_directory,'/');
	p_tmp++;
	sprintf(p_tmp_log_file_name,"%s/tmp_file.%s.log",p_stru_file_info.log_tmp_directory,p_tmp);
	if(NULL == (tmp_log_fp=fopen(p_tmp_log_file_name,"a")))
	{
		fprintf(running_file_fp,"Open tmp_log file error!\n");
		tmp_time=time(NULL);
		fprintf(running_file_fp,"FTP程序异常结束于：%s",ctime(&tmp_time));
		fclose(running_file_fp);
		exit(1);
	}
	if(!get_file_list(p_socket,&p_stru_support_info,p_local_ip,p_stru_file_info.src_directory,p_list_file,err_code))
	{
		fclose(tmp_log_fp);
		return 0;
	}
	if(NULL == (list_file_fp=fopen(p_list_file,"r")))
	{
		fprintf(running_file_fp,"Open list file error!\n");
		tmp_time=time(NULL);
		fprintf(running_file_fp,"FTP程序异常结束于：%s",ctime(&tmp_time));
		fclose(running_file_fp);
		exit(1);
	}
	while(!feof(list_file_fp))
	{
		memset(p_file_line_val,0,FUN_PUB_LENGTH_OF_LINE_MAX);
		fgets(p_file_line_val,FUN_PUB_LENGTH_OF_LINE_MAX,list_file_fp);
		if('\n'==p_file_line_val[strlen(p_file_line_val)-1])
		{
			p_file_line_val[strlen(p_file_line_val)-1]=0;
		}
		if('\r'==p_file_line_val[strlen(p_file_line_val)-1])
		{
			p_file_line_val[strlen(p_file_line_val)-1]=0;
		}
		if(0!=p_file_line_val[0])
		{
			if(!get_retr_file_info(p_file_line_val,p_stru_file_info,p_src_file_name_ora,p_tmp_file_name,p_dst_file_name,err_code))
			{
				continue;
			}
			sprintf(p_src_file_name,"%s/%s",p_stru_file_info.src_directory,p_src_file_name_ora);
			if(Identify_file_name(p_src_file_name_ora,p_stru_file_info.name_include,p_stru_file_info.name_exclude))
			{
				tmp_time=time(NULL);
				tmp_tm1=localtime(&tmp_time);
				tmp_tm=*tmp_tm1;
				sprintf(p_start_time,"20%02d%02d%02d%02d%02d%02d",tmp_tm.tm_year-100,tmp_tm.tm_mon+1,tmp_tm.tm_mday,tmp_tm.tm_hour,tmp_tm.tm_min,tmp_tm.tm_sec);
				if(!get_bin_file(p_socket,&p_stru_support_info,p_local_ip,p_file_line_val,p_src_file_name,p_dst_file_name,p_tmp_file_name,&p_file_size,err_code))
				{
					fclose(list_file_fp);
					fclose(tmp_log_fp);
					return 0;
				}
				if(0==strcmp(p_stru_file_info.remote_backup_flag,PUB_CONF_YES_STR))
				{
					sprintf(p_bak_file_name,"%s/%s",p_stru_file_info.bak_directory,p_src_file_name_ora);
					if(!move_remote_file(p_socket,p_src_file_name,p_bak_file_name,err_code))
					{
						fclose(list_file_fp);
						fclose(tmp_log_fp);
						return 0;
					}
				}
				else
				{
					if(!delete_file(p_socket,p_src_file_name,err_code))
					{
						fclose(list_file_fp);
						fclose(tmp_log_fp);
						return 0;
					}
				}
				tmp_time=time(NULL);
				tmp_tm1=localtime(&tmp_time);
				tmp_tm=*tmp_tm1;
				sprintf(p_end_time,"20%02d%02d%02d%02d%02d%02d",tmp_tm.tm_year-100,tmp_tm.tm_mon+1,tmp_tm.tm_mday,tmp_tm.tm_hour,tmp_tm.tm_min,tmp_tm.tm_sec);
				fprintf(tmp_log_fp,"%s:%s:%s:T %ld:D DOWNLOAD\n",p_src_file_name,p_start_time,p_end_time,p_file_size);
			}
		}
	}
	fclose(list_file_fp);
	fclose(tmp_log_fp);
	return 1;
}

static int up_load_directory(int p_socket,FILE *running_file_fp,struct STRU_FILE_SYSTEM_INFO p_stru_file_info,struct STRU_SUPPORT_ARGUMENTS p_stru_support_info,char *p_local_ip,char *err_code)
{
	char p_tmp_log_file_name[FUN_PUB_DIR_LEN_MAX]={
		0	};
	char p_src_file_name[FUN_PUB_DIR_LEN_MAX]={
		0	};
	char p_src_file_name_ora[FUN_PUB_DIR_LEN_MAX]={
		0	};
	char p_dst_file_name[FUN_PUB_DIR_LEN_MAX]={
		0	};
	char p_dst_tmp_file_name[FUN_PUB_DIR_LEN_MAX]={
		0	};
	char p_bak_file_name[FUN_PUB_DIR_LEN_MAX]={
		0	};
	char *p_tmp;
	char p_start_time[15]={
		0	};
	char p_end_time[15]={
		0	};

	time_t tmp_time;
	struct tm *tmp_tm1;
	struct tm tmp_tm;
	FILE *tmp_log_fp;

	DIR *dirp;
	struct dirent *dp;
	struct stat st_file_type;
	unsigned long p_file_size;

	memset(p_tmp_log_file_name,0,FUN_PUB_DIR_LEN_MAX);
	p_tmp=strrchr(p_stru_file_info.dst_directory,'/');
	p_tmp++;
	sprintf(p_tmp_log_file_name,"%s/tmp_file.%s.log",p_stru_file_info.log_tmp_directory,p_tmp);
	if(NULL == (tmp_log_fp=fopen(p_tmp_log_file_name,"a")))
	{
		fprintf(running_file_fp,"Open tmp_log file error!\n");
		tmp_time=time(NULL);
		fprintf(running_file_fp,"FTP程序异常结束于：%s",ctime(&tmp_time));
		fclose(running_file_fp);
		exit(1);
	}
	if(NULL==(dirp=opendir(p_stru_file_info.src_directory)))
	{
		fprintf(running_file_fp,"Open source directory error!\n");
		tmp_time=time(NULL);
		fprintf(running_file_fp,"FTP程序异常结束于：%s",ctime(&tmp_time));
		fclose(running_file_fp);
		exit(1);
	}
	while(NULL!=(dp=readdir(dirp)))
	{
		sprintf(p_src_file_name_ora,"%s",dp->d_name);
		sprintf(p_src_file_name,"%s/%s",p_stru_file_info.src_directory,dp->d_name);
		stat(p_src_file_name,&st_file_type);
		if(S_ISREG(st_file_type.st_mode) && Identify_file_name(p_src_file_name_ora,p_stru_file_info.name_include,p_stru_file_info.name_exclude))
		{
			tmp_time=time(NULL);
			tmp_tm1=localtime(&tmp_time);
			tmp_tm=*tmp_tm1;
			sprintf(p_start_time,"20%02d%02d%02d%02d%02d%02d",tmp_tm.tm_year-100,tmp_tm.tm_mon+1,tmp_tm.tm_mday,tmp_tm.tm_hour,tmp_tm.tm_min,tmp_tm.tm_sec);
			sprintf(p_dst_file_name,"%s/%s%s%s",p_stru_file_info.dst_directory,p_stru_file_info.file_prefix,dp->d_name,p_stru_file_info.file_postfix);
			sprintf(p_dst_tmp_file_name,"%s/uploading_file.tmp",p_stru_file_info.tmp_directory);
			sprintf(p_bak_file_name,"%s/%s",p_stru_file_info.bak_directory,dp->d_name);
			p_file_size=st_file_type.st_size;
			if(!put_bin_file(p_socket,&p_stru_support_info,p_local_ip,p_src_file_name,p_dst_file_name,p_dst_tmp_file_name,st_file_type.st_size,err_code))
			{
				fclose(tmp_log_fp);
				closedir(dirp);
				return 0;
			}
			if(-1==rename(p_src_file_name,p_bak_file_name))
			{
				fprintf(running_file_fp,"Rename [%s] to [%s] error!\n",p_src_file_name,p_bak_file_name);
				tmp_time=time(NULL);
				fprintf(running_file_fp,"FTP程序异常结束于：%s",ctime(&tmp_time));
				fclose(running_file_fp);
				closedir(dirp);
				exit(1);
			}
			tmp_time=time(NULL);
			tmp_tm1=localtime(&tmp_time);
			tmp_tm=*tmp_tm1;
			sprintf(p_end_time,"20%02d%02d%02d%02d%02d%02d",tmp_tm.tm_year-100,tmp_tm.tm_mon+1,tmp_tm.tm_mday,tmp_tm.tm_hour,tmp_tm.tm_min,tmp_tm.tm_sec);
			fprintf(tmp_log_fp,"%s:%s:%s:T %ld:D UPLOAD\n",p_src_file_name,p_start_time,p_end_time,p_file_size);
		}
		else
		{
			continue;
		}
	}
	fclose(tmp_log_fp);
	closedir(dirp);
	return 1;
}
