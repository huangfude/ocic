#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <io.h>
#include <fcntl.h>

#include <float.h>

#include <m_random.h>

#include <mocioper.h>

#define BD_MAX		30
#define RD_BUF		1024

#define TEST_ERR(err)	if(OCI_ErrGetStat((err))){ \
	printf("%s",OCI_ErrGetmsg((err))); \
	return; \
						}

#define TEST_ERR1(err)	if(OCI_ErrGetStat((err))){ \
	printf("%s",OCI_ErrGetmsg((err))); \
						}

#define USER		STRING "jsbdp"
#define PWD			STRING "jsbdp"
#define OID			STRING "jsbst92"

/*
数据库目标表：
create table TEST_OCIDATATYPE
(
i     NUMBER,
f     NUMBER(10,2),
str   VARCHAR2(50),
c     CHAR(20),
dt    DATE,
stamp TIMESTAMP(6),
b     RAW(200),
blb   BLOB,
clb   CLOB
)
*/

/*
测试目的：
1.单次绑定，多次设置数据，多次执行（插入语句）。
2.批处理执行。
3.使用索引设置数据。
4.自动提交。
5.设置和取消执行选项。
*/
void test_repeat_exec_insert(OCI_Conn hd,OCI_Stmt cs){
	OCI_Bind stc_id,stc_val;
	FILE *fp;
	int i,j;
	char	buf[RD_BUF];
	BOOL	fp_nodata;
	OCI_StmtParse(cs,STRING "insert into TEST_OCIDATATYPE(i,str) values(:id,:str)",0);

	//TEST_ERR(err);

	fp=fopen("E:\\ftp\\OCI\\test.c","r");
	if(!fp)
		err_sys(errno,"fopen()");

	stc_id=OCI_NewStcBindByName(cs,STRING ":ID",OCI_INT,BD_MAX,0);
	stc_val=OCI_NewStcBindByName(cs,STRING ":STR",OCI_STR,BD_MAX,0);
	OCI_BindByName(0,stc_id,stc_val,0);

	OCI_StmtSetOpt(cs,OCI_EXEC_BATCHERR|OCI_EXEC_AUTOCOMM,TRUE);

	fp_nodata=TRUE;
	j=0;
	while(fp_nodata){
		for(i=0;i<BD_MAX;i++,j++){
			if(!fgets(buf,sizeof(buf),fp)){
				fp_nodata=FALSE;
				break;
			}
			//printf("%s\n",buf);
			OCI_BindDataSetvalIdx(stc_val,i,buf,strlen(buf));
			OCI_BindDataSetvalIdx(stc_id,i,&j,0);
		}
		OCI_ExecBatch(cs,OCI_BindGetCur(stc_id),0);
		OCI_BindAllReuse(stc_id,stc_val,0);
	}
	OCI_StmtSetOpt(cs,OCI_EXEC_BATCHERR|OCI_EXEC_AUTOCOMM,FALSE);
	fclose(fp);
	OCI_FreeBind(stc_id,stc_val,0);
	return;
}

/*
测试目的：
1.单次绑定，多次设置数据，多次执行（带returning into子句插入语句）。
2.批处理执行。
3.使用索引设置数据。
4.设置和取消执行选项。
*/
void test_repeat_exec_insert1(OCI_Conn hd,OCI_Stmt cs){
	OCI_Bind stc_id,dnc_val;
	FILE *fp;
	int i,j;
	float f;
	char	buf[RD_BUF];
	BOOL	fp_nodata;
	OCI_StmtParse(cs,STRING "insert into TEST_OCIDATATYPE(f2,clb) values(:id,empty_clob()) returning clb into :clb",0);

	//TEST_ERR(err);

	fp=fopen("E:\\clob.txt","r");
	if(!fp)
		err_sys(errno,"fopen()");

	stc_id=OCI_NewStcBindByName(cs,STRING ":ID",OCI_FLT,BD_MAX,0);
	dnc_val=OCI_NewDncBindByName(cs,STRING ":CLB",OCI_CLOB,0,0);
	OCI_BindByName(0,stc_id,dnc_val,0);

	OCI_StmtSetOpt(cs,OCI_EXEC_BATCHERR,TRUE);

	fp_nodata=TRUE;
	j=0;
	Rand();
	while(fp_nodata){
		for(i=0;i<BD_MAX;i++,j++){
			f=(float)RandReal(1,5000);
			OCI_BindDataSetvalIdx(stc_id,i,&f,0);
		}
		OCI_ExecBatch(cs,OCI_BindGetCur(stc_id),0);

		for(i=0;i<OCI_BindGetCur(dnc_val);i++){
read_again:
			if(!fgets(buf,sizeof(buf),fp)){
				fp_nodata=FALSE;
				break;
			}
			TrimString(STRING buf,0);
			if(strlen(buf)==0)
				goto read_again;
			OCI_BindLobSetvalIdx(dnc_val,i,0,buf,strlen(buf));
		}

		OCI_Commit(hd,0);
		OCI_BindAllReuse(stc_id,dnc_val,0);
		//fp_nodata=FALSE;
	}
	OCI_StmtSetOpt(cs,OCI_EXEC_BATCHERR,FALSE);
	fclose(fp);
	OCI_FreeBind(stc_id,dnc_val,0);
	return;
}

