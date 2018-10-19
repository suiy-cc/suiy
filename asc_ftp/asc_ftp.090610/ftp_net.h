/*********************************************************************
*
*	Copyright (C), 1995-2003, Si-Tech Information Technology Ltd.
*
**********************************************************************
*
*	File_name:	ftp_net.h
*	Author:		����ΰ
*	Version:	�汾��V1.00�� 
*	Date:		2003/11/15
*	Description:
*		���ļ���������ftp_net.c���õ��ĺ궨�塣
*		����궨��ĺ��壬�μ��궨����ע�͡�
*
**********************************************************************/

#define NET_TIME_OUT	900

/*************************************************
*	�Դ������͵Ķ���
*/
#define NET_ERR_IP_TRAN		"EcaintSY1"	/*���ַ���IPת������������IPʱ����*/
#define NET_ERR_SOCKET		"EcaintSY2"	/*����socketʱ����*/
#define NET_ERR_CONN		"EcaintSY3"	/*����socket����ʱ����*/
#define NET_ERR_SELECT		"EcaintSY4"	/*SELECTʱ����*/
#define NET_ERR_TIMEOUT		"EcaintSY5"	/*socket��ʱ����*/
#define NET_ERR_SEND		"EcaintSY6"	/*SEND��������*/
#define NET_ERR_READ		"EcaintSY7"	/*READ��������*/
#define NET_ERR_WRITE		"EcaintSY8"	/*WRITE��������*/
#define NET_ERR_BIND		"EcaintSY9"	/*bind�˿�ʱ����*/
#define NET_ERR_GET_SOCK_NAME	"EcaintSYa"	/*������socket��Ϣʱ����*/

/*************************************************
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

/*************************************************
*	����һ��socket����
*/
extern int make_connection
(
	int *,			/*socket���*/
	char *,			/*�ַ���IP*/
	unsigned int,		/*�˿ں�*/
	char *			/*�������*/
);
/*************************************************
*	����һ��ָ��
*/
extern int send_command
(
	int,			/*socket���*/
	char *,			/*�����ַ���*/
	char *			/*�������*/
);
/*************************************************
*	���һ���ַ���Ϣ
*/
extern int get_line_msg
(
	int,			/*socket���*/
	char *,			/*��õ���Ϣ*/
	char *			/*�������*/
);
/*************************************************
*	��socket�л��ָ����С������
*/
extern int get_data_with_proper_size
(
	int ,		/*socket���*/
	unsigned long ,	/*ָ����Ҫ���ص����ݳ���*/
	char *,		/*���ص�����*/
	unsigned long *,/*ʵ�ʷ��ص����ݳ���*/
	char *		/*�������*/
);
/*************************************************
*	��socket��д��ָ����С������
*/
extern int put_data_with_proper_size
(
	int ,		/*socket���*/
	long ,		/*ָ����Ҫд������ݳ���*/
	char *,		/*д�������*/
	char *		/*�������*/
);
/*************************************************
*	���ɷ���ˣ����ṩ����socket��
*/
extern int make_socket_server
(
	int *,		/*����socket*/
	unsigned int *,	/*���������Ŀ��ж˿ں�*/
	char *		/*�������*/
);
