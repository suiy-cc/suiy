/*********************************************************************
*
*	Copyright (C), 1995-2003, Si-Tech Information Technology Ltd.
*
**********************************************************************
*
*	File_name:	ftp_public.h
*	Author:		张奇伟
*	Version:	版本（V1.00） 
*	Date:		2003/11/15
*	Description:
*		本文件定义了所有ftp_public.c中用到的宏定义。
*
**********************************************************************/

#define PUB_DIR_LEN_MAX		1000		/*文件名最大长度（含目录）*/
#define PUB_PREFIX_LEN_MAX	100		/*添加前缀字符最大长度*/
#define PUB_POSTFIX_LEN_MAX	100		/*添加后缀字符最大长度*/
#define PUB_TRANS_DIR_LEN_MAX	10		/*描述传输方向的字串最大长度*/
#define PUB_LENGTH_OF_LINE_MAX	1000		/*文本文件一行中字符最大数目*/
#define PUB_IP_ADD_LEN		16		/*IP地址字串最大长度*/
#define PUB_CONF_YES_STR	"YES"
#define PUB_CONF_NO_STR		"NO"
#define PUB_CONF_AUTO_STR	"AUTO"
#define PUB_CONF_YES		1		/*"YES"在程序中的标识*/
#define PUB_CONF_NO		2		/*"NO"在程序中的标识*/
#define PUB_CONF_AUTO		3		/*"AUTO"在程序中的标识*/
#define PUB_CONF_UP		"UP"		/*"UP"在程序中的标识*/
#define PUB_CONF_DOWN		"DOWN"		/*"DOWN"在程序中的标识*/
#define PUB_CONF_RUN		"RUN"		/*"RUN"在程序中的标识*/
#define PUB_CONF_STOP		"STOP"		/*"STOP"在程序中的标识*/
/******************************************
*	对配置文件段名和记录名的定义
*/
#define PUB_SEG_SYS			"SYS_INFO"
#define PUB_SEG_SYS_PARAM_IP		"HOST_IP"
#define PUB_SEG_SYS_PARAM_LOCAL_IP	"LOCAL_HOST_IP"
#define PUB_SEG_SYS_PARAM_PORT		"HOST_PORT"
#define PUB_SEG_SYS_PARAM_COUNT		"FILE_SYSTEM_GROUP_COUNT"
#define PUB_SEG_SYS_PARAM_RESUME	"RESUME_SUPPORT"
#define PUB_SEG_SYS_PARAM_SIZE		"SIZE_SUPPORT"
#define PUB_SEG_SYS_PARAM_PASV		"PASV_SUPPORT"
#define PUB_SEG_SYS_PARAM_PARTS		"THREADS_PER_FILE"

#define PUB_SEG_FILE			"FILE_SYSTEM"
#define PUB_SEG_FILE_PARAM_SRCDIR	"SRC_DIRECTORY"		/*后面跟两位数字区别不同分组*/
#define PUB_SEG_FILE_PARAM_DSTDIR	"DST_DIRECTORY"		/*后面跟两位数字区别不同分组*/
#define PUB_SEG_FILE_PARAM_LOGDIR	"LOG_DIRECTORY"		/*后面跟两位数字区别不同分组*/
#define PUB_SEG_FILE_PARAM_TMPDIR	"TMP_DIRECTORY"		/*后面跟两位数字区别不同分组*/
#define PUB_SEG_FILE_PARAM_BAKDIR	"BAK_DIRECTORY"		/*后面跟两位数字区别不同分组*/
#define PUB_SEG_FILE_PARAM_LOGTMPDIR	"LOG_TMP_DIRECTORY"	/*后面跟两位数字区别不同分组*/
#define PUB_SEG_FILE_PARAM_NAME_INC	"NAME_INCLUDE"		/*后面跟两位数字区别不同分组*/
#define PUB_SEG_FILE_PARAM_NAME_EXC	"NAME_EXCLUDE"		/*后面跟两位数字区别不同分组*/
#define PUB_SEG_FILE_PARAM_PREFIX	"PREFIX_CODES"		/*后面跟两位数字区别不同分组*/
#define PUB_SEG_FILE_PARAM_POSTFIX	"POSTFIX_CODES"		/*后面跟两位数字区别不同分组*/
#define PUB_SEG_FILE_PARAM_TRANS_DIR	"UP_DOWN_FLAG"		/*后面跟两位数字区别不同分组*/
#define PUB_SEG_FILE_PARAM_BACK_FLAG	"DOWNLOAD_BAK_FLAG"	/*后面跟两位数字区别不同分组*/

