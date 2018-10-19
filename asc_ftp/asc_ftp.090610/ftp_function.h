/*********************************************************************
*
*	Copyright (C), 1995-2003, Si-Tech Information Technology Ltd.
*
**********************************************************************
*
*	File_name:	ftp_function.h
*	Author:		张奇伟
*	Version:	版本（V1.00） 
*	Date:		2003/11/15
*	Description:
*		本文件包含在ftp_function.c中用到的所有宏定义。
*	具体宏定义的含义参见宏定义后的注释。
*
**********************************************************************/

#include "ftp_public.h"
#include "ftp_net.h"

#define FUN_RET_LINE_LEN_MAX		2048			/*会话中返回信息字串最大长度*/
#define FUN_RET_BUFF_LEN_MAX		1024			/*缓冲区长度*/
#define FUN_CMD_LINE_LEN_MAX		1024			/*会话时指令长度*/
#define FUN_KEY_VALUE			"HAHAHACAIBUZHAO"	/*加密密钥*/
#define FUN_PUB_DIR_LEN_MAX		PUB_DIR_LEN_MAX		/*文件名最大长度（连同目录）*/
#define FUN_PUB_LENGTH_OF_LINE_MAX	PUB_LENGTH_OF_LINE_MAX	/*字符文件中一行最大长度*/
#define FUN_PUB_CONF_YES		PUB_CONF_YES		/*"YES"在程序中的标识*/
#define FUN_PUB_CONF_NO			PUB_CONF_NO		/*"NO"在程序中的标识*/
#define FUN_PUB_CONF_AUTO		PUB_CONF_AUTO		/*"AUTO"在程序中的标识*/
#define FUN_PUB_CONF_UP			PUB_CONF_UP		/*"UP"在程序中的标识*/
#define FUN_PUB_CONF_DOWN		PUB_CONF_DOWN		/*"DOWN"在程序中的标识*/
#define FUN_PUB_CONF_RUN		PUB_CONF_RUN		/*"RUN"在程序中的标识*/
#define FUN_PUB_CONF_STOP		PUB_CONF_STOP		/*"STOP"在程序中的标识*/

/******************************************
*	对错误类型的定义
*/
#define FUN_ERR_FOPEN		"EcaifnSY1"	/*打开文件出错*/
#define FUN_ERR_F_IO		"EcaifnSY2"	/*读写文件IO出错*/
#define FUN_ERR_MSG_TAG		"EcaifnSY3"	/*返回信息头标记错误*/
#define FUN_ERR_PASV_INFO	"EcaifnSY4"	/*PASV信息错误*/
#define FUN_ERR_NOT_SAME	"EcaifnSY5"	/*校验文件出错*/
#define FUN_ERR_NOT_FILE	"EcaifnSY6"	/*不是所需的文件*/
#define FUN_ERR_RENAME		"EcaifnSY7"	/*移动文件时出错*/
#define FUN_ERR_DELETE_FILE	"EcaifnSY8"	/*删除远程主机文件时出错*/
#define FUN_ERR_VERIFY_FILE	"EcaifnSY9"	/*校验传输的文件时出错*/
#define FUN_ERR_PORT_INFO	"EcaifnSYa"	/*处理PORT信息时出错*/

/******************************************
*	对调试开关的定义
*/	
#define DEBUG_FUN_INIT_ENVIRONMENT
#define DEBUG_FUN_BLK_MSG
#define DEBUG_FUN_PASV_INFO
#define DEBUG_FUN_INFO_MSG
#define DEBUG_FUN_FILE_LIST
#define DEBUG_FUN_RETR_FILE_INFO
#define DEBUG_FUN_GET_FILE_SIZE
#define DEBUG_FUN_GET_BIN_FILE
#define DEBUG_FUN_DELETE_FILE
#define DEBUG_FUN_PUT_BIN_FILE
#define DEBUG_FUN_RETR_PORT_INFO

/******************************************
*	对结构的定义
*/
struct STRU_SUPPORT_ARGUMENTS
{
	int Is_support_pasv;
	int Is_support_size;
	int Is_support_resume;
};

/******************************************
*	对返回信息代码的定义
*/
#define	FUN_FTP_TAG_CMD_OK	"250 "		/*命令被服务器确认*/
#define	FUN_FTP_TAG_RESUME_OK	"350 "		/*服务器允许resume*/
#define	FUN_FTP_TAG_SIZE_OK	"213 "		/*服务器允许size*/
#define	FUN_FTP_TAG_PASV_OK	"227 "		/*服务器允许pasv*/
#define	FUN_FTP_TAG_TRANS_OK	"226 "		/*文件成功传送*/
#define	FUN_FTP_TAG_PASS_ERR	"530 "		/*登陆用户名/口令错误*/

#define	FUN_FTP_TAG_HEAD_OK	'2'		/*命令执行成功返回标记*/
#define	FUN_FTP_TAG_HEAD_ERR	'5'		/*命令执行失败返回标记*/
#define	FUN_FTP_TAG_HEAD_RESUME	'3'		/*命令等待数据返回标记*/
/******************************************
*	对函数的声名
*/


/******************************************
*	初始化配置文件中描述的配置信息
*	建立到服务器的连接
*/
extern int init_environment
(
	char *,					/*口令文件名*/
	int ,					/*用户名变量长度*/
	int ,					/*口令变量长度*/
	char *,					/*配置文件名*/
	char *,					/*用户名*/
	char *,					/*用户口令*/
	int *,					/*socket句柄*/
	struct STRU_FILE_SYSTEM_INFO *,		/*文件信息*/
	struct STRU_SERVER_INFO *,		/*服务器信息*/
	struct STRU_SUPPORT_ARGUMENTS *,	/*服务器支持信息*/
	char *					/*错误代码*/
);
/******************************************
*	获取并返回一行返回信息
*/
extern int get_info_msg
(
	int ,		/*socket句柄*/
	char *,		/*返回信息*/
	char *		/*错误代码*/
);
/******************************************
*	获取不返回RFC描述的一块返回信息
*/
extern int get_blk_msg
(
	int ,		/*socket句柄*/
	char *		/*错误代码*/
);
/******************************************
*	处理pasv反馈的信息，并得到
*	数据链路端口号。
*/
extern int Retr_PASV_Info
(
	char *,		/*前一个函数返回的PASV信息*/
	char *,		/*从PASV中提取的IP信息*/
	unsigned int  *,/*从PASV中提取的PORT信息*/
	char *		/*错误代码*/
);
/******************************************
*	根据文件大小比较两个文件
*/
extern int diff_tow_file
(
	char *,		/*本地文件名*/
	unsigned long ,	/*远程文件大小*/
	char *		/*错误代码*/
);

/*-----------------------------------------------------------------------*/

/******************************************
*	得到服务器上指定路径下的文件列表
*/
extern int get_file_list
(
	int  ,					/*socket句柄*/
	struct STRU_SUPPORT_ARGUMENTS *,	/*服务器支持的命令*/
	char *,					/*本地主机IP地址*/
	char *,					/*远程目录名*/
	char *,					/*本地存放目录内容的文件*/
	char *					/*错误代码*/
);
/******************************************
*	根据远程服务器返回的文件列表
*	得到文件名信息
*/
extern int get_retr_file_info
(
	char *,				/*从目录列表中取出一行文件信息*/
	struct STRU_FILE_SYSTEM_INFO ,	/*文件系统信息*/
	char *,				/*源文件*/
	char *,				/*临时文件*/
	char *,				/*目标文件*/
	char *				/*错误代码*/
);
/******************************************
*	确认指定的文件是否是所需的文件
*/
extern int Identify_file_name
(
	char *,		/*需要确认的文件名*/
	char *,		/*包含标志*/
	char *		/*不包含标志*/
);
/******************************************
*	得到远程服务器上指定文件的大小
*/
extern int get_file_size
(
	int ,		/*socket句柄*/
	int ,		/*服务器是否支持SIZE命令*/
	char *,		/*从目录列表中取出一行文件信息*/
	char *,		/*要获取大小的文件名*/
	unsigned long *,/*文件大小*/
	char *		/*错误代码*/
);
/******************************************
*	获取一个指定的文件
*/
extern int get_bin_file
(
	int ,					/*socket句柄*/
	struct STRU_SUPPORT_ARGUMENTS * ,	/*服务器支持的命令*/
	char *,					/*本地主机IP地址*/
	char *,					/*从目录列表中取出一行文件信息*/
	char *,					/*要获取数据的文件名*/
	char *,					/*目标文件*/
	char *,					/*临时文件*/
	unsigned long *,			/*文件大小*/
	char *					/*错误代码*/
);
/******************************************
*	删除一个指定的文件
*/
extern int delete_file
(
	int ,		/*socket句柄*/
	char *,		/*远程主机上待删除的文件名*/
	char *		/*错误代码*/
);
/******************************************
*	上传一个指定的文件
*/
extern int put_bin_file
(
	int ,					/*socket句柄*/
	struct STRU_SUPPORT_ARGUMENTS *,	/*服务器支持的命令*/
	char *,					/*本地主机IP地址*/
	char *,					/*源文件*/
	char *,					/*目标文件*/
	char *,					/*目标临时文件*/
	unsigned long ,				/*文件大小*/
	char *					/*错误代码*/
);
/******************************************
*	得到运行调度标志
*/
extern int get_running_flag
(
	char *,				/*配置文件名*/
	char *				/*错误代码*/
);
/******************************************
*	移动远程主机上的文件
*/
extern int move_remote_file
(
	int ,	/*socket句柄*/
	char *,		/*源文件名*/
	char *,		/*目标文件名*/
	char *		/*错误代码*/
);
