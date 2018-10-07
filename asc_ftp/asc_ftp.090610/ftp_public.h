/*********************************************************************
*
*	Copyright (C), 1995-2003, Si-Tech Information Technology Ltd.
*
**********************************************************************
*
*	File_name:	ftp_public.h
*	Author:		����ΰ
*	Version:	�汾��V1.00�� 
*	Date:		2003/11/15
*	Description:
*		���ļ�����������ftp_public.c���õ��ĺ궨�塣
*
**********************************************************************/

#define PUB_DIR_LEN_MAX		1000		/*�ļ�����󳤶ȣ���Ŀ¼��*/
#define PUB_PREFIX_LEN_MAX	100		/*���ǰ׺�ַ���󳤶�*/
#define PUB_POSTFIX_LEN_MAX	100		/*��Ӻ�׺�ַ���󳤶�*/
#define PUB_TRANS_DIR_LEN_MAX	10		/*�������䷽����ִ���󳤶�*/
#define PUB_LENGTH_OF_LINE_MAX	1000		/*�ı��ļ�һ�����ַ������Ŀ*/
#define PUB_IP_ADD_LEN		16		/*IP��ַ�ִ���󳤶�*/
#define PUB_CONF_YES_STR	"YES"
#define PUB_CONF_NO_STR		"NO"
#define PUB_CONF_AUTO_STR	"AUTO"
#define PUB_CONF_YES		1		/*"YES"�ڳ����еı�ʶ*/
#define PUB_CONF_NO		2		/*"NO"�ڳ����еı�ʶ*/
#define PUB_CONF_AUTO		3		/*"AUTO"�ڳ����еı�ʶ*/
#define PUB_CONF_UP		"UP"		/*"UP"�ڳ����еı�ʶ*/
#define PUB_CONF_DOWN		"DOWN"		/*"DOWN"�ڳ����еı�ʶ*/
#define PUB_CONF_RUN		"RUN"		/*"RUN"�ڳ����еı�ʶ*/
#define PUB_CONF_STOP		"STOP"		/*"STOP"�ڳ����еı�ʶ*/
/******************************************
*	�������ļ������ͼ�¼���Ķ���
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
#define PUB_SEG_FILE_PARAM_SRCDIR	"SRC_DIRECTORY"		/*�������λ��������ͬ����*/
#define PUB_SEG_FILE_PARAM_DSTDIR	"DST_DIRECTORY"		/*�������λ��������ͬ����*/
#define PUB_SEG_FILE_PARAM_LOGDIR	"LOG_DIRECTORY"		/*�������λ��������ͬ����*/
#define PUB_SEG_FILE_PARAM_TMPDIR	"TMP_DIRECTORY"		/*�������λ��������ͬ����*/
#define PUB_SEG_FILE_PARAM_BAKDIR	"BAK_DIRECTORY"		/*�������λ��������ͬ����*/
#define PUB_SEG_FILE_PARAM_LOGTMPDIR	"LOG_TMP_DIRECTORY"	/*�������λ��������ͬ����*/
#define PUB_SEG_FILE_PARAM_NAME_INC	"NAME_INCLUDE"		/*�������λ��������ͬ����*/
#define PUB_SEG_FILE_PARAM_NAME_EXC	"NAME_EXCLUDE"		/*�������λ��������ͬ����*/
#define PUB_SEG_FILE_PARAM_PREFIX	"PREFIX_CODES"		/*�������λ��������ͬ����*/
#define PUB_SEG_FILE_PARAM_POSTFIX	"POSTFIX_CODES"		/*�������λ��������ͬ����*/
#define PUB_SEG_FILE_PARAM_TRANS_DIR	"UP_DOWN_FLAG"		/*�������λ��������ͬ����*/
#define PUB_SEG_FILE_PARAM_BACK_FLAG	"DOWNLOAD_BAK_FLAG"	/*�������λ��������ͬ����*/

#define PUB_SEG_TERMINAL			"TERMINAL_INFO"
#define PUB_SEG_TERMINAL_PARAM_FLAG		"RUNNING_STATUS"
/******************************************
*	�Դ������͵Ķ���
*/
#define PUB_ERR_FOPEN		"EcaipbSY1"	/*���ļ�����*/
#define PUB_ERR_MALLOC		"EcaipbSY2"	/*�ڴ�������*/
#define PUB_ERR_SEG_FORMAT	"EcaipbSY3"	/*�������ô���*/
#define PUB_ERR_PARAM_FORMAT	"EcaipbSY4"	/*�������ô���*/
#define PUB_ERR_PWDFILE		"EcaipbSY5"	/*�����ļ����ô���*/
#define PUB_ERR_PWDFILE_IO	"EcaipbSY6"	/*�������ļ�IO����*/