void test_exec_insert_timestamp(OCI_Stmt cs){
	OCI_Bind bd,ret;
	OCI_DateTime tm;
	char buf[100];
	size_t len;
	if(!OCI_StmtParse(cs,STRING "insert into TEST_OCIDATATYPE(stamp) values(:1) returning stamp into :2",0))
		return;
	bd=OCI_NewStcBindByPos(cs,1,OCI_TMSTAMP,0,0);
	ret=OCI_NewStcBindByPos(cs,2,OCI_TMSTAMP,0,0);
	if(!bd)
		return;
	if(!OCI_BindByPos(0,bd,ret,0)){
		//OCI_FreeBind(bd,0);
		return;
	}
	tm=OCI_NewDateTime(OCI_DATETIME_TS);

	OCI_DateTimeConstruct(tm,2013,7,1,23,59,59,3400,0);
	len=sizeof(buf);
	OCI_DateTimeToString(tm,STRING buf,len,0,0);
	OCI_BindDataSetval(bd,tm,0);

	OCI_StmtSetOpt(cs,OCI_EXEC_AUTOCOMM,TRUE);
	OCI_ExecNoQuery(cs,0);
	OCI_StmtSetOpt(cs,OCI_EXEC_AUTOCOMM,FALSE);

	OCI_BindDataGetval(ret,tm,0);
	len=sizeof(buf);
	OCI_DateTimeToString(tm,STRING buf,len,0,0);
	printf("return timestamp:%s",buf);
	OCI_FreeDateTime(tm);
	//OCI_FreeBind(bd,0);
}

void test_exec_plsqlget_timestamp(OCI_Stmt cs){

}

/*
测试目的：
1.使用不带returning into子句的insert into 直接插入LOB数据。
2.抹去LOB定位符的所有数据和指定字节的数据。
3.在指定位置处设置值。
*/
void test_erase_lob(OCI_Conn hd,OCI_Stmt cs){
	OCI_Bind id,clob,blob;
	char buf1[]="第一次设置：周凯，后面是测试信息";
	char buf2[]="第二次";
	char buf3[]="第一次设置";

	if(!OCI_StmtParse(cs,STRING "insert into TEST_OCIDATATYPE(i,clb,blb) values(:id,:clb,:blb)",0))
		return;

	clob=OCI_NewStcBindByName(cs,STRING ":CLB",OCI_CLOB,0,0);
	blob=OCI_NewStcBindByName(cs,STRING ":BLB",OCI_BLOB,0,0);
	id=OCI_NewStcBindByName(cs,STRING ":ID",OCI_INT,0,0);

	if(!id || !blob || !clob)
		return;

	if(!OCI_BindByName(0,id,blob,clob,0))
		return;

	OCI_BindDataSetval(id,id,0);

	/*第一次设置*/
	OCI_BindLobSetval(clob,1,buf1,0);
	/*摸去“第一次设置”五个字,由于lob绑定是字符型，则传递‘5’*/
	OCI_BindLobErase(clob,1,5);
	/*第二次设置*/
	OCI_BindLobSetval(clob,1,buf2,0);

	/*第一次设置*/
	OCI_BindLobSetval(blob,1,buf1,strlen(buf1));
	/*摸去“第一次设置”五个字,由于lob绑定是二进制型，则传递strlen("第一次设置")*/
	OCI_BindLobErase(blob,1,strlen(buf3));
	/*第二次设置*/
	OCI_BindLobSetval(blob,1,buf2,strlen(buf2));

	OCI_StmtSetOpt(cs,OCI_EXEC_AUTOCOMM,TRUE);

	OCI_ExecNoQuery(cs,0);

	/*全部抹去LOB数据，再次插入*/
	OCI_BindAllErase(id,blob,clob,0);
	OCI_ExecNoQuery(cs,0);

	OCI_StmtSetOpt(cs,OCI_EXEC_AUTOCOMM,FALSE);
	//OCI_Commit(hd,0);

	OCI_FreeBind(id,blob,clob,0);
}

/*
测试目的：
1.select语句测试
2.根据列的不同类型，获取不同类型的值
3.用OCI_Number*()系列函数操作OCINumber类型。
*/
void printf_res(OCI_Res rs,OCI_Stmt cs){
	int ival;
	ub4 uval;
	
	OCINumber num;
	char val[100];
	size_t size;
	OCIDate dt;
	OCI_GETSTAT stat;
	switch(OCI_ResGetOutType(rs)){
	case OCI_NUM:
		{
			int pre,scale;
			OCI_NumberSetZero(&num);
			stat=OCI_ResDataGetval(rs,&num,0);
			if(stat==OCI_GETSTAT_SUCCESS){
				//浮点型
				if(OCI_ResGetColPreScale(rs,&pre,&scale) && scale>0){
					float fval;
					if(OCI_NumberToReal(&num,&fval,OCI_NUMBER_FLOAT)){
						printf("%-5.*f\t",scale,fval);
					}
					else
						stat=OCI_GETSTAT_ERROR;
				}
				//整型
				else{
					int ival;
					if(OCI_NumberToInt(&num,&ival,OCI_NUMBER_SIGNED)){
						printf("%-5d\t",ival);
					}
					else
						stat=OCI_GETSTAT_ERROR;
				}
				
			}
			if(stat==OCI_GETSTAT_ERROR)
				printf("%-5s\t","ERROR");
			if(stat==OCI_GETSTAT_NONDATA || stat==OCI_GETSTAT_NULL)
				printf("%-5s\t","NULL");
		}
		break;
	case OCI_BLOB:
	case OCI_CLOB:
		{
			if(OCI_ResLobGetLen(rs,&size))
				printf("%u:",size);
			size=sizeof(val);
			stat=OCI_ResLobGetval(rs,0,val,&size);
			if(stat==OCI_GETSTAT_SUCCESS)
				printf("%-20.*s\t",size,val);
			else if(stat==OCI_GETSTAT_ERROR)
				printf("%-20s\t","ERROR");
			else
				printf("%-20s\t","NULL");
			printf("%-5s\t","NULL");
		}
		break;
	case OCI_STR:
		{
			size=sizeof(val);
			stat=OCI_ResDataGetval(rs,val,&size);
			if(stat==OCI_GETSTAT_SUCCESS)
				printf("%-20.*s\t",size,val);
			else if(stat==OCI_GETSTAT_ERROR)
				printf("%-20s\t","ERROR");
			else
				printf("%-20s\t","NULL");
		}
		break;
	case OCI_DATE:
		{
			stat=OCI_ResDataGetval(rs,&dt,0);
			if(stat==OCI_GETSTAT_SUCCESS){
				OCI_DateToString(&dt,STRING val,sizeof(val),0,0);
				printf("%-20.*s\t",strlen(val),val);
			}
			else if(stat==OCI_GETSTAT_ERROR)
				printf("%-20s\t","ERROR");
			else
				printf("%-20s\t","NULL");
		}
		break;
	default:
		{
			
			OCI_DateTime dt;
			char hour,min,sec,mon,day;
			short year;
			unsigned fsec;
			OCI_GETSTAT stat;

			size=sizeof(val);
			dt=OCI_NewDateTime(OCI_DATETIME_TS);
			stat=OCI_ResDataGetval(rs,dt,0);
			if(stat==OCI_GETSTAT_SUCCESS){
				/*if(OCI_DateTimeGetDate(dt,&year,&mon,&day)){
					printf("%d/%d/%d ",year,mon,day);
					if(OCI_DateTimeGetTime(dt,&hour,&min,&sec,&fsec))
						printf("%d:%d:%d.%d",hour,min,sec,fsec);
				}*/
				OCI_DateTimeGetTimeZone(dt,&hour,&min);
				if(OCI_DateTimeToString(dt,STRING val,size,0,0))
					printf("%s\t",val);
				else
					stat=OCI_GETSTAT_ERROR;
			}

			if(stat==OCI_GETSTAT_ERROR)
				printf("%-20s\t","ERROR");
			if(stat==OCI_GETSTAT_NULL)
				printf("%-20s\t","NULL");

			OCI_FreeDateTime(dt);
		}
	}
}

void test_select(OCI_Conn hd,OCI_Stmt cs,string sql){
	OCI_Bind	id;
	OCI_Res		rs;
	int			i;
	
	if(!OCI_StmtParse(cs,sql,0))
		return;
	OCI_StmtSetOpt(cs,OCI_EXEC_SCROLL,TRUE);
	if(!OCI_ExecQuery(cs,&rs,100,0))
		return;
	OCI_StmtSetOpt(cs,OCI_EXEC_SCROLL,FALSE);
	printf("****************next data in scrollable cursor****************\n");
	i=0;

	while(OCI_ResNextCol(rs)){
		printf("%s\t",OCI_ResGetColName(rs));
	}
	printf("\n");
	while(OCI_FetchNext(rs,0)==OCI_GETSTAT_SUCCESS){
		printf("fetched some rows:%u\n",OCI_ResGetFetchedRows(rs));
		while(OCI_ResNextRow(rs)){
			/*while(OCI_ResNextCol(rs)){
				printf_res(rs);
			}
			printf("\n");
			OCI_ResResetColIter(rs);*/
			while(OCI_ResNextCol(rs)){
				printf_res(rs,cs);
			}
			printf("\n");
		}
		if(OCI_ResGetFetchedRows(rs)<OCI_ResGetMaxRows(rs))
			break;
	}

	/*printf("****************current data in scrollable cursor****************\n");
	if(OCI_FetchCurrent(rs,0)==OCI_GETSTAT_SUCCESS){
		printf("fetched some rows:%u\n",OCI_ResGetFetchedRows(rs));
		while(OCI_ResNextRow(rs)){
			while(OCI_ResNextCol(rs)){
				printf_res(rs);
			}
			printf("\n");
		}
	}

	printf("****************prior data in scrollable cursor****************\n");
	while(OCI_FetchPrior(rs,0)==OCI_GETSTAT_SUCCESS){
		printf("fetched some rows:%u\n",OCI_ResGetFetchedRows(rs));
		while(OCI_ResNextRow(rs)){
			while(OCI_ResNextCol(rs)){
				printf_res(rs);
			}
			printf("\n");
		}
	}*/

	OCI_FreeRes(rs);
}

/*
测试目的：
1.执行PL/SQL块
2.绑定OCI_DATE类型的标量。
3.用OCI_Date*()系列函数操作OCI_DATE类型。
*/

void test_exec_plsql(OCI_Conn hd,OCI_Stmt cs){
	OCI_Bind dt;
	size_t		len;
	OCIDate		vdt,vdt1;
	OCI_GETSTAT	stat;
	char		buf[50];
	int			days;

	dt=OCI_NewStcBindByName(cs,STRING ":DT",OCI_DATE,0,0);
	if(!OCI_StmtParse(cs,STRING "begin select sysdate into :dt from dual; end;",0))
		return;
	if(!OCI_BindByName(0,dt,0))
		return;
	if(!OCI_ExecNoQuery(cs,0))
		return;

	len=sizeof(vdt);
	stat=OCI_BindDataGetval(dt,&vdt,&len);
	/*stat=OCI_BindDataGetval(dt,&vdt,0);*/
	/*len的值为0时，表示PL/SQL块没有对该值进行赋值.*/
	if(stat==OCI_GETSTAT_SUCCESS){
		if(OCI_DateToString(&vdt,STRING buf,sizeof(buf),OCI_DATE_FORMAT,0))
			printf("server date : %s\n",buf);
		if(OCI_DateToString(&vdt,STRING buf,sizeof(buf),OCI_TIME_FORMAT,0))
			printf("server time : %s\n",buf);
		/*下一个周一的日期*/
		if(OCI_DateNextDay(&vdt,&vdt1,OCI_NEXTDAY_MON)){
			if(OCI_DateToString(&vdt1,STRING buf,sizeof(buf),OCI_DATE_FORMAT,0))
				printf("next %s date : %s\n",OCI_NEXTDAY_MON,buf);
		}
		/*增加两个月的日期*/
		if(OCI_DateAddMons(&vdt,&vdt1,2)){
			if(OCI_DateToString(&vdt1,STRING buf,sizeof(buf),OCI_DATE_FORMAT,0))
				printf("add tow months date : %s\n",buf);
		}
		/*减少两天的日期*/
		if(OCI_DateAddDays(&vdt,&vdt1,-2)){
			if(OCI_DateToString(&vdt1,STRING buf,sizeof(buf),OCI_DATE_FORMAT,0))
				printf("sub tow days date : %s\n",buf);
		}
		/*本地计算机时间*/
		if(OCI_DateLocalDate(&vdt1)){
			if(OCI_DateToString(&vdt1,STRING buf,sizeof(buf),OCI_DATEFULL_FORMAT,0))
				printf("local sysdate : %s\n",buf);
		}
		/*服务器与本地时间差*/
		if(OCI_DateDaysBetween(&vdt,&vdt1,&days)){
			printf("between local and server days : %d\n",days);
		}
	}
}

/*
测试目的：
1.执行带declare的PL/SQL块
2.同时绑定数组和标量
3.测试不赋值时，输出变量的获取情况。
*/
static string sqlplsql2=STRING "\
DECLARE \
  TYPE T_ARRAY IS TABLE OF VARCHAR2(2000) INDEX BY BINARY_INTEGER; \
  PROCEDURE PRO_ARRAY(IN_ARRAY  IN T_ARRAY, \
                      I         IN NUMBER, \
                      OUT_ARRAY OUT T_ARRAY, \
                      O         OUT NUMBER, \
                      T         OUT NUMBER) AS \
  BEGIN \
	O := 0; \
    FOR J IN 1 .. I LOOP \
	  O := O + NVL(LENGTH(IN_ARRAY(J)),0); \
      IF LENGTH(IN_ARRAY(J)) > 50 THEN \
        OUT_ARRAY(J) := 'length great than 50,length is:' || LENGTH(IN_ARRAY(J)) || IN_ARRAY(J); \
      ELSE \
        OUT_ARRAY(J) := 'length less than 50,length is:' || LENGTH(IN_ARRAY(J)) || IN_ARRAY(J); \
      END IF; \
    END LOOP; \
  END; \
BEGIN \
  PRO_ARRAY(:iarr,:i,:oarr,:o,:t); \
END; \
							   ";
#define MAX_SIZE		500
#define MAX_ARR			10
void test_exec_plsql2(OCI_Conn hd,OCI_Stmt cs){
	OCI_Bind i,iarr,o,oarr,t;
	FILE *fp;
	size_t len;
	char buf[MAX_SIZE];
	OCI_GETSTAT stat;
	int num;


	if(!OCI_StmtParse(cs,sqlplsql2,0))
		return;

	i=OCI_NewStcBindByName(cs,STRING ":I",OCI_INT,0,0);/*静态输入标量*/
	iarr=OCI_NewStcBindByName(cs,STRING ":IARR",OCI_STR,MAX_ARR,MAX_SIZE);/*静态输入数组*/
	o=OCI_NewStcBindByName(cs,STRING ":O",OCI_INT,0,0);/*静态输出标量*/
	oarr=OCI_NewStcBindByName(cs,STRING ":OARR",OCI_STR,MAX_ARR,MAX_SIZE);/*静态输出数组*/

	t=OCI_NewStcBindByName(cs,STRING ":T",OCI_NUM,0,0);/*静态输出标量，用于测试不赋值情况下的返回*/

	if(!(i&&iarr&&o&&oarr&&t))
		return;

	if(!OCI_BindByName(0,iarr,oarr,t,i,o,0))
		return;

	fp=fopen("e:\\oracle\\OCI函数.c","r");
	if(!fp)
		err_sys(errno,"fopen()");

	/*赋值*/

	while(OCI_BindInNext(iarr)){
		if(!fgets(buf,sizeof(buf),fp))
			break;
		TrimString(STRING buf,0);
		OCI_BindDataSetval(iarr,buf,0);
	}

	num=OCI_BindGetCur(iarr);
	OCI_BindDataSetval(i,&num,0);


	/*执行*/
	if(!OCI_ExecNoQuery(cs,0))
		goto err_pos;

	/*获取值*/
	
	/*未赋值的数据查看*/
	len=sizeof(num);
	stat=OCI_BindDataGetval(t,&num,&len);
	if(stat==OCI_GETSTAT_SUCCESS)
		printf("test data:%d\n",num);
	else if(stat==OCI_GETSTAT_NULL)
		printf("test data:NULL\n");
	else
		printf("test data:GET ERROR,len %d\n",len);

	len=sizeof(num);
	stat=OCI_BindDataGetval(o,&num,&len);
	if(stat==OCI_GETSTAT_SUCCESS)
		printf("number of data:%d\n",num);
	else if(stat==OCI_GETSTAT_NULL)
		printf("number of data:NULL\n");
	else
		printf("number of data:GET ERROR,len %d\n",len);

	OCI_BindAllResetIter(iarr,oarr,0);
	while(OCI_BindInAllNext(iarr,oarr,0)){
		len=sizeof(buf);
		stat=OCI_BindDataGetval(iarr,buf,&len);
		if(stat==OCI_GETSTAT_SUCCESS)
			printf("old:%.*s\t",len,buf);
		else if(stat==OCI_GETSTAT_NULL)
			printf("old:NULL\t");
		else
			printf("old:GET ERROR\t");

		len=sizeof(buf);
		stat=OCI_BindDataGetval(oarr,buf,&len);
		if(stat==OCI_GETSTAT_SUCCESS)
			printf("new:%.*s\n",len,buf);
		else if(stat==OCI_GETSTAT_NULL)
			printf("new:NULL\n");
		else
			printf("new:GET ERROR\n");

	}

err_pos:
	fclose(fp);
	OCI_FreeBind(i,iarr,o,oarr,t,0);
}

void test_exec_plsql1(OCI_Stmt cs,OCI_Err err){
	OCI_Bind num;

	OCI_StmtParse(cs,STRING "\
					   DECLARE \
						  PROCEDURE PROC(n OUT NUMBER) AS \
						  BEGIN \
							NULL; \
						  END; \
						BEGIN \
						  PROC(:n); \
						END; \
					   ",err);

	num=OCI_NewStcBindByName(cs,STRING ":N",OCI_INT,0,0);

	OCI_BindByName(err,num,0);

	OCI_ExecNoQuery(cs,err);

	OCI_FreeBind(num,0);

}

/*
测试目的：
	1.select数据之后，将某列转换成一个临时的绑定变量，插入另一个表。
*/

void test_exec_select_to_insert(OCI_Conn hd){
	OCI_Res rs;
	OCI_Stmt cs1,cs2;
	OCI_Bind bd1,bd2,bd3;

	cs1=OCI_NewStmt(hd,0);
	cs2=OCI_NewStmt(hd,0);

	if(!OCI_StmtParse(cs1,STRING "select f2,str,clb from TEST_OCIDATATYPE",0))return;
	if(!OCI_StmtParse(cs2,STRING "insert into TEST_OCIDATATYPE1(f2,clb) values(:1,:2)",0))return;

	if(!OCI_ExecQuery(cs1,&rs,3000,0))return;

	bd1=OCI_ResConvertToBindByPos(rs,0,1,cs2);
	bd2=OCI_ResConvertToBindByPos(rs,2,2,cs2);
	if(!bd1 && !bd2)return;
	OCI_BindByPos(0,bd1,bd2,0);
	while(OCI_FetchNext(rs,0)==OCI_GETSTAT_SUCCESS){
		OCI_BindAllSetCur(OCI_ResGetFetchedRows(rs),bd1,bd2,0);
		OCI_ExecBatch(cs2,OCI_BindGetCur(bd1),0);
		//OCI_BindErase(bd3);
		OCI_Commit(hd,0);
	}
	OCI_FreeBind(bd1,0);
	OCI_FreeStmt(cs1,cs2,0);
}

void test_oper_tow_database(){
	OCI_Conn hd1,hd2;
	OCI_Stmt cs1,cs2;

	OCI_Res rs;
	OCI_Bind bd1,bd2,bd3,bd4;
	OCI_DateTime tm;
	int i;
	char buf[50];

	hd1=OCI_NewConn(STRING"jsbdp",STRING"jsbdp",STRING"jsbst92",0);
	if(!hd1)return;
	hd2=OCI_NewConn(STRING"jsbdp",STRING"jsbdp",STRING"jsbst92",0);
	if(!hd2)return;

	cs1=OCI_NewStmt(hd1,0);
	if(!cs1)return;
	cs2=OCI_NewStmt(hd2,0);
	if(!cs2)return;

	if(!OCI_StmtParse(cs1,STRING "select i,blb from TEST_OCIDATATYPE where i>=:1 and i<:1+100 order by i",0))return;
	if(!OCI_StmtParse(cs2,STRING "insert into TEST_OCIDATATYPE1(i,stamp,blb) values(:a,:b,:c)",0))return;

	bd4=OCI_NewStcBindByPos(cs1,1,OCI_INT,0,0);
	if(!bd4)return;
	if(!OCI_BindByPos(0,bd4,0))return;
	i=2;OCI_BindDataSetval(bd4,&i,0);

	if(!OCI_ExecQuery(cs1,&rs,10,0))return;

	bd1=OCI_ResConvertToBindByName(rs,0,STRING":a",cs2);
	if(!bd1)return;
	bd2=OCI_ResConvertToBindByName(rs,1,STRING":c",cs2);
	if(!bd2)return;
	bd3=OCI_NewStcBindByName(cs2,STRING ":B",OCI_TMSTAMP,OCI_ResGetMaxRows(rs),0);
	if(!bd3)return;

	if(!OCI_BindByName(0,bd1,bd2,bd3,0))return;
	tm=OCI_NewDateTime(OCI_DATETIME_TS);
	OCI_StmtSetOpt(cs2,OCI_EXEC_AUTOCOMM|OCI_EXEC_BATCHERR,TRUE);
	while(OCI_FetchNext(rs,0)==OCI_GETSTAT_SUCCESS){

		OCI_DateTimeLocalTimeStamp(tm);
		OCI_BindDataSetvalIdx(bd3,0,tm,0);
		OCI_BindAllSetCur(OCI_ResGetFetchedRows(rs),bd1,bd2,0);
		OCI_DateTimeLocalTimeStamp(tm);
		OCI_BindDataSetvalIdx(bd3,1,tm,0);

		OCI_DateTimeLocalTimeStamp(tm);
		OCI_DateTimeToString(tm,STRING buf,sizeof(buf),0,0);
		printf("exec before: %s\n",buf);
		if(!OCI_ExecBatch(cs2,OCI_ResGetFetchedRows(rs),0))break;
		//OCI_ExecBatch(cs2,OCI_ResGetFetchedRows(rs),0);
		OCI_DateTimeLocalTimeStamp(tm);
		OCI_DateTimeToString(tm,STRING buf,sizeof(buf),0,0);
		printf("exec after: %s\n",buf);

	}
	OCI_StmtSetOpt(cs2,OCI_EXEC_AUTOCOMM|OCI_EXEC_BATCHERR,FALSE);
	OCI_FreeDateTime(tm);
	OCI_FreeConn(hd1,hd2,0);
}

void test_oper_tow_database1(){
	OCI_Conn hd1,hd2;
	OCI_Stmt cs1,cs2;
	char buf[50];
	size_t len;

	OCI_Res rs;
	OCI_Bind bd1,bd2,bd3;
	OCI_DateTime tm;
	hd1=OCI_NewConn(STRING"jsbdp",STRING"jsbdp",STRING"jsbst92",0);
	if(!hd1)return;
	hd2=OCI_NewConn(STRING"jsbdp",STRING"jsbdp",STRING"jsbst92",0);//OCI_NewConn(STRING"jsbdp",STRING"js_bdp_2",STRING"jsyw",0);
	//hd2=hd1;
	if(!hd2)return;

	cs1=OCI_NewStmt(hd1,0);
	if(!cs1)return;
	cs2=OCI_NewStmt(hd2,0);
	if(!cs2)return;

	if(!OCI_StmtParse(cs1,STRING "select i,blb from TEST_OCIDATATYPE where rownum<101 order by i",0))return;
	if(!OCI_StmtParse(cs2,STRING "insert into TEST_OCIDATATYPE1(i,stamp,blb) values(:a,:b,:c)",0))return;

	if(!OCI_ExecQuery(cs1,&rs,10,0))return;
	bd1=OCI_NewStcBindByName(cs2,STRING ":A",OCI_ResGetOutTypeIdx(rs,0),OCI_ResGetMaxRows(rs),OCI_ResGetColMaxSizeIdx(rs,0));
	if(!bd1)return;
	bd2=OCI_NewStcBindByName(cs2,STRING ":B",OCI_TMSTAMP,OCI_ResGetMaxRows(rs),0);
	if(!bd2)return;
	bd3=OCI_NewStcBindByName(cs2,STRING ":C",OCI_ResGetOutTypeIdx(rs,1),OCI_ResGetMaxRows(rs),OCI_ResGetColMaxSizeIdx(rs,1));
	if(!bd3)return;

	if(!OCI_BindByName(0,bd1,bd2,bd3,0))return;
	tm=OCI_NewDateTime(OCI_DATETIME_TS);
	OCI_StmtSetOpt(cs2,OCI_EXEC_AUTOCOMM|OCI_EXEC_BATCHERR,TRUE);
	while(OCI_FetchNext(rs,0)==OCI_GETSTAT_SUCCESS){
		OCI_ResAssignToBind(rs,0,bd1);
		OCI_DateTimeLocalTimeStamp(tm);
		OCI_BindDataSetvalIdx(bd2,0,tm,0);
		OCI_ResAssignToBind(rs,1,bd3);
		OCI_DateTimeLocalTimeStamp(tm);
		OCI_BindDataSetvalIdx(bd2,1,tm,0);
		OCI_DateTimeLocalTimeStamp(tm);
		OCI_DateTimeToString(tm,STRING buf,sizeof(buf),0,0);
		printf("exec before: %s\n",buf);
		if(!OCI_ExecBatch(cs2,OCI_ResGetFetchedRows(rs),0))break;
		OCI_DateTimeLocalTimeStamp(tm);
		OCI_DateTimeToString(tm,STRING buf,sizeof(buf),0,0);
		printf("exec after: %s\n",buf);
	}
	OCI_FreeDateTime(tm);
	OCI_StmtSetOpt(cs2,OCI_EXEC_AUTOCOMM|OCI_EXEC_BATCHERR,FALSE);
	OCI_FreeConn(hd1,hd2,0);
}

int test_insert_blob(){
	OCI_Conn hd;
    OCI_Stmt cs;
    OCI_Bind bd1,bd2;

    int fd;
    char buff[BUF_SIZE];
    int len,i;
	BOOL stat;
	//"/home/oracle/cbw.dmp"
    fd=open("E:\\clob.txt",O_RDONLY|O_RAW);
    if(fd<0)err_sys(errno,"open() error");
    //if(!OCI_InitEnv(OCI_THREADED|OCI_OBJECT))return OCI_ERROR;

    hd=OCI_NewConn(STRING"jsbdp",STRING"jsbdp",STRING"jsbst92",0);
    if(!hd)return OCI_ERROR;
    cs=OCI_NewStmt(hd,0);
    if(!cs)return OCI_ERROR;
	OCI_StmtSetOpt(cs,OCI_EXEC_AUTOCOMM|OCI_EXEC_BATCHERR,TRUE);
	if(!OCI_StmtParse(cs,STRING "insert into TEST_OCIDATATYPE(i,blb) values(:1,:2)",0))return OCI_ERROR;

    bd1=OCI_NewStcBindByPos(cs,2,OCI_BLOB,100,0);
	if(!bd1)return OCI_ERROR;
	bd2=OCI_NewStcBindByPos(cs,1,OCI_INT,100,0);
	if(!bd2)return OCI_ERROR;

	if(!OCI_BindByPos(0,bd2,bd1,0))return OCI_ERROR;
	
	for(i=1,stat=FALSE;!stat;){
		while(OCI_BindInAllNext(bd1,bd2,0)){
			len=read(fd,buff,sizeof(buff));
			if(len<0)err_sys(errno,"read() error");
			if(len==0){
				stat=TRUE;
				break;
			}
			i++;
			if(!OCI_BindDataSetval(bd2,&i,0))break;
			if(!OCI_BindLobSetval(bd1,0,buff,(size_t)len))break;
		}
		if(!OCI_ExecBatch(cs,OCI_BindGetCur(bd1),0))break;
		OCI_BindTrunc(bd2,0);
		OCI_BindAllReuse(bd1,bd2,0);
	}
	printf("insert %d rows\n",i);
	//OCI_EndEnv();
	close(fd);
	return OCI_SUCCESS;
}

int test_select_update(OCI_Conn hd,OCI_Stmt cs,OCI_Err err){
	OCI_Res rs;
	OCI_Bind bd;
	if(!OCI_StmtParse(cs,STRING "select i,blb from TEST_OCIDATATYPE1 for update",err))return OCI_ERROR;
	if(!OCI_ExecQuery(cs,&rs,100,err))return OCI_ERROR;

	bd=OCI_ResConvertToBindByPos(rs,1,1,cs);
	if(!bd)return OCI_ERROR;

	while(OCI_FetchNext(rs,err)==OCI_GETSTAT_SUCCESS){
		OCI_BindSetCur(bd,OCI_ResGetFetchedRows(rs));
		while(OCI_BindOutNext(bd)){
			OCI_BindLobTrunc(bd,0);
		}
		OCI_Commit(hd,err);
		//OCI_BindResetIter(bd);
		OCI_BindReuse(bd);
	}
	return OCI_SUCCESS;
}

int main(){
	OCI_Conn	hd;
	OCI_Stmt	cs;
	OCI_Err	err;

	if(!OCI_InitEnv(OCI_MULTI_ENV|OCI_OBJECT)){
		printf("Initialize fail.\n");
		system("pause");
		exit(OCI_ERROR);
	}

	err=OCI_NewErr();
	if(!err){
		printf("Can't create handler of error fail.\n");
		system("pause");
		exit(OCI_ERROR);
	}

	hd=OCI_NewConn(USER,PWD,OID,err);
	if(!hd){
		printf("Can't create handler of connection.\n");
		system("pause");
		exit(OCI_ERROR);
	}

	cs=OCI_NewStmt(hd,err);
	if(!cs){
		printf("Can't create handler of cursor.\n");
		system("pause");
		exit(OCI_ERROR);
	}
	test_exec_plsql(hd,cs);
	test_oper_tow_database1();
	test_select_update(hd,cs,err);
	OCI_EndEnv();
	system("pause");

	return OCI_SUCCESS;
}