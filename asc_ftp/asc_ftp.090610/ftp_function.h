/*********************************************************************
*
*	Copyright (C), 1995-2003, Si-Tech Information Technology Ltd.
*
**********************************************************************
*
*	File_name:	ftp_function.h
*	Author:		����ΰ
*	Version:	�汾��V1.00�� 
*	Date:		2003/11/15
*	Description:
*		���ļ�������ftp_function.c���õ������к궨�塣
*	����궨��ĺ���μ��궨����ע�͡�
*
**********************************************************************/

#include "ftp_public.h"
#include "ftp_net.h"

#define FUN_RET_LINE_LEN_MAX		2048			/*�Ự�з�����Ϣ�ִ���󳤶�*/
#define FUN_RET_BUFF_LEN_MAX		1024			/*����������*/
#define FUN_CMD_LINE_LEN_MAX		1024			/*�Ựʱָ���*/
#define FUN_KEY_VALUE			"HAHAHACAIBUZHAO"	/*������Կ*/
#define FUN_PUB_DIR_LEN_MAX		PUB_DIR_LEN_MAX		/*�ļ�����󳤶ȣ���ͬĿ¼��*/
#define FUN_PUB_LENGTH_OF_LINE_MAX	PUB_LENGTH_OF_LINE_MAX	/*�ַ��ļ���һ����󳤶�*/
#define FUN_PUB_CONF_YES		PUB_CONF_YES		/*"YES"�ڳ����еı�ʶ*/
#define FUN_PUB_CONF_NO			PUB_CONF_NO		/*"NO"�ڳ����еı�ʶ*/
#define FUN_PUB_CONF_AUTO		PUB_CONF_AUTO		/*"AUTO"�ڳ����еı�ʶ*/
#define FUN_PUB_CONF_UP			PUB_CONF_UP		/*"UP"�ڳ����еı�ʶ*/
#define FUN_PUB_CONF_DOWN		PUB_CONF_DOWN		/*"DOWN"�ڳ����еı�ʶ*/
#define FUN_PUB_CONF_RUN		PUB_CONF_RUN		/*"RUN"�ڳ����еı�ʶ*/
#define FUN_PUB_CONF_STOP		PUB_CONF_STOP		/*"STOP"�ڳ����еı�ʶ*/

/******************************************
*	�Դ������͵Ķ���
*/
#define FUN_ERR_FOPEN		"EcaifnSY1"	/*���ļ�����*/
#define FUN_ERR_F_IO		"EcaifnSY2"	/*��д�ļ�IO����*/
#define FUN_ERR_MSG_TAG		"EcaifnSY3"	/*������Ϣͷ��Ǵ���*/
#define FUN_ERR_PASV_INFO	"EcaifnSY4"	/*PASV��Ϣ����*/
#define FUN_ERR_NOT_SAME	"EcaifnSY5"	/*У���ļ�����*/
#define FUN_ERR_NOT_FILE	"EcaifnSY6"	/*����������ļ�*/
#define FUN_ERR_RENAME		"EcaifnSY7"	/*�ƶ��ļ�ʱ����*/
#define FUN_ERR_DELETE_FILE	"EcaifnSY8"	/*ɾ��Զ�������ļ�ʱ����*/
#define FUN_ERR_VERIFY_FILE	"EcaifnSY9"	/*У�鴫����ļ�ʱ����*/
#define FUN_ERR_PORT_INFO	"EcaifnSYa"	/*����PORT��Ϣʱ����*/

/******************************************
*	�Ե��Կ��صĶ���
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
*	�Խṹ�Ķ���
*/
struct STRU_SUPPORT_ARGUMENTS
{
	int Is_support_pasv;
	int Is_support_size;
	int Is_support_resume;
};

/******************************************
*	�Է�����Ϣ����Ķ���
*/
#define	FUN_FTP_TAG_CMD_OK	"250 "		/*���������ȷ��*/
#define	FUN_FTP_TAG_RESUME_OK	"350 "		/*����������resume*/
#define	FUN_FTP_TAG_SIZE_OK	"213 "		/*����������size*/
#define	FUN_FTP_TAG_PASV_OK	"227 "		/*����������pasv*/
#define	FUN_FTP_TAG_TRANS_OK	"226 "		/*�ļ��ɹ�����*/
#define	FUN_FTP_TAG_PASS_ERR	"530 "		/*��½�û���/�������*/

#define	FUN_FTP_TAG_HEAD_OK	'2'		/*����ִ�гɹ����ر��*/
#define	FUN_FTP_TAG_HEAD_ERR	'5'		/*����ִ��ʧ�ܷ��ر��*/
#define	FUN_FTP_TAG_HEAD_RESUME	'3'		/*����ȴ����ݷ��ر��*/
/******************************************
*	�Ժ���������
*/


