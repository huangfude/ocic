#include <mocioper.h>

/*状态指示器*/
/*已连接*/
#define OCI_STAT_CONN	0x0001
/*已开始会话*/
#define	OCI_STAT_START	0x0002
/*NLS_DATE_LANGUAGE格式*/
#define OCI_DATE_LANGUAGE	STRING "AMERICAN"


/*判断指示器是否可以判断值有效*/
//#if OCI_INIT_IDCT_VAL > -3&0x0ffff
//#error "Use OCI_INIT_IDCT can't guarantee the initialization value of indictor less than -2"
//#endif

/*断言错误*/
#define		assert_error(msg,func,expr) \
	if(!(expr)){\
		check_err(OCI_ERR_ASSERT,NULL,(msg),(func));\
		assert((expr));\
	}

#define	OCI_ERR_BIND_OPER	0
#define	OCI_ERR_BIND_TYPE	OCI_ERR_BIND_OPER

/*绑定数据类型分类*/
#define		OCI_ISNORMAL_TYPE(type)		\
	((type)==OCI_INT || (type)==OCI_FLT || (type)==OCI_NUM || \
	(type)==OCI_STR || (type)==OCI_LNG || (type)==OCI_LGB || \
	(type)==OCI_BIN || (type)==OCI_DATE)

#define		OCI_ISLOB_TYPE(type)		\
	((type)==OCI_BLOB || (type)==OCI_CLOB)

#define		OCI_ISDT_TYPE(type)			\
	((type)==OCI_TMSTAMP || (type)==OCI_TMSTAMP_LTZ || (type)==OCI_TMSTAMP_TZ)

/*************************************LOB变量*************************************/
typedef struct	_OCILob			*OCI_Lob;
typedef OCILobLocator			*OCI_LobLocator;
/*************************************普通数据变量*********************************/
typedef struct	_OCIData		*OCI_Data;
/*************************************日期数据变量*********************************/
#define OCI_DATE_BYTES			sizeof(OCIDate)
/*************************************时间戳数据变量*******************************/
#define OCI_TMSTAMP_BYTES		11
typedef struct	_OCITmStamp		*OCI_TmStamp;
/*************************************日期时间变量*******************************/
typedef struct _OCIDateTime{
	uint type;
	OCIDateTime *dt;
}_OCIDateTime;
/*************************************时间间隔变量*******************************/
typedef struct _OCIInterval{
	uint type;
	OCIInterval *dt;
}_OCIInterval;
/*************************************数值数据变量*******************************/
#define OCI_NUMBER_BYTES		sizeof(OCINumber)
/*************************************绑定操作类型*********************************/
typedef enum	_OCI_BIND_TYPE	OCI_BIND_TYPE;
/**************************************列属性*************************************/
typedef struct	_OCIColParam	*OCI_ColParam;

typedef enum	_OCI_EXEC_TYPE	OCI_EXEC_TYPE;

typedef enum	_OCI_FREE_TYPE	OCI_FREE_TYPE;

typedef enum	_OCI_ADT_TYPE	OCI_ADT_TYPE;


/******************************释放管理链表释放方式******************************/
typedef enum _OCI_FREE_TYPE{
	OCI_FREET_FATHER=0x1234,
	OCI_FREET_CHILD
}_OCI_FREE_TYPE;

/*******************************执行类型******************************/
typedef enum _OCI_EXEC_TYPE{
	OCI_EXEC_TFETCH=0x4321,
	OCI_EXEC_TPLSQL,
	OCI_EXEC_TBATCH,
	OCI_EXEC_TOTHER
}_OCI_EXEC_TYPE;

/*******************************数据类型******************************/
typedef enum _OCI_ADT_TYPE{
	OCI_ADT_TBIND=0x2134,
	OCI_ADT_TRESET
#define	OCI_ERR_ADT_TYPE	0/*错误的自定义类型*/
}_OCI_ADT_TYPE;

/*********************************绑定操作类型*************************************/
typedef enum _OCI_BIND_TYPE{
	OCI_BINDT_STC=0x3456,/*静态输入*/
	OCI_BINDT_DNC,/*动态输出*/
	OCI_BINDT_DEF/*定义输出*/
}_OCI_BIND_TYPE;

/*************************************全局环境句柄*********************************/
struct _OCI_ENV{
	OCIEnv	*env;
	int		mode;
	link_p	connlink;
	link_p	msglink;
} __OCI_ENV_DATA__={0,OCI_DEFAULT,0,0};


/*************************************连接语句*************************************/
typedef struct _OCIConnHandle{
	int			status;/*服务器连接级会话状态*/
	string		sid;/*oracle标示*/
	string		usr;/*oracle用户*/
	string		pwd;/*oracle用户密码*/
	link_p		stmtlink;/*管理光标变量指针*/
	time_t		id;/*创建ID*/

	OCIEnv		*env;/*OCI环境句柄指针*/
	OCIServer	*svr;/*OCI服务器句柄指针*/
	OCISvcCtx	*ctx;/*OCI上下文句柄指针*/
	OCISession	*sen;/*OCI会话句柄指针*/
}_OCIConnHandle;


/*光标句柄*/
typedef struct _OCIStmtHandle{
	int			mode;/*执行模式*/
	int			type;/*当前分析的SQL语句类型*/
	link_p		bdlink;/*管理绑定的变量指针*/
	link_p		rslink;/*管理绑定的变量指针*/
	time_t		id;/*创建ID*/

	OCI_Conn	pconn;
	OCIStmt		*stmt;/*OCI语句句柄*/
}_OCIStmtHandle;

/*********************************LOB变量*************************************/

typedef struct _OCILob{
	OCI_LobLocator	*lob;/*绑定变量是LOB时分配一个定位符*/
	ub1			piece;/*块读取类型*/
	ub2			csid;/*字符集ID*/
    ub1			csfrm;/*字符集形式*/

	ub4			*off;
	void		*parent;/*父级*/
	OCI_ADT_TYPE ptype;
}_OCILob;

/*********************************普通数据变量***********************************/
typedef struct _OCIData{
	ub4			max;/*单个成员最大字节*/
	void		*data;/*绑定数据*/

	void		*parent;/*父级*/
	OCI_ADT_TYPE ptype;
}_OCIData;

/*********************************时间戳数据变量***********************************/
typedef struct	_OCITmStamp{
	OCIDateTime		**dt;
	ub4				type;
	void			*parent;
	OCI_ADT_TYPE	ptype;
}_OCITmStamp;

/*************************************绑定句柄*************************************/
typedef struct _OCIBind{
	OCI_BIND_TYPE	bdtype;/*绑定操作类型*/
	BOOL		ctype;/*创建类型：是否为临时*/
	ub2			type;/*绑定数据类型*/
	string		name;/*绑定名字*/
	ushort		pos;/*绑定位置*/

	/*辅助属性*/
	ub4			mem;/*最大数组成员数*/
	ub4			cur;/*当前成员数*/
	BOOL		flag;/*是否为标量*/
	sb4			iter;/*获取适配器*/

	void		*data;/*绑定数据*/
	void		*size;/*绑定数据字节大小，动态绑定时为ub4、否则为ub2*/
	sb2			*idct;/*指示器*/
	ub2			*rcode;/*返回码*/

	time_t		id;/*创建ID*/
	OCI_Stmt	pstmt;/*父级光标句柄*/
	OCIBind		*bindp;/*私有绑定数据，不能主动释放*/
}_OCIBind;

typedef struct _OCIColParam{
	ub2			type;/*列数据类型*/
	ub2			dtype;/*定义输出类型*/
	char		*name;/*列名字*/
	ub4			pos;/*列位置*/
	ub2			max;/*列最大字节数*/
	ub2			pre;/*精度*/
	sb1			scale;/*刻度*/

	/*数据属性*/
	void		*data;/*绑定数据*/
	ub2			*size;/*绑定数据字节大小，动态绑定时为ub4、否则为ub2*/
	sb2			*idct;/*指示器*/
	ub2			*rcode;/*返回码*/
	OCIDefine	*definep;
	OCI_Res	preset;
}_OCIColParam;

typedef struct _OCIReset{
	OCI_ColParam	values;
	BOOL		isfetched;/*提取过一次*/
	ub2			cols;
	ub4			cur;/*当前提取数*/
	ub4			mem;/*最大提取数*/

	sb4			row_iter;
	sb4			col_iter;

	time_t		id;/*创建ID*/
	OCI_Stmt	pstmt;/*父级光标句柄*/
}_OCIReset;

/*********************************错误句柄*************************************/
typedef struct _OCIErr{
	char		msg[DEF_ERRMSG_LEN];
	char		fun[DEF_ERRFUN_LEN];
	short		code;
	OCIError	*err;
	time_t		id;
}_OCIErr;

static BOOL		free_conn(OCI_Conn hd,OCI_FREE_TYPE mode);
static BOOL		free_stmt(OCI_Stmt cs,OCI_FREE_TYPE mode);

/*得到影响的行数*/
static size_t	get_stmt_affects(OCI_Stmt cs,OCI_Err err);


/*绑定变量函数*/
static OCI_Bind	new_bind(OCI_Stmt cs,const string name,ushort pos,ushort type,ushort mem,size_t size,OCI_BIND_TYPE bdtp);
static OCI_Bind	new_temp_bind(const OCI_Res rs,ushort col,OCI_Stmt cs);
static BOOL		free_bind(OCI_Bind bd,OCI_FREE_TYPE mode);

/*自定义数据类型的生成与释放*/
static OCI_Data	new_oci_data(void *parent,size_t size,OCI_ADT_TYPE ptype);
static OCI_Lob	new_oci_lob(void *parent,OCI_ADT_TYPE ptype);
static OCI_TmStamp new_oci_datetime(void *parent,OCI_ADT_TYPE ptype,ub4 type);
static void		free_oci_data(OCI_Data dt);
static void		free_oci_lob(OCI_Lob dt);
static void		free_oci_datetime(OCI_TmStamp dt);

static void		bind_lob_reset(OCI_Bind bd);
static void		bind_data_reset(OCI_Bind bd);

/*动态绑定时增加一组数据*/
static void		add_oci_data(OCI_Bind bd);
static void		add_oci_lob(OCI_Bind bd);
static void		add_oci_datetime(OCI_Bind bd);
static void		add_oci(OCI_Bind bd,ushort add);


static BOOL		bind_data_setval_idx(OCI_Bind bd,ushort idx,const void *data,size_t size);
static OCI_GETSTAT bind_getval_idx(OCI_Bind bd,ushort idx,void *data,size_t *size);

static BOOL		bind_lob_getlen_idx(OCI_Bind bd,ushort idx,size_t *size);
static void		bind_lob_resetoff_idx(OCI_Bind bd,ushort idx);
static BOOL		bind_lob_setval_idx(OCI_Bind bd,ushort idx,size_t off,const void *data,size_t size,size_t *rsize);
static OCI_GETSTAT bind_lob_getval_idx(OCI_Bind bd,ushort idx,size_t off,void *data,size_t *size,size_t *rsize);
static BOOL		bind_lob_erase(OCI_Bind bd,ushort idx,size_t len,size_t off);
static BOOL		bind_lob_truncate(OCI_Bind bd,ushort idx,size_t len);

/*从绑定管理链表中得到最大的绑定值数量的最大值，用于批量操作*/
static BOOL		get_bind_cur(OCI_Bind bd,ushort *max);
static BOOL		stmt_execsql(OCI_Stmt cs,ushort max,OCI_Err err,OCI_EXEC_TYPE mode);

/*管理链表操作函数*/
/*在环境数据中的消息句柄变量管理链表中查找一个节点*/
static BOOL		find_msg_from_env(OCI_Err data,OCI_Err er);
/*在环境数据中的连接句柄变量管理链表中查找一个节点*/
static BOOL		find_conn_from_env(OCI_Conn data,OCI_Conn hd);
/*在连接句柄中的光标句柄变量管理链表中查找一个节点*/
static BOOL		find_stmt_from_conn(OCI_Stmt data,OCI_Stmt cs);
/*在光标句柄中的绑定句柄管理链表中查找一个节点*/
static BOOL		find_bind_from_stmt(OCI_Bind data,OCI_Bind bd);
/*在光标句柄中的结果集句柄管理链表中查找一个节点*/
static BOOL		find_reset_from_stmt(OCI_Res data,OCI_Res rs);


/*在获取数据时判断绑定变量的索引是否有效及是否包含有效数据*/
static BOOL		bind_get_isvalid(OCI_Bind bd,ushort idx);
static BOOL		bind_set_isvalid(OCI_Bind bd,ushort idx);

/*动态绑定回调函数*/
static sb4 dnc_bind_callback_out(dvoid *ctxp,OCIBind *bindp,ub4 iter,ub4 index,dvoid **bufpp,ub4 **alenp,ub1 *piecep,dvoid **indpp,ub2 **rcodepp);
static sb4 dnc_bind_callback_in (dvoid *ctxp,OCIBind *bindp,ub4 iter,ub4 index,dvoid **bufpp,ub4 *alenpp,ub1 *piecep,dvoid **indpp );

/*获取和释放一个OCIError对象*/
static OCIError	*new_oci_err();
static void		free_oci_err(OCIError *err);

/*释放自定义错误类型*/
static BOOL		free_err(OCI_Err err,OCI_FREE_TYPE mode);
static BOOL		check_err(int flag,OCI_Err er,char *msg,char *fun);

static BOOL		check_date(const OCIDate *dt,OCI_Err er);
static BOOL		check_datetime(const OCI_DateTime dt,OCI_Err er);

static OCI_Res	new_reset(OCI_Stmt cs,ushort max,OCI_Err err);
static BOOL		free_reset(OCI_Res rs,OCI_FREE_TYPE type);
static BOOL		res_init_colparam(OCI_ColParam cp,OCI_Res rs,OCI_Err err);

static OCI_GETSTAT res_data_getval_idx(OCI_Res rs,ushort row,ushort col,void *data,size_t *size);
static OCI_GETSTAT res_lob_getval_idx(OCI_Res rs,ushort row,ushort col,size_t off,void *data,size_t *size);
static BOOL		res_lob_getlen_idx(OCI_Res rs,ushort row,ushort col,size_t *len);

static ushort	res_get_outtype_idx(OCI_Res rs,ushort col);
static string	res_get_colname(OCI_Res rs,ushort col);
static BOOL		res_get_colprescale_idx(OCI_Res rs,ushort col,int *pre,int *scale);
static size_t	res_get_colmaxsize_idx(OCI_Res rs,ushort col);

static BOOL		res_get_isvalid(OCI_Res rs,ushort row,ushort col);
static string	res_get_colname(OCI_Res rs,ushort col);

static OCI_GETSTAT	stmt_fetchsql(OCI_Res rs,ushort type,OCI_Err err);
static void		stmt_reset_fetchstat(OCI_Res rs);


BOOL		find_conn_from_env(OCI_Conn data,OCI_Conn hd){
	assert_error("Invalid pointer","find_conn_from_env()",data && hd);
	if(data==hd && data->id==hd->id)
		return TRUE;
	return FALSE;
}

/*在环境数据中的消息句柄变量管理链表中查找一个节点*/
BOOL		find_msg_from_env(OCI_Err data,OCI_Err err){
	assert_error("Invalid pointer","find_msg_from_env()",data && err);
	if(data==err && data->id==err->id)
		return TRUE;
	return FALSE;
}

BOOL		free_err(OCI_Err err,OCI_FREE_TYPE mode){
	BOOL	ret;

	assert_error("Invalid pointer","free_err()",err);

	
	if(err->err)OCIHandleFree(err->err,OCI_HTYPE_ERROR);

	/*被动调用，删除环境数据中错误句柄管理链表中的节点*/
	if(mode==OCI_FREET_CHILD){
		node_p node;
		node=TravLink(__OCI_ENV_DATA__.msglink,(travlink_f)find_msg_from_env,err);
		if(node==NULL){
			check_err(OCI_ERR_ENV_MGRHD,NULL,"Managed error-linker faild.","freeerr()");
			exit(OCI_ERR_ENV_MGRHD);
		}
		DelNodeLink(node,__OCI_ENV_DATA__.msglink);
		FreeNode(node,NULL);
		ret=TRUE;
	}
	else
		ret=FALSE;

	Free(err);
	return ret;
}

BOOL		free_conn(OCI_Conn hd,OCI_FREE_TYPE mode){
	OCI_Err	err;
	assert_error("Invalid pointer","free_conn()",hd);

	err=OCI_NewErr();
	if(hd->status & OCI_STAT_START)
		check_err(OCISessionEnd(hd->ctx,err->err,hd->sen,OCI_DEFAULT),err,"OCISessionEnd()","free_conn()");
	if(hd->status & OCI_STAT_CONN)
		check_err(OCIServerDetach(hd->svr,err->err,OCI_DEFAULT),err,"OCIServerDetach()","free_conn()");
	
	/*释放已分配的光标指针*/
	if(hd->stmtlink){
		TravLink(hd->stmtlink,(travlink_f)free_stmt,(data_t)OCI_FREET_FATHER);
		FreeLink(hd->stmtlink,NULL);
	}
	
	/*被动调用，删除环境数据中连接句柄管理链表中的节点*/
	if(mode==OCI_FREET_CHILD){
		node_p node;
		node=TravLink(__OCI_ENV_DATA__.connlink,(travlink_f)find_conn_from_env,hd);
		if(node==NULL){
			check_err(OCI_ERR_ENV_MGRHD,NULL,"freehdl(),Managed handle-link error()","freehdl()");
			exit(OCI_ERR_ENV_MGRHD);
		}
		DelNodeLink(node,__OCI_ENV_DATA__.connlink);
		FreeNode(node,NULL);
	}
	
	if(hd->sen)OCIHandleFree(hd->sen,OCI_HTYPE_SESSION);
	if(hd->ctx)OCIHandleFree(hd->ctx,OCI_HTYPE_SVCCTX);
	if(hd->svr)OCIHandleFree(hd->svr,OCI_HTYPE_SERVER);
	Free(hd);
	OCI_FreeErr(err);
	return FALSE;
}

BOOL		OCI_InitEnv(int mode){
	sword flag;
/*判断编译条件*/
	assert_error("The OCI_INIT_IDCT can't guarantee the initialization value of indictor less than -2.","OCI_InitEnv()",(short)OCI_INIT_IDCT_VAL < -2);

#ifndef INIT_ENV_MODE_8I
	flag=OCIInitialize((ub4) OCI_DEFAULT,NULL,NULL,NULL,NULL);
	if(!check_err(flag,NULL,"OCIInitialize()","OCI_InitEnv()"))
		exit(OCI_ERR_ENV_CREAT);
	flag=OCIEnvInit(&__OCI_ENV_DATA__,mode,0,NULL);

#else
	flag=OCIEnvCreate(&__OCI_ENV_DATA__.env,mode,NULL,NULL,NULL,NULL,0,NULL);
#endif
	if(!check_err(flag,NULL,"OCIEnvCreate()","OCI_InitEnv()"))
		exit(OCI_ERR_ENV_CREAT);
	__OCI_ENV_DATA__.mode=mode;
	__OCI_ENV_DATA__.connlink=NewLink();
	__OCI_ENV_DATA__.msglink=NewLink();
	if(mode & OCI_THREADED){
		OCI_Err err;
		err=OCI_NewErr();
		if(!err){
			check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_InitEnv()");
			exit(OCI_ERR_HDL_ALLOC);
		}
		OCIThreadProcessInit();
		flag=OCIThreadInit(__OCI_ENV_DATA__.env,err->err);
		if(!check_err(flag,err,"OCIThreadInit()","OCI_InitEnv()"))
			exit(OCI_ERR_ENV_CREAT);
		OCI_FreeErr(err);
	}
	return TRUE;
}

