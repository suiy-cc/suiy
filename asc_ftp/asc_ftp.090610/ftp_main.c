/*********************************************************************
*
*	Copyright (C), 1995-2003, Si-Tech Information Technology Ltd.
*
**********************************************************************
*
*	File_name:	ftp_main.c
*	Author:		����ΰ
*	Version:	�汾��V1.00�� 
*	Date:		2003/11/15
*	Description:
*		���������FTP(File Trasfer Protocol)Э��(RFC 959)
*	ʵ����̨�������ļ��Ŀɿ����䡣����ͨ�������ļ����ʵ����
*	�����ϴ������ء�
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
*	�´�ָ��Ŀ¼�µ����з��ϱ�׼���ļ�
*/
static int down_load_directory
(
int ,				/*socket���*/
FILE * ,			/*������־���*/
struct STRU_FILE_SYSTEM_INFO ,	/*�ļ�ϵͳ��Ϣ*/
struct STRU_SUPPORT_ARGUMENTS ,	/*ϵͳ֧����Ϣ*/
char *,				/*��������IP��ַ*/
char *,				/*Ŀ¼�ļ���*/
char *				/*�������*/
);
/******************************************************
*	�ϴ�ָ��Ŀ¼�µ����з��ϱ�׼���ļ�
*/
static int up_load_directory
(
int ,				/*socket���*/
FILE *,				/*������־���*/
struct STRU_FILE_SYSTEM_INFO ,	/*�ļ�ϵͳ��Ϣ*/
struct STRU_SUPPORT_ARGUMENTS ,	/*ϵͳ֧����Ϣ*/
char *,				/*��������IP��ַ*/
char *				/*�������*/
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
			fprintf(stderr,"\t\t\tFTP�������\n\n");
			fprintf(stderr,"\t����˵����\n");
			fprintf(stderr,"\t-h:	��ӡ������Ϣ\n");
			fprintf(stderr,"\t-v:	��ʾ�汾��Ϣ\n");
			fprintf(stderr,"\t-g:	���ε��ý�������ɿ����ļ���Ĭ��Ϊ����ģʽ\n");
			fprintf(stderr,"\t-u:	�û�����Ϣ,�ڲ��������ļ�ʱ��Ч\n");
			fprintf(stderr,"\t-p:	�û�������Ϣ,�ڲ��������ļ�ʱ��Ч\n");
			fprintf(stderr,"\t-f:	�����ļ���,�ڲ��������ļ�ʱ��Ч\n");
			fprintf(stderr,"\t����ģʽ:	���÷�ʽΪ��������+�����ļ�+�����ļ�+��ʱ�ļ�+������־�ļ�\n\n");
			fprintf(stderr,"\t----------------------------------------\n");
			exit(0);
		case 'v':
			has_flags=1;
			fprintf(stderr,"\t****************************************\n");
			fprintf(stderr,"\t\t\tFTP�������\n\n");
			fprintf(stderr,"\t�汾��:	ver1.0\n");
			fprintf(stderr,"\t����ʱ��:	%s %s\n",__DATE__,__TIME__);
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
			fprintf(stderr,"�밴�� %s -h ��ʽ�쿴������Ϣ��\n",argv[0]);
			exit(1);
		}
	}
	if(pwd_flag)
	{
		if(0==p_password_file[0])
		{
			fprintf(stderr,"���ɿ����ļ��������-f������\n");
			exit(1);
		}
		if(0==p_uname[0])
		{
			fprintf(stderr,"���ɿ����ļ��������-u������\n");
			exit(1);
		}
		if(0==p_pass[0])
		{
			fprintf(stderr,"���ɿ����ļ��������-p������\n");
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
		fprintf(stderr,"�밴�� %s -h ��ʽ�쿴������Ϣ��\n",argv[0]);
		exit(1);
	}
	/**************************************************
	*	��������־
	*/
	if(NULL==(running_file_fp=fopen(argv[4],"a")))
	{
		fprintf(stderr,"������־�ļ��򿪴���\n");
		exit(1);
	}
	setbuf(running_file_fp,NULL);/*�رջ������*/
	tmp_time=time(NULL);
	fprintf(running_file_fp,"FTP����������ģʽ��ʼ�����ڣ�%s",ctime(&tmp_time));

	strcpy(p_config_file,argv[2]);
	strcpy(p_list_file,argv[3]);

	if(get_running_flag(p_config_file,err_code))
	{
		exit_flag=0;
		while(!exit_flag)
		{
			/**************************************************
			*	��ʼ�����л���
			*/
#ifdef DEBUG_MAIN_RUNNING
			fprintf(stderr,"Running:	Begin initializing configuartion file!\n");
#endif
			memset(p_uname,0,MAIN_USER_NAME_LEN_MAX);
			memset(p_pass,0,MAIN_PASSWORD_LEN_MAX);
			/*�������ӵ�½Զ���������ж��ϴ������أ��ж��Ƿ�ϵ�������*/
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
						*	�ϴ��ļ�����
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
						*	�´��ļ�����
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
						fprintf(running_file_fp,"FTP�����쳣�����ڣ�%s",ctime(&tmp_time));
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
	fprintf(running_file_fp,"FTP�����������������ڣ�%s",ctime(&tmp_time));
	fclose(running_file_fp);

	return 0;/*����������0��ʾ�����˳����˴��뺯������ֵ�Ķ����෴*/
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
		fprintf(running_file_fp,"FTP�����쳣�����ڣ�%s",ctime(&tmp_time));
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
		fprintf(running_file_fp,"FTP�����쳣�����ڣ�%s",ctime(&tmp_time));
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
		fprintf(running_file_fp,"FTP�����쳣�����ڣ�%s",ctime(&tmp_time));
		fclose(running_file_fp);
		exit(1);
	}
	if(NULL==(dirp=opendir(p_stru_file_info.src_directory)))
	{
		fprintf(running_file_fp,"Open source directory error!\n");
		tmp_time=time(NULL);
		fprintf(running_file_fp,"FTP�����쳣�����ڣ�%s",ctime(&tmp_time));
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
				fprintf(running_file_fp,"FTP�����쳣�����ڣ�%s",ctime(&tmp_time));
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