/******************************************
*	��ʼ�������ļ���������������Ϣ
*	������������������
*/
extern int init_environment
(
	char *,					/*�����ļ���*/
	int ,					/*�û�����������*/
	int ,					/*�����������*/
	char *,					/*�����ļ���*/
	char *,					/*�û���*/
	char *,					/*�û�����*/
	int *,					/*socket���*/
	struct STRU_FILE_SYSTEM_INFO *,		/*�ļ���Ϣ*/
	struct STRU_SERVER_INFO *,		/*��������Ϣ*/
	struct STRU_SUPPORT_ARGUMENTS *,	/*������֧����Ϣ*/
	char *					/*�������*/
);
/******************************************
*	��ȡ������һ�з�����Ϣ
*/
extern int get_info_msg
(
	int ,		/*socket���*/
	char *,		/*������Ϣ*/
	char *		/*�������*/
);
/******************************************
*	��ȡ������RFC������һ�鷵����Ϣ
*/
extern int get_blk_msg
(
	int ,		/*socket���*/
	char *		/*�������*/
);
/******************************************
*	����pasv��������Ϣ�����õ�
*	������·�˿ںš�
*/
extern int Retr_PASV_Info
(
	char *,		/*ǰһ���������ص�PASV��Ϣ*/
	char *,		/*��PASV����ȡ��IP��Ϣ*/
	unsigned int  *,/*��PASV����ȡ��PORT��Ϣ*/
	char *		/*�������*/
);
/******************************************
*	�����ļ���С�Ƚ������ļ�
*/
extern int diff_tow_file
(
	char *,		/*�����ļ���*/
	unsigned long ,	/*Զ���ļ���С*/
	char *		/*�������*/
);

/*-----------------------------------------------------------------------*/

/******************************************
*	�õ���������ָ��·���µ��ļ��б�
*/
extern int get_file_list
(
	int  ,					/*socket���*/
	struct STRU_SUPPORT_ARGUMENTS *,	/*������֧�ֵ�����*/
	char *,					/*��������IP��ַ*/
	char *,					/*Զ��Ŀ¼��*/
	char *,					/*���ش��Ŀ¼���ݵ��ļ�*/
	char *					/*�������*/
);
/******************************************
*	����Զ�̷��������ص��ļ��б�
*	�õ��ļ�����Ϣ
*/
extern int get_retr_file_info
(
	char *,				/*��Ŀ¼�б���ȡ��һ���ļ���Ϣ*/
	struct STRU_FILE_SYSTEM_INFO ,	/*�ļ�ϵͳ��Ϣ*/
	char *,				/*Դ�ļ�*/
	char *,				/*��ʱ�ļ�*/
	char *,				/*Ŀ���ļ�*/
	char *				/*�������*/
);
/******************************************
*	ȷ��ָ�����ļ��Ƿ���������ļ�
*/
extern int Identify_file_name
(
	char *,		/*��Ҫȷ�ϵ��ļ���*/
	char *,		/*������־*/
	char *		/*��������־*/
);
/******************************************
*	�õ�Զ�̷�������ָ���ļ��Ĵ�С
*/
extern int get_file_size
(
	int ,		/*socket���*/
	int ,		/*�������Ƿ�֧��SIZE����*/
	char *,		/*��Ŀ¼�б���ȡ��һ���ļ���Ϣ*/
	char *,		/*Ҫ��ȡ��С���ļ���*/
	unsigned long *,/*�ļ���С*/
	char *		/*�������*/
);
/******************************************
*	��ȡһ��ָ�����ļ�
*/
extern int get_bin_file
(
	int ,					/*socket���*/
	struct STRU_SUPPORT_ARGUMENTS * ,	/*������֧�ֵ�����*/
	char *,					/*��������IP��ַ*/
	char *,					/*��Ŀ¼�б���ȡ��һ���ļ���Ϣ*/
	char *,					/*Ҫ��ȡ���ݵ��ļ���*/
	char *,					/*Ŀ���ļ�*/
	char *,					/*��ʱ�ļ�*/
	unsigned long *,			/*�ļ���С*/
	char *					/*�������*/
);
/******************************************
*	ɾ��һ��ָ�����ļ�
*/
extern int delete_file
(
	int ,		/*socket���*/
	char *,		/*Զ�������ϴ�ɾ�����ļ���*/
	char *		/*�������*/
);
/******************************************
*	�ϴ�һ��ָ�����ļ�
*/
extern int put_bin_file
(
	int ,					/*socket���*/
	struct STRU_SUPPORT_ARGUMENTS *,	/*������֧�ֵ�����*/
	char *,					/*��������IP��ַ*/
	char *,					/*Դ�ļ�*/
	char *,					/*Ŀ���ļ�*/
	char *,					/*Ŀ����ʱ�ļ�*/
	unsigned long ,				/*�ļ���С*/
	char *					/*�������*/
);
/******************************************
*	�õ����е��ȱ�־
*/
extern int get_running_flag
(
	char *,				/*�����ļ���*/
	char *				/*�������*/
);
/******************************************
*	�ƶ�Զ�������ϵ��ļ�
*/
extern int move_remote_file
(
	int ,	/*socket���*/
	char *,		/*Դ�ļ���*/
	char *,		/*Ŀ���ļ���*/
	char *		/*�������*/
);