/******************************************
*	�Ե��Կ��صĶ���

*/
#define DEBUG_PUB_INIT_CONFIG_INFO
#define DEBUG_PUB_GET_CONFIG_VAL
#define DEBUG_PUB_GET_PWD_USER

/*************************************************************************************
*	�Խṹ�Ķ���
*/
struct STRU_FILE_SYSTEM_INFO
{
	char src_directory[PUB_DIR_LEN_MAX];			/*ԴĿ¼*/
	char dst_directory[PUB_DIR_LEN_MAX];			/*Ŀ��Ŀ¼*/
	char log_directory[PUB_DIR_LEN_MAX];			/*��־Ŀ¼*/
	char tmp_directory[PUB_DIR_LEN_MAX];			/*��ʱĿ¼*/
	char bak_directory[PUB_DIR_LEN_MAX];			/*����Ŀ¼*/
	char log_tmp_directory[PUB_DIR_LEN_MAX];		/*��־��ʱĿ¼*/
	char name_include[PUB_DIR_LEN_MAX];			/*�ļ�����Ҫ�����ı�ʶ�ִ�*/
	char name_exclude[PUB_DIR_LEN_MAX];			/*�ļ�����Ҫ���벻�����ı�ʶ�ִ�*/
	char file_prefix[PUB_PREFIX_LEN_MAX];			/*��ӵ��ļ�ǰ׺*/
	char file_postfix[PUB_POSTFIX_LEN_MAX];			/*��ӵ��ļ���׺*/
	char trans_file_direction[PUB_TRANS_DIR_LEN_MAX];	/*���ͷ���*/
	char remote_backup_flag[PUB_TRANS_DIR_LEN_MAX];		/*Զ�������Ƿ񱸷ݱ�־*/
};
struct STRU_SERVER_INFO
{
	char host_ip[PUB_IP_ADD_LEN];			/*Զ������IP��ַ*/
	char local_host_ip[PUB_IP_ADD_LEN];		/*��������IP��ַ*/
	unsigned int host_port;				/*�����˿ں�*/
	unsigned int file_sys_group_count;		/*�ļ����ò��ֹ��ж�����*/
	unsigned int resume_support;			/*�ϵ�������־*/
	unsigned int size_support;			/*size֧�ֱ�־*/
	unsigned int pasv_support;			/*����ģʽ֧�ֱ�־*/
	unsigned int split_parts_count;			/*��ֵĿ���*/
};

/*************************************************************************************
*	�Ժ���������
*/

/******************************************
*	��������ʱ������Ϣ
*/
extern void deal_run_err_msg
(
	char *,		/*���󼶱�*/
	char *,		/*����Դ�ļ�*/
	int ,		/*������*/
	char *,		/*�������*/
	char *,		/*���Ӵ�����Ϣ*/
	char *		/*���ش�����Ϣ*/
);

/******************************************
*	���ݶκ����ñ������������ļ���
*	���������Ϣ
*/
extern int get_config_val
(
	FILE *,		/*�����ļ����*/
	char *,		/*�����ļ��еĶ���*/
	char *,		/*�����ļ��е����ñ�����*/
	char *,		/*���ص����ñ�����ֵ*/
	char *		/*�������*/
);

/******************************************
*	��ʼ�������ļ�
*/
extern int init_config_info
(
	char *,				/*�����ļ���*/
	struct STRU_FILE_SYSTEM_INFO *,	
	struct STRU_SERVER_INFO *,
	char *				/*�������*/
);
/******************************************
*	ȷ���Ƿ�����Ҫ���ļ�
*/
extern int File_is_ok
(
	char *,				/*�ļ���,����Ŀ¼��*/
	char *,				/*Ҫ���ļ����б���������ִ�*/
	char *				/*Ҫ���ļ����в��ܰ������ִ�*/
);
/******************************************
*	�õ��û����Ϳ���
*/
extern int get_pwd_info
(
	char *,				/*�����ļ���*/
	char *,				/*������Կ*/
	char *,				/*���ܺ�õ����û���*/
	int,				/*�����������¼���ܺ��û������ִ�����*/
	char *,				/*���ܺ�õ����û�����*/
	int,				/*�����������¼���ܺ��û�������ִ�����*/
	char *				/*�������*/
);
/******************************************
*	�����û����Ϳ������ɿ����ļ�
*/
extern int gen_pwd_file
(
	char *,				/*�����ļ���*/
	char *,				/*������Կ*/
	char *,				/*���ܵ��û���*/
	char *,				/*���ܵ��û�����*/
	char *				/*�������*/
);