#define PUB_SEG_TERMINAL			"TERMINAL_INFO"
#define PUB_SEG_TERMINAL_PARAM_FLAG		"RUNNING_STATUS"
/******************************************
*	对错误类型的定义
*/
#define PUB_ERR_FOPEN		"EcaipbSY1"	/*打开文件错误*/
#define PUB_ERR_MALLOC		"EcaipbSY2"	/*内存分配错误*/
#define PUB_ERR_SEG_FORMAT	"EcaipbSY3"	/*段名配置错误*/
#define PUB_ERR_PARAM_FORMAT	"EcaipbSY4"	/*参数配置错误*/
#define PUB_ERR_PWDFILE		"EcaipbSY5"	/*口令文件配置错误*/
#define PUB_ERR_PWDFILE_IO	"EcaipbSY6"	/*读口令文件IO错误*/

/******************************************
*	对调试开关的定义

*/
#define DEBUG_PUB_INIT_CONFIG_INFO
#define DEBUG_PUB_GET_CONFIG_VAL
#define DEBUG_PUB_GET_PWD_USER

/*************************************************************************************
*	对结构的定义
*/
struct STRU_FILE_SYSTEM_INFO
{
	char src_directory[PUB_DIR_LEN_MAX];			/*源目录*/
	char dst_directory[PUB_DIR_LEN_MAX];			/*目的目录*/
	char log_directory[PUB_DIR_LEN_MAX];			/*日志目录*/
	char tmp_directory[PUB_DIR_LEN_MAX];			/*临时目录*/
	char bak_directory[PUB_DIR_LEN_MAX];			/*备份目录*/
	char log_tmp_directory[PUB_DIR_LEN_MAX];		/*日志临时目录*/
	char name_include[PUB_DIR_LEN_MAX];			/*文件名需要包含的标识字串*/
	char name_exclude[PUB_DIR_LEN_MAX];			/*文件名需要必须不包含的标识字串*/
	char file_prefix[PUB_PREFIX_LEN_MAX];			/*添加的文件前缀*/
	char file_postfix[PUB_POSTFIX_LEN_MAX];			/*添加的文件后缀*/
	char trans_file_direction[PUB_TRANS_DIR_LEN_MAX];	/*传送方向*/
	char remote_backup_flag[PUB_TRANS_DIR_LEN_MAX];		/*远程主机是否备份标志*/
};
struct STRU_SERVER_INFO
{
	char host_ip[PUB_IP_ADD_LEN];			/*远程主机IP地址*/
	char local_host_ip[PUB_IP_ADD_LEN];		/*本地主机IP地址*/
	unsigned int host_port;				/*主机端口号*/
	unsigned int file_sys_group_count;		/*文件配置部分共有多少组*/
	unsigned int resume_support;			/*断点续传标志*/
	unsigned int size_support;			/*size支持标志*/
	unsigned int pasv_support;			/*被动模式支持标志*/
	unsigned int split_parts_count;			/*拆分的块数*/
};

/*************************************************************************************
*	对函数的声名
*/

/******************************************
*	处理运行时错误信息
*/
extern void deal_run_err_msg
(
	char *,		/*错误级别*/
	char *,		/*错误源文件*/
	int ,		/*错误行*/
	char *,		/*错误代码*/
	char *,		/*附加错误信息*/
	char *		/*返回错误信息*/
);

/******************************************
*	根据段和配置变量名在配置文件中
*	查找相关信息
*/
extern int get_config_val
(
	FILE *,		/*配置文件句柄*/
	char *,		/*配置文件中的段名*/
	char *,		/*配置文件中的配置变量名*/
	char *,		/*返回的配置变量数值*/
	char *		/*错误代码*/
);

/******************************************
*	初始化配置文件
*/
extern int init_config_info
(
	char *,				/*配置文件名*/
	struct STRU_FILE_SYSTEM_INFO *,	
	struct STRU_SERVER_INFO *,
	char *				/*错误代码*/
);
/******************************************
*	确定是否是需要的文件
*/
extern int File_is_ok
(
	char *,				/*文件名,不带目录的*/
	char *,				/*要求文件名中必须包含的字串*/
	char *				/*要求文件名中不能包含的字串*/
);
/******************************************
*	得到用户名和口令
*/
extern int get_pwd_info
(
	char *,				/*口令文件名*/
	char *,				/*解密密钥*/
	char *,				/*解密后得到的用户名*/
	int,				/*传入的用来记录解密后用户名的字串长度*/
	char *,				/*解密后得到的用户口令*/
	int,				/*传入的用来记录解密后用户口令的字串长度*/
	char *				/*错误代码*/
);
/******************************************
*	根据用户名和口令生成口令文件
*/
extern int gen_pwd_file
(
	char *,				/*口令文件名*/
	char *,				/*加密密钥*/
	char *,				/*加密的用户名*/
	char *,				/*加密的用户口令*/
	char *				/*错误代码*/
);
