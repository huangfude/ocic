#ifndef __MOCIDEF_H__
#define	__MOCIDEF_H__

#include <oci.h>

#ifdef	__DEBUG_MODE
#define OCI_PRO_DEBUG_MODE	__DEBUG_MODE
#endif

#define	DEF_ERRMSG_LEN		1024
#define	DEF_ERRFUN_LEN		50

#define DEF_COLPARAM_NAMELEN	50

/*��ORACLEΪ8i���°汾���붨��κ�*/
#define	INIT_ENV_MODE_8I

/*��������ʱʹ��OCILogon()��¼���ݿ⣬���е��û������ݿ����*/
//#define SINGLE_USER
//#ifndef	SINGLE_USER
	//�����ݿ����
	#define OCI_SINGLE_ENV	OCI_DEFAULT
	//�����ݿ����
	#define OCI_MULTI_ENV	OCI_DEFAULT|OCI_THREADED
//#endif

/*��������ض���*/

/*���Ӿ��*/
typedef struct _OCIConnHandle	*OCI_Conn;
/*�����*/
typedef struct _OCIStmtHandle	*OCI_Stmt;
/*������*/
typedef struct _OCIErr		*OCI_Err;
/*�󶨾��*/
typedef struct _OCIBind		*OCI_Bind;
/*��������*/
typedef struct _OCIReset	*OCI_Res;

typedef struct _OCIDateTime		*OCI_DateTime;
typedef struct _OCIInterval		*OCI_Interval;
typedef struct OCIDate			*OCI_Date;
typedef struct OCINumber		*OCI_Number;

/*���ݻ�ȡ����״̬*/
typedef enum _OCI_GETSTAT{
	OCI_GETSTAT_ERROR=0x7213,//��ȡ����
	OCI_GETSTAT_SUCCESS,//�ɹ�
	OCI_GETSTAT_NONDATA,//û�����ݣ������а󶨱�����Ϊ���ʱ�������丳ֵ����ȡ����ʱ���᷵�ش�״̬
	OCI_GETSTAT_NULL//��ֵΪNULL��Oracle��NULLֵ��
}OCI_GETSTAT;

/*ָʾ����ʼ�����ֽ�ֵ*/
#define OCI_INIT_IDCT		0xef
/*ָʾ����ʼ��˫�ֽ�ֵ�����뱣֤С��(short)-2*/
#define OCI_INIT_IDCT_VAL	(OCI_INIT_IDCT|(OCI_INIT_IDCT<<8))
/*��Ч��ָʾ��ֵ*/
#define OCI_IDCT_INVALID	OCI_INIT_IDCT_VAL
/*��Ч�ķ�����ֵ*/
#define OCI_RCODE_INVALID	0xffff
/*CLOB���͵�һ���ַ����ֽڴ�С����������ΪGBK�ַ���ʱ��һ���ַ�վ2�ֽ�*/
#define OCI_CLOB_SIGBYTE	0x02

/*���󶨳�Ա���������Զ��ض�*/
#define OCI_MAX_BINDS		32000
#define	OCI_MAX_DEFINE		OCI_MAX_BINDS
/*�Զ���ʱ��һ�����ӵİ󶨱�����Ա��*/
#define OCI_DNC_BIND_ADDMIN	100
/*
�ⲿ��������
����oracle�����������͵��໥ת����Ҫ˵����
��ʽ��
	[�ⲿ����<===>�ڲ�����]
��ֵ�ͣ�
	OCI_INT<===>NUMBER
	OCI_FLT<===>NUMBER
	OCI_STR<===>NUMBER
�����ͣ�
	OCI_DT<===>DATE
	OCI_STR<===>DATE
�ַ���
	OCI_STR<===>VARCHAR2
	OCI_STR<===>CHAR
��ͨ�������ͣ�
	OCI_BIN<===>RAW
	OCI_STR<===>RAW(���ܳ���2000�ֽ�)
�������ַ��ͣ�
	OCI_LNG<===>LONG
	OCI_STR<===>LONG(���ܳ���4000�ֽ�)
	OCI_CLOB<===>CLOB
	OCI_STR<===>CLOB(���ܳ���4000�ֽ�)
�����ݶ������ͣ�
	OCI_LGB<===>LONG RAW
	OCI_BLOB<===>BLOB
	OCI_RAW<===>BLOB(���ܳ���2000�ֽ�)

***ע������ʱ���ܳ����е���󳤶�***
*/
#define OCI_ERR_TYPE		0
#define OCI_STR				SQLT_STR
#define OCI_INT				SQLT_INT/*�������ݱ�����int*/
#define	OCI_FLT				SQLT_FLT/*�������ݱ�����float*/
#define OCI_NUM				SQLT_VNU/*�������ݱ�����OCINumber*/

/*�������ݱ�����c�����ݿ�*/
#define OCI_LNG				SQLT_LNG
#define OCI_LGB				SQLT_LBI/*long binary/long raw*/
#define OCI_BIN				SQLT_BIN/*raw*/
#define	OCI_CLOB			SQLT_CLOB
#define	OCI_BLOB			SQLT_BLOB

/*����*/
#define	OCI_DATE			SQLT_ODT/*oracle�ⲿ�������ͣ�7���ֽڣ��������ݱ�����OCIDate*/
//#define	OCI_TMSTAMP			180
#define OCI_TMSTAMP			SQLT_TIMESTAMP
#define OCI_TMSTAMP_LTZ		SQLT_TIMESTAMP_LTZ
#define OCI_TMSTAMP_TZ		SQLT_TIMESTAMP_TZ


