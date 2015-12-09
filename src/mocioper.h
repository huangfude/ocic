#ifndef __MOCIOPER_H__
#define __MOCIOPER_H__

#include <m_define.h>
#include <m_string.h>
#include <m_memory.h>
#include <m_printmsg.h>
#include <m_link.h>

#include <mocidef.h>

__BEGIN_DECLS


/*************************************环境设置************************************************/
BOOL	OCI_InitEnv(int mode);
void	OCI_EndEnv(void);


/*************************************连接句柄************************************************/

OCI_Conn	OCI_NewConn(string usr,string pwd,string sid,OCI_Err err);
void	OCI_FreeConn(OCI_Conn hd,...);

/*************************************事务操作************************************************/

BOOL	OCI_Commit(OCI_Conn hd,OCI_Err err);
BOOL	OCI_Rollback(OCI_Conn hd,OCI_Err err);

/*************************************光标句柄************************************************/

OCI_Stmt	OCI_NewStmt(OCI_Conn hd,OCI_Err err);
void	OCI_FreeStmt(OCI_Stmt cs,...);

BOOL	OCI_StmtParse(OCI_Stmt cs,string sql,OCI_Err err);

void	OCI_StmtSetOpt(OCI_Stmt cs,int mode,BOOL set);
ushort	OCI_StmtGetStmtType(OCI_Stmt cs);
size_t	OCI_StmtGetAffectRows(OCI_Stmt cs,OCI_Err err);

BOOL	OCI_ExecNoQuery(OCI_Stmt cs,OCI_Err err);
BOOL	OCI_ExecBatch(OCI_Stmt cs,ushort max,OCI_Err err);


/*************************************绑定句柄************************************************/
OCI_Bind	OCI_NewStcBindByName(OCI_Stmt cs,const string name,ushort type,ushort max,size_t size);
OCI_Bind	OCI_NewDncBindByName(OCI_Stmt cs,const string name,ushort type,ushort max,size_t size);
OCI_Bind	OCI_NewStcBindByPos(OCI_Stmt cs,ushort pos,ushort type,ushort max,size_t size);
OCI_Bind	OCI_NewDncBindByPos(OCI_Stmt cs,ushort pos,ushort type,ushort max,size_t size);

void	OCI_FreeBind(OCI_Bind bd,...);

void	OCI_BindReuse(OCI_Bind bd);
void	OCI_BindAllReuse(OCI_Bind bd,...);

void	OCI_BindErase(OCI_Bind bd);
void	OCI_BindAllErase(OCI_Bind bd,...);
void	OCI_BindTrunc(OCI_Bind bd,size_t len);
void	OCI_BindAllTrunc(size_t len,OCI_Bind bd,...);
/*************************************绑定句柄之间的转换************************************************/
OCI_Bind	BindCopy(OCI_Bind des);/*暂时未实现*/
OCI_Bind	BindReCopy(OCI_Bind des);/*暂时未实现*/

/***********************************绑定句柄适配器操作***************************************/
BOOL	OCI_BindInNext(OCI_Bind bd);
BOOL	OCI_BindInAllNext(OCI_Bind bd,...);

BOOL	OCI_BindOutNext(OCI_Bind bd);
BOOL	OCI_BindOutAllNext(OCI_Bind bd,...);

void	OCI_BindResetIter(OCI_Bind bd);

void	OCI_BindAllResetIter(OCI_Bind bd,...);
/***********************************绑定句柄辅助属性操作***************************************/
ushort	OCI_BindGetDataType(OCI_Bind bd);

short	OCI_BindGetIdct(OCI_Bind bd);
short	OCI_BindGetIdctIdx(OCI_Bind bd,ushort idx);

ushort	OCI_BindGetRecode(OCI_Bind bd);
ushort	OCI_BindGetRecodeIdx(OCI_Bind bd,ushort idx);

ushort	OCI_BindGetCur(OCI_Bind bd);
ushort	OCI_BindGetMax(OCI_Bind bd);

/*************************************绑定句柄的普通数据操作***************************************/

BOOL	OCI_BindDataSetval(OCI_Bind bd,const void *data,size_t size);
BOOL	OCI_BindDataSetvalIdx(OCI_Bind bd,ushort idx,const void *data,size_t size);

OCI_GETSTAT OCI_BindDataGetval(OCI_Bind bd,void *data,size_t *size);
OCI_GETSTAT	OCI_BindDataGetvalIdx(OCI_Bind bd,ushort idx,void *data,size_t *size);

