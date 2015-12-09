#ifndef __MOCIDEF_H__
#define	__MOCIDEF_H__

#include <oci.h>

#ifdef	__DEBUG_MODE
#define OCI_PRO_DEBUG_MODE	__DEBUG_MODE
#endif

#define	DEF_ERRMSG_LEN		1024
#define	DEF_ERRFUN_LEN		50

#define DEF_COLPARAM_NAMELEN	50

/*当ORACLE为8i以下版本，请定义次宏*/
#define	INIT_ENV_MODE_8I

/*当被定义时使用OCILogon()登录数据库，进行单用户单数据库操作*/
//#define SINGLE_USER
//#ifndef	SINGLE_USER
	//单数据库操作
	#define OCI_SINGLE_ENV	OCI_DEFAULT
	//多数据库操作
	#define OCI_MULTI_ENV	OCI_DEFAULT|OCI_THREADED
//#endif

/*操作句柄重定义*/

/*连接句柄*/
typedef struct _OCIConnHandle	*OCI_Conn;
/*语句句柄*/
typedef struct _OCIStmtHandle	*OCI_Stmt;
/*错误句柄*/
typedef struct _OCIErr		*OCI_Err;
/*绑定句柄*/
typedef struct _OCIBind		*OCI_Bind;
/*结果集句柄*/
typedef struct _OCIReset	*OCI_Res;

typedef struct _OCIDateTime		*OCI_DateTime;
typedef struct _OCIInterval		*OCI_Interval;
typedef struct OCIDate			*OCI_Date;
typedef struct OCINumber		*OCI_Number;

/*数据获取返回状态*/
typedef enum _OCI_GETSTAT{
	OCI_GETSTAT_ERROR=0x7213,//获取出错
	OCI_GETSTAT_SUCCESS,//成功
	OCI_GETSTAT_NONDATA,//没有数据，过程中绑定标量做为输出时，不对其赋值，获取数据时，会返回此状态
	OCI_GETSTAT_NULL//该值为NULL（Oracle的NULL值）
}OCI_GETSTAT;

/*指示器初始化单字节值*/
#define OCI_INIT_IDCT		0xef
/*指示器初始化双字节值，必须保证小于(short)-2*/
#define OCI_INIT_IDCT_VAL	(OCI_INIT_IDCT|(OCI_INIT_IDCT<<8))
/*无效的指示器值*/
#define OCI_IDCT_INVALID	OCI_INIT_IDCT_VAL
/*无效的返回码值*/
#define OCI_RCODE_INVALID	0xffff
/*CLOB类型的一个字符的字节大小，服务器端为GBK字符集时，一个字符站2字节*/
#define OCI_CLOB_SIGBYTE	0x02

/*最大绑定成员数，超过自动截断*/
#define OCI_MAX_BINDS		32000
#define	OCI_MAX_DEFINE		OCI_MAX_BINDS
/*自动绑定时，一次增加的绑定变量成员数*/
#define OCI_DNC_BIND_ADDMIN	100
/*
外部数据类型
常用oracle内外数据类型的相互转换重要说明：
格式：
	[外部数据<===>内部数据]
数值型：
	OCI_INT<===>NUMBER
	OCI_FLT<===>NUMBER
	OCI_STR<===>NUMBER
日期型：
	OCI_DT<===>DATE
	OCI_STR<===>DATE
字符型
	OCI_STR<===>VARCHAR2
	OCI_STR<===>CHAR
普通二进制型：
	OCI_BIN<===>RAW
	OCI_STR<===>RAW(不能超过2000字节)
大数据字符型：
	OCI_LNG<===>LONG
	OCI_STR<===>LONG(不能超过4000字节)
	OCI_CLOB<===>CLOB
	OCI_STR<===>CLOB(不能超过4000字节)
大数据二进制型：
	OCI_LGB<===>LONG RAW
	OCI_BLOB<===>BLOB
	OCI_RAW<===>BLOB(不能超过2000字节)

***注：操作时不能超过列的最大长度***
*/
#define OCI_ERR_TYPE		0
#define OCI_STR				SQLT_STR
#define OCI_INT				SQLT_INT/*本地数据必须是int*/
#define	OCI_FLT				SQLT_FLT/*本地数据必须是float*/
#define OCI_NUM				SQLT_VNU/*本地数据必须是OCINumber*/