/*�����ⲿ����ʱĬ�ϵ��ֽڴ�С*/
#define OCI_TMSTAMPSIZE		40
#define	OCI_STRSIZE			4000//�ַ�����С
#define	OCI_BINSIZE			2000//���������ݴ�С
#define	OCI_LNGSIZE			4000//�����ݶ��������ݴ�С

/*�������*/
#ifndef OCI_STMT_UNKNOWN
#define OCI_STMT_UNKNOWN	0
#endif;
#define  OCI_UNKNOWN	OCI_STMT_UNKNOWN/* Unknown statement */
#define  OCI_SELECT		OCI_STMT_SELECT/* select statement */
#define  OCI_UPDATE		OCI_STMT_UPDATE/* update statement */
#define  OCI_DELETE		OCI_STMT_DELETE/* delete statement */
#define  OCI_INSERT		OCI_STMT_INSERT/* Insert Statement */
#define  OCI_CREATE		OCI_STMT_CREATE/* create statement */
#define  OCI_DROP		OCI_STMT_DROP/* drop statement */
#define  OCI_ALTER		OCI_STMT_ALTER/* alter statement */
#define  OCI_BEGIN		OCI_STMT_BEGIN/* begin ... (pl/sql statement)*/
#define  OCI_DECLARE	OCI_STMT_DECLARE/* declare .. (pl/sql statement ) */
#define  OCI_CALL		OCI_STMT_CALL/* corresponds to kpu call */


/*�Զ������*/
#define OCI_ERR_ENV_MGRHD	-199/*��������е����ӹ������*/
#define	OCI_ERR_HDL_MGRCS	-200/*���Ӿ���еĹ��������*/
#define	OCI_ERR_CS_MGRBD	-201/*������еİ󶨹������*/
#define OCI_ERR_ENV_CREAT	-202/*������ʼ��ʧ��*/
#define	OCI_ERR_CS_ERRSTMT	-203/*������������*/
#define	OCI_ERR_BD_ERRTYPE	-204/*�Ƿ������Ͱ�*/
#define	OCI_ERR_DATA_DTTRUC	-205/*�����нضϾ���*/
#define OCI_ERR_ATTR_GET	-206/*��ȡ��Ҫ���Գ���*/
#define OCI_ERR_HDL_ALLOC	-207/*������Ҫ�������*/
#define OCI_ERR_ENV_END		-208/*������ʼ��ʧ��*/



/*assert����*/
#define OCI_ERR_ASSERT		-1989

/*�Զ��徯��*/
#define OCI_WAR_WR_ZERO		-1990/*д��������*/
#define OCI_WAR_RD_ZERO		-1991/*��ȡ������*/
#define OCI_WAR_DATA_INVALID	-1992/*��ǰλ��ָ����Ч*/
#define OCI_WAR_DATA_NODATA		-1993/*��ǰλ��û������*/
#define OCI_WAR_POS_INVALID		-1994/*��Ч��λ������*/
#define OCI_ERR_DATE_INVALID	-1995/*��Ч����������*/
#define OCI_MSG_NORMAL			-1996/*һ����Ϣ*/

/*ִ��ѡ��*/
#define OCI_EXEC_BATCHERR	OCI_BATCH_ERRORS/*���ú�����ִ�У�����ʱ���ж�*/
#define OCI_EXEC_AUTOCOMM	OCI_COMMIT_ON_SUCCESS/*���ú�ִ�гɹ����Զ��ύ*/
#ifdef OCI_STMT_SCROLLABLE_READONLY
#define OCI_EXEC_SCROLL		OCI_STMT_SCROLLABLE_READONLY/*������� SELECTʱ��Ч��*/
#endif

/*OCI_DateToString()��OCI_StringToDate()����ת����������*/
#define OCI_DATEFULL_FORMAT		STRING "YYYY/MM/DD HH24:MI:SS"
#define OCI_DATE_FORMAT		STRING "YYYY/MM/DD"
#define OCI_TIME_FORMAT		STRING "HH24:MI:SS"
#define OCI_TIMESTAMP_FORMAT	STRING "YYYY/MM/DD HH24:MI:SS.FF9"
#define OCI_TIMEZONE_DEF	STRING "+8:00"
/*OCI_DateNextDay()����*/
#define OCI_NEXTDAY_MON		STRING "MONDAY"
#define OCI_NEXTDAY_TUE		STRING "TUESDAY"
#define	OCI_NEXTDAY_WED		STRING "WEDNESDAY"
#define	OCI_NEXTDAY_THU		STRING "THURSDAY"
#define	OCI_NEXTDAY_FRI		STRING "FRIDAY"
#define	OCI_NEXTDAY_SAT		STRING "SATURDAY"
#define	OCI_NEXTDAY_SUN		STRING "SUNDAY"
//
#define OCI_DATETIME_TS			OCI_DTYPE_TIMESTAMP
#define OCI_DATETIME_TS_TZ		OCI_DTYPE_TIMESTAMP_TZ
#define OCI_DATETIME_TS_LTZ		OCI_DTYPE_TIMESTAMP_LTZ

/*����ת����������*/
#define OCI_NUMBER_FORMAT	STRING "99999999.9999"
//#define OCI_NUMBER_UNSIGNED
//#define OCI_NUMBER_SIGNED
#define	OCI_NUMBER_FLOAT		0xf001
#define	OCI_NUMBER_DOUBLE		0xf002
#define	OCI_NUMBER_LDOUBLE		0xf003

#endif