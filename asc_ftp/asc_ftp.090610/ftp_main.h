/*********************************************************************
*
*	Copyright (C), 1995-2003, Si-Tech Information Technology Ltd.
*
**********************************************************************
*
*	File_name:	ftp_main.h
*	Author:		����ΰ
*	Version:	�汾��V1.00�� 
*	Date:		2003/11/15
*	Description:
		����ftp_main.c�õ������к궨�������ļ���
*		����궨���ʾ�ĺ���μ��궨����ע�͡�
*
**********************************************************************/

#include "ftp_function.h"

#define MAIN_ERRCODES_LEN_MAX		2000			/*������볤��*/
#define MAIN_USER_NAME_LEN_MAX		256			/*�û�������*/
#define MAIN_PASSWORD_LEN_MAX		256			/*�û������*/
#define MAIN_FILE_LEN_MAX		256			/*�ļ�������*/
#define MAIN_RECONN_INTERVAL		10			/*�������ӷ������������*/
#define MAIN_GRP_PROC_INTERVAL		15			/*������һ��ԴĿ¼��ֹͣ�ļ��*/
#define MAIN_TIME_INTERVAL		900			/*�³���־�ļ��ļ��*/
#define MAIN_FUN_KEY_VALUE		FUN_KEY_VALUE		/*������Կ*/
#define MAIN_FUN_LOG_FILE_HEAD		"cai"			/*��־�ļ���ͷ��*/

/*********************************************************************
*	�Ե��Կ��صĶ���
*/
#define DEBUG_MAIN_RUNNING