/*************************************绑定句柄的LOB数据类型操作***************************************/

BOOL	OCI_BindLobSetval(OCI_Bind bd,size_t off,const void *data,size_t len);
BOOL	OCI_BindLobSetvalIdx(OCI_Bind bd,ushort idx,size_t off,const void *data,size_t len);

OCI_GETSTAT OCI_BindLobGetval(OCI_Bind bd,size_t off,void *data,size_t *len);
OCI_GETSTAT OCI_BindLobGetvalIdx(OCI_Bind bd,ushort idx,size_t off,void *data,size_t *len);

void	OCI_BindLobResetOff(OCI_Bind bd);
void	OCI_BindLobResetOffIdx(OCI_Bind bd,ushort idx);
void	OCI_BindAllLobResetOffIdx(ushort idx,OCI_Bind bd,...);

BOOL	OCI_BindLobGetLen(OCI_Bind bd,size_t *size);
BOOL	OCI_BindLobGetLenIdx(OCI_Bind bd,ushort idx,size_t *size);

BOOL	OCI_BindLobErase(OCI_Bind bd,size_t off,size_t len);
BOOL	OCI_BindLobEraseIdx(OCI_Bind bd,ushort idx,size_t off,size_t len);

BOOL	OCI_BindLobTrunc(OCI_Bind bd,size_t len);
BOOL	OCI_BindLobTruncIdx(OCI_Bind bd,ushort idx,size_t len);

/*************************************绑定操作************************************************/

BOOL	OCI_BindByName(OCI_Err err,OCI_Bind bd,...);
BOOL	OCI_BindByPos(OCI_Err err,OCI_Bind bd,...);

/*************************************结果集句柄************************************************/
BOOL	OCI_ExecQuery(OCI_Stmt cs,OCI_Res *rs,ub2 max,OCI_Err err);

OCI_GETSTAT	OCI_FetchNext(OCI_Res rs,OCI_Err err);
#ifdef OCI_STMT_SCROLLABLE_READONLY
OCI_GETSTAT	OCI_FetchPrior(OCI_Res rs,OCI_Err err);
OCI_GETSTAT	OCI_FetchCurrent(OCI_Res rs,OCI_Err err);
#endif

void	OCI_FetchCancle(OCI_Res rs);
void	OCI_FreeRes(OCI_Res rs);

ushort	OCI_ResGetCols(OCI_Res rs);
ushort	OCI_ResGetMaxRows(OCI_Res rs);
ushort	OCI_ResGetFetchedRows(OCI_Res rs);

BOOL	OCI_ResNextRow(OCI_Res rs);
BOOL	OCI_ResNextCol(OCI_Res rs);

void	OCI_ResResetColIter(OCI_Res rs);
void	OCI_ResResetRowIter(OCI_Res rs);

string	OCI_ResGetColName(OCI_Res rs);
string	OCI_ResGetColNameIdx(OCI_Res rs,ushort col);

BOOL	OCI_ResGetColPreScale(OCI_Res rs,int *pre,int *scale);
BOOL	OCI_ResGetColPreScaleIdx(OCI_Res rs,ushort col,int *pre,int *scale);

size_t	OCI_ResGetColMaxSize(OCI_Res rs);
size_t	OCI_ResGetColMaxSizeIdx(OCI_Res rs,ushort col);

ushort	OCI_ResGetOutType(OCI_Res rs);
ushort	OCI_ResGetOutTypeIdx(OCI_Res rs,ushort col);

OCI_GETSTAT OCI_ResDataGetval(OCI_Res rs,void *data,size_t *size);
OCI_GETSTAT OCI_ResDataGetvalIdx(OCI_Res rs,ushort row,ushort col,void *data,size_t *size);

OCI_GETSTAT OCI_ResLobGetval(OCI_Res rs,size_t off,void *data,size_t *len);
OCI_GETSTAT OCI_ResLobGetvalIdx(OCI_Res rs,ushort row,ushort col,size_t off,void *data,size_t *len);

BOOL	OCI_ResLobGetLen(OCI_Res rs,size_t *len);
BOOL	OCI_ResLobGetLenIdx(OCI_Res rs,ushort row,ushort col,size_t *len);

/**************************************集合与绑定的转换操作***********************************************/
OCI_Bind	OCI_ResConvertToBindByPos(const OCI_Res rs,ushort col,ushort pos,OCI_Stmt cs);
OCI_Bind	OCI_ResConvertToBindByName(const OCI_Res rs,ushort col,const string name,OCI_Stmt cs);
void		OCI_BindSetCur(OCI_Bind bd,ushort cur);
void		OCI_BindAllSetCur(ushort cur,OCI_Bind bd,...);