/*本地数据必须是c的数据块*/
#define OCI_LNG				SQLT_LNG
#define OCI_LGB				SQLT_LBI/*long binary/long raw*/
#define OCI_BIN				SQLT_BIN/*raw*/
#define	OCI_CLOB			SQLT_CLOB
#define	OCI_BLOB			SQLT_BLOB

/*日期*/
#define	OCI_DATE			SQLT_ODT/*oracle外部日期类型，7个字节，本地数据必须是OCIDate*/
//#define	OCI_TMSTAMP			180
#define OCI_TMSTAMP			SQLT_TIMESTAMP
#define OCI_TMSTAMP_LTZ		SQLT_TIMESTAMP_LTZ
#define OCI_TMSTAMP_TZ		SQLT_TIMESTAMP_TZ


/*分配外部数据时默认的字节大小*/
#define OCI_TMSTAMPSIZE		40
#define	OCI_STRSIZE			4000//字符串大小
#define	OCI_BINSIZE			2000//二进制数据大小
#define	OCI_LNGSIZE			4000//大数据二进制数据大小

/*语句类型*/
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


/*自定义错误*/
#define OCI_ERR_ENV_MGRHD	-199/*环境句柄中的连接管理错误*/
#define	OCI_ERR_HDL_MGRCS	-200/*连接句柄中的光标管理错误*/
#define	OCI_ERR_CS_MGRBD	-201/*光标句柄中的绑定管理错误*/
#define OCI_ERR_ENV_CREAT	-202/*环境初始化失败*/
#define	OCI_ERR_CS_ERRSTMT	-203/*错误的语句类型*/
#define	OCI_ERR_BD_ERRTYPE	-204/*非法的类型绑定*/
#define	OCI_ERR_DATA_DTTRUC	-205/*数据有截断警告*/
#define OCI_ERR_ATTR_GET	-206/*获取重要属性出错*/
#define OCI_ERR_HDL_ALLOC	-207/*分配重要句柄出错*/
#define OCI_ERR_ENV_END		-208/*环境初始化失败*/



/*assert测试*/
#define OCI_ERR_ASSERT		-1989

/*自定义警告*/
#define OCI_WAR_WR_ZERO		-1990/*写入零数据*/
#define OCI_WAR_RD_ZERO		-1991/*读取零数据*/
#define OCI_WAR_DATA_INVALID	-1992/*当前位置指针无效*/
#define OCI_WAR_DATA_NODATA		-1993/*当前位置没有数据*/
#define OCI_WAR_POS_INVALID		-1994/*无效的位置索引*/
#define OCI_ERR_DATE_INVALID	-1995/*无效的日期数据*/
#define OCI_MSG_NORMAL			-1996/*一般消息*/

/*执行选项*/
#define OCI_EXEC_BATCHERR	OCI_BATCH_ERRORS/*设置后，批量执行，遇错时不中断*/
#define OCI_EXEC_AUTOCOMM	OCI_COMMIT_ON_SUCCESS/*设置后，执行成功后自动提交*/
#ifdef OCI_STMT_SCROLLABLE_READONLY
#define OCI_EXEC_SCROLL		OCI_STMT_SCROLLABLE_READONLY/*滚动光标 SELECT时有效？*/
#endif

/*OCI_DateToString()和OCI_StringToDate()日期转换函数常量*/
#define OCI_DATEFULL_FORMAT		STRING "YYYY/MM/DD HH24:MI:SS"
#define OCI_DATE_FORMAT		STRING "YYYY/MM/DD"
#define OCI_TIME_FORMAT		STRING "HH24:MI:SS"
#define OCI_TIMESTAMP_FORMAT	STRING "YYYY/MM/DD HH24:MI:SS.FF9"
#define OCI_TIMEZONE_DEF	STRING "+8:00"
/*OCI_DateNextDay()常量*/
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

/*数字转换函数常量*/
#define OCI_NUMBER_FORMAT	STRING "99999999.9999"
//#define OCI_NUMBER_UNSIGNED
//#define OCI_NUMBER_SIGNED
#define	OCI_NUMBER_FLOAT		0xf001
#define	OCI_NUMBER_DOUBLE		0xf002
#define	OCI_NUMBER_LDOUBLE		0xf003

#endif