void		OCI_EndEnv(void){
	if(__OCI_ENV_DATA__.mode & OCI_THREADED){
		OCI_Err err;
		sword flag;
		err=OCI_NewErr();
		if(!err){
			check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_EndEnv()");
			exit(OCI_ERR_HDL_ALLOC);
		}
		OCIThreadProcessInit();
		flag=OCIThreadTerm(__OCI_ENV_DATA__.env,err->err);
		if(!check_err(flag,err,"OCIThreadTerm()","OCI_EndEnv()"))
			exit(OCI_ERR_ENV_END);
		OCI_FreeErr(err);
	}
	TravLink(__OCI_ENV_DATA__.connlink,(travlink_f)free_conn,(data_t)OCI_FREET_FATHER);
	FreeLink(__OCI_ENV_DATA__.connlink,NULL);

	TravLink(__OCI_ENV_DATA__.msglink,(travlink_f)free_err,(data_t)OCI_FREET_FATHER);
	FreeLink(__OCI_ENV_DATA__.msglink,NULL);
	OCIHandleFree(__OCI_ENV_DATA__.env,OCI_HTYPE_ENV);
}

OCI_Conn	OCI_NewConn(string usr,string pwd,string sid,OCI_Err err){
	OCI_Conn hd;
	sword	flag;
	OCI_Err	terr;
	assert_error("Invalid pointer","OCI_NewConn()",sid && usr && pwd);

	terr=err?err:OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_NewConn()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	hd=New(OCI_Conn);
	hd->env=__OCI_ENV_DATA__.env;
	/*初始化光标管理链表*/
	hd->stmtlink=NewLink();
	time(&hd->id);
	/*插入到管理链表*/
	InsertAtHeadLink(hd,__OCI_ENV_DATA__.connlink);

	flag=OCIHandleAlloc(__OCI_ENV_DATA__.env,(void **)&hd->svr,OCI_HTYPE_SERVER,0,NULL);
	if(!check_err(flag,terr,"OCIHandleAlloc() allocation OCI_HTYPE_SERVER","OCI_NewConn()"))
		goto ret;
	/*分配上下文*/
	flag=OCIHandleAlloc(__OCI_ENV_DATA__.env,(void **)&hd->ctx,OCI_HTYPE_SVCCTX,0,NULL);
	if(!check_err(flag,terr,"OCIHandleAlloc() allocation OCI_HTYPE_SVCCTX","OCI_NewConn()"))
		goto ret;
	flag=OCIHandleAlloc(__OCI_ENV_DATA__.env,(void **)&hd->sen,OCI_HTYPE_SESSION,0,NULL);
	if(!check_err(flag,terr,"OCIHandleAlloc() allocation OCI_HTYPE_SESSION","OCI_NewConn()"))
		goto ret;


	/*连接数据库*/
	flag=OCIServerAttach(hd->svr,terr->err,(text*)sid,(sb4)StringLen(sid),0);
	if(!check_err(flag,terr,"OCIServerAttach() Attach Server","OCI_NewConn()"))
		goto ret;

	hd->status=OCI_STAT_CONN;

	/*将服务添加到上下文*/
	flag=OCIAttrSet(hd->ctx,OCI_HTYPE_SVCCTX,hd->svr,0,OCI_ATTR_SERVER,terr->err);
	if(!check_err(flag,terr,"OCIAttrSet() Service initialize","OCI_NewConn()"))
		goto ret;

	/*新建回话*/
	flag=OCIAttrSet(hd->sen,OCI_HTYPE_SESSION,usr,(ub4)StringLen(usr),OCI_ATTR_USERNAME,terr->err);
	if(!check_err(flag,terr,"OCIAttrSet() Set session username","OCI_NewConn()"))
		goto ret;

	flag=OCIAttrSet(hd->sen,OCI_HTYPE_SESSION,pwd,(ub4)StringLen(pwd),OCI_ATTR_PASSWORD,terr->err);
	if(!check_err(flag,terr,"OCIAttrSet() Set session password","OCI_NewConn()"))
		goto ret;

	flag=OCISessionBegin(hd->ctx,terr->err,hd->sen,OCI_CRED_RDBMS,OCI_DEFAULT);
	if(!check_err(flag,terr,"OCISessionBegin()","OCI_NewConn()"))
		goto ret;

	hd->status=hd->status|OCI_STAT_START;

	/*将会话添加到上下文*/
	flag=OCIAttrSet(hd->ctx,OCI_HTYPE_SVCCTX,hd->sen,0,OCI_ATTR_SESSION,terr->err);
	if(!check_err(flag,terr,"OCIAttrSet() Attach service context","OCI_NewConn()"))
		goto ret;

	/*flag=OCIHandleAlloc(__OCI_ENV_DATA__,(void **)&hd->trans,OCI_HTYPE_TRANS,0,NULL);
	if(CHECKERR(flag)){
		GetErr(NULL,"OCI_NewConn(),OCIHandleAlloc transaction faild.","OCIHandleAlloc()",err);
		goto ret;
	}*/

	if(!err)OCI_FreeErr(terr);
	return hd;
ret:
	OCI_FreeConn(hd);
	if(!err)OCI_FreeErr(terr);
	return NULL;
}

void		OCI_FreeConn(OCI_Conn hd,...){
	va_list vp;
	va_start(vp,hd);
	do{
		free_conn(hd,OCI_FREET_CHILD);
		hd=va_arg(vp,OCI_Conn);
	}while(hd!=0);
	va_end(vp);
	return;
}

OCI_Stmt	OCI_NewStmt(OCI_Conn hd,OCI_Err err){
	OCI_Stmt	cs;
	OCI_Err	terr;
	sword	flag;
	
	assert_error("Invalid pointer","OCI_NewStmt()",hd);

	terr=err?err:OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_NewStmt()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	cs=New(OCI_Stmt);
	cs->pconn=hd;
	cs->mode=OCI_DEFAULT;
	/*初始化绑定变量管理链表*/
	cs->bdlink=NewLink();
	cs->rslink=NewLink();
	time(&cs->id);

	/*加入连接句柄的管理链表中*/
	InsertAtHeadLink(cs,cs->pconn->stmtlink);

	flag=OCIHandleAlloc(hd->env,(void **)&cs->stmt,OCI_HTYPE_STMT,0,NULL);
	if(check_err(flag,terr,"OCIHandleAlloc() error","OCI_NewStmt()")==FALSE){
		OCI_FreeStmt(cs);
		goto ret;
	}
	if(!err)OCI_FreeErr(terr);
	return cs;
ret:
	if(!err)OCI_FreeErr(terr);
	return NULL;
}

BOOL		find_stmt_from_conn(OCI_Stmt data,OCI_Stmt cs){
	assert_error("Invalid pointer","find_stmt_from_conn()",cs && data);
	if(data==cs && data->id==cs->id)
		return TRUE;
	return FALSE;
}

BOOL		free_stmt(OCI_Stmt cs,OCI_FREE_TYPE mode){
	BOOL ret;
	sword flag;

	assert_error("Invalid pointer","free_stmt()",cs);
	

	/*释放该光标拥有的所有绑定变量*/
	if(cs->bdlink){
		TravLink(cs->bdlink,(travlink_f)free_bind,(data_t)OCI_FREET_FATHER);
		FreeLink(cs->bdlink,NULL);
	}
	/*释放该光标拥有的所有结果集变量*/
	if(cs->rslink){
		TravLink(cs->rslink,(travlink_f)free_reset,(data_t)OCI_FREET_FATHER);
		FreeLink(cs->rslink,NULL);
	}

	/*是否是被动调用*/
	if(mode==OCI_FREET_CHILD){
		node_p node;
		node=TravLink(cs->pconn->stmtlink,(travlink_f)find_stmt_from_conn,cs);
		if(node==NULL){
			check_err(OCI_ERR_HDL_MGRCS,NULL,"Managed cursor-link error","free_stmt()");
			exit(OCI_ERR_HDL_MGRCS);
		}
		/*从连接句柄上的光标管理链表中剥离失效的节点*/
		DelNodeLink(node,cs->pconn->stmtlink);
		FreeNode(node,NULL);
	}
	/*是否语句句柄，同时会自动是否该句柄上拥有的绑定句柄和定义句柄*/
	check_err(OCIHandleFree(cs->stmt,OCI_HTYPE_STMT),NULL,"Call OCIHandleFree() for free a statement handle","free_stmt()");
	Free(cs);
	return FALSE;
}

void		OCI_FreeStmt(OCI_Stmt cs,...){
	va_list vp;
	va_start(vp,cs);
	do{
		free_stmt(cs,OCI_FREET_CHILD);
		cs=va_arg(vp,OCI_Stmt);
	}while(cs!=0);
	va_end(vp);
	return;
}

//OCIStmt		*const CsGetstat(OCI_Stmt cs){
//	assert_error("Invalid pointer","CsGetstat()",cs);
//	return cs->stmt;
//}

short		OCI_StmtGetStmtType(OCI_Stmt cs){
	assert_error("Invalid pointer","OCI_StmtGetStmtType()",cs);
	return cs->type;
}

size_t		getaffects(OCI_Stmt cs,OCI_Err err){
	OCI_Err	terr;
	sword	flag;
	int		rows;
	assert_error("Invalid pointer","getaffects()",cs);

	terr=err?err:OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","getaffects()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	flag=OCIAttrGet(cs->stmt,OCI_HTYPE_STMT,&rows,0,OCI_ATTR_ROW_COUNT,terr->err);
	if(!check_err(flag,terr,"OCIAttrGet() for OCI_ATTR_ROW_COUNT error","getaffects()"))
		return 0;
	if(!err)OCI_FreeErr(terr);
	return rows;
}

size_t		OCI_StmtGetAffectRows(OCI_Stmt cs,OCI_Err err){
	return getaffects(cs,err);
}

BOOL		OCI_Commit(OCI_Conn hd,OCI_Err err){
	OCI_Err	terr;
	sword	flag;
	BOOL	ret;
	assert_error("Invalid pointer","OCI_Commit()",hd);

	terr=err?err:OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_Commit()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	flag=OCITransCommit(hd->ctx,terr->err,0);
	ret=check_err(flag,terr,"OCITransCommit() error","OCI_Commit()");
	if(!err)OCI_FreeErr(terr);
	return ret;
}

BOOL		OCI_Rollback(OCI_Conn hd,OCI_Err err){
	OCI_Err	terr;
	sword	flag;
	BOOL	ret;
	assert_error("Invalid pointer","OCI_Rollback()",hd);

	terr=err?err:OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_Rollback()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	flag=OCITransRollback(hd->ctx,terr->err,0);
	ret=check_err(flag,terr,"OCITransRollback() error","OCI_Rollback()");
	if(!err)OCI_FreeErr(terr);
	return ret;
}

BOOL		OCI_StmtParse(OCI_Stmt cs,string sql,OCI_Err err){
	OCI_Err	terr;
	BOOL	ret;
	sword	flag;
	assert_error("Invalid pointer","OCI_StmtParse()",cs && sql);

	cs->type=OCI_UNKNOWN;
	terr=err?err:OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_StmtParse()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	/*准备语句*/
	flag=OCIStmtPrepare(cs->stmt,terr->err,(text*)sql,StringLen(sql),OCI_NTV_SYNTAX,OCI_DEFAULT);
	ret=check_err(flag,terr,"OCIStmtPrepare() error","OCI_StmtParse()");
	if(!ret)
		goto result;
	/*判断语句是否正确*/
	flag=OCIStmtExecute(cs->pconn->ctx,cs->stmt,terr->err,1,0,NULL,NULL,OCI_PARSE_ONLY);
	ret=check_err(flag,terr,"OCIStmtExecute() error","OCI_StmtParse()");
	if(!ret)
		goto result;
	/*得到语句类型*/
	flag=OCIAttrGet(cs->stmt,OCI_HTYPE_STMT,&cs->type,0,OCI_ATTR_STMT_TYPE,terr->err);
	ret=check_err(flag,terr,"OCIAttrGet() for OCI_HTYPE_STMT error","OCI_StmtParse()");
	
result:
	if(!err)OCI_FreeErr(terr);
	return ret;
}

void		OCI_StmtSetOpt(OCI_Stmt cs,int mode,BOOL set){
	assert_error("Invalid pointer","OCI_StmtSetOpt()",cs);
	if(set)
		cs->mode|=mode;
	else
		cs->mode&=~mode;
}

OCI_Err		OCI_NewErr(){
	OCI_Err	err;
	sword	flag;

	err=New(OCI_Err);
	time(&err->id);
	/*插入管理链表*/
	InsertAtHeadLink(err,__OCI_ENV_DATA__.msglink);
	flag=OCIHandleAlloc(__OCI_ENV_DATA__.env,(void**)&err->err,OCI_HTYPE_ERROR,0,0);
	if(!check_err(flag,NULL,"OCIHandleAlloc() allocation OCI_HTYPE_ERROR","OCI_NewErr()")){
		OCI_FreeErr(err);
		return NULL;
	}
	
	return err;
}

void		OCI_FreeErr(OCI_Err err){
	assert_error("Invalid pointer","OCI_Err()",err);
	free_err(err,OCI_FREET_CHILD);
}

string		OCI_ErrGetmsg(OCI_Err err){
	assert_error("Invalid pointer","OCI_ErrGetmsg()",err);
	return STRING err->msg;
}
string		OCI_ErrGetfunc(OCI_Err err){
	assert_error("Invalid pointer","OCI_ErrGetfunc()",err);
	return STRING err->fun;
}
short	OCI_ErrGetStat(OCI_Err err){
	assert_error("Invalid pointer","OCI_ErrGetStat()",err);
	return err->code;
}

OCIError	*new_oci_err(){
	OCIError *err;
	sword flag;

	flag=OCIHandleAlloc(__OCI_ENV_DATA__.env,(void**)&err,OCI_HTYPE_ERROR,0,0);
	if(!check_err(flag,NULL,"OCIHandleAlloc() allocation OCI_HTYPE_ERROR","new_oci_err()"))
		return NULL;
	return err;
}

void		free_oci_err(OCIError *err){
	assert_error("Invalid pointer","free_oci_err()",err);
	check_err(OCIHandleFree(err,OCI_HTYPE_ERROR),NULL,"OCIHandleFree() error","free_oci_err()");
}

//BOOL		get_bind_cur(OCI_Bind bd,ushort *val){
//	assert(bd && val);
//	assert(bd->data);
//
//	if(bd->cur>(*val))
//		(*val)=bd->cur;
//	return FALSE;
//}

