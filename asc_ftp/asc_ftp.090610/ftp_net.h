/*********************************************************************
*
*	Copyright (C), 1995-2003, Si-Tech Information Technology Ltd.
*
**********************************************************************
*
*	File_name:	ftp_net.h
*	Author:		张奇伟
*	Version:	版本（V1.00） 
*	Date:		2003/11/15
*	Description:
*		本文件包含所有ftp_net.c中用到的宏定义。
*		具体宏定义的含义，参见宏定义后的注释。
*
**********************************************************************/

#define NET_TIME_OUT	900

/*************************************************
*	对错误类型的定义
*/
#define NET_ERR_IP_TRAN		"EcaintSY1"	/*由字符串IP转换到网络数字IP时出错*/
#define NET_ERR_SOCKET		"EcaintSY2"	/*创建socket时出错*/
#define NET_ERR_CONN		"EcaintSY3"	/*建立socket连接时出错*/
#define NET_ERR_SELECT		"EcaintSY4"	/*SELECT时出错*/
#define NET_ERR_TIMEOUT		"EcaintSY5"	/*socket超时出错*/
#define NET_ERR_SEND		"EcaintSY6"	/*SEND函数出错*/
#define NET_ERR_READ		"EcaintSY7"	/*READ函数出错*/
#define NET_ERR_WRITE		"EcaintSY8"	/*WRITE函数出错*/
#define NET_ERR_BIND		"EcaintSY9"	/*bind端口时出错*/
#define NET_ERR_GET_SOCK_NAME	"EcaintSYa"	/*反向获得socket信息时出错*/

/*************************************************
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

/*************************************************
*	建立一个socket连接
*/
extern int make_connection
(
	int *,			/*socket句柄*/
	char *,			/*字符串IP*/
	unsigned int,		/*端口号*/
	char *			/*错误代码*/
);
/*************************************************
*	发出一条指令
*/
extern int send_command
(
	int,			/*socket句柄*/
	char *,			/*命令字符串*/
	char *			/*错误代码*/
);
/*************************************************
*	获得一行字符信息
*/
extern int get_line_msg
(
	int,			/*socket句柄*/
	char *,			/*获得的信息*/
	char *			/*错误代码*/
);
/*************************************************
*	从socket中获得指定大小的数据
*/
extern int get_data_with_proper_size
(
	int ,		/*socket句柄*/
	unsigned long ,	/*指定需要返回的数据长度*/
	char *,		/*返回的数据*/
	unsigned long *,/*实际返回的数据长度*/
	char *		/*错误代码*/
);
/*************************************************
*	从socket中写入指定大小的数据
*/
extern int put_data_with_proper_size
(
	int ,		/*socket句柄*/
	long ,		/*指定需要写入的数据长度*/
	char *,		/*写入的数据*/
	char *		/*错误代码*/
);
/*************************************************
*	生成服务端，并提供服务socket。
*/
extern int make_socket_server
(
	int *,		/*服务socket*/
	unsigned int *,	/*本地主机的空闲端口号*/
	char *		/*错误代码*/
);
