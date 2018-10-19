/*********************************************************************
*
*	Copyright (C), 1995-2003, Si-Tech Information Technology Ltd.
*
**********************************************************************
*
*	File_name:	ftp_main.h
*	Author:		张奇伟
*	Version:	版本（V1.00） 
*	Date:		2003/11/15
*	Description:
		这是ftp_main.c用到的所有宏定义声名文件。
*		具体宏定义表示的含义参见宏定义后的注释。
*
**********************************************************************/

#include "ftp_function.h"

#define MAIN_ERRCODES_LEN_MAX		2000			/*错误代码长度*/
#define MAIN_USER_NAME_LEN_MAX		256			/*用户名长度*/
#define MAIN_PASSWORD_LEN_MAX		256			/*用户口令长度*/
#define MAIN_FILE_LEN_MAX		256			/*文件名长度*/
#define MAIN_RECONN_INTERVAL		10			/*重新连接服务器操作间隔*/
#define MAIN_GRP_PROC_INTERVAL		15			/*处理完一个源目录后停止的间隔*/
#define MAIN_TIME_INTERVAL		900			/*吐出日志文件的间隔*/
#define MAIN_FUN_KEY_VALUE		FUN_KEY_VALUE		/*加密密钥*/
#define MAIN_FUN_LOG_FILE_HEAD		"cai"			/*日志文件名头部*/

/*********************************************************************
*	对调试开关的定义
*/
#define DEBUG_MAIN_RUNNING