BOOL		stmt_execsql(OCI_Stmt cs,ushort max,OCI_Err err,OCI_EXEC_TYPE mode){
	OCI_Err	terr;
	sword	flag;
	BOOL	ret,herr;
	ushort	rows,cur,off;

	assert_error("Invalid pointer","stmt_execsql()",cs);

	

	terr=err?err:OCI_NewErr();
	if(!terr){
		ret=check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","stmt_execsql()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	if(cs->type==OCI_UNKNOWN){
		ret=check_err(OCI_ERR_CS_ERRSTMT,terr,"Executed sql is unkown type","stmt_execsql()");
		goto haveerr;
	}

	if(mode!=OCI_EXEC_TFETCH && cs->type==OCI_SELECT){
		ret=check_err(OCI_ERR_CS_ERRSTMT,terr,"Executed sql is select-sql","stmt_execsql()");
		goto haveerr;
	}

	if(mode==OCI_EXEC_TFETCH && cs->type!=OCI_SELECT){
		ret=check_err(OCI_ERR_CS_ERRSTMT,terr,"Executed sql not is select-sql","stmt_execsql()");
		goto haveerr;
	}

	if(mode==OCI_EXEC_TBATCH){
		if(cs->type!=OCI_INSERT && cs->type!=OCI_DELETE && cs->type!=OCI_UPDATE && cs->type!=OCI_DROP){
			ret=check_err(OCI_ERR_CS_ERRSTMT,terr,"Executed sql not is DML [insert|update|delete|drop]","stmt_execsql()");
			goto haveerr;
		}
		if(!max){
			ret=check_err(OCI_ERR_CS_ERRSTMT,terr,"Invalid argument","stmt_execsql()");
			goto haveerr;
		}
	}

	if(mode == OCI_EXEC_TFETCH){
		flag=OCIStmtExecute(cs->pconn->ctx,cs->stmt,terr->err,0,0,NULL,NULL,cs->mode & ~OCI_BATCH_ERRORS);
		ret=check_err(flag,terr,"OCIStmtExecute() error","stmt_execsql()");
	}
	else{
		for(herr=FALSE,rows=cur=off=0;off<max;cur++){
			flag=OCIStmtExecute(cs->pconn->ctx,cs->stmt,terr->err,max,off,NULL,NULL,cs->mode & ~OCI_BATCH_ERRORS & ~OCI_STMT_SCROLLABLE_READONLY);
			ret=check_err(flag,terr,"OCIStmtExecute() error","stmt_execsql()");
			if(ret)
				herr=TRUE;
			if(!ret && !(cs->mode & OCI_BATCH_ERRORS))
				break;
			rows+=getaffects(cs,NULL);
			off=rows+cur+1;
		}
		if(herr && cur>0 && max>1 && (cs->mode & OCI_EXEC_AUTOCOMM))
			OCI_Commit(cs->pconn,NULL);
		ret=herr;
	}
haveerr:
	if(!err)OCI_FreeErr(terr);
	return ret;
}

BOOL		OCI_ExecNoQuery(OCI_Stmt cs,OCI_Err err){
	return stmt_execsql(cs,1,err,OCI_EXEC_TOTHER);
}

BOOL		OCI_ExecBatch(OCI_Stmt cs,ushort max,OCI_Err err){
	return stmt_execsql(cs,max,err,OCI_EXEC_TBATCH);
}

/*********************************自定义数据操作******************************************/
OCI_Data	new_oci_data(void *parent,size_t max,OCI_ADT_TYPE ptype){
	OCI_Data	dt;
	size_t		mem;

	assert_error("Invalid pointer","newbdlob()",parent);
	if(ptype==OCI_ADT_TBIND)
		mem=((OCI_Bind)parent)->mem;
	else if(ptype==OCI_ADT_TRESET)
		mem=((OCI_Res)parent)->mem;
	else
		assert_error("Invalid adt type","new_oci_data()",OCI_ERR_ADT_TYPE);

	dt=New(OCI_Data);
	dt->max=max;
	dt->parent=(void*)parent;
	dt->ptype=ptype;

	dt->data=NewBlock(mem,max);
	return dt;
}

void		free_oci_data(OCI_Data dt){
	assert_error("Invalid pointer","free_oci_data()",dt);
	Free(dt->data);
	Free(dt);
}

OCI_Lob		new_oci_lob(void *parent,OCI_ADT_TYPE ptype){
	OCI_Lob	lb;
	BOOL	flag;
	ushort	i;
	size_t		mem;
	OCIEnv		*env;
	OCISvcCtx	*ctx;
	OCI_Err	terr;

	assert_error("Invalid pointer","new_oci_lob()",parent);
	if(ptype==OCI_ADT_TBIND){
		mem=((OCI_Bind)parent)->mem;
		env=((OCI_Bind)parent)->pstmt->pconn->env;
		ctx=((OCI_Bind)parent)->pstmt->pconn->ctx;
	}
	else if(ptype==OCI_ADT_TRESET){
		mem=((OCI_Res)parent)->mem;
		env=((OCI_Res)parent)->pstmt->pconn->env;
		ctx=((OCI_Res)parent)->pstmt->pconn->ctx;
	}
	else
		assert_error("Invalid adt type","new_oci_lob()",OCI_ERR_ADT_TYPE);


	lb=New(OCI_Lob);
	lb->csid=OCI_DEFAULT;
	lb->csfrm=SQLCS_IMPLICIT;
	lb->piece=OCI_ONE_PIECE;
	lb->lob=NewArray(mem,OCI_LobLocator);
	lb->parent=parent;
	lb->ptype=ptype;

	terr=OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","new_oci_lob()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	for(i=0;i<mem;i++){
		flag=check_err(OCIDescriptorAlloc(env,(void**)&lb->lob[i],OCI_DTYPE_LOB,0,0),0,"OCIDescriptorAlloc() error","new_oci_lob()");
		if(!flag)goto ret;
		if(ptype==OCI_ADT_TBIND && ((OCI_Bind)parent)->bdtype==OCI_BINDT_STC){
			/*静态绑定时，创建临时(使用数据库的临时表空间)LOB描述符，在执行后，临时描述变为与数据库连接的正式描述符，就不能调用OCILobFreeTemporary()，否则报错*/
			ub1		type;
			type=((OCI_Bind)parent)->type==OCI_CLOB?OCI_TEMP_CLOB:OCI_TEMP_BLOB;
			flag=check_err(OCILobCreateTemporary(ctx,terr->err,lb->lob[i],lb->csid,lb->csfrm,type,FALSE,OCI_DURATION_SESSION),terr,"OCIDescriptorAlloc() error","new_oci_lob()");
			if(!flag)goto ret;
		}
	}
	
	lb->off=NewArray(mem,ub4);
ret:
	if(!flag)free_oci_lob(lb);
	OCI_FreeErr(terr);
	return flag?lb:NULL;
}

void		free_oci_lob(OCI_Lob dt){
	ushort	i;
	ushort	mem;
	assert_error("Invalid pointer","free_oci_lob()",dt);
	if(dt->ptype==OCI_ADT_TBIND)
		mem=((OCI_Bind)dt->parent)->mem;
	else if(dt->ptype==OCI_ADT_TRESET)
		mem=((OCI_Res)dt->parent)->mem;
	else
		assert_error("Invalid adt type","free_oci_lob()",OCI_ERR_ADT_TYPE);

	for(i=0;i<mem;i++){
		sword	flag;
		if(dt->lob[i]){
			flag=OCIDescriptorFree(dt->lob[i],OCI_DTYPE_LOB);
			check_err(flag,0,"OCIDescriptorFree() error","free_oci_lob()");
		}
	}
	Free(dt->lob);
	Free(dt->off);
	Free(dt);
}



OCI_TmStamp	new_oci_datetime(void *parent,OCI_ADT_TYPE ptype,ub4 type){
	OCI_TmStamp		dt;
	BOOL			flag;
	ushort			i;
	size_t			mem;
	OCIEnv			*env;

	assert_error("Invalid pointer","new_oci_datetime()",parent);
	if(ptype==OCI_ADT_TBIND){
		mem=((OCI_Bind)parent)->mem;
		env=((OCI_Bind)parent)->pstmt->pconn->env;
	}
	else if(ptype==OCI_ADT_TRESET){
		mem=((OCI_Res)parent)->mem;
		env=((OCI_Res)parent)->pstmt->pconn->env;
	}
	else
		assert_error("Invalid adt type","new_oci_datetime()",OCI_ERR_ADT_TYPE);

	dt=New(OCI_TmStamp);
	dt->parent=parent;
	dt->ptype=ptype;
	dt->dt=NewArray(mem,OCIDateTime *);
	dt->type=type;

	for(i=0;i<mem;i++){
		flag=check_err(OCIDescriptorAlloc(env,(void **)&dt->dt[i],type,0,0),NULL,"OCIDescriptorAlloc() error","new_oci_datetime()");
		if(!flag)
			break;
	}

	if(!flag)free_oci_datetime(dt);
	return flag?dt:NULL;
}

void	free_oci_datetime(OCI_TmStamp dt){
	ushort	i;
	ushort	mem;
	assert_error("Invalid pointer","free_oci_datetime()",dt);
	if(dt->ptype==OCI_ADT_TBIND)
		mem=((OCI_Bind)dt->parent)->mem;
	else if(dt->ptype==OCI_ADT_TRESET)
		mem=((OCI_Res)dt->parent)->mem;
	else
		assert_error("Invalid adt type","free_oci_datetime()",OCI_ERR_ADT_TYPE);

	for(i=0;i<mem;i++){
		sword	flag;
		if(dt->dt[i]){
			flag=OCIDescriptorFree(dt->dt[i],dt->type);
			check_err(flag,0,"OCIDescriptorFree() error","free_oci_datetime()");
		}
	}
	Free(dt->dt);
	Free(dt);
}

/*****************************************绑定变量*****************************************/
OCI_Bind	new_bind(OCI_Stmt cs,const string name,ushort pos,ushort type,ushort mem,size_t size,OCI_BIND_TYPE bdtp){
	OCI_Bind	bind;

	assert_error("Invalid argument","new_bind()",name || pos);
	assert_error("Invalid pointer","new_bind()",cs);

	/*不能大于最大绑定成员数*/
	mem=mem>OCI_MAX_BINDS?OCI_MAX_BINDS:mem;

	bind=New(OCI_Bind);

	/*标量检测*/
	bind->flag=mem==0?TRUE:FALSE;
	/*动态绑定初始化时，始终为标量*/
	//bind->flag=bdtp==OCI_BINDT_DNC?TRUE:bind->flag;
	bind->iter=-1;
	bind->mem=mem=bind->flag?1:mem;
	bind->pstmt=cs;
	bind->type=type;
	bind->bdtype=bdtp;
	bind->ctype=FALSE;
	time(&bind->id);

	bind->idct=NewArray(bind->mem,sb2);
	bind->rcode=NewArray(bind->mem,ub2);
	if(bind->bdtype==OCI_BINDT_STC)
		bind->size=NewArray(bind->mem,ub2);
	else
		bind->size=NewArray(bind->mem,ub4);

	Memset(bind->idct,OCI_INIT_IDCT,bind->mem*sizeof(sb2));
	if(name!=NULL)
		bind->name=StringCpy(name);
	bind->pos=pos;

	/*插入管理链表*/
	InsertAtHeadLink(bind,cs->bdlink);

	switch(type){
	case OCI_NUM:
		bind->data=new_oci_data(bind,OCI_NUMBER_BYTES,OCI_ADT_TBIND);
		break;
	case OCI_INT:
		bind->data=new_oci_data(bind,sizeof(int),OCI_ADT_TBIND);
		break;
	case OCI_FLT:
		bind->data=new_oci_data(bind,sizeof(float),OCI_ADT_TBIND);
		break;
	case OCI_STR:
		size=(size==0?OCI_STRSIZE:size)+1;
		bind->data=new_oci_data(bind,size,OCI_ADT_TBIND);
		break;
	case OCI_LNG:
	case OCI_LGB:
		size=size==0?OCI_LNGSIZE:size;
		bind->data=new_oci_data(bind,size,OCI_ADT_TBIND);
		break;
	case OCI_BIN:
		size=size==0?OCI_BINSIZE:size;
		bind->data=new_oci_data(bind,size,OCI_ADT_TBIND);
		break;
	case OCI_CLOB:
	case OCI_BLOB:
		bind->data=new_oci_lob(bind,OCI_ADT_TBIND);
		break;
	case OCI_DATE:
		bind->data=new_oci_data(bind,OCI_DATE_BYTES,OCI_ADT_TBIND);
		break;
	case OCI_TMSTAMP:
		bind->data=new_oci_datetime(bind,OCI_ADT_TBIND,OCI_DTYPE_TIMESTAMP);
		break;
	case OCI_TMSTAMP_TZ:
		bind->data=new_oci_datetime(bind,OCI_ADT_TBIND,OCI_DTYPE_TIMESTAMP_TZ);
		break;
	case OCI_TMSTAMP_LTZ:
		bind->data=new_oci_datetime(bind,OCI_ADT_TBIND,OCI_DTYPE_TIMESTAMP_LTZ);
		break;
	default:
		check_err(OCI_ERR_BD_ERRTYPE,NULL,"Don't bind this data-type","new_bind()");
		free_bind(bind,OCI_FREET_CHILD);
		return NULL;
	}

	if(bind->data==NULL){
		free_bind(bind,OCI_FREET_CHILD);
		return NULL;
	}

	return bind;
}

OCI_Bind	OCI_NewStcBindByName(OCI_Stmt cs,const string name,ushort type,ushort max,size_t size){
	return new_bind(cs,name,0,type,max,size,OCI_BINDT_STC);
}

OCI_Bind	OCI_NewStcBindByPos(OCI_Stmt cs,ushort pos,ushort type,ushort max,size_t size){
	return new_bind(cs,NULL,pos,type,max,size,OCI_BINDT_STC);
}

/*分配一个名称用于动态绑定变量*/
OCI_Bind	OCI_NewDncBindByName(OCI_Stmt cs,const string name,ushort type,ushort max,size_t size){
	return new_bind(cs,name,0,type,max,size,OCI_BINDT_DNC);
}
/*分配一个位置用于动态绑定变量*/
OCI_Bind	OCI_NewDncBindByPos(OCI_Stmt cs,ushort pos,ushort type,ushort max,size_t size){
	return new_bind(cs,NULL,pos,type,max,size,OCI_BINDT_DNC);
}


BOOL		find_bind_from_stmt(OCI_Bind data,OCI_Bind bd){
	assert_error("Invalid pointer","find_bind_from_stmt()",data && bd);
	if(data == bd && data->id==bd->id)
		return TRUE;
	return FALSE;
}

BOOL		free_bind(OCI_Bind bd,OCI_FREE_TYPE mode){
	BOOL	ret;
	assert_error("Invalid pointer","free_bind()",bd);

	if(!bd->ctype){
		if(OCI_ISLOB_TYPE(bd->type))
			free_oci_lob((OCI_Lob)bd->data);
		else if(OCI_ISNORMAL_TYPE(bd->type))
			free_oci_data((OCI_Data)bd->data);
		else if(OCI_ISDT_TYPE(bd->type))
			free_oci_datetime((OCI_TmStamp)bd->data);
		else
			assert_error("Invalid bind type","free_bind()",OCI_ERR_BIND_TYPE);
		Free(bd->size);
		Free(bd->idct);
		Free(bd->rcode);
	}
	/*被动调用*/
	if(mode==OCI_FREET_CHILD){
		node_p node;
		node=TravLink(bd->pstmt->bdlink,(travlink_f)find_bind_from_stmt,bd);
		if(node==NULL){
			check_err(OCI_ERR_CS_MGRBD,NULL,"Managed bind-link error","free_bind()");
			exit(OCI_ERR_CS_MGRBD);
		}
		DelNodeLink(node,bd->pstmt->bdlink);
		FreeNode(node,NULL);
	}
	Free(bd->name);
	Free(bd);
	return FALSE;
}

void		OCI_FreeBind(OCI_Bind bd,...){
	va_list vp;
	va_start(vp,bd);
	do{
		free_bind(bd,OCI_FREET_CHILD);
		bd=va_arg(vp,OCI_Bind);
	}while(bd!=NULL);
	va_end(vp);
	return;
}

BOOL	OCI_BindInAllNext(OCI_Bind bd,...){
	va_list vp;
	BOOL	ret;
	va_start(vp,bd);
	do{
		ret=OCI_BindInNext(bd);
		if(!ret)
			break;
		bd=va_arg(vp,OCI_Bind);
	}while(bd!=0);
	va_end(vp);
	return ret;
}

BOOL		OCI_BindInNext(OCI_Bind bd){
	assert_error("Invalid pointer.","OCI_BindInNext()",bd && bd->data);
	if(bd->ctype==TRUE)
		return FALSE;
	if(bd->flag && bd->bdtype==OCI_BINDT_STC)
		return TRUE;
	if(bd->bdtype==OCI_BINDT_STC && (float)bd->iter+1<(float)bd->mem){
		++bd->iter;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL	OCI_BindOutNext(OCI_Bind bd){
	assert_error("Invalid pointer.","OCI_BindOutNext()",bd && bd->data);
	if(bd->flag && bd->bdtype==OCI_BINDT_STC)
		return TRUE;
	if((float)bd->iter+1<(float)bd->cur){
		++bd->iter;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL	OCI_BindOutAllNext(OCI_Bind bd,...){
	va_list vp;
	BOOL	ret;
	va_start(vp,bd);
	do{
		ret=OCI_BindOutNext(bd);
		if(!ret)
			break;
		bd=va_arg(vp,OCI_Bind);
	}while(bd!=0);
	va_end(vp);
	return ret;
}

BOOL		OCI_BindDataSetval(OCI_Bind bd,const void *data,size_t size){
	assert_error("Invalid pointer","OCI_BindDataSetval()",bd);
	if(!bd->flag)
		assert_error("Invalid parameter","OCI_BindDataSetval()",bd->iter!=-1);
	return bind_data_setval_idx(bd,bd->iter,data,size);
}

BOOL		OCI_BindDataSetvalIdx(OCI_Bind bd,ushort idx,const void *data,size_t size){
	return bind_data_setval_idx(bd,idx,data,size);
}

void	OCI_BindErase(OCI_Bind bd){
	ushort i,max;
	OCI_Data	dt;
	assert_error("Invalid pointer","OCI_BindErase()",bd);

	if(OCI_ISLOB_TYPE(bd->type)){
		bzero(((OCI_Lob)bd->data)->off,bd->mem*sizeof(ub4));
		if(bd->bdtype==OCI_BINDT_DNC)
			max=bd->cur;
		else
			max=bd->mem;
		for(i=0;i<max;i++)
			bind_lob_erase(bd,i,0,1);
	}
	if(OCI_ISNORMAL_TYPE(bd->type)){
		if(bd->bdtype==OCI_BINDT_DNC)
			max=bd->cur;
		else
			max=bd->mem;
		dt=(OCI_Data)bd->data;
		Memset((char*)dt->data,0,max*dt->max);
	}

	if(bd->bdtype==OCI_BINDT_DNC)
		bzero(bd->size,bd->mem*sizeof(ub4));
	else
		bzero(bd->size,bd->mem*sizeof(ub2));
}

void	OCI_BindAllErase(OCI_Bind bd,...){
	va_list vp;
	va_start(vp,bd);
	do{
		OCI_BindErase(bd);
		bd=va_arg(vp,OCI_Bind);
	}while(bd!=0);
	va_end(vp);
	return;
}

void	OCI_BindTrunc(OCI_Bind bd,size_t len){
	ushort i,max;
	assert_error("Invalid pointer","OCI_BindErase()",bd);

	if(OCI_ISLOB_TYPE(bd->type)){
		bzero(((OCI_Lob)bd->data)->off,bd->mem*sizeof(ub4));
		if(bd->bdtype==OCI_BINDT_DNC)
			max=bd->cur;
		else
			max=bd->mem;
		for(i=0;i<max;i++)
			bind_lob_truncate(bd,i,len);
		if(bd->bdtype==OCI_BINDT_DNC)
			bzero(bd->size,bd->mem*sizeof(ub4));
		else
			bzero(bd->size,bd->mem*sizeof(ub2));
	}
}

void	OCI_BindAllTrunc(size_t len,OCI_Bind bd,...){
	va_list vp;
	va_start(vp,bd);
	do{
		OCI_BindTrunc(bd,len);
		bd=va_arg(vp,OCI_Bind);
	}while(bd!=0);
	va_end(vp);
	return;
}


void		OCI_BindResetIter(OCI_Bind bd){
	assert_error("Invalid pointer","OCI_BindResetIter()",bd && bd->data);
	if(OCI_ISLOB_TYPE(bd->type)){
		bzero(((OCI_Lob)bd->data)->off,bd->mem*sizeof(ub4));
	}
	bd->iter=-1;
}

void	OCI_BindAllResetIter(OCI_Bind bd,...){
	va_list vp;
	va_start(vp,bd);
	do{
		OCI_BindResetIter(bd);
		bd=va_arg(vp,OCI_Bind);
	}while(bd!=0);
	va_end(vp);
	return;
}

void	OCI_BindReuse(OCI_Bind bd){
	assert_error("Invalid pointer","OCI_BindReuse()",bd && bd->data);

	//OCI_BindErase(bd);

	bd->cur=0;
	/*LOB数据要重置偏移*/
	if(OCI_ISLOB_TYPE(bd->type)){
		bzero(((OCI_Lob)bd->data)->off,bd->mem*sizeof(ub4));
	}
	/*if(bd->bdtype==OCI_BINDT_DNC)
		Memset(bd->size,0,bd->mem*sizeof(ub4));
	else
		Memset(bd->size,0,bd->mem*sizeof(ub2));*/
	Memset(bd->idct,OCI_INIT_IDCT,bd->mem*sizeof(sb2));
	Memset(bd->rcode,0,bd->mem*sizeof(ub2));
	bd->iter=-1;
}

void	OCI_BindAllReuse(OCI_Bind bd,...){
	va_list vp;
	va_start(vp,bd);
	do{
		OCI_BindReuse(bd);
		bd=va_arg(vp,OCI_Bind);
	}while(bd!=0);
	va_end(vp);
	return;
}

OCI_GETSTAT OCI_BindDataGetval(OCI_Bind bd,void *data,size_t *size){
	assert_error("Invalid pointer","OCI_BindDataGetval()",bd && bd->data);
	if(!bd->flag)
		assert_error("Invalid parameter","OCI_BindDataGetval()",bd->iter!=-1);
	return bind_getval_idx(bd,bd->iter,data,size);
}

OCI_GETSTAT OCI_BindDataGetvalIdx(OCI_Bind bd,ushort idx,void *data,size_t *size){
	return bind_getval_idx(bd,idx,data,size);
}

ushort		OCI_BindGetCur(OCI_Bind bd){
	assert_error("Invalid pointer","OCI_BindGetCur()",bd && bd->data);
	if(bd->bdtype==OCI_BINDT_STC)
		return bd->flag?1:bd->cur;
	return bd->cur;
}

ushort		OCI_BindGetMax(OCI_Bind bd){
	assert_error("Invalid pointer","OCI_BindGetMax()",bd && bd->data);
	return bd->bdtype==OCI_BINDT_STC?bd->mem:bd->cur;
}

ushort	OCI_BindGetDataType(OCI_Bind bd){
	assert_error("Invalid pointer","OCI_BindGetDataType()",bd && bd->data);
	return bd->type;
}

/*
得到当前获取适配器或指定位置的指示器值
*/
short	OCI_BindGetIdct(OCI_Bind bd){
	assert_error("Invalid pointer","BdGetgIdct()",bd && bd->data);
	if(!bd->flag)
		assert_error("Invalid parameter","BdGetgIdct()",bd->iter>-1);
	return OCI_BindGetIdctIdx(bd,bd->iter);
}

short	OCI_BindGetIdctIdx(OCI_Bind bd,ushort idx){
	assert_error("Invalid pointer","OCI_BindGetIdctIdx()",bd && bd->data);
	
	if(bind_get_isvalid(bd,idx)==FALSE)
		return OCI_IDCT_INVALID;

	idx=bd->iter?0:idx;
	return bd->idct[idx];
}
/*
得到当前获取适配器或指定位置的返回码值
*/
ushort	OCI_BindGetRecode(OCI_Bind bd){
	assert_error("Invalid pointer","OCI_BindGetRecode()",bd && bd->data);
	if(!bd->flag)
		assert_error("Invalid parameter","OCI_BindGetRecode()",bd->iter>-1);
	return OCI_BindGetRecodeIdx(bd,bd->iter);
}

ushort	OCI_BindGetRecodeIdx(OCI_Bind bd,ushort idx){
	assert_error("Invalid pointer","OCI_BindGetRecodeIdx()",bd && bd->data);

	if(bind_get_isvalid(bd,idx)==FALSE)
		return OCI_RCODE_INVALID;

	idx=bd->iter?0:idx;
	return bd->rcode[idx];
}

BOOL	bind_data_setval_idx(OCI_Bind bd,ushort idx,const void *data,size_t size){
	char *ptr;
	OCI_Data dt;
	OCIDateTime **tm;
	OCI_Err err;
	BOOL	ret;
	sword	flag;

	assert_error("Invalid parameter","bind_data_setval_idx()",!OCI_ISLOB_TYPE(bd->type));
	if(bind_set_isvalid(bd,idx)==FALSE)
		return FALSE;

	idx=bd->flag?0:idx;
	if(bd->idct[idx]==(short)OCI_INIT_IDCT_VAL)
		bd->cur++;

	err=OCI_NewErr();
	if(!err){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","bind_lob_setval_idx()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	
	if(data==UNDEF || data==NULL){
		bd->idct[idx]=-1;
		ret=TRUE;
	}
	else if(OCI_ISDT_TYPE(bd->type)){
		bd->idct[idx]=0;
		tm=((OCI_TmStamp)bd->data)->dt;
		flag=OCIDateTimeConvert(__OCI_ENV_DATA__.env,err->err,((OCI_DateTime)data)->dt,tm[idx]);
		ret=check_err(flag,err,"OCIDateTimeConvert() error","bind_data_setval_idx()");
	}
	else if(OCI_ISNORMAL_TYPE(bd->type)){
		dt=(OCI_Data)bd->data;
		ptr=(char*)dt->data+idx*(dt->max);
		bzero(ptr,dt->max);
		/*数值型数据*/
		if(bd->type==OCI_INT || bd->type==OCI_FLT || bd->type==OCI_NUM || bd->type==OCI_DATE)
			size=dt->max;
		else{
			/*字符串数据*/
			if(bd->type==OCI_STR)
				size=size==0?strlen((char*)data):0;
			if(size==0)
				check_err(OCI_WAR_WR_ZERO,NULL,"Warning write zero-data","bind_lob_setval_idx()");
			/*判断最大字节，超出截断*/
			size=size>dt->max?dt->max:size;
		}
		/*
		当传递一个零字节数据时，系统默认为ORACLE的NULL值
		*/
		if(size>0){
			Memcpy(ptr,data,size);
			if(bd->type==OCI_STR){
				size+=1;
				size=size>dt->max?dt->max:size;
				ptr[dt->max-1]='\0';
			}
			if(bd->bdtype==OCI_BINDT_STC)
				*((ub2*)bd->size+idx)=(ub2)size;
			else
				*((ub4*)bd->size+idx)=(ub4)size;
			bd->idct[idx]=0;
		}
		else
			bd->idct[idx]=-1;
		ret=TRUE;
	}
	else
		assert_error("Invalid parameter","bind_data_setval_idx()",OCI_ERR_BIND_TYPE);

	if(!ret){
		bd->cur--;
		bd->idct[idx]=(short)OCI_INIT_IDCT_VAL;
	}
	OCI_FreeErr(err);
	return ret;
}

OCI_GETSTAT	bind_getval_idx(OCI_Bind bd,ushort idx,void *data,size_t *size){
	char	*ptr;
	OCI_Data	dt;
	size_t	rlen;
	OCIDateTime **tm;
	OCI_Err		err;
	OCI_GETSTAT	ret;
	sword		flag;
	assert_error("Invalid pointer","bind_getval_idx()",data);
	assert_error("Invalid parameter","bind_getval_idx()",!OCI_ISLOB_TYPE(bd->type));
	if(bind_get_isvalid(bd,idx)==FALSE)
		return OCI_GETSTAT_ERROR;
	idx=bd->flag?0:idx;
	if(bd->idct[idx]==-1)
		return OCI_GETSTAT_NULL;
	if(bd->idct[idx]==(short)OCI_INIT_IDCT_VAL)
		return OCI_GETSTAT_ERROR;
	if(bd->idct[idx]!=0)
		check_err(OCI_ERR_DATA_DTTRUC,NULL,"Data may be truncated","bind_getval_idx()");

	err=OCI_NewErr();
	if(!err){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","bind_lob_setval_idx()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	if(OCI_ISDT_TYPE(bd->type)){
		tm=((OCI_TmStamp)bd->data)->dt;
		flag=OCIDateTimeConvert(__OCI_ENV_DATA__.env,err->err,tm[idx],((OCI_DateTime)data)->dt);
		if(check_err(flag,err,"OCIDateTimeConvert() error","bind_data_setval_idx()"))
			ret=OCI_GETSTAT_SUCCESS;
		else
			ret=OCI_GETSTAT_ERROR;	
	}
	else if(OCI_ISNORMAL_TYPE(bd->type)){
		/*非数值、日期型，则size必须有效*/
		if(bd->type!=OCI_INT && bd->type!=OCI_FLT && bd->type!=OCI_DATE && bd->type!=OCI_NUM)
			assert_error("Invalid argument","bind_getval_idx()",size);
		if(size && *size==0){
			check_err(OCI_WAR_RD_ZERO,0,"Zero-length of size","bind_getval_idx()");
			return OCI_GETSTAT_ERROR;
		}
		else{
			dt=((OCI_Data)bd->data);
			rlen=size?*size:dt->max;
			bzero(data,rlen);
			if(bd->bdtype==OCI_BINDT_DNC)
				rlen=rlen>((ub4*)bd->size)[idx]?((ub4*)bd->size)[idx]:rlen;
			else
				rlen=rlen>(size_t)((ub2*)bd->size)[idx]?(size_t)((ub2*)bd->size)[idx]:rlen;
			/*数据没有被更新，数据无效*/
			if(rlen==0){
				check_err(OCI_WAR_DATA_NODATA,0,"No Data where present position.","bind_getval_idx()");
				if(size)*size=0;
				return OCI_GETSTAT_NONDATA;
			}
			if(bd->type==OCI_NUM)rlen+=1;/*OCINumber的第一字节为真实数据长度*/
			if(size)*size=rlen;
			ptr=(char*)dt->data+dt->max*idx;
			Memcpy(data,ptr,rlen);

			ret=OCI_GETSTAT_SUCCESS;
		}
	}
	else
		assert_error("Invalid bind type","bind_getval_idx()",OCI_ERR_BIND_TYPE);
	OCI_FreeErr(err);
	return ret;
}

BOOL		OCI_BindByName(OCI_Err err,OCI_Bind bd,...){
	BOOL		ret;
	va_list		vp;
	sword		flag;
	void		*bddata;
	sb4			bdmax;
	ub4			bdnext;
	void		*bdsize;
	BOOL		type;
	OCI_Err		terr;

	terr=err?err:OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_BindByName()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	va_start(vp,bd);
	do{
		assert_error("Invalid pointer","OCI_BindByName()",bd && bd->data && bd->name);
		/*类型判断*/
		if(OCI_ISNORMAL_TYPE(bd->type)){
			bddata=((OCI_Data)bd->data)->data;
			bdnext=bdmax=((OCI_Data)bd->data)->max;
		}
		else if(OCI_ISLOB_TYPE(bd->type)){
			bddata=((OCI_Lob)bd->data)->lob;
			bdmax=-1;
			bdnext=sizeof(OCI_LobLocator);
		}
		else if(OCI_ISDT_TYPE(bd->type)){
			bddata=((OCI_TmStamp)bd->data)->dt;
			bdmax=-1;
			bdnext=sizeof(OCI_DateTime);
		}
		else{
			assert_error("Invalid bind operation.","OCI_BindByName()",OCI_ERR_BIND_TYPE);
		}

		bdsize=bd->size;
		if(bd->bdtype==OCI_BINDT_STC){
			/*绑定SQL*/
			if(bd->pstmt->type!=OCI_BEGIN && bd->pstmt->type!=OCI_DECLARE){
				flag=OCIBindByName(bd->pstmt->stmt,&bd->bindp,terr->err,(text*)bd->name,StringLen(bd->name),bddata,bdmax,bd->type,bd->idct,0,0,0,0,OCI_DEFAULT);
				type=FALSE;
			}
			else{
			/*绑定PLSQL*/
				flag=OCIBindByName(bd->pstmt->stmt,&bd->bindp,terr->err,(text*)bd->name,StringLen(bd->name),bddata,bdmax,bd->type,bd->idct,(ub2*)bdsize,bd->rcode,bd->flag?0:bd->mem,bd->flag?0:&bd->cur,OCI_DEFAULT);
				type=TRUE;
			}
			ret=check_err(flag,terr,"OCIBindByName() error","OCI_BindByName()");
			if(!ret)
				break;
			flag=OCIBindArrayOfStruct(bd->bindp,terr->err,bdnext,sizeof(sb2),type?sizeof(ub2):0,type?sizeof(ub2):0);
			ret=check_err(flag,terr,"OCIBindByName() error","OCI_BindByName()");
			if(!ret)
				break;
					
		}
		else if(bd->bdtype==OCI_BINDT_DNC){
			flag=OCIBindByName(bd->pstmt->stmt,&bd->bindp,terr->err,(text*)bd->name,StringLen(bd->name),0,bdmax,bd->type,0,0,0,0,0,OCI_DATA_AT_EXEC);
			ret=check_err(flag,terr,"OCIBindByName() error","OCI_BindByName()");
			if(!ret)
				break;
			flag=OCIBindDynamic(bd->bindp,terr->err,bd,dnc_bind_callback_in,bd,dnc_bind_callback_out);
			ret=check_err(flag,terr,"OCIBindDynamic() error","OCI_BindByName()");
			if(!ret)
				break;
		}
		else{
			assert_error("Invalid bind operation.","OCI_BindByName()",OCI_ERR_BIND_OPER);
		}
		bd=va_arg(vp,OCI_Bind);
	}while(bd);

	va_end(vp);
	if(!err)OCI_FreeErr(terr);
	return ret;
}

BOOL		OCI_BindByPos(OCI_Err err,OCI_Bind bd,...){
	BOOL		ret;
	va_list		vp;
	sword		flag;
	void		*bddata;
	sb4			bdmax;
	ub4			bdnext;
	void		*bdsize;
	BOOL		type;
	OCI_Err		terr;

	
	terr=err?err:OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_BindByPos()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	va_start(vp,bd);
	do{
		assert_error("Invalid pointer","OCI_BindByPos()",bd && bd->data && bd->pos);

		/*类型判断*/
		if(OCI_ISNORMAL_TYPE(bd->type)){
			bddata=((OCI_Data)bd->data)->data;
			bdnext=bdmax=((OCI_Data)bd->data)->max;
		}
		else if(OCI_ISLOB_TYPE(bd->type)){
			bddata=((OCI_Lob)bd->data)->lob;
			bdmax=-1;
			bdnext=sizeof(OCI_LobLocator);
		}
		else if(OCI_ISDT_TYPE(bd->type)){
			bddata=((OCI_TmStamp)bd->data)->dt;
			bdmax=-1;
			bdnext=sizeof(OCI_DateTime);
		}
		else{
			assert_error("Invalid bind operation.","OCI_BindByName()",OCI_ERR_BIND_TYPE);
		}
		bdsize=bd->size;

		if(bd->bdtype==OCI_BINDT_STC){
			/*绑定SQL*/
			if(bd->pstmt->type!=OCI_BEGIN && bd->pstmt->type!=OCI_DECLARE){
				flag=OCIBindByPos(bd->pstmt->stmt,&bd->bindp,terr->err,bd->pos,bddata,bdmax,bd->type,bd->idct,0,0,0,0,OCI_DEFAULT);
				type=FALSE;
			}
			else{
			/*绑定PLSQL*/
				flag=OCIBindByPos(bd->pstmt->stmt,&bd->bindp,terr->err,bd->pos,bddata,bdmax,bd->type,bd->idct,(ub2*)bdsize,bd->rcode,bd->flag?0:bd->mem,bd->flag?0:&bd->cur,OCI_DEFAULT);
				type=TRUE;
			}
			ret=check_err(flag,terr,"OCIBindByPos() error","OCI_BindByPos()");
			if(!ret)
				break;
			flag=OCIBindArrayOfStruct(bd->bindp,terr->err,bdnext,sizeof(sb2),type?sizeof(ub2):0,type?sizeof(ub2):0);
			ret=check_err(flag,terr,"OCIBindArrayOfStruct() error","OCI_BindByPos()");
			if(!ret)
				break;
		}
		else if(bd->bdtype==OCI_BINDT_DNC){
			flag=OCIBindByPos(bd->pstmt->stmt,&bd->bindp,terr->err,bd->pos,0,bdmax,bd->type,0,0,0,0,0,OCI_DATA_AT_EXEC);
			ret=check_err(flag,terr,"OCIBindByPos() error","OCI_BindByPos()");
			if(!ret)
				break;
			flag=OCIBindDynamic(bd->bindp,terr->err,bd,dnc_bind_callback_in,bd,dnc_bind_callback_out);
			ret=check_err(flag,terr,"OCIBindDynamic() error","OCI_BindByPos()");
			if(!ret)
				break;
		}
		else{
			assert_error("Invalid bind operation","OCI_BindByPos()",OCI_ERR_BIND_OPER);
		}
		bd=va_arg(vp,OCI_Bind);
	}while(bd);

	va_end(vp);
	if(!err)OCI_FreeErr(terr);
	return ret;
}

void		add_oci_data(OCI_Bind bd){
	OCI_Data	dt;
	assert_error("Invalid pointer","add_oci_data()",bd && bd->data);
	dt=(OCI_Data)bd->data;

	dt->data=NewArrayCpy(dt->data,bd->mem*dt->max,char);
	bzero((char*)dt->data+(bd->cur)*dt->max,(bd->mem-bd->cur)*dt->max);
}

void		add_oci_lob(OCI_Bind bd){
	BOOL	ret;
	sword	flag;
	OCI_Lob	lb;
	ushort	i;

	assert_error("Invalid pointer","add_oci_lob()",bd && bd->data);

	lb=(OCI_Lob)bd->data;
	lb->csfrm=SQLCS_IMPLICIT;
	lb->piece=OCI_ONE_PIECE;

	lb->lob=NewArrayCpy(lb->lob,bd->mem,OCI_LobLocator);
	for(i=bd->cur;i<bd->mem;i++){
		flag=OCIDescriptorAlloc(bd->pstmt->pconn->env,(void**)&lb->lob[i],OCI_DTYPE_LOB,0,0);
		ret=check_err(flag,NULL,"OCIDescriptorAlloc() error","add_oci_lob()");
		if(ret==FALSE)
			exit(OCI_ERR_HDL_ALLOC);
	}
	
	lb->off=NewArrayCpy(lb->off,bd->mem,ub4);
	bzero(lb->off+bd->cur,(bd->mem-bd->cur)*sizeof(ub4));

	return;
}

void		add_oci_datetime(OCI_Bind bd){
	BOOL	ret;
	sword	flag;
	OCI_TmStamp	dt;
	ushort	i;

	assert_error("Invalid pointer","add_oci_datetime()",bd && bd->data);
	dt=(OCI_TmStamp)bd->data;
	dt->dt=NewArrayCpy(dt->dt,bd->mem,OCIDateTime*);
	
	for(i=bd->cur;i<bd->mem;i++){
		flag=OCIDescriptorAlloc(bd->pstmt->pconn->env,(void**)&dt->dt[i],dt->type,0,0);
		ret=check_err(flag,NULL,"OCIDescriptorAlloc() error","add_oci_datetime()");
		if(ret==FALSE)
			exit(OCI_ERR_HDL_ALLOC);
	}
}

void		add_oci(OCI_Bind bd,ushort add){
	assert_error("Invalid pointer","add_oci()",bd && bd->data);

	if(bd->cur+add<=bd->mem)
		return;

	/*至少增加OCI_ADD_BIND_MEM*/
	add=add>OCI_DNC_BIND_ADDMIN?add:OCI_DNC_BIND_ADDMIN;

	bd->mem+=add;

	if(OCI_ISLOB_TYPE(bd->type))
		add_oci_lob(bd);
	else if(OCI_ISDT_TYPE(bd->type))
		add_oci_datetime(bd);
	else if(OCI_ISNORMAL_TYPE(bd->type))
		add_oci_data(bd);
	else
		assert_error("Invalid bind type","add_oci()",OCI_ERR_BIND_TYPE);

	if(bd->bdtype==OCI_BINDT_STC){
		bd->size=NewArrayCpy(bd->size,bd->mem,ub2);
		bzero((ub2*)bd->size+bd->cur,(bd->mem-bd->cur)*sizeof(ub2));
	}
	else{
		bd->size=NewArrayCpy(bd->size,bd->mem,ub4);
		bzero((ub4*)bd->size+bd->cur,(bd->mem-bd->cur)*sizeof(ub4));
	}

	bd->idct=NewArrayCpy(bd->idct,bd->mem,sb2);
	Memset(bd->idct+bd->cur,OCI_INIT_IDCT,(bd->mem-bd->cur)*sizeof(sb2));
	bd->rcode=NewArrayCpy(bd->rcode,bd->mem,ub2);
	bzero(bd->rcode+bd->cur,(bd->mem-bd->cur)*sizeof(ub2));
}

sb4 dnc_bind_callback_out(dvoid *ctxp,OCIBind *bindp,ub4 iter,ub4 index,dvoid **bufpp,ub4 **alenp,ub1 *piecep,dvoid **indpp,ub2 **rcodepp){
	OCI_Bind	bd;
	ub4		add;
	sword	flag;
	OCI_Err	terr;

	terr=OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_NewConn()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	bd=(OCI_Bind)ctxp;

	/*是否增加空间*/
	if(index==0){
		/*每次执行的第一个需求的索引处判断*/
		flag=OCIAttrGet(bindp,OCI_HTYPE_BIND,&add,0,OCI_ATTR_ROWS_RETURNED,terr->err);
		if(check_err(flag,terr,"Call OCIAttrGet() for OCI_ATTR_ROWS_RETURNED error","dncbdcallback_out()")==FALSE)
			exit(OCI_ERR_ATTR_GET);
		add_oci(bd,add);
	}
	
	switch(bd->type){
	case OCI_BLOB:
	case OCI_CLOB:
		{
			OCI_Lob	lb;
			lb=(OCI_Lob)bd->data;
			*bufpp=lb->lob[bd->cur];
			*((ub4*)bd->size+bd->cur)=86;
		}
		break;
	case OCI_TMSTAMP:
	case OCI_TMSTAMP_TZ:
	case OCI_TMSTAMP_LTZ:
		{
			OCI_TmStamp ts;
			ts=(OCI_TmStamp)bd->data;
			*bufpp=ts->dt[bd->cur];
			*((ub4*)bd->size+bd->cur)=86;
		}
		break;
	default:
		{
			OCI_Data	dt;
			dt=(OCI_Data)bd->data;
			*bufpp=(char*)dt->data+bd->cur*dt->max;
			*((ub4*)bd->size+bd->cur)=(ub4)dt->max;
		}
	}
	*piecep=OCI_ONE_PIECE;
	*alenp=(ub4*)bd->size+bd->cur;
	*indpp=&bd->idct[bd->cur];
	*rcodepp=&bd->rcode[bd->cur];
	bd->cur++;
	/*为数组时重置一些属性*/
	bd->flag=bd->cur>1?FALSE:TRUE;
	OCI_FreeErr(terr);
	return OCI_CONTINUE;
}

sb4 dnc_bind_callback_in (dvoid *ctxp,OCIBind *bindp,ub4 iter,ub4 index,dvoid **bufpp,ub4 *alenpp,ub1 *piecep,dvoid **indpp ){
	static sb2 null_ind=-1;
	*bufpp=NULL;
	*alenpp=0;
	*indpp=&null_ind;
	*piecep=OCI_ONE_PIECE;

	return OCI_CONTINUE;
}

BOOL	bind_lob_setval_idx(OCI_Bind bd,ushort idx,size_t off,const void *data,size_t size,size_t *rsize){
	BOOL	ret;
	OCI_Lob	lb;
	OCI_LobLocator lob;
	sword	flag;
	ub4		rlen;
	OCI_Err	terr;

	if(bind_set_isvalid(bd,idx)==FALSE){
		return FALSE;
	}
	assert_error("Invalid parameter","bind_lob_setval_idx()",OCI_ISLOB_TYPE(bd->type));

	idx=bd->flag?0:idx;
	if(bd->idct[idx]==(short)OCI_INIT_IDCT_VAL)
		bd->cur++;

	if(data==UNDEF || data==NULL){
		bd->idct[idx]=-1;
		ret=TRUE;
	}
	else{
		rlen=size;
		bd->idct[idx]=0;
		if(bd->type==OCI_CLOB && size==0)
			rlen=size=strlen((char*)data);

		if(size==0){
			check_err(OCI_WAR_WR_ZERO,NULL,"Warning write zero-data","bind_lob_setval_idx()");
			ret=TRUE;
		}
		else{
			lb=(OCI_Lob)bd->data;
			lob=lb->lob[idx];

			if(bd->bdtype==OCI_BINDT_STC)
				*((ub2*)bd->size+idx)=86;
			else
				*((ub4*)bd->size+idx)=86;

			if(lb->off[idx]==0)
				lb->off[idx]=1;

			terr=OCI_NewErr();
			if(!terr){
				check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","bind_lob_setval_idx()");
				exit(OCI_ERR_HDL_ALLOC);
			}

			flag=OCILobWrite(bd->pstmt->pconn->ctx,terr->err,lob,&rlen,off?off:lb->off[idx],(void*)data,size,lb->piece,0,0,lb->csid,lb->csfrm);
			ret=check_err(flag,terr,"OCILobWrite() error","bind_lob_setval_idx()");
			if(rlen==0)
				check_err(OCI_WAR_WR_ZERO,NULL,"Warning zero-data be writed","bind_lob_setval_idx()");
			if(rsize!=NULL)
				*rsize=rlen;
			if(ret==TRUE && off==0)
				lb->off[idx]+=rlen;
			OCI_FreeErr(terr);
			return ret;
		}	
	}
	return ret;
}


sb4 bdlobreadback_out(void  *ctxp,const void  *bufp,ub4 len,ub1 piece){
	switch(piece){
	case OCI_ONE_PIECE:
		printf("....OCI_ONE_PIECE....\n");
		break;
	case OCI_FIRST_PIECE:
		printf("....OCI_FIRST_PIECE....\n");
		break;
	case OCI_NEXT_PIECE:
		printf("....OCI_NEXT_PIECE....\n");
		break;
	case OCI_LAST_PIECE:
		printf("....OCI_LAST_PIECE....\n");
		break;
	default:
		printf("....UNKOWN-PIECE....\n");
	}
	printf("value:%.*s\nlen:%d\n",len,bufp,len);
	return OCI_SUCCESS;
}

OCI_GETSTAT bind_lob_getval_idx(OCI_Bind bd,ushort idx,size_t off,void *data,size_t *size,size_t *rsize){
	BOOL	ret;
	OCI_Lob	lb;
	OCI_LobLocator lob;
	sword	flag;
	ub4		rlen,alen;
	OCI_Err	terr;
	
	assert_error("Invalid pointer or argument","bind_lob_getval_idx()",data && size);
	if(bind_get_isvalid(bd,idx)==FALSE)
		return OCI_GETSTAT_ERROR;
	assert_error("Invalid parameter","bind_lob_getval_idx()",OCI_ISLOB_TYPE(bd->type));

	idx=bd->flag?0:idx;
	if(bd->idct[idx]==-1)
		return OCI_GETSTAT_NULL;
	if(bd->idct[idx]==(short)OCI_INIT_IDCT_VAL)
		return OCI_GETSTAT_ERROR;

	if(*size==0){
		check_err(OCI_WAR_RD_ZERO,0,"Buffer is zero-length ","bind_lob_getval_idx()");
		return OCI_GETSTAT_ERROR;
	}
	
	bzero(data,*size);
	lb=(OCI_Lob)bd->data;
	lob=lb->lob[idx];

	if(lb->off[idx]==0)
		lb->off[idx]=1;

	/*警告：如果OCI_CLOB读取的数据存在双字节，则读取字符数要比缓冲字节数的二分之一小，否则报OCI_NEED_DATA错误，如果不处理（再次读取数据），会引起不可预测的OCI通信错误*/
	alen=rlen=bd->type==OCI_CLOB?rlen=(*size)/OCI_CLOB_SIGBYTE:(*size);
	if(rlen==0){
		check_err(OCI_WAR_RD_ZERO,0,"Buffer is too small","bind_lob_getval_idx()");
		*size=0;
		return OCI_GETSTAT_ERROR;
	}

	terr=OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","bind_lob_setval_idx()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	flag=OCILobRead(bd->pstmt->pconn->ctx,terr->err,lob,&rlen,off?off:lb->off[idx],(void*)data,*size,0,0,lb->csid,lb->csfrm);
	ret=check_err(flag,terr,"OCILobRead() error","bind_lob_setval_idx()");
	OCI_FreeErr(terr);
	if(!ret && flag!=OCI_NEED_DATA)
		return OCI_GETSTAT_ERROR;

	*size=rlen;
	if(rlen==0){
		check_err(OCI_WAR_WR_ZERO,NULL,"There is not data","bind_lob_setval_idx()");
		return OCI_GETSTAT_NONDATA;
	}

	if(off==0)
		lb->off[idx]+=bd->type==OCI_CLOB?(alen<rlen?alen:rlen):rlen;//???

	return OCI_GETSTAT_SUCCESS;
}


BOOL	OCI_BindLobSetvalIdx(OCI_Bind bd,ushort idx,size_t off,const void *data,size_t len){
	return bind_lob_setval_idx(bd,idx,off,data,len,NULL);
}

BOOL	OCI_BindLobSetval(OCI_Bind bd,size_t off,const void *data,size_t len){
	assert_error("Invalid pointer","OCI_BindLobSetval()",bd && bd->data);
	if(!bd->flag)
		assert_error("Invalid pointer","OCI_BindLobSetval()",bd->iter!=-1);
	return bind_lob_setval_idx(bd,bd->iter,off,data,len,NULL);
}

/*在指定偏移处进行值写入，内部偏移指示器不改变*/
OCI_GETSTAT OCI_BindLobGetvalIdx(OCI_Bind bd,ushort idx,size_t off,void *data,size_t *len){
	return bind_lob_getval_idx(bd,idx,off,data,len,NULL);
}

OCI_GETSTAT OCI_BindLobGetval(OCI_Bind bd,size_t off,void *data,size_t *len){
	assert_error("Invalid pointer","OCI_BindLobGetval()",bd && bd->data);
	if(!bd->flag)
		assert_error("Invalid pointer","OCI_BindLobGetval()",bd->iter!=-1);
	return bind_lob_getval_idx(bd,bd->iter,off,data,len,NULL);
}

BOOL	bind_get_isvalid(OCI_Bind bd,ushort idx){
	assert_error("Invalid pointer","bind_get_isvalid()",bd && bd->data);
	idx=bd->flag?0:idx;
	if(bd->bdtype==OCI_BINDT_DNC){
		if(bd->cur==0){
			check_err(OCI_WAR_DATA_INVALID,NULL,"There is not data","bind_get_isvalid()");
			return FALSE;
		}
		else
			assert_error("Invalid argument","bind_get_isvalid()",idx<bd->cur);
	}
	else{
		if(!bd->flag && bd->cur==0){//???
			check_err(OCI_WAR_DATA_INVALID,NULL,"There is not data","bind_get_isvalid()");
			return FALSE;
		}
		assert_error("Invalid argument","bind_get_isvalid()",idx<bd->mem);
	}
	return TRUE;
}

BOOL	bind_set_isvalid(OCI_Bind bd,ushort idx){
	assert_error("Invalid pointer","bind_set_isvalid()",bd && bd->data);
	idx=bd->flag?0:idx;
	if(bd->bdtype==OCI_BINDT_STC){
		assert_error("Invalid argument","bind_set_isvalid()",idx<bd->mem);
	}
	else{
		if(!bd->cur){
			check_err(OCI_WAR_DATA_INVALID,NULL,"No valid data in dynamic bind","bind_set_isvalid()");
			return FALSE;
		}
		assert_error("Invalid argument","bind_set_isvalid()",idx<bd->cur);
	}
	return TRUE;
}

BOOL		bind_lob_getlen_idx(OCI_Bind bd,ushort idx,size_t *size){
	BOOL	ret;
	sword	flag;
	OCI_LobLocator	lb;
	OCI_Lob	lob;
	OCI_Err	terr;

	assert_error("Invalid pointer","bind_lob_getlen_idx()",size);
	assert_error("Invalid parameter","bind_lob_getlen_idx()",OCI_ISLOB_TYPE(bd->type));
	if(!bind_get_isvalid(bd,idx))
		return FALSE;
	

	idx=bd->flag?0:idx;
	lob=(OCI_Lob)bd->data;
	lb=lob->lob[idx];

	terr=OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","bind_lob_getlen_idx()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	flag=OCILobGetLength(bd->pstmt->pconn->ctx,terr->err,lb,size);
	ret=check_err(flag,terr,"OCILobGetLength()error","bind_lob_getlen_idx()");
	OCI_FreeErr(terr);
	return ret;
}

BOOL		OCI_BindLobGetLen(OCI_Bind bd,size_t *size){
	assert_error("Invalid pointer","BdLobGetglen()",bd && bd->data);
	if(!bd->flag)
		assert_error("Invalid pointer","BdLobGetglen()",bd->iter!=-1);
	return bind_lob_getlen_idx(bd,bd->iter,size);
}

BOOL		OCI_BindLobGetLenIdx(OCI_Bind bd,ushort idx,size_t *size){
	return bind_lob_getlen_idx(bd,idx,size);
}

void		bind_lob_resetoff_idx(OCI_Bind bd,ushort idx){
	OCI_Lob	lob;
	assert_error("Invalid pointer","bind_lob_resetoff_idx()",bd && bd->data);
	assert_error("Invalid parameter","bind_lob_resetoff_idx()",OCI_ISLOB_TYPE(bd->type));
	if(!bind_get_isvalid(bd,idx))
		return;
	idx=bd->flag?0:idx;
	assert_error("Invalid argument","bind_lob_resetoff_idx()",idx<bd->mem);
	lob=(OCI_Lob)bd->data;
	lob->off[idx]=1;
}

void		OCI_BindLobResetOff(OCI_Bind bd){
	assert_error("Invalid pointer","OCI_BindLobResetOff()",bd && bd->data);
	if(!bd->flag)
		assert_error("Invalid pointer","OCI_BindLobResetOff()",bd->iter!=-1);
	bind_lob_resetoff_idx(bd,bd->iter);
}

void		OCI_BindLobResetOffIdx(OCI_Bind bd,ushort idx){
	bind_lob_resetoff_idx(bd,idx);
}

void		OCI_BindAllLobResetOffIdx(ushort idx,OCI_Bind bd,...){
	va_list vp;
	va_start(vp,bd);
	do{
		bind_lob_resetoff_idx(bd,idx);
		bd=va_arg(vp,OCI_Bind);
	}while(bd!=0);
	va_end(vp);
	return;
}

BOOL	bind_lob_erase(OCI_Bind bd,ushort idx,size_t len,size_t off){
	OCI_Lob	lob;
	BOOL	ret;
	OCI_Err	terr;

	assert_error("Invalid parameter","bind_lob_resetoff_idx()",OCI_ISLOB_TYPE(bd->type));
	if(!bind_get_isvalid(bd,idx))
		return FALSE;
	
	idx=bd->flag?0:idx;
	lob=(OCI_Lob)bd->data;
	
	//如果为off不为0，则使用off做偏移，否则flag为真时用设置偏移，为假时用获取偏移。
	off=off?off:lob->off[idx];
	
	if(len==0 && !bind_lob_getlen_idx(bd,idx,&len))
		return FALSE;

	terr=OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","bind_lob_erase()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	ret=check_err(OCILobErase(bd->pstmt->pconn->ctx,terr->err,lob->lob[idx],&len,off?off:1),terr,"OCILobErase()","bdlogerease()");

	OCI_FreeErr(terr);
	return ret;
}

BOOL	OCI_BindLobErase(OCI_Bind bd,size_t off,size_t len){
	assert_error("Invalid pointer","OCI_BindLobErase()",bd);
	if(!bd->flag)
		assert_error("Invalid parameter","OCI_BindLobErase()",bd->iter!=-1);
	return bind_lob_erase(bd,bd->iter,len,off);
}

BOOL	OCI_BindLobEraseIdx(OCI_Bind bd,ushort idx,size_t off,size_t len){
	return bind_lob_erase(bd,idx,len,off);
}

BOOL	bind_lob_truncate(OCI_Bind bd,ushort idx,size_t len){
	OCI_Lob	lob;
	BOOL	ret;
	OCI_Err	terr;

	assert_error("Invalid parameter","bind_lob_truncate()",OCI_ISLOB_TYPE(bd->type));
	if(!bind_get_isvalid(bd,idx))
		return FALSE;
	
	idx=bd->flag?0:idx;
	lob=(OCI_Lob)bd->data;
	terr=OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","bind_lob_erase()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	ret=check_err(OCILobTrim(bd->pstmt->pconn->ctx,terr->err,lob->lob[idx],len),terr,"OCILobTrim() error","bind_lob_truncate()");
	OCI_FreeErr(terr);
	return ret;
}

BOOL	OCI_BindLobTrunc(OCI_Bind bd,size_t len){
	assert_error("Invalid pointer","OCI_BindLobErase()",bd);
	if(!bd->flag)
		assert_error("Invalid parameter","OCI_BindLobErase()",bd->iter!=-1);
	return bind_lob_truncate(bd,bd->iter,len);

}

BOOL	OCI_BindLobTruncIdx(OCI_Bind bd,ushort idx,size_t len){
	return bind_lob_truncate(bd,idx,len);
}

/**************************************日期操作函数*******************************************/
BOOL	OCI_DateToString(const OCIDate *date,string buf,size_t len,const string format,OCI_Err err){
	OCI_Err	terr;
	sword	flag;
	BOOL	ret;
	size_t	tlen;

	assert_error("Invalid argument","OCI_DateToString()",date && buf && len);

	terr=err?err:OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_DateToString()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	bzero(buf,len);
	len-=1;
	tlen=format?strlen((char *)format):strlen((char *)OCI_DATEFULL_FORMAT);
	flag=OCIDateToText(terr->err,date,(text *)(format?format:OCI_DATEFULL_FORMAT),tlen,(text*)OCI_DATE_LANGUAGE,strlen((char*)OCI_DATE_LANGUAGE),&len,buf);
	buf[len]='\0';
	ret=check_err(flag,terr,"OCIDateToText() error","OCI_DateToString()");
	if(!err)OCI_FreeErr(terr);
	return ret;
}

BOOL	OCI_StringToDate(const string buf,OCIDate *date,const string format,OCI_Err err){
	OCI_Err	terr;
	sword	flag;
	BOOL	ret;
	size_t	tlen;

	assert_error("Invalid argument","OCI_StringToDate()",date && buf);
	terr=err?err:OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_StringToDate()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	bzero(date,OCI_DATE_BYTES);
	tlen=format?strlen((char *)format):strlen((char *)OCI_DATEFULL_FORMAT);
	flag=OCIDateFromText(terr->err,buf,strlen((char*)buf),(text *)(format?format:OCI_DATEFULL_FORMAT),tlen,(text*)OCI_DATE_LANGUAGE,strlen((char*)OCI_DATE_LANGUAGE),date);
	ret=check_err(flag,terr,"OCIDateFromText() error","OCI_StringToDate()");
	if(!err)OCI_FreeErr(terr);
	return ret;
}

BOOL	OCI_DateSetDate(OCIDate *date,short year,char mon,char day){
	assert_error("Invalid argument","OCI_DateSetDate()",date);
	OCIDateSetDate(date,year,mon,day);
	return check_date(date,0);
}

BOOL	OCI_DateSetTime(OCIDate *date,char hour,char min,char sec){
	assert_error("Invalid argument","OCI_DateSetTime()",date);
	OCIDateSetTime(date,hour,min,sec);
	return check_date(date,0);
}

BOOL	OCI_DateGetDate(const OCIDate *date,short *year,char *mon,char *day){
	BOOL	ret;

	assert_error("Invalid argument","OCI_DateGetDate()",date && year && mon && day);
	ret=check_date(date,0);
	if(ret)
		OCIDateGetDate(date,year,mon,day);
	return ret;
}

BOOL	OCI_DateGetTime(const OCIDate *date,char *hour,char *min,char *sec){
	BOOL	ret;

	assert_error("Invalid argument","OCI_DateGetTime()",date && hour && min && sec);
	ret=check_date(date,0);
	if(ret)
		OCIDateGetTime(date,hour,min,sec);
	return ret;	
}

BOOL	OCI_DateAddDays(const OCIDate *old_dt,OCIDate *new_dt,int days){
	OCI_Err	terr;
	sword	flag;
	BOOL	ret;

	assert_error("Invalid argument","OCI_DateAddDays()",old_dt && new_dt);
	terr=OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_DateAddDays()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	flag=OCIDateAddDays(terr->err,old_dt,days,new_dt);
	ret=check_err(flag,terr,"OCIDateAddDays() error","OCI_DateAddDays()");
	OCI_FreeErr(terr);
	return ret;	
}

BOOL	OCI_DateAddMons(const OCIDate *old_dt,OCIDate *new_dt,int mons){
	OCI_Err	terr;
	sword	flag;
	BOOL	ret;

	assert_error("Invalid argument","OCI_DateAddMons()",old_dt && new_dt);
	terr=OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_DateAddMons()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	flag=OCIDateAddMonths(terr->err,old_dt,mons,new_dt);
	ret=check_err(flag,terr,"OCIDateAddMonths() error","OCI_DateAddMons()");
	OCI_FreeErr(terr);
	return ret;	
}

BOOL	OCI_DateNextDay(const OCIDate *old_dt,OCIDate *new_dt,const string day){
	OCI_Err	terr;
	sword	flag;
	BOOL	ret;
	size_t	len;

	assert_error("Invalid argument","OCI_DateNextDay()",old_dt && new_dt);
	terr=OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_DateNextDay()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	len=day?strlen((char*)day):strlen((char*)OCI_NEXTDAY_MON);
	flag=OCIDateNextDay(terr->err,old_dt,(text*)(day?day:OCI_NEXTDAY_MON),len,new_dt);
	ret=check_err(flag,terr,"OCIDateNextDay() error","OCI_DateNextDay()");
	OCI_FreeErr(terr);
	return ret;	
}

BOOL	OCI_DateLastDay(const OCIDate *old_dt,OCIDate *new_dt){
	OCI_Err	terr;
	sword	flag;
	BOOL	ret;

	assert_error("Invalid argument","OCI_DateLastDay()",old_dt && new_dt);
	terr=OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_DateLastDay()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	flag=OCIDateLastDay(terr->err,old_dt,new_dt);
	ret=check_err(flag,terr,"OCIDateLastDay() error","OCI_DateLastDay()");
	OCI_FreeErr(terr);
	return ret;	
}

BOOL	OCI_DateDaysBetween(const OCIDate *dt1,const OCIDate *dt2,int *days){
	OCI_Err	terr;
	sword	flag;
	BOOL	ret;

	assert_error("Invalid argument","OCI_DateDaysBetween()",dt1 && dt2 && days);
	terr=OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_DateDaysBetween()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	flag=OCIDateDaysBetween(terr->err,dt1,dt2,days);
	ret=check_err(flag,terr,"OCIDateDaysBetween() error","OCI_DateDaysBetween()");
	OCI_FreeErr(terr);
	return ret;	
}

BOOL	OCI_DateLocalDate(OCIDate *date){
	OCI_Err	terr;
	sword	flag;
	BOOL	ret;

	assert_error("Invalid argument","OCI_DateLocalDate()",date);
	terr=OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_DateLocalDate()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	flag=OCIDateSysDate(terr->err,date);
	ret=check_err(flag,terr,"OCIDateSysDate() error","OCI_DateLocalDate()");
	OCI_FreeErr(terr);
	return ret;	
}

BOOL	check_date(const OCIDate *dt,OCI_Err err){
	sword	flag;
	uword	check;
	BOOL	ret;
	OCI_Err	terr;

	assert_error("Invalid argument","check_date()",dt);
	terr=err?err:OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","check_date()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	flag=OCIDateCheck(terr->err,dt,&check);
	ret=check_err(flag,terr,"OCIDateCheck() error","check_date()");
	if(ret && check){
		if((check & OCI_DATE_INVALID_DAY) || (check & OCI_DATE_DAY_BELOW_VALID))
			check_err(OCI_ERR_DATE_INVALID,terr,"Invalid 'day' in the pointer Date","check_date()");
		else if((check & OCI_DATE_INVALID_MONTH) || (check & OCI_DATE_MONTH_BELOW_VALID))
			check_err(OCI_ERR_DATE_INVALID,terr,"Invalid 'month' in the pointer Date","check_date()");
		else if((check & OCI_DATE_INVALID_YEAR) || (check & OCI_DATE_YEAR_BELOW_VALID))
			check_err(OCI_ERR_DATE_INVALID,terr,"Invalid 'year' in the pointer Date","check_date()");
		else if((check & OCI_DATE_INVALID_SECOND) || (check & OCI_DATE_SECOND_BELOW_VALID))
			check_err(OCI_ERR_DATE_INVALID,terr,"Invalid 'second' in the pointer Date","check_date()");
		else if((check & OCI_DATE_INVALID_MINUTE) || (check & OCI_DATE_MINUTE_BELOW_VALID))
			check_err(OCI_ERR_DATE_INVALID,terr,"Invalid 'minute' in the pointer Date","check_date()");
		else if((check & OCI_DATE_INVALID_HOUR) || (check & OCI_DATE_HOUR_BELOW_VALID))
			check_err(OCI_ERR_DATE_INVALID,terr,"Invalid 'hour' in the pointer Date","check_date()");
		else
			check_err(OCI_ERR_DATE_INVALID,terr,"Other error in the pointer Date","check_date()");

		ret=FALSE;
	}
	if(!err)OCI_FreeErr(terr);
	return ret;
}

BOOL	check_datetime(const OCI_DateTime dt,OCI_Err err){
	sword	flag;
	ub4		check;
	BOOL	ret;
	OCI_Err	terr;

	assert_error("Invalid argument","check_datetime()",dt);
	terr=err?err:OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","check_datetime()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	flag=OCIDateTimeCheck(__OCI_ENV_DATA__.env,terr->err,dt->dt,&check);
	ret=check_err(flag,terr,"OCIDateCheck() error","check_date()");
	if(ret && check){
		if((check & OCI_DATE_INVALID_DAY) || (check & OCI_DATE_DAY_BELOW_VALID))
			check_err(OCI_ERR_DATE_INVALID,terr,"Invalid 'day' in the pointer Date","check_datetime()");
		else if((check & OCI_DATE_INVALID_MONTH) || (check & OCI_DATE_MONTH_BELOW_VALID))
			check_err(OCI_ERR_DATE_INVALID,terr,"Invalid 'month' in the pointer Date","check_datetime()");
		else if((check & OCI_DATE_INVALID_YEAR) || (check & OCI_DATE_YEAR_BELOW_VALID))
			check_err(OCI_ERR_DATE_INVALID,terr,"Invalid 'year' in the pointer Date","check_datetime()");
		else if((check & OCI_DATE_INVALID_SECOND) || (check & OCI_DATE_SECOND_BELOW_VALID))
			check_err(OCI_ERR_DATE_INVALID,terr,"Invalid 'second' in the pointer Date","check_datetime()");
		else if((check & OCI_DATE_INVALID_MINUTE) || (check & OCI_DATE_MINUTE_BELOW_VALID))
			check_err(OCI_ERR_DATE_INVALID,terr,"Invalid 'minute' in the pointer Date","check_datetime()");
		else if((check & OCI_DATE_INVALID_HOUR) || (check & OCI_DATE_HOUR_BELOW_VALID))
			check_err(OCI_ERR_DATE_INVALID,terr,"Invalid 'hour' in the pointer Date","check_datetime()");
		/*else if((check & OCI_DATE_INVALID_TIMEZONE))
			check_err(OCI_ERR_DATE_INVALID,terr,"Invalid 'time zero' in the pointer Date","check_datetime()");*/
		else
			check_err(OCI_ERR_DATE_INVALID,terr,"Other error in the pointer Date","check_datetime()");

		ret=FALSE;
	}
	if(!err)OCI_FreeErr(terr);
	return ret;
}

OCI_DateTime	OCI_NewDateTime(uint type){
	OCI_DateTime dt;
	
	dt=New(OCI_DateTime);
	dt->type=type;
	if(!check_err(OCIDescriptorAlloc(__OCI_ENV_DATA__.env,(void**)&dt->dt,type,0,0),0,"OCIDescriptorAlloc() error","OCI_NewDateTime()"))
		return NULL;
	return dt;
}

void	OCI_FreeDateTime(OCI_DateTime dt){
	assert_error("Invalid pointer","OCI_FreeDateTime()",dt);
	check_err(OCIDescriptorFree(dt->dt,dt->type),0,"OCIDescriptorFree() error","OCI_FreeDateTime()");
}

BOOL	OCI_DateTimeGetDate(const OCI_DateTime dt,short *year,uchar *mon,uchar *day){
	BOOL	ret;
	OCI_Err	err;

	err=OCI_NewErr();
	if(!err){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_DateTimeGetDate()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	assert_error("Invalid argument","OCI_DateGetTime()",dt && year && mon && day);
	ret=check_datetime(dt,err);
	if(ret)
		ret=check_err(OCIDateTimeGetDate(__OCI_ENV_DATA__.env,err->err,dt->dt,(sb2*)year,(ub1*)mon,(ub1*)day),err,"OCIDateTimeGetDate() error","OCI_DateTimeGetDate()");
	OCI_FreeErr(err);
	return ret;
}

BOOL	OCI_DateTimeGetTime(const OCI_DateTime dt,uchar *hour,uchar *min,uchar *sec,unsigned *fsec){
	BOOL	ret;
	OCI_Err	err;

	err=OCI_NewErr();
	if(!err){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_DateTimeGetTime()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	assert_error("Invalid argument","OCI_DateGetTime()",dt && hour && min && sec && fsec);
	ret=check_datetime(dt,err);
	if(ret)
		ret=check_err(OCIDateTimeGetTime(__OCI_ENV_DATA__.env,err->err,dt->dt,(ub1*)hour,(ub1*)min,(ub1*)sec,(ub4*)fsec),err,"OCIDateTimeGetDate() error","OCI_DateTimeGetDate()");
	OCI_FreeErr(err);
	return ret;
}

BOOL	OCI_DateTimeToString(const OCI_DateTime dt,string buf,size_t len,const string format,OCI_Err err){
	OCI_Err	terr;
	sword	flag;
	BOOL	ret;
	size_t	tlen;

	assert_error("Invalid argument","OCI_DateTimeToString()",dt && buf && len);
	
	terr=err?err:OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_DateTimeToString()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	ret=check_datetime(dt,terr);
	if(!ret)
		goto err_ret;
	bzero(buf,len);
	len-=1;
	tlen=format?strlen((char *)format):(OCI_TIMESTAMP_FORMAT?strlen((char *)OCI_TIMESTAMP_FORMAT):0);
	flag=OCIDateTimeToText(__OCI_ENV_DATA__.env,terr->err,dt->dt,(text *)(format?format:OCI_TIMESTAMP_FORMAT),tlen,9,0,0,&len,buf);
	ret=check_err(flag,terr,"OCIDateTimeToText() error","OCI_DateTimeToString()");
	buf[len]='\0';
err_ret:
	if(!err)OCI_FreeErr(terr);
	return ret;
}

BOOL	OCI_StringToDateTime(const string buf,OCI_DateTime dt,const string format,OCI_Err err){
	OCI_Err	terr;
	sword	flag;
	BOOL	ret;
	size_t	tlen;

	assert_error("Invalid argument","OCI_StringToDateTime()",dt && buf);
	
	terr=err?err:OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_StringToDateTime()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	ret=check_datetime(dt,terr);
	if(!ret)
		goto err_ret;
	tlen=format?strlen((char *)format):(OCI_TIMESTAMP_FORMAT?strlen((char *)OCI_TIMESTAMP_FORMAT):0);
	flag=OCIDateTimeFromText(__OCI_ENV_DATA__.env,terr->err,(text*)buf,strlen((char*)buf),(text *)(format?format:OCI_TIMESTAMP_FORMAT),tlen,0,0,dt->dt);
	ret=check_err(flag,terr,"OCIDateTimeFromText() error","OCI_StringToDateTime()");
err_ret:
	if(!err)OCI_FreeErr(terr);
	return ret;
}

BOOL	OCI_DateTimeConstruct(OCI_DateTime dt,short year,uchar month,uchar day,uchar hour,uchar min,uchar sec,uint fsec,string timezone){
	OCI_Err	terr;
	sword	flag;
	BOOL	ret;
	size_t	tlen;
	
	assert_error("Invalid pointer","OCI_DateTimeConstruct()",dt);
	terr=OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_DateTimeConstruct()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	
	tlen=timezone?strlen((char *)timezone):strlen((char *)OCI_TIMEZONE_DEF);
	flag=OCIDateTimeConstruct(__OCI_ENV_DATA__.env,terr->err,dt->dt,year,month,day,hour,min,sec,fsec,(text *)(timezone?timezone:OCI_TIMEZONE_DEF),tlen);
	ret=check_err(flag,terr,"OCIDateTimeConstruct() error","OCI_DateTimeConstruct()");
	ret=check_datetime(dt,terr);
	OCI_FreeErr(terr);
	return ret;
}

BOOL	OCI_DateTimeLocalTimeStamp(OCI_DateTime dt){
	OCI_Err	terr;
	BOOL	ret;
	
	assert_error("Invalid pointer","OCI_DateTimeLocalDateTime()",dt);
	terr=OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_DateTimeLocalDateTime()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	ret=check_err(OCIDateTimeSysTimeStamp(__OCI_ENV_DATA__.env,terr->err,dt->dt),terr,"OCIDateTimeSysTimeStamp() error","OCI_DateTimeLocalTimeStamp()");
	return ret;
}

BOOL	OCI_DateTimeGetTimeZone(const OCI_DateTime dt,char *hour,char *min){
	OCI_Err err;
	BOOL	ret;
	assert_error("Invalid pointer","OCI_DateTimeGetTimeZone()",dt && hour && min);
	err=OCI_NewErr();
	if(!err){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_DateTimeGetTimeZone()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	if(!check_datetime(dt,err)){
		ret=FALSE;
		goto ret_err;
	}
	if(!check_err(OCIDateTimeGetTimeZoneOffset(__OCI_ENV_DATA__.env,err->err,dt->dt,(sb1*)hour,(sb1*)min),err,"OCIDateTimeGetTimeZoneOffset() error","OCI_DateTimeGetTimeZone"))
		ret=FALSE;
	else
		ret=TRUE;
ret_err:
	OCI_FreeErr(err);
	return ret;
}

BOOL	OCI_DateTimeGetTimeZoneName(const OCI_DateTime dt,string name,size_t len){
	OCI_Err err;
	BOOL	ret;
	assert_error("Invalid pointer","OCI_DateTimeGetTimeZoneName()",dt && name && len);
	err=OCI_NewErr();
	if(!err){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","OCI_DateTimeGetTimeZoneName()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	if(!check_datetime(dt,err)){
		ret=FALSE;
		goto ret_err;
	}
	bzero(name,len);
	if(!check_err(OCIDateTimeGetTimeZoneName(__OCI_ENV_DATA__.env,err->err,dt->dt,(ub1*)name,&len),err,"OCIDateTimeGetTimeZoneName() error","OCI_DateTimeGetTimeZoneName"))
		ret=FALSE;
	else
		ret=TRUE;
	name[len]='\0';
ret_err:
	OCI_FreeErr(err);
	return ret;
}

/**************************************ORACLE的number类型操作函数*******************************************/
void	OCI_NumberSetZero(OCINumber *num){
	OCI_Err err;
	assert_error("Invalid pointer","OCI_NumberSetZero()",num);
	err=OCI_NewErr();
	assert_error("Can't create internal error-handle","OCI_NumberSetZero()",err);
	bzero(num,sizeof(OCINumber));
	OCINumberSetZero(err->err,num);
	OCI_FreeErr(err);
	return;
}
/*加、减、乘、除、模、乘方*/
BOOL	OCI_NumberAdd(const OCINumber *num1,const OCINumber *num2,OCINumber *num){
	BOOL	ret;
	OCI_Err err;
	assert_error("Invalid pointer","OCI_NumberAdd()",num1 && num2 && num);
	err=OCI_NewErr();
	assert_error("Can't create internal error-handle","OCI_NumberAdd()",err);
	ret=check_err(OCINumberAdd(err->err,num1,num2,num),err,"OCINumberAdd() error","OCI_NumberAdd()");
	OCI_FreeErr(err);
	return ret;
}
BOOL	OCI_NumberSub(const OCINumber *num1,const OCINumber *num2,OCINumber *num){
	BOOL	ret;
	OCI_Err err;
	assert_error("Invalid pointer","OCI_NumberSub()",num1 && num2 && num);
	err=OCI_NewErr();
	assert_error("Can't create internal error-handle","OCI_NumberAdd()",err);
	ret=check_err(OCINumberSub(err->err,num1,num2,num),err,"OCINumberSub() error","OCI_NumberSub()");
	OCI_FreeErr(err);
	return ret;
}
BOOL	OCI_NumberMul(const OCINumber *num1,const OCINumber *num2,OCINumber *num){
	BOOL	ret;
	OCI_Err err;
	assert_error("Invalid pointer","OCI_NumberMul()",num1 && num2 && num);
	err=OCI_NewErr();
	assert_error("Can't create internal error-handle","OCI_NumberAdd()",err);
	ret=check_err(OCINumberMul(err->err,num1,num2,num),err,"OCINumberMul() error","OCI_NumberMul()");
	OCI_FreeErr(err);
	return ret;
}
BOOL	OCI_NumberDiv(const OCINumber *num1,const OCINumber *num2,OCINumber *num){
	BOOL	ret;
	OCI_Err err;
	assert_error("Invalid pointer","OCI_NumberDiv()",num1 && num2 && num);
	err=OCI_NewErr();
	assert_error("Can't create internal error-handle","OCI_NumberAdd()",err);
	ret=check_err(OCINumberDiv(err->err,num1,num2,num),err,"OCINumberDiv() error","OCI_NumberDiv()");
	OCI_FreeErr(err);
	return ret;
}
BOOL	OCI_NumberMod(const OCINumber *num1,const OCINumber *num2,OCINumber *num){
	BOOL	ret;
	OCI_Err err;
	assert_error("Invalid pointer","OCI_NumberMod()",num1 && num2 && num);
	err=OCI_NewErr();
	assert_error("Can't create internal error-handle","OCI_NumberAdd()",err);
	ret=check_err(OCINumberMod(err->err,num1,num2,num),err,"OCINumberMod() error","OCI_NumberMod()");
	OCI_FreeErr(err);
	return ret;
}
BOOL	OCI_NumberIntPower(const OCINumber *num1,int exponent,OCINumber *num){
	BOOL	ret;
	OCI_Err err;
	assert_error("Invalid pointer","OCI_NumberIntPower()",num1 && num);
	err=OCI_NewErr();
	assert_error("Can't create internal error-handle","OCI_NumberIntPower()",err);
	ret=check_err(OCINumberIntPower(err->err,num1,exponent,num),err,"OCINumberIntPower() error","OCI_NumberIntPower()");
	OCI_FreeErr(err);
	return ret;
}
/*转换*/
BOOL	OCI_NumberToString(const OCINumber *num1,string buf,size_t len,const string format,OCI_Err err){
	BOOL	ret;
	OCI_Err terr;
	size_t	fmtlen;
	assert_error("Invalid pointer","OCI_NumberToString()",num1 && buf && len);
	terr=err?err:OCI_NewErr();
	assert_error("Can't create internal error-handle","OCI_NumberToString()",terr);

	fmtlen=format?strlen((const char*)format):strlen((const char*)OCI_NUMBER_FORMAT);
	ret=check_err(OCINumberToText(terr->err,num1,(const text*)(format?format:OCI_NUMBER_FORMAT),fmtlen,0,0,&len,(text*)buf),terr,"OCINumberToText() error","OCI_NumberToString()");
	if(!err)OCI_FreeErr(terr);
	return ret;
}

BOOL	OCI_StringToNumber(const string buf,OCINumber *num1,const string format,OCI_Err err){
	BOOL	ret;
	OCI_Err terr;
	size_t	fmtlen;
	assert_error("Invalid pointer","OCI_StringToNumber()",num1 && buf);
	terr=err?err:OCI_NewErr();
	assert_error("Can't create internal error-handle","OCI_StringToNumber()",terr);

	fmtlen=format?strlen((const char*)format):strlen((const char*)OCI_NUMBER_FORMAT);
	ret=check_err(OCINumberFromText(terr->err,(const text*)buf,strlen((const char*)buf),(const text*)(format?format:OCI_NUMBER_FORMAT),fmtlen,0,0,num1),err,"OCINumberFromText() error","OCI_StringToNumber()");
	if(!err)OCI_FreeErr(terr);
	return ret;
}

BOOL	OCI_NumberToInt(const OCINumber *num1,void *result,ushort flag){
	BOOL	ret;
	OCI_Err terr;
	assert_error("Invalid pointer","OCI_NumberToInt()",num1 && result);
	terr=OCI_NewErr();
	assert_error("Can't create internal error-handle","OCI_NumberToInt()",terr);

	ret=check_err(OCINumberToInt(terr->err,num1,flag==OCI_NUMBER_SIGNED?sizeof(int):sizeof(unsigned),flag==OCI_NUMBER_SIGNED?OCI_NUMBER_SIGNED:OCI_NUMBER_UNSIGNED,result),terr,"OCINumberToInt() error","OCI_NumberToInt()");
	OCI_FreeErr(terr);
	return ret;
}
BOOL	OCI_IntToNumber(const void *result,OCINumber *num1,ushort flag){
	BOOL	ret;
	OCI_Err terr;
	assert_error("Invalid pointer","OCI_IntToNumber()",num1 && result);
	terr=OCI_NewErr();
	assert_error("Can't create internal error-handle","OCI_IntToNumber()",terr);

	ret=check_err(OCINumberFromInt(terr->err,result,flag==OCI_NUMBER_SIGNED?sizeof(int):sizeof(unsigned),flag==OCI_NUMBER_SIGNED?OCI_NUMBER_SIGNED:OCI_NUMBER_UNSIGNED,num1),terr,"OCINumberToInt() error","OCI_IntToNumber()");
	OCI_FreeErr(terr);
	return ret;
}

BOOL	OCI_NumberToReal(const OCINumber *num1,void *result,ushort flag){
	BOOL	ret;
	OCI_Err terr;
	size_t	len;
	assert_error("Invalid pointer","OCI_NumberToReal()",num1 && result);
	terr=OCI_NewErr();
	assert_error("Can't create internal error-handle","OCI_NumberToReal()",terr);
	switch(flag){
	case OCI_NUMBER_LDOUBLE:
		len=sizeof(long double);
		break;
	case OCI_NUMBER_DOUBLE:
		len=sizeof(double);
		break;
	default:
		len=sizeof(float);
	}
	ret=check_err(OCINumberToReal(terr->err,num1,len,result),terr,"OCINumberToReal() error","OCI_NumberToReal()");
	OCI_FreeErr(terr);
	return ret;
}
BOOL	OCI_RealToNumber(const void *result,OCINumber *num1,ushort flag){
	BOOL	ret;
	OCI_Err terr;
	size_t	len;
	assert_error("Invalid pointer","OCI_RealToNumber()",num1 && result);
	terr=OCI_NewErr();
	assert_error("Can't create internal error-handle","OCI_RealToNumber()",terr);
	switch(flag){
	case OCI_NUMBER_LDOUBLE:
		len=sizeof(long double);
		break;
	case OCI_NUMBER_DOUBLE:
		len=sizeof(double);
		break;
	default:
		len=sizeof(float);
	}
	ret=check_err(OCINumberFromReal(terr->err,result,len,num1),terr,"OCINumberFromReal() error","OCI_RealToNumber()");
	OCI_FreeErr(terr);
	return ret;
}

BOOL	res_init_colparam(OCI_ColParam cp,OCI_Res	rs,OCI_Err err){
	OCI_Err		terr;
	BOOL		flag;
	void		*dfaddr;
	sb4			dfsize;
	ub4			dfnext;

	assert_error("Invalid pointer or argument.","res_init_colparam()",rs && cp);

	cp->preset=rs;
	switch(cp->type){
	case SQLT_NUM:
		cp->dtype=OCI_NUM;
		cp->data=new_oci_data(rs,OCI_NUMBER_BYTES,OCI_ADT_TRESET);
		break;
	case SQLT_CHR:
	case SQLT_AFC:
		cp->dtype=OCI_STR;
		cp->data=new_oci_data(rs,cp->max+1,OCI_ADT_TRESET);
		break;
	case SQLT_BIN:
		cp->dtype=cp->type;
		cp->data=new_oci_data(rs,cp->max,OCI_ADT_TRESET);
		break;
	/*case OCI_LNG:
	case OCI_LGB:
		break;*/
	case SQLT_CLOB:
	case SQLT_BLOB:
		cp->dtype=cp->type;
		cp->data=new_oci_lob(rs,OCI_ADT_TRESET);
		break;
	case SQLT_DAT:
		cp->dtype=OCI_DATE;
		cp->data=new_oci_data(rs,OCI_DATE_BYTES,OCI_ADT_TRESET);
		break;
	case SQLT_TIMESTAMP:
		cp->dtype=OCI_TMSTAMP;
		cp->data=new_oci_datetime(rs,OCI_ADT_TRESET,OCI_DTYPE_TIMESTAMP);
		break;
	case SQLT_TIMESTAMP_TZ:
		cp->dtype=OCI_TMSTAMP_TZ;
		cp->data=new_oci_datetime(rs,OCI_ADT_TRESET,OCI_DTYPE_TIMESTAMP_TZ);
		break;
	case SQLT_TIMESTAMP_LTZ:
		cp->dtype=OCI_TMSTAMP_LTZ;
		cp->data=new_oci_datetime(rs,OCI_ADT_TRESET,OCI_DTYPE_TIMESTAMP_LTZ);
		break;
	default:
		check_err(OCI_ERR_BD_ERRTYPE,NULL,"Don't select this data-type","res_init_colparam()");
		return FALSE;
	}

	if(!cp->data)
		return FALSE;

	cp->idct=NewArray(rs->mem,sb2);
	cp->size=NewArray(rs->mem,ub2);
	cp->rcode=NewArray(rs->mem,ub2);
	Memset(cp->idct,OCI_INIT_IDCT,rs->mem*sizeof(sb2));

	if(OCI_ISLOB_TYPE(cp->dtype)){
		dfaddr=((OCI_Lob)cp->data)->lob;
		dfsize=-1;
		dfnext=sizeof(OCI_LobLocator);
	}
	else if(OCI_ISNORMAL_TYPE(cp->dtype)){
		dfaddr=((OCI_Data)cp->data)->data;
		dfnext=dfsize=((OCI_Data)cp->data)->max;	
	}
	else if(OCI_ISDT_TYPE(cp->dtype)){
		dfaddr=((OCI_TmStamp)cp->data)->dt;
		dfsize=-1;
		dfnext=sizeof(OCIDateTime*);
	}
	else
		assert_error("Invalid output type","res_init_colparam()",0);

	terr=err?err:OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","res_init_colparam()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	flag=check_err(OCIDefineByPos(rs->pstmt->stmt,&cp->definep,terr->err,cp->pos,dfaddr,dfsize,cp->dtype,cp->idct,cp->size,cp->rcode,OCI_DEFAULT),terr,"OCIDefineByPos() error","res_init_colparam()");
	if(!flag)
		goto ret;
	if(rs->mem>1)
		flag=check_err(OCIDefineArrayOfStruct(cp->definep,terr->err,dfnext,sizeof(sb2),sizeof(ub2),sizeof(ub2)),terr,"OCIDefineArrayOfStruct() error","res_init_colparam()");
ret:
	if(!err)OCI_FreeErr(terr);
	return flag?TRUE:FALSE;
}

BOOL	find_reset_from_stmt(OCI_Res data,OCI_Res rs){
	assert_error("Invalid pointer","find_reset_from_stmt()",data && rs);
	if(data == rs && data->id==rs->id)
		return TRUE;
	return FALSE;
}

BOOL	free_reset(OCI_Res rs,OCI_FREE_TYPE mode){
	int i;
	BOOL	ret;
	
	if(rs==NULL)
		return FALSE;
	/*释放列*/
	if(rs->values){
		for(i=0;i<rs->cols;i++){
			if(OCI_ISLOB_TYPE(rs->values[i].dtype))
				free_oci_lob((OCI_Lob)rs->values[i].data);
			else if(OCI_ISNORMAL_TYPE(rs->values[i].dtype))
				free_oci_data((OCI_Data)rs->values[i].data);
			else if(OCI_ISDT_TYPE(rs->values[i].dtype))
				free_oci_datetime((OCI_TmStamp)rs->values[i].data);
			else
				assert_error("Invalid output type","free_reset()",0);


			Free(rs->values[i].size);
			Free(rs->values[i].idct);
			Free(rs->values[i].rcode);
			Free(rs->values[i].name);
		}
		Free(rs->values);
	}
	
	/*被动调用*/
	if(mode==OCI_FREET_CHILD){
		node_p node;
		node=TravLink(rs->pstmt->rslink,(travlink_f)find_reset_from_stmt,rs);
		if(node==NULL){
			check_err(OCI_ERR_CS_MGRBD,NULL,"Managed resultset-link error","free_reset()");
			exit(OCI_ERR_CS_MGRBD);
		}
		DelNodeLink(node,rs->pstmt->rslink);
		FreeNode(node,NULL);
		ret=TRUE;
	}
	else
		ret=FALSE;
	Free(rs);
	return ret;
}

OCI_Res	new_reset(OCI_Stmt cs,ushort max,OCI_Err err){
	OCI_Err		terr;
	OCI_Res	rs;
	BOOL		flag;
	ushort		i;
	OCIParam	*par;
	ub4			namelen;
	char		*name;
	assert_error("Invalid pointer or argument.","new_reset()",cs && max);

	rs=New(OCI_Res);
	rs->mem=max>OCI_MAX_DEFINE?OCI_MAX_DEFINE:max;
	time(&rs->id);
	rs->pstmt=cs;

	rs->col_iter=rs->row_iter=-1;

	InsertAtHeadLink(rs,rs->pstmt->rslink);

	terr=err?err:OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","new_reset()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	/*得到列数*/
	flag=check_err(OCIStmtExecute(cs->pconn->ctx,cs->stmt,terr->err,1,0,0,0,OCI_DESCRIBE_ONLY),terr,"OCIStmtExecute() error","new_reset()");
	if(!flag)
		goto ret;
	flag=check_err(OCIAttrGet(cs->stmt,OCI_HTYPE_STMT,&rs->cols,0,OCI_ATTR_PARAM_COUNT,terr->err),terr,"OCIAttrGet() error","new_reset()");
	if(!flag)
		goto ret;
	rs->values=NewArray(rs->cols,_OCIColParam);
	for(par=NULL,i=0;i<rs->cols;i++){
		rs->values[i].pos=i+1;
		flag=check_err(OCIParamGet(cs->stmt,OCI_HTYPE_STMT,terr->err,(void**)&par,i+1),terr,"OCIParamGet() error","new_reset()");
		if(!flag)
			goto ret;
		namelen=0;
		flag=check_err(OCIAttrGet(par,OCI_DTYPE_PARAM,&name,&namelen,OCI_ATTR_NAME,terr->err),terr,"OCIAttrGet() for OCI_ATTR_NAME error","new_reset()");
		rs->values[i].name=NewBlock(namelen+2,sizeof(char));
		strncpy(rs->values[i].name+1,name,namelen);/*为了返回到客户层时，不被释放*/
		if(!flag)
			goto ret;
		flag=check_err(OCIAttrGet(par,OCI_DTYPE_PARAM,&rs->values[i].type,0,OCI_ATTR_DATA_TYPE,terr->err),terr,"OCIAttrGet() for OCI_ATTR_DATA_TYPE error","new_reset()");
		if(!flag)
			goto ret;
		flag=check_err(OCIAttrGet(par,OCI_DTYPE_PARAM,&rs->values[i].max,0,OCI_ATTR_DATA_SIZE,terr->err),terr,"OCIAttrGet() for OCI_ATTR_DATA_SIZE error","new_reset()");
		if(!flag)
			goto ret;

		/*精度与刻度获取*/
		if(rs->values[i].type==SQLT_NUM){
			flag=check_err(OCIAttrGet(par,OCI_DTYPE_PARAM,&rs->values[i].pre,0,OCI_ATTR_PRECISION,terr->err),terr,"OCIAttrGet() for OCI_ATTR_PRECISION error","new_reset()");
			if(!flag)
				goto ret;
			if(rs->values[i].pre>0){
				flag=check_err(OCIAttrGet(par,OCI_DTYPE_PARAM,&rs->values[i].scale,0,OCI_ATTR_SCALE,terr->err),terr,"OCIAttrGet() for OCI_ATTR_SCALE error","new_reset()");
				if(!flag)
					goto ret;
			}
		}

		flag=check_err(OCIDescriptorFree(par,OCI_DTYPE_PARAM),NULL,"OCIDescriptorFree() error","new_reset()");
		if(!flag)
			goto ret;
		flag=res_init_colparam(&rs->values[i],rs,terr);
		if(!flag)
			goto ret;
	}

ret:
	if(!err)OCI_FreeErr(terr);
	if(!flag)
		free_reset(rs,OCI_FREET_CHILD);
	return flag?rs:NULL;
}

BOOL	OCI_ExecQuery(OCI_Stmt cs,OCI_Res *rs,ub2 max,OCI_Err err){
	assert_error("Invalid pointer or argument","OCI_ExecQuery()",cs && rs && max);
	*rs=new_reset(cs,max,err);
	if(!*rs)
		return FALSE;
	return stmt_execsql(cs,max,err,OCI_EXEC_TFETCH);
}

void	OCI_FreeRes(OCI_Res rs){
	assert_error("Invalid pointer","OCI_FreeRes()",rs);
	free_reset(rs,OCI_FREET_CHILD);
}

void	stmt_reset_fetchstat(OCI_Res rs){
	sb4		i;
	ub2		dtype;
	/*重置一些本地数据*/
	assert_error("Invalid pointer or argument","stmt_reset_fetchstat()",rs);
	/*LOB的偏移数据*/
	for(i=0;i<rs->cols;i++){
		dtype=rs->values[i].dtype;
		if(OCI_ISLOB_TYPE(dtype)){
			bzero(((OCI_Lob)rs->values[i].data)->off,rs->mem*sizeof(ub4));
		}
	}
	rs->row_iter=rs->col_iter=-1;
}


OCI_GETSTAT	stmt_fetchsql(OCI_Res rs,ushort type,OCI_Err err){
	sword	flag;
	OCI_Err terr;
	OCI_GETSTAT result;
	ub4		fetchrows;
	assert_error("Invalid pointer or argument","stmt_fetchsql()",rs);

	if(rs->isfetched)
		return OCI_GETSTAT_ERROR;

	terr=err?err:OCI_NewErr();

	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","stmt_fetchsql()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	stmt_reset_fetchstat(rs);

#if defined OCI_STMT_SCROLLABLE_READONLY
	switch(type){
	case OCI_FETCH_CURRENT:
	case OCI_FETCH_PRIOR:
		fetchrows=1;
		break;
	default:
		fetchrows=rs->mem;
	}
	flag=OCIStmtFetch2(rs->pstmt->stmt,terr->err,fetchrows,type,0,OCI_DEFAULT);
#else
	flag=OCIStmtFetch(rs->pstmt->stmt,terr->err,rs->mem,type,OCI_DEFAULT);
#endif
	if(flag!=OCI_NO_DATA && !check_err(flag,terr,"OCIStmtFetch() error","stmt_fetchsql()")){
		result=OCI_GETSTAT_ERROR;
		goto end;
	}

	/*获取提取行数*/
	if(!check_err(OCIAttrGet(rs->pstmt->stmt,OCI_HTYPE_STMT,&rs->cur,0,OCI_ATTR_ROWS_FETCHED,terr->err),terr,"OCIAttrGet() for OCI_ATTR_ROWS_FETCHED error","stmt_fetchsql")){
		result=OCI_GETSTAT_ERROR;
		goto end;
	}

	if(rs->cur==0)
		result=OCI_GETSTAT_NONDATA;
	else
		result=OCI_GETSTAT_SUCCESS;
	if(rs->cur<rs->mem)
		rs->isfetched=TRUE;
end:
	if(!err)OCI_FreeErr(terr);
	return result;
}

OCI_GETSTAT	OCI_FetchNext(OCI_Res rs,OCI_Err err){
	return stmt_fetchsql(rs,OCI_FETCH_NEXT,err);
}

OCI_GETSTAT	OCI_FetchPrior(OCI_Res rs,OCI_Err err){
	return stmt_fetchsql(rs,OCI_FETCH_PRIOR,err);
}

OCI_GETSTAT	OCI_FetchCurrent(OCI_Res rs,OCI_Err err){
	return stmt_fetchsql(rs,OCI_FETCH_CURRENT,err);
}

ushort	OCI_ResGetFetchedRows(OCI_Res rs){
	assert_error("Invalid pointer","OCI_ResGetFetchedRows()",rs);
	return (ushort)rs->cur;
}

ushort	OCI_ResGetMaxRows(OCI_Res rs){
	assert_error("Invalid pointer","OCI_ResGetMaxRows()",rs);
	return (ushort)rs->mem;
}

ushort	OCI_ResGetCols(OCI_Res rs){
	assert_error("Invalid pointer","OCI_ResGetCols()",rs);
	return rs->cols;
}

BOOL	res_get_isvalid(OCI_Res rs,ushort row,ushort col){
	assert_error("Invalid pointer","res_get_isvalid()",rs && rs->values);
	assert_error("Invalid argument","res_get_isvalid()",row<rs->cur);
	assert_error("Invalid argument","res_get_isvalid()",col<rs->cols);
	return TRUE;
}

OCI_GETSTAT res_data_getval_idx(OCI_Res rs,ushort row,ushort col,void *data,size_t *size){
	OCI_GETSTAT result;
	OCI_Data	dt;
	OCI_TmStamp ts;
	size_t		rlen;
	char		*ptr;
	assert_error("Invalid pointer","res_data_getval_idx()",data);
	if(!res_get_isvalid(rs,row,col))
		return OCI_GETSTAT_ERROR;

	assert_error("Invalid parameter","res_data_getval_idx()",OCI_ISNORMAL_TYPE(rs->values[col].dtype)||OCI_ISDT_TYPE(rs->values[col].dtype));

	if(rs->values[col].idct[row]==-1)
		return OCI_GETSTAT_NULL;
	if(rs->values[col].idct[row]==(sb2)OCI_INIT_IDCT_VAL)
		return OCI_GETSTAT_ERROR;
	if(rs->values[col].idct[row]!=0)
		check_err(OCI_ERR_DATA_DTTRUC,NULL,"Data may be truncated","res_data_getval_idx()");

	if(OCI_ISDT_TYPE(rs->values[col].dtype)){
		OCI_Err err;
		sword	flag;

		err=OCI_NewErr();
		ts=(OCI_TmStamp)rs->values[col].data;
		flag=OCIDateTimeConvert(__OCI_ENV_DATA__.env,err->err,ts->dt[row],((OCI_DateTime)data)->dt);
		
		if(!check_err(flag,err,"OCIDateTimeAssign() error","res_data_getval_idx()"))
			result=/*err->code==932?OCI_GETSTAT_SUCCESS:*/OCI_GETSTAT_ERROR;
		else
			result=OCI_GETSTAT_SUCCESS;
		OCI_FreeErr(err);
		return result;
	}

	dt=(OCI_Data)rs->values[col].data;
	if(rs->values[col].dtype!=OCI_NUM && rs->values[col].dtype!=OCI_DATE)
		assert_error("Invalid argument","res_data_getval_idx()",size);
	if(size && *size==0){
		check_err(OCI_WAR_RD_ZERO,0,"Buffer is zero-length","res_data_getval_idx()");
		return OCI_GETSTAT_ERROR;
	}
	rlen=size?*size:dt->max;
	bzero(data,rlen);
	rlen=rlen>(size_t)(rs->values[col].size)[row]?(size_t)(rs->values[col].size)[row]:rlen;
	/*数据没有被更新，数据无效*/
	if(rlen==0){
		check_err(OCI_WAR_DATA_NODATA,0,"No Data where present position.","res_data_getval_idx()");
		if(size)*size=rlen;
		return OCI_GETSTAT_NONDATA;
	}
	if(rs->values[col].dtype==OCI_NUM)rlen+=1;
	if(size)*size=rlen;
	ptr=(char*)dt->data+dt->max*row;
	Memcpy(data,ptr,rlen);

	return OCI_GETSTAT_SUCCESS;
}

OCI_GETSTAT OCI_ResDataGetval(OCI_Res rs,void *data,size_t *size){
	assert_error("Invalid pointer","OCI_ResDataGetval()",rs);
	assert_error("Invalid parameter","OCI_ResDataGetval()",rs->col_iter!=-1 && rs->row_iter!=-1);
	return res_data_getval_idx(rs,rs->row_iter,rs->col_iter,data,size);
}

OCI_GETSTAT OCI_ResDataGetvalIdx(OCI_Res rs,ushort row,ushort col,void *data,size_t *size){
	return res_data_getval_idx(rs,row,col,data,size);
}

OCI_GETSTAT res_lob_getval_idx(OCI_Res rs,ushort row,ushort col,size_t off,void *data,size_t *size){
	BOOL	ret;
	OCI_Lob	lb;
	OCI_LobLocator lob;
	sword	flag;
	ub4		rlen,alen;
	OCI_Err	terr;
	ub2		dtype;
	
	assert_error("Invalid pointer or argument","res_lob_getval_idx()",data && size);
	if(!res_get_isvalid(rs,row,col))
		return OCI_GETSTAT_ERROR;

	dtype=rs->values[col].dtype;
	assert_error("Invalid parameter","res_lob_getval_idx()",OCI_ISLOB_TYPE(dtype));

	if(*size==0){
		check_err(OCI_WAR_RD_ZERO,0,"Buffer is zero-length ","res_lob_getval_idx()");
		return OCI_GETSTAT_ERROR;
	}

	if(rs->values[col].idct[row]==-1)
		return OCI_GETSTAT_NULL;
	if(rs->values[col].idct[row]==(ushort)OCI_INIT_IDCT_VAL)
		return OCI_GETSTAT_ERROR;
	/*if(rs->values[col].idct[row]!=0)
		check_err(OCI_ERR_DATA_DTTRUC,NULL,"Data may be truncated","res_data_getval_idx()");*/

	bzero(data,*size);
	lb=(OCI_Lob)rs->values[col].data;
	lob=lb->lob[row];

	if(lb->off[row]==0)
		lb->off[row]=1;

	/*警告：如果OCI_CLOB读取的数据存在双字节，则读取字符数要比缓冲字节数的二分之一小，否则报OCI_NEED_DATA错误，如果不处理（再次读取数据），会引起不可预测的OCI通信错误*/
	alen=rlen=dtype==OCI_CLOB?rlen=(*size)/OCI_CLOB_SIGBYTE:(*size);
	if(rlen==0){
		check_err(OCI_WAR_RD_ZERO,0,"Buffer is too small","res_lob_getval_idx()");
		*size=0;
		return OCI_GETSTAT_ERROR;
	}

	terr=OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","res_lob_getval_idx()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	flag=OCILobRead(rs->pstmt->pconn->ctx,terr->err,lob,&rlen,off?off:lb->off[row],(void*)data,*size,0,0,lb->csid,lb->csfrm);
	ret=check_err(flag,terr,"OCILobRead() error","res_lob_getval_idx()");
	OCI_FreeErr(terr);
	if(!ret && flag!=OCI_NEED_DATA)
		return OCI_GETSTAT_ERROR;

	*size=rlen;
	if(rlen==0){
		check_err(OCI_WAR_WR_ZERO,NULL,"There is not data","res_lob_getval_idx()");
		return OCI_GETSTAT_NONDATA;
	}

	if(off==0)
		lb->off[row]+=dtype==OCI_CLOB?(alen<rlen?alen:rlen):rlen;//???

	return OCI_GETSTAT_SUCCESS;
}

OCI_GETSTAT OCI_ResLobGetval(OCI_Res rs,size_t off,void *data,size_t *len){
	assert_error("Invalid pointer","OCI_ResLobGetval()",rs);
	assert_error("Invalid parameter","OCI_ResLobGetval()",rs->col_iter!=-1 && rs->row_iter!=-1);
	return res_lob_getval_idx(rs,rs->row_iter,rs->col_iter,off,data,len);
}

OCI_GETSTAT OCI_ResLobGetvalIdx(OCI_Res rs,ushort row,ushort col,size_t off,void *data,size_t *len){
	return res_lob_getval_idx(rs,row,col,off,data,len);
}

BOOL	res_lob_getlen_idx(OCI_Res rs,ushort row,ushort col,size_t *len){
	BOOL	ret;
	sword	flag;
	OCI_LobLocator	lb;
	OCI_Lob	lob;
	OCI_Err	terr;

	assert_error("Invalid pointer","reslobgetlen()",len);

	if(!res_get_isvalid(rs,row,col))
		return FALSE;
	assert_error("Invalid parameter","reslobgetlen()",OCI_ISLOB_TYPE(rs->values[col].dtype));

	if(rs->values[col].idct[row]==-1)
		return FALSE;

	lob=(OCI_Lob)rs->values[col].data;
	lb=lob->lob[row];

	terr=OCI_NewErr();
	if(!terr){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","reslobgetlen()");
		exit(OCI_ERR_HDL_ALLOC);
	}

	flag=OCILobGetLength(rs->pstmt->pconn->ctx,terr->err,lb,len);
	ret=check_err(flag,terr,"OCILobGetLength()error","bind_lob_getlen_idx()");
	OCI_FreeErr(terr);
	return ret;
}

BOOL		OCI_ResLobGetLen(OCI_Res rs,size_t *len){
	assert_error("Invalid pointer","OCI_ResLobGetLen()",rs);
	assert_error("Invalid parameter","OCI_ResLobGetLen()",rs->col_iter!=-1 && rs->row_iter!=-1);
	return res_lob_getlen_idx(rs,rs->row_iter,rs->col_iter,len);
}

BOOL		OCI_ResLobGetLenIdx(OCI_Res rs,ushort row,ushort col,size_t *len){
	return res_lob_getlen_idx(rs,row,col,len);
}

BOOL	OCI_ResNextRow(OCI_Res rs){
	assert_error("Invalid pointer","OCI_ResNextRow()",rs && rs->values);
	/*if((long)rs->row_iter<(long)rs->cur)
		rs->row_iter++;
	OCI_ResResetColIter(rs);
	return (ub4)rs->row_iter<rs->cur?TRUE:FALSE;*/
	if((long)rs->row_iter+1<(long)rs->cur){
		rs->row_iter++;
		OCI_ResResetColIter(rs);
		return TRUE;
	}
	return FALSE;
}

BOOL	OCI_ResNextCol(OCI_Res rs){
	assert_error("Invalid pointer","OCI_ResNextCol()",rs && rs->values);
	if(rs->col_iter+1<(sb4)rs->cols){
		rs->col_iter++;
		return TRUE;
	}
	return FALSE;
}

void	OCI_ResResetColIter(OCI_Res rs){
	assert_error("Invalid pointer","OCI_ResResetColIter()",rs && rs->values);
	/*重置当前行的属性数据*/
	if(rs->row_iter !=-1 && (long)rs->row_iter<(long)rs->cur && rs->col_iter!=-1){
		sb4		i;
		ub2		dtype;
		/*LOB的偏移数据*/
		for(i=0;i<rs->cols;i++){
			dtype=rs->values[i].dtype;
			if(OCI_ISLOB_TYPE(dtype)){
				((OCI_Lob)rs->values[i].data)->off[rs->row_iter]=1;
				//bzero(((OCI_Lob)rs->values[i].data)->off,sizoef(ub4)*rs->mem);
			}
		}
	}
	rs->col_iter=-1;
}

void	OCI_ResResetRowIter(OCI_Res rs){
	assert_error("Invalid pointer","OCI_ResResetRowIter()",rs && rs->values);
	stmt_reset_fetchstat(rs);
}

ushort	res_get_outtype_idx(OCI_Res rs,ushort col){
	assert_error("Invalid pointer","res_get_outtype_idx()",rs && rs->values);
	if(col<rs->cols)
		return rs->values[col].dtype;
	return OCI_ERR_TYPE;
}

ushort	OCI_ResGetOutType(OCI_Res rs){
	assert_error("Invalid pointer","OCI_ResGetOutType()",rs && rs->values);
	assert_error("Invalid parameter","OCI_ResGetOutType()",rs->col_iter!=-1);
	return res_get_outtype_idx(rs,rs->col_iter);
}

ushort	OCI_ResGetOutTypeIdx(OCI_Res rs,ushort col){
	return res_get_outtype_idx(rs,col);
}

string res_get_colname(OCI_Res rs,ushort col){
	assert_error("Invalid pointer","res_get_outtype_idx()",rs && rs->values && col<rs->cols);
	return STRING rs->values[col].name+1;
}

string OCI_ResGetColName(OCI_Res rs){
	assert_error("Invalid pointer","OCI_ResGetColName()",rs && rs->values);
	assert_error("Invalid parameter","OCI_ResGetColName()",rs->col_iter!=-1);
	return res_get_colname(rs,rs->col_iter);
}

string OCI_ResGetColNameIdx(OCI_Res rs,ushort col){
	return res_get_colname(rs,col);
}

BOOL	res_get_colprescale_idx(OCI_Res rs,ushort col,int *pre,int *scale){
	assert_error("Invalid pointer","res_get_colprescale_idx()",rs && pre && scale && rs->values && col<rs->cols);
	if(rs->values[col].pre>0){
		*pre=(int)rs->values[col].pre;
		*scale=(int)rs->values[col].scale;
		return TRUE;
	}
	return FALSE;
}

/*得到列的精度与刻度*/
BOOL	OCI_ResGetColPreScale(OCI_Res rs,int *pre,int *scale){
	assert_error("Invalid pointer","OCI_ResGetColPreScale()",rs && rs->values);
	assert_error("Invalid parameter","OCI_ResGetColPreScale()",rs->col_iter!=-1);
	return res_get_colprescale_idx(rs,rs->col_iter,pre,scale);
}

BOOL	OCI_ResGetColPreScaleIdx(OCI_Res rs,ushort col,int *pre,int *scale){
	return res_get_colprescale_idx(rs,col,pre,scale);
}

size_t	res_get_colmaxsize_idx(OCI_Res rs,ushort col){
	assert_error("Invalid pointer","res_get_colmaxsize_idx()",rs && rs->values && col<rs->cols);
	return (size_t)rs->values[col].max;
}

size_t	OCI_ResGetColMaxSize(OCI_Res rs){
	assert_error("Invalid pointer","OCI_ResGetColMaxSize()",rs && rs->values);
	assert_error("Invalid parameter","OCI_ResGetColMaxSize()",rs->col_iter!=-1);
	return res_get_colmaxsize_idx(rs,rs->col_iter);
}

size_t	OCI_ResGetColMaxSizeIdx(OCI_Res rs,ushort col){
	return res_get_colmaxsize_idx(rs,col);
}

OCI_Bind	new_temp_bind(const OCI_Res rs,ushort col,OCI_Stmt cs){
	OCI_Bind bd;
	assert_error("Invalid pointer or argument","new_temp_bind()",rs && col<rs->cols);
	bd=New(OCI_Bind);
	bd->bdtype=OCI_BINDT_STC;
	bd->mem=rs->mem;
	bd->type=rs->values[col].dtype;
	bd->flag=FALSE;
	bd->iter=-1;
	time(&bd->id);
	bd->pstmt=cs?cs:rs->pstmt;

	bd->idct=rs->values[col].idct;
	bd->data=rs->values[col].data;
	bd->size=rs->values[col].size;
	bd->rcode=rs->values[col].rcode;

	bd->ctype=TRUE;/*临时变量*/
	InsertAtHeadLink(bd,bd->pstmt->bdlink);
	return bd;
}

OCI_Bind	OCI_ResConvertToBindByPos(const OCI_Res rs,ushort col,ushort pos,OCI_Stmt cs){
	OCI_Bind bd;
	assert_error("Invalid argument","OCI_ResToStcBindByPos()",pos);
	bd=new_temp_bind(rs,col,cs);
	bd->pos=pos;
	return bd;
}

OCI_Bind	OCI_ResConvertToBindByName(const OCI_Res rs,ushort col,const string name,OCI_Stmt cs){
	OCI_Bind bd;
	assert_error("Invalid argument","OCI_ResToStcBindByPos()",name);
	bd=new_temp_bind(rs,col,cs);
	bd->name=StringCpy(name);;
	return bd;
}

void		OCI_BindSetCur(OCI_Bind bd,ushort cur){
	assert_error("Invalid pointer or argument","OCI_BindSetCur()",bd && bd->mem>=cur);
	assert_error("Invalid parameter","OCI_BindSetCur()",bd->ctype);
	bd->cur=cur;
}

void		OCI_BindAllSetCur(ushort cur,OCI_Bind bd,...){
	va_list vp;

	va_start(vp,bd);
	do{
		OCI_BindSetCur(bd,cur);
		bd=va_arg(vp,OCI_Bind);
	}while(bd!=0);
	va_end(vp);

	return;
}

BOOL		OCI_ResAssignToBind(const OCI_Res rs,ushort col,OCI_Bind bd){
	OCI_Err err;
	BOOL	ret;
	assert_error("Invalid pointer or argument","OCI_ResAssignToBind()",rs && col<rs->cols && bd && bd->data && !bd->ctype && bd->bdtype==OCI_BINDT_STC);
	if(!rs->cur)
		return check_err(OCI_MSG_NORMAL,0,"There is no data in OCI_Res","OCI_ResAssignToBind()");
	if(rs->cur>bd->mem)
		return check_err(OCI_MSG_NORMAL,0,"The data buffer capacity is too small in OCI_Bind","OCI_ResAssignToBind()");
	if(rs->values[col].dtype!=bd->type)
		return check_err(OCI_MSG_NORMAL,0,"OCI_Res and OCI_Bind have different data type","OCI_ResAssignToBind()");

	err=OCI_NewErr();
	if(!err){
		check_err(OCI_ERR_HDL_ALLOC,NULL,"Can't create internal error-handle","res_lob_getval_idx()");
		exit(OCI_ERR_HDL_ALLOC);
	}
	bd->cur=rs->cur;
	bd->iter=-1;
	bd->flag=FALSE;
	Memcpy(bd->size,rs->values[col].size,sizeof(ub2)*rs->cur);
	Memcpy(bd->idct,rs->values[col].idct,sizeof(sb2)*rs->cur);
	Memcpy(bd->rcode,rs->values[col].rcode,sizeof(ub2)*rs->cur);
	if(OCI_ISNORMAL_TYPE(bd->type)){
		assert_error("Invalid parameter","OCI_ResAssignToBind",((OCI_Data)bd->data)->max==((OCI_Data)rs->values[col].data)->max);
		Memcpy(((OCI_Data)bd->data)->data,((OCI_Data)rs->values[col].data)->data,((OCI_Data)rs->values[col].data)->max*rs->cur);
		ret=TRUE;
	}
	else if(OCI_ISLOB_TYPE(bd->type)){
		ushort	i;
		uint	len;
		for(i=0;i<rs->cur;i++){
			((OCI_Lob)bd->data)->off[i]=1;
			/*截断目标LOB*/
			ret=check_err(OCILobTrim(bd->pstmt->pconn->ctx,err->err,((OCI_Lob)bd->data)->lob[i],0),err,"OCILobTrim() error","OCI_ResAssignToBind()");
			if(ret)ret=check_err(OCILobGetLength(rs->pstmt->pconn->ctx,err->err,((OCI_Lob)rs->values[col].data)->lob[i],&len),err,"OCILobGetLength() error","OCI_ResAssignToBind()");
			if(ret)ret=check_err(OCILobCopy(bd->pstmt->pconn->ctx,err->err,((OCI_Lob)bd->data)->lob[i],((OCI_Lob)rs->values[col].data)->lob[i],len,1,1),err,"OCILobCopy() error","OCI_ResAssignToBind()");
			if(!ret)break;
		}
	}
	else if(OCI_ISDT_TYPE(bd->type)){
		ushort i;
		for(i=0;i<rs->cur;i++){
			ret=check_err(OCIDateTimeAssign(__OCI_ENV_DATA__.env,err->err,((OCI_TmStamp)rs->values[col].data)->dt[i],((OCI_TmStamp)bd->data)->dt[i]),err,"OCIDateTimeAssign() error","OCI_ResAssignToBind()");
			if(!ret)break;
		}
	}
	else
		assert_error("Invalid data type","OCI_ResAssignToBind()",OCI_ERR_BIND_TYPE);
	OCI_FreeErr(err);
	return ret;
}

/*****************************************检测错误*************************************************/
BOOL		check_err(int flag,OCI_Err er,char *msg,char *fun){
	text errbuf[DEF_ERRMSG_LEN];
	sb4 errcode = 0;
	ub4 len;
	assert_error("Invalid pointer.","check_err()",msg && fun);

	if(er){
		bzero(er->fun,DEF_ERRFUN_LEN);
		bzero(er->msg,DEF_ERRMSG_LEN);
	}

	len=Snprintf(STRING errbuf,sizeof(errbuf),STRING "At %s,%s:",fun,msg);

	switch (flag){
	case OCI_SUCCESS:
		goto suc;
	case OCI_SUCCESS_WITH_INFO:
		Snprintf(STRING errbuf+len,sizeof(errbuf)-len,STRING "%s.","OCI_SUCCESS_WITH_INFO");
		break;
	case OCI_NEED_DATA:
		Snprintf(STRING errbuf+len,sizeof(errbuf)-len,STRING "%s.","OCI_NEED_DATA");
		break;
	case OCI_NO_DATA:
		Snprintf(STRING errbuf+len,sizeof(errbuf)-len,STRING "%s.","OCI_NODATA");
		break;
	case OCI_INVALID_HANDLE:
		Snprintf(STRING errbuf+len,sizeof(errbuf)-len,STRING "%s.","OCI_INVALID_HANDLE");
		break;
	case OCI_STILL_EXECUTING:
		Snprintf(STRING errbuf+len,sizeof(errbuf)-len,STRING "%s.","OCI_STILL_EXECUTE");
		break;
	case OCI_CONTINUE:
		Snprintf(STRING errbuf+len,sizeof(errbuf)-len,STRING "%s.","OCI_CONTINUE");
		break;
	case OCI_ERROR:
		if(!er || !er->err){
#ifdef OCI_PRO_DEBUG_MODE
			printf("%s:Error argument.\n",errbuf);
#endif
			exit(OCI_ERROR);
		}
		if(OCIErrorGet(er->err,1,0,&errcode,errbuf+len,sizeof(errbuf)-len,OCI_HTYPE_ERROR)){
			Snprintf(STRING errbuf+len,sizeof(errbuf)-len,STRING "%s.",
				"Don't get error message from OCIError");
#ifdef OCI_PRO_DEBUG_MODE
			printf("%s\n",errbuf);
#endif
			exit(OCI_ERROR);
		}
		RTrimString(errbuf,'\n');
		break;
	default:
		errbuf[len-1]='\0';
	}
	/*是否输出调试信息*/
#ifdef OCI_PRO_DEBUG_MODE
	err_msg("%s",(char*)errbuf);
#endif
suc:
	if(er && flag!=OCI_SUCCESS){
		size_t len;
		strncpy(er->msg,(char*)errbuf,DEF_ERRMSG_LEN);
		len=strlen(fun);
		strncpy(er->fun,(char*)fun,len>DEF_ERRFUN_LEN?DEF_ERRFUN_LEN:len+1);
		er->code=errcode==0?flag:errcode;
	}	
	return flag==OCI_SUCCESS || flag==OCI_SUCCESS_WITH_INFO ?TRUE:FALSE;
}