/*********************************************************************
*
*	Copyright (C), 1995-2003, Si-Tech Information Technology Ltd.
*
**********************************************************************
*
*	File_name:	ftp_public.c
*	Author:		张奇伟
*	Version:	版本（V1.00） 
*	Date:		2003/11/15
*	Description:
*		本文件包含可公用的函数。
*
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "ftp_public.h"

extern void deal_run_err_msg(char *err_level,char *err_source,int err_line,char *err_no,char *err_affix,char *err_code)
{
	char p_log_time[20]={
		0	};
	time_t tmp_time;
	struct tm *tmp_tm1;
	struct tm tmp_tm;

	tmp_time=time(NULL);
	tmp_tm1=localtime(&tmp_time);
	tmp_tm=*tmp_tm1;
	sprintf(p_log_time,"20%02d/%02d/%02d %02d:%02d:%02d",tmp_tm.tm_year-100,tmp_tm.tm_mon+1,tmp_tm.tm_mday,tmp_tm.tm_hour,tmp_tm.tm_min,tmp_tm.tm_sec);
	sprintf(err_code,"%s  app  %s  错误来源:filename[%s],line[%d]  错误原因:[%s]  附加信息:[%s]\n",p_log_time,err_level,err_source,err_line,err_no,err_affix);
}

extern int init_config_info(char *config_file,struct STRU_FILE_SYSTEM_INFO *struct_file_sys,struct STRU_SERVER_INFO *struct_server_info,char *err_code)
{
	FILE *config_fp;
	char tmp_port[7]={
		0	};
	char tmp_count[3]={
		0	};
	char tmp_param[80]={
		0	};
	char tmp_parts_count[3]={
		0	};
	char tmp_resume_support[5]={
		0	};
	char tmp_size_support[5]={
		0	};
	char tmp_pasv_support[5]={
		0	};
	int i;
	char err_affix[1000]={
		0	};

	memset(struct_server_info,0,sizeof(struct STRU_SERVER_INFO));
	if(NULL==(config_fp=fopen(config_file,"r")))
	{
		sprintf(err_affix,"associated with [%s]",config_file);
		deal_run_err_msg("warn",__FILE__,__LINE__,PUB_ERR_FOPEN,err_affix,err_code);
		return 0;
	}
	if(0==get_config_val(config_fp,PUB_SEG_SYS,PUB_SEG_SYS_PARAM_IP,struct_server_info->host_ip,err_code))
	{
		fclose(config_fp);
		return 0;
	}
	if(0==get_config_val(config_fp,PUB_SEG_SYS,PUB_SEG_SYS_PARAM_LOCAL_IP,struct_server_info->local_host_ip,err_code))
	{
		fclose(config_fp);
		return 0;
	}
	if(0==get_config_val(config_fp,PUB_SEG_SYS,PUB_SEG_SYS_PARAM_PORT,tmp_port,err_code))
	{
		fclose(config_fp);
		return 0;
	}
	if(0==get_config_val(config_fp,PUB_SEG_SYS,PUB_SEG_SYS_PARAM_COUNT,tmp_count,err_code))
	{
		fclose(config_fp);
		return 0;
	}
	if(0==get_config_val(config_fp,PUB_SEG_SYS,PUB_SEG_SYS_PARAM_RESUME,tmp_resume_support,err_code))
	{
		fclose(config_fp);
		return 0;
	}
	if(0==get_config_val(config_fp,PUB_SEG_SYS,PUB_SEG_SYS_PARAM_SIZE,tmp_size_support,err_code))
	{
		fclose(config_fp);
		return 0;
	}
	if(0==get_config_val(config_fp,PUB_SEG_SYS,PUB_SEG_SYS_PARAM_PASV,tmp_pasv_support,err_code))
	{
		fclose(config_fp);
		return 0;
	}
	if(0==get_config_val(config_fp,PUB_SEG_SYS,PUB_SEG_SYS_PARAM_PARTS,tmp_parts_count,err_code))
	{
		fclose(config_fp);
		return 0;
	}
	struct_server_info->host_port=(unsigned long)atol(tmp_port);
	struct_server_info->file_sys_group_count=(unsigned int)atoi(tmp_count);
	struct_server_info->split_parts_count=(unsigned int)atoi(tmp_parts_count);
	if(0==strcmp(tmp_resume_support,PUB_CONF_YES_STR))
	{
		struct_server_info->resume_support=PUB_CONF_YES;
	}
	else if(0==strcmp(tmp_resume_support,PUB_CONF_AUTO_STR))
	{
		struct_server_info->resume_support=PUB_CONF_AUTO;
	}
	else
	{
		struct_server_info->resume_support=PUB_CONF_NO;
	}
	if(0==strcmp(tmp_size_support,PUB_CONF_YES_STR))
	{
		struct_server_info->size_support=PUB_CONF_YES;
	}
	else if(0==strcmp(tmp_size_support,PUB_CONF_AUTO_STR))
	{
		struct_server_info->size_support=PUB_CONF_AUTO;
	}
	else
	{
		struct_server_info->size_support=PUB_CONF_NO;
	}
	if(0==strcmp(tmp_pasv_support,PUB_CONF_YES_STR))
	{
		struct_server_info->pasv_support=PUB_CONF_YES;
	}
	else if(0==strcmp(tmp_pasv_support,PUB_CONF_AUTO_STR))
	{
		struct_server_info->pasv_support=PUB_CONF_AUTO;
	}
	else
	{
		struct_server_info->pasv_support=PUB_CONF_NO;
	}


	for(i=0;i<struct_server_info->file_sys_group_count;i++)
	{
		sprintf(tmp_param,"%s%02d",PUB_SEG_FILE_PARAM_SRCDIR,i+1);
		if(0==get_config_val(config_fp,PUB_SEG_FILE,tmp_param,struct_file_sys[i].src_directory,err_code))
		{
			fclose(config_fp);
			return 0;
		}
		sprintf(tmp_param,"%s%02d",PUB_SEG_FILE_PARAM_DSTDIR,i+1);
		if(0==get_config_val(config_fp,PUB_SEG_FILE,tmp_param,struct_file_sys[i].dst_directory,err_code))
		{
			fclose(config_fp);
			return 0;
		}
		sprintf(tmp_param,"%s%02d",PUB_SEG_FILE_PARAM_LOGDIR,i+1);
		if(0==get_config_val(config_fp,PUB_SEG_FILE,tmp_param,struct_file_sys[i].log_directory,err_code))
		{
			fclose(config_fp);
			return 0;
		}
		sprintf(tmp_param,"%s%02d",PUB_SEG_FILE_PARAM_TMPDIR,i+1);
		if(0==get_config_val(config_fp,PUB_SEG_FILE,tmp_param,struct_file_sys[i].tmp_directory,err_code))
		{
			fclose(config_fp);
			return 0;
		}
		sprintf(tmp_param,"%s%02d",PUB_SEG_FILE_PARAM_BAKDIR,i+1);
		if(0==get_config_val(config_fp,PUB_SEG_FILE,tmp_param,struct_file_sys[i].bak_directory,err_code))
		{
			fclose(config_fp);
			return 0;
		}
		sprintf(tmp_param,"%s%02d",PUB_SEG_FILE_PARAM_LOGTMPDIR,i+1);
		if(0==get_config_val(config_fp,PUB_SEG_FILE,tmp_param,struct_file_sys[i].log_tmp_directory,err_code))
		{
			fclose(config_fp);
			return 0;
		}
		sprintf(tmp_param,"%s%02d",PUB_SEG_FILE_PARAM_NAME_INC,i+1);
		if(0==get_config_val(config_fp,PUB_SEG_FILE,tmp_param,struct_file_sys[i].name_include,err_code))
		{
			fclose(config_fp);
			return 0;
		}
		sprintf(tmp_param,"%s%02d",PUB_SEG_FILE_PARAM_NAME_EXC,i+1);
		if(0==get_config_val(config_fp,PUB_SEG_FILE,tmp_param,struct_file_sys[i].name_exclude,err_code))
		{
			fclose(config_fp);
			return 0;
		}
		sprintf(tmp_param,"%s%02d",PUB_SEG_FILE_PARAM_PREFIX,i+1);
		if(0==get_config_val(config_fp,PUB_SEG_FILE,tmp_param,struct_file_sys[i].file_prefix,err_code))
		{
			fclose(config_fp);
			return 0;
		}
		sprintf(tmp_param,"%s%02d",PUB_SEG_FILE_PARAM_POSTFIX,i+1);
		if(0==get_config_val(config_fp,PUB_SEG_FILE,tmp_param,struct_file_sys[i].file_postfix,err_code))
		{
			fclose(config_fp);
			return 0;
		}
		sprintf(tmp_param,"%s%02d",PUB_SEG_FILE_PARAM_TRANS_DIR,i+1);
		if(0==get_config_val(config_fp,PUB_SEG_FILE,tmp_param,struct_file_sys[i].trans_file_direction,err_code))
		{
			fclose(config_fp);
			return 0;
		}
		sprintf(tmp_param,"%s%02d",PUB_SEG_FILE_PARAM_BACK_FLAG,i+1);
		if(0==get_config_val(config_fp,PUB_SEG_FILE,tmp_param,struct_file_sys[i].remote_backup_flag,err_code))
		{
			fclose(config_fp);
			return 0;
		}
	}
#ifdef DEBUG_PUB_INIT_CONFIG_INFO
	fprintf(stderr,"[%s]\n",struct_server_info->host_ip);
	fprintf(stderr,"[%s]\n",struct_server_info->local_host_ip);
	fprintf(stderr,"[%d]\n",struct_server_info->host_port);
	fprintf(stderr,"[%d]\n",struct_server_info->file_sys_group_count);
	fprintf(stderr,"[%d]\n",struct_server_info->resume_support);
	fprintf(stderr,"[%d]\n",struct_server_info->size_support);
	fprintf(stderr,"[%d]\n",struct_server_info->pasv_support);
	fprintf(stderr,"[%d]\n",struct_server_info->split_parts_count);
	for(i=0;i<struct_server_info->file_sys_group_count;i++)
	{
		fprintf(stderr,"---------------------------------------\n");
		fprintf(stderr,"[%s]\n",struct_file_sys[i].src_directory);
		fprintf(stderr,"[%s]\n",struct_file_sys[i].dst_directory);
		fprintf(stderr,"[%s]\n",struct_file_sys[i].log_directory);
		fprintf(stderr,"[%s]\n",struct_file_sys[i].tmp_directory);
		fprintf(stderr,"[%s]\n",struct_file_sys[i].bak_directory);
		fprintf(stderr,"[%s]\n",struct_file_sys[i].log_tmp_directory);
		fprintf(stderr,"[%s]\n",struct_file_sys[i].name_include);
		fprintf(stderr,"[%s]\n",struct_file_sys[i].name_exclude);
		fprintf(stderr,"[%s]\n",struct_file_sys[i].file_prefix);
		fprintf(stderr,"[%s]\n",struct_file_sys[i].file_postfix);
		fprintf(stderr,"[%s]\n",struct_file_sys[i].trans_file_direction);
	}
#endif
	fclose(config_fp);
	return 1;
}

extern int get_config_val(FILE *cfg_fp,char *seg_name,char *val_name,char *ret_val,char *err_code)
{
	char p_buff[PUB_LENGTH_OF_LINE_MAX];
	char p_line_info[PUB_LENGTH_OF_LINE_MAX];
	char p_seg_name[PUB_LENGTH_OF_LINE_MAX];
	char p_ret_val[PUB_LENGTH_OF_LINE_MAX];
	char *tmp;
	int tmp_i;
	char err_affix[1000]={
		0	};

#ifdef DEBUG_PUB_GET_CONFIG_VAL
	fprintf(stderr,"seg_name=[%s]	val_name=[%s]\n",seg_name,val_name);
#endif
	rewind(cfg_fp);
	do
	{
		memset(p_buff,0,PUB_LENGTH_OF_LINE_MAX);
		clearerr(cfg_fp);
		if(NULL==fgets(p_buff,PUB_LENGTH_OF_LINE_MAX,cfg_fp))
		{
			if(ferror(cfg_fp))
			{
				deal_run_err_msg("warn",__FILE__,__LINE__,PUB_ERR_FOPEN,"n/a",err_code);
				return 0;
			}
		}
		tmp=p_buff;
		while('\t'==tmp[0] || ' '==tmp[0])
		{
			tmp++;
		}
		strcpy(p_line_info,tmp);
		if(NULL!=(tmp=strchr(p_line_info,'#')))
		{
			tmp[0]=0;
		}
		if(NULL!=(tmp=strchr(p_line_info,'\r')))
		{
			tmp[0]=0;
		}
		if(NULL!=(tmp=strchr(p_line_info,'\n')))
		{
			tmp[0]=0;
		}
		if(strlen(p_line_info))
		{
			if('['==p_line_info[0])
			{
				tmp=strchr(p_line_info,']');
				if(NULL==tmp)
				{
					sprintf(err_affix,"associated with [%s]",p_line_info);
					deal_run_err_msg("warn",__FILE__,__LINE__,PUB_ERR_SEG_FORMAT,err_affix,err_code);
					return 0;
				}
				tmp[0]=0;
				strcpy(p_seg_name,p_line_info+1);
			}
			if((size_t)0!=strlen(seg_name) && 0==strcmp(p_seg_name,seg_name))
			{
				tmp_i=strlen(val_name);
				if(0==strncmp(p_line_info,val_name,tmp_i))
				{
					if(' '==p_line_info[tmp_i] || '\t'==p_line_info[tmp_i] || '='==p_line_info[tmp_i])
					{
						tmp=strchr(p_line_info,'\"');
						if(NULL==tmp)
						{
							sprintf(err_affix,"associated with [%s]",p_line_info);
							deal_run_err_msg("warn",__FILE__,__LINE__,PUB_ERR_PARAM_FORMAT,err_affix,err_code);
							return 0;
						}
						strcpy(p_ret_val,tmp+1);
						tmp=strchr(p_ret_val,'\"');
						if(NULL==tmp)
						{
							sprintf(err_affix,"associated with [%s]",p_ret_val);
							deal_run_err_msg("warn",__FILE__,__LINE__,PUB_ERR_PARAM_FORMAT,err_affix,err_code);
							return 0;
						}
						tmp[0]=0;
						strcpy(ret_val,p_ret_val);
						return 1;
					}
				}
			}
		}
	}while(!feof(cfg_fp));
	deal_run_err_msg("warn",__FILE__,__LINE__,PUB_ERR_PARAM_FORMAT,"n/a",err_code);
	return 0;
}

extern int File_is_ok(char *file_name,char *include_flag,char *exclude_flag)
{
	char include_name[PUB_LENGTH_OF_LINE_MAX]={
		0	};
	char exclude_name[PUB_LENGTH_OF_LINE_MAX]={
		0	};
	char *in_p;
	char *ex_p;
	char *tmp_p;
	int not_in_include;
	int s1_strlen,s2_strlen;
	int in_str_flag;

	if(include_flag[0])
		sprintf(include_name,"|%s",include_flag);
	if(exclude_flag[0])
		sprintf(exclude_name,"|%s",exclude_flag);
	not_in_include=1;
	while(NULL!=(in_p=strrchr(include_name,'|')))
	{
		if('^'==in_p[1])
		{
			if(0==strncmp(file_name,in_p+2,strlen(in_p+2)))
			{
				not_in_include=0;
				break;
			}
			else
			{
				in_p[0]=0;
				continue;
			}
		}
		else if('$'==in_p[1])
		{
			s1_strlen=strlen(file_name);
			s2_strlen=strlen(in_p+2);
			in_str_flag=1;
			if(s1_strlen >= s2_strlen)
			{
				if(0!=strcmp(in_p+2,file_name+(s1_strlen-s2_strlen)))
				{
					in_str_flag=0;
				}
			}
			else
			{
				in_str_flag=0;
			}
			if(in_str_flag)
			{
				not_in_include=0;
				break;
			}
			else
			{
				in_p[0]=0;
				continue;
			}
		}
		else if('*'==in_p[1])
		{
			not_in_include=0;
			break;
		}
		else
		{
			if(NULL!=(tmp_p=strstr(file_name,in_p+1)))
			{
				not_in_include=0;
				break;
			}
			else
			{
				in_p[0]=0;
				continue;
			}
		}
	}
	if(not_in_include)
	{
		return 0;
	}

	while(NULL!=(ex_p=strrchr(exclude_name,'|')))
	{
		if('^'==ex_p[1])
		{
			if(0==strncmp(file_name,ex_p+2,strlen(ex_p+2)))
			{
				return 0;
			}
			else
			{
				ex_p[0]=0;
				continue;
			}
		}
		else if('$'==ex_p[1])
		{
			s1_strlen=strlen(file_name);
			s2_strlen=strlen(ex_p+2);
			in_str_flag=1;
			if(s1_strlen >= s2_strlen)
			{
				if(0!=strcmp(ex_p+2,file_name+(s1_strlen-s2_strlen)))
				{
					in_str_flag=0;
				}
			}
			else
			{
				in_str_flag=0;
			}
			if(in_str_flag)
			{
				return 0;
			}
			else
			{
				ex_p[0]=0;
				continue;
			}
		}
		else if('*'==ex_p[1])
		{
			return 0;
		}
		else
		{
			if(NULL!=(tmp_p=strstr(file_name,ex_p+1)))
			{
				return 0;
			}
			else
			{
				ex_p[0]=0;
				continue;
			}
		}
	}
	return 1;
}

extern int get_pwd_info(char *file_name,char *key_word,char *user_name,int name_len,char *pwd_val,int pwd_len,char *err_code)
{
	FILE *fp;
	char buff[256];
	int i,j;
	int key_len;
	char err_affix[1000]={
		0	};

	key_len=strlen(key_word);
	if(NULL==(fp=fopen(file_name,"rb")))
	{
		sprintf(err_affix,"associated with [%s]",file_name);
		deal_run_err_msg("warn",__FILE__,__LINE__,PUB_ERR_FOPEN,err_affix,err_code);
		return 0;
	}

	if((size_t)256!=fread(buff,1,256,fp))
	{
		if(ferror(fp))
		{
			deal_run_err_msg("warn",__FILE__,__LINE__,PUB_ERR_PWDFILE_IO,"n/a",err_code);
			fclose(fp);
			return 0;
		}
		if(feof(fp))
		{
			deal_run_err_msg("warn",__FILE__,__LINE__,PUB_ERR_PWDFILE,"n/a",err_code);
			fclose(fp);
			return 0;
		}
	}
	for(i=0;i<256;i++)
	{
		j=i-i/key_len*key_len;
		user_name[i]=buff[i]^key_word[j];
		if(i==name_len)
		{
			user_name[i]=0;
			break;
		}
	}

	if((size_t)256!=fread(buff,1,256,fp))
	{
		if(ferror(fp))
		{
			deal_run_err_msg("warn",__FILE__,__LINE__,PUB_ERR_PWDFILE_IO,"n/a",err_code);
			fclose(fp);
			return 0;
		}
		if(feof(fp))
		{
			deal_run_err_msg("warn",__FILE__,__LINE__,PUB_ERR_PWDFILE,"n/a",err_code);
			fclose(fp);
			return 0;
		}
	}
	for(i=0;i<256;i++)
	{
		j=i-i/key_len*key_len;
		pwd_val[i]=buff[i]^key_word[j];
		if(i==pwd_len)
		{
			pwd_val[i]=0;
			break;
		}
	}
	fclose(fp);
#ifdef DEBUG_PUB_GET_PWD_USER
	fprintf(stderr,"Get user name from password file! The user name is [%s]\n",user_name);
	fprintf(stderr,"Get password from password file! The password is [%s]\n",pwd_val);
#endif
	return 1;
}

extern int gen_pwd_file(char *file_name,char *key_word,char *user_name,char *pwd_val,char *err_code)
{
	FILE *fp;
	char buff[256];
	int i,j;
	int key_len;
	char err_affix[1000]={
		0	};

	key_len=strlen(key_word);
	if(NULL==(fp=fopen(file_name,"wb")))
	{
		sprintf(err_affix,"associated with [%s]",file_name);
		deal_run_err_msg("warn",__FILE__,__LINE__,PUB_ERR_FOPEN,err_affix,err_code);
		return 0;
	}

	strcpy(buff,user_name);
	for(i=0;i<256;i++)
	{
		j=i-i/key_len*key_len;
		buff[i]=user_name[i]^key_word[j];
		if(0==user_name[i])
		{
			break;
		}
	}
	if((size_t)256!=fwrite(buff,1,256,fp))
	{
		if(ferror(fp))
		{
			deal_run_err_msg("warn",__FILE__,__LINE__,PUB_ERR_PWDFILE_IO,"n/a",err_code);
			fclose(fp);
			return 0;
		}
		deal_run_err_msg("warn",__FILE__,__LINE__,PUB_ERR_PWDFILE,"n/a",err_code);
		fclose(fp);
		return 0;
	}

	strcpy(buff,pwd_val);
	for(i=0;i<256;i++)
	{
		j=i-i/key_len*key_len;
		buff[i]=pwd_val[i]^key_word[j];
		if(0==pwd_val[i])
		{
			break;
		}
	}
	if((size_t)256!=fwrite(buff,1,256,fp))
	{
		if(ferror(fp))
		{
			deal_run_err_msg("warn",__FILE__,__LINE__,PUB_ERR_PWDFILE_IO,"n/a",err_code);
			fclose(fp);
			return 0;
		}
		deal_run_err_msg("warn",__FILE__,__LINE__,PUB_ERR_PWDFILE,"n/a",err_code);
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return 1;
}