BOOL		OCI_ResAssignToBind(const OCI_Res rs,ushort col,OCI_Bind bd);
/*************************************错误句柄操作**********************************************/

OCI_Err	OCI_NewErr();
void	OCI_FreeErr(OCI_Err err);
string	OCI_ErrGetmsg(OCI_Err err);
string	OCI_ErrGetfunc(OCI_Err err);
short	OCI_ErrGetStat(OCI_Err err);

/**************************************日期操作函数*******************************************/
BOOL	OCI_DateToString(const OCIDate *date,string buf,size_t len,const string format,OCI_Err err);
BOOL	OCI_StringToDate(const string buf,OCIDate *date,const string format,OCI_Err err);
BOOL	OCI_DateSetDate(OCIDate *date,short year,char mon,char day);
BOOL	OCI_DateSetTime(OCIDate *date,char hour,char min,char sec);
BOOL	OCI_DateGetDate(const OCIDate *date,short *year,char *mon,char *day);
BOOL	OCI_DateGetTime(const OCIDate *date,char *hour,char *min,char *sec);
BOOL	OCI_DateAddDays(const OCIDate *old_dt,OCIDate *new_dt,int days);
BOOL	OCI_DateAddMons(const OCIDate *old_dt,OCIDate *new_dt,int mons);
BOOL	OCI_DateNextDay(const OCIDate *old_dt,OCIDate *new_dt,const string day);
BOOL	OCI_DateLastDay(const OCIDate *old_dt,OCIDate *new_dt);
BOOL	OCI_DateDaysBetween(const OCIDate *dt1,const OCIDate *dt2,int *days);
BOOL	OCI_DateLocalDate(OCIDate *date);
/**************************************时间戳类型操作函数*******************************************/
OCI_DateTime	OCI_NewDateTime(uint type);
void	OCI_FreeDateTime(OCI_DateTime dt);
BOOL	OCI_DateTimeGetDate(const OCI_DateTime dt,short *year,uchar *mon,uchar *day);
BOOL	OCI_DateTimeGetTime(const OCI_DateTime dt,uchar *hour,uchar *min,uchar *sec,uint *fsec);
BOOL	OCI_DateTimeToString(const OCI_DateTime dt,string buf,size_t len,const string format,OCI_Err err);
BOOL	OCI_StringToDateTime(const string buf,OCI_DateTime dt,const string format,OCI_Err err);
BOOL	OCI_DateTimeConstruct(OCI_DateTime dt,short year,uchar month,uchar day,uchar hour,uchar min,uchar sec,uint fsec,string timezone);
BOOL	OCI_DateTimeLocalTimeStamp(OCI_DateTime dt);
BOOL	OCI_DateTimeGetTimeZone(const OCI_DateTime dt,char *hour,char *min);
BOOL	OCI_DateTimeGetTimeZoneName(const OCI_DateTime dt,string name,size_t len);
/**************************************ORACLE的number类型操作函数*******************************************/
void	OCI_NumberSetZero(OCINumber *num);
BOOL	OCI_NumberAdd(const OCINumber *num1,const OCINumber *num2,OCINumber *num);
BOOL	OCI_NumberSub(const OCINumber *num1,const OCINumber *num2,OCINumber *num);
BOOL	OCI_NumberMul(const OCINumber *num1,const OCINumber *num2,OCINumber *num);
BOOL	OCI_NumberDiv(const OCINumber *num1,const OCINumber *num2,OCINumber *num);
BOOL	OCI_NumberMod(const OCINumber *num1,const OCINumber *num2,OCINumber *num);
BOOL	OCI_NumberIntPower(const OCINumber *num1,int exponent,OCINumber *num);
BOOL	OCI_NumberToString(const OCINumber *num1,string buf,size_t len,const string format,OCI_Err err);
BOOL	OCI_StringToNumber(const string buf,OCINumber *num1,const string format,OCI_Err err);
BOOL	OCI_NumberToInt(const OCINumber *num1,void *result,ushort flag);
BOOL	OCI_IntToNumber(const void *result,OCINumber *num1,ushort flag);
BOOL	OCI_NumberToReal(const OCINumber *num1,void *result,ushort flag);
BOOL	OCI_RealToNumber(const void *result,OCINumber *num1,ushort flag);

__END_DECLS

#endif