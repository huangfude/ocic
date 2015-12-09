/*
简介：
		1.该函数库为OCI V8+的封装，可以进行DDL、DML、DCL、DQL语句操作。
		2.在操作数据时，可以对列进行单数据或多数据操作，单数据称为标量绑定，对应标量绑定变量的操作，多数据称为数组绑定，对应数组绑定变量的操作。
		3.所有操作对象都称为句柄。句柄具有从属（父子）关系，整个函数库的句柄关系如下：环境句柄--->连接句柄--->光标句柄--->绑定句柄、数据集合句柄 和 环境句柄--->错误句柄
		4.环境句柄，在使用该函数库时，将有一个隐藏全局变量，即环境句柄。环境句柄用于其他子句柄分配和初始化，它具有唯一性，所有其他句柄都从属于它，并自动管理着所有用句柄分配函数分配的子句柄，如链接句柄，光标句柄等。该全局变量用OCI_InitEnv()初始化，并用OCI_EndEnv()释放，当被释放时，所有分配的从属的子句柄都将会被释放。其他的句柄也有这个特性，当其被释放时，其子句柄都会被自动释放掉。
		5.连接句柄，管理着事务会话级的属性和操作。如数据库连接，会话开始，会话结束，数据库断开，事务提交、回滚等。连接就是会话，可以分配多个，且可以面向不同目标数据库的不同用户。从属于环境句柄，当被释放时，其下的所有子句柄被自动释放。
		6.光标句柄，管理着语句级的属性和操作。如SQL、PQL/SQL语句的分析，执行；绑定变量分配和释放等。每一个语句的操作可以拥有一个独立光标句柄，也可以重复利用。其从属于连接句柄，当被释放时，其下的所有子句柄被自动释放。
		7.绑定句柄，管理着所有数据级的属性和操作。如数据的类型获取和设置，数据的提取与装载等。绑定句柄可以被重复利用，单次绑定，多次执行等特性。绑定句柄从属于光标句柄，是一个分类和操作复杂，注意事项很多句柄，也是操作最频繁的句柄。具体函数介绍时，请仔细阅读。
		8.数据集合句柄，仅用于SELECT语句的提取操作，作为提取结果的容器。从属于光标句柄。
		9.错误句柄。从属环境句柄，用于重要错误的获取。
		
函数操作说明：
		1.在操作可变参数的函数时，必须以NULL值结束调用。
		2.所以带错误句柄参数的函数，都可以将错误句柄设置为NULL，即不获取错误消息。
		3.函数以BOOL型，void *指针型或自定义枚举指示调用的结果。
		5.绑定句柄的适配器操作，只用于数组绑定变量。当用于标量时，返回特定的恒定值。

一般操作步骤说明：
		1.初始化环境句柄，如果需要分配一个错误句柄
		2.连接数据库，得到一个连接句柄
		3.用连接句柄得到一个光标句柄
		4.用光标句柄分析SQL
		5.如果需要绑定变量，则使用光标句柄分配一个或多个绑定句柄（可无）
		6.如果第5步为真，则装载数据（如果是输入绑定）到绑定变量，并用相应的绑定函数进行绑定
		7.用光标句柄执行语句
			7.1如果是SELECT语句，则得到一个数据集合句柄，执行后从集合中得到数据
			7.2如果是数据操纵，且绑定了输出变量，则从绑定变量中获取数据
		8.如果第5步为真，则释放掉绑定句柄。（可以跳过）
		9.释放光标句柄。（可以跳过）
		10.释放连接句柄。（可以跳过）
		11.清理环境句柄。（不能跳过）
	注：此上为一般过程，有如用INSERT INTO RETURNING INTO语句操作LOB类型的语句有特殊的步骤顺序，详解具体函数说明。
	
函数说明：
*************************************环境句柄集合************************************************
功能：
		初始化数据库连接环境，清理数据库连接环境。
参数：
		mode：当设置为SINGLE_ENV时，为单数据库但用户操作。当设置为MULTI_ENV时，为多数据库多用户操作。
BOOL	OCI_InitEnv(int mode);
void	OCI_EndEnv(void);
注：
		以上两个函数为必须调用的函数。分别在数据库操作的开始和介绍时调用OCI_InitEnv()、OCI_EndEnv()；

*************************************连接句柄集合************************************************
功能：
		建立数据库连接，释放数据库连接
参数：
		usr：数据库用户名
		pwd：数据库密码
		sid：数据库SID
		err：错误句柄
OCI_Conn	OCI_NewConn(string usr,string pwd,string sid,OCI_Err err);
BOOL	OCI_FreeConn(OCI_Conn hd);

功能：
		事务的提交和回滚
参数：
		hd：连接句柄
		err：错误句柄
BOOL	OCI_Commit(OCI_Conn hd,OCI_Err err);
BOOL	OCI_Rollback(OCI_Conn hd,OCI_Err err);
注：
		但释放数据连接时，所有事务会被自动提交。

*************************************光标句柄集合************************************************
功能：
		分配和释放一个光标句柄
OCI_Stmt	OCI_NewStmt(OCI_Conn hd,OCI_Err err);
BOOL			OCI_FreeStmt(OCI_Stmt cs,....);

功能：
		分析SQL、PL/SQL语句
BOOL			OCI_StmtParse(OCI_Stmt cs,string sql,OCI_Err err);
注：
		sql语句不能以分号结尾，PL/SQL语句必须以分号结尾。
		

功能：
		设置执行模式
参数：
		mode:被设置的模式
			1.OCI_EXEC_BATCHERR	，设置后，批量执行，遇错时不中断。
			2.OCI_EXEC_AUTOCOMM	，设置后，执行成功后自动提交，仅对批处理SQL语句有效。
			3.OCI_EXEC_SCROLL	，设置后，SELECT语句被打开为可滚动模式，仅对SELECT SQL语句有效。
		set:TRUE设置，FALSE取消。
void	OCI_StmtSetOpt(OCI_Stmt cs,int mode,BOOL set);
注：
		1.当两个执行模式都被设置时，出错前的数据会被提交。
		2.当使用动态绑定数据类型时，不能设置为自动提交，否则不能写入或读取动态类型的变量。
		3.当使用select ... for update时，亦不能设置为自动提交。

功能：
		得到分析语句的类型
short			OCI_StmtGetStmtType(OCI_Stmt cs);
注：
		语句类型如下：
    OCI_UNKNOWN：未知类型或未分析
    OCI_SELECT：查询语句
    OCI_UPDATE：更新语句
    OCI_DELETE：删除语句
    OCI_INSERT：插入语句
    OCI_CREATE：创建对象语句
    OCI_DROP：删除对象语句
    OCI_ALTER：修改对象语句
    OCI_BEGIN：不带定义的PL/SQL语句
    OCI_DECLARE：带定义的PL/SQL语句
    OCI_CALL
    

功能：
		得到影响的列数
size_t			OCI_StmtGetAffectRows(OCI_Stmt cs,OCI_Err err);
注：
		对DML语句有效。


功能：
		执行DCL，DDL，DML，PL/SQL语句
BOOL			OCI_ExecNoQuery(OCI_Stmt cs,OCI_Err err);
注：
		1.DDL，DML的绑定必须是标量语句
		2.唯一执行PL/SQL语句的函数
		


功能：
		批量执行DML语句
参数：
		max：批处理的行数，小于或等于当前数组绑定变量中的有效数据量，且不能为0。
BOOL			OCI_ExecBatch(OCI_Stmt cs,ushort max,OCI_Err err);
注：
	1.批处理的行数，必须由程序员保证其正确性。否则会出现不可预知的错误。
	2.只能用于执行绑定了数组绑定变量的DML语句。


功能：
		执行查询
参数：
		max:每次OCI_FetchNext()的提取行数，不能为0，小于OCI_MAX_DEFINE。
BOOL			OCI_ExecQuery(OCI_Stmt cs,OCI_Res *rs,ushort max,OCI_Err err);
		


*************************************绑定句柄************************************************
重要说明：
	1.利用SQL插入CLOB或BLOB数据时，利用像insert into tab values (other_col,empty_clob(),empty_blob(),...) returning [lob_col,..] into [:lob_dnc_bind,...]的SQL语句，且动态绑定到into后的定位符。
		1).必须在执行之后写入绑定LOB变量的数据。
		2).在写入数据之前，不能进行提交，包括自动提交。
		3).占用所在表的表空间，不占用本地内存。
		4).LOB列的绑定类型必须是OCI_CLOB或OCI_BLOB外部数据类型，见外部数据类型说明。
	2.利用SQL插入CLOB或BLOB数据时，利用像insert into tab values (:col_other,:col_clob,:col_blob,...)的SQL语句，且静态绑定到定位符。
		1).必须在执行之前写入绑定变量的数据。
		2).可以自动提交。
		3).绑定类型为OCI_BLOB或OCI_CLOB时，占用所在临时的表空间，不占用本地内存，其他转换类型，占用本地内存。
		4).LOB列的绑定类型可以是能相互转换外部数据类型，见外部数据类型说明。
	3.在执行像update/delete ... returning col1,... into :bind_col1,...来获取删除的数据时，必须动态绑定into后的定位符。
		1).必须执行之后读取的数据。
		2).在读取数据之前，不能进行提交，包括自动提交。
	4.在静态绑定变量用于PL/SQL时，除OCI_CLOB、OCI_BLOB外，包括各种数据转换绑定，其他的外部数据类型都不能超过64K字节的长度。
	5.对于绑定句柄的重复使用（单次绑定，重复执行，设置值，提交），使用前必须调用OCI_BindReuse()。
	6.抹去数据的操作，数据占用的空间不变，具体规则如下：
		1).对于CLOB字段，抹去的字符被空格填充。
		2).对于BLOB字段，抹去的字节被'\0'填充。
		3).对于OCI_INT/OCI_FLT绑定字段，值被写为0。
		4).对于OCI_DATE/OCI_NUM绑定字段，值被写未知。
		5).对于OCI_TMSTAMP绑定字段，无效。
	7.对于用于输出的绑定，只能对有效值进行设置，即索引小于OCI_BindGetCur()的值。动态绑定，结果集合转换得到的绑定都属于输出绑定。
	8.对于LOB数据可以使用OCI_BindLobTrunc*()进行截断，从而减小其在数据库中的占用空间。


功能：
		分配一个绑定变量数组或标量
参数：
  	1.name、pos:占位符的名字和位置，占位符位置必须从1开始
  	2.type:外部类型
  		1).能处理OCI_INT、OCI_FLD、OCI_STR、OCI_LNG、OCI_LNG、OCI_LGB、OCI_BIN、OCI_CLOB、OCI_BLOB、OCI_DATE、OCI_NUM、OCI_TMSTAMP数据类型
  	3.max:数组大小，标量为0，大于0为数组，必须小于小于OCI_MAX_BINDS
  	4.size:当外部类型不为数值型数据时，单个数组的最大值，
  		1).处理数据类型为OCI_INT、OCI_FLD、OCI_DATE数据类型时被忽略有效。
  		2).处理除OCI_INT、OCI_FLD、OCI_DATE为0时，将使用mocidef.h中定义的默认数据（比较消耗内存）。

OCI_Bind			OCI_NewStcBindByName(OCI_Stmt cs,const string name,ushort type,ushort max,size_t size);
OCI_Bind			OCI_NewStcBindByPos(OCI_Stmt cs,ushort pos,ushort type,ushort max,size_t size);

OCI_Bind			OCI_NewDncBindByName(OCI_Stmt cs,const string name,ushort type,ushort max,size_t size);
OCI_Bind			OCI_NewDncBindByPos(OCI_Stmt cs,ushort pos,ushort type,ushort max,size_t size);
注：
  	1.当SQL语句中的定位符为字符，且SQL语句不带returning [col,..] into [bind,...] 子句时，用OCI_NewStcBindByName()分配一个用于静态绑定的变量，而后必须用OCI_BindByName()进行绑定；
  	2.当SQL语句中的定位符为数值，且SQL语句不带returning [col,..] into [bind,...]子句时，用OCI_NewStcBindByPos()分配一个用于静态绑定的变量，而后必须用OCI_BindByPos()进行绑定；
  	3.当SQL语句中的定位符为字符，且SQL语句带returning [col,..] into [bind,...]子句时，用OCI_NewDncBindByName()分配一个用于动态绑定的变量，而后必须用OCI_BindByName()进行绑定；
  	4.当SQL语句中的定位符为数值，且SQL语句带returning [col,..] into [bind,...]子句时，用OCI_NewDncBindByPos()分配一个用于动态绑定的变量，而后必须用OCI_BindByPos()进行绑定；
  	5.不要将OCI_LNG、OCI_LGB数据类型作为PL/SQL的绑定。
	6.在使用INSERT INTO语句操作LONG、LONG RAW类型数据时，可以操作大于64K的数据，但是必须从逻辑写入字节大小，比如在数据库的字段中存储当前操作数据的字节数。

功能：
		抹去绑定变量的所有已设置的数据
BOOL				OCI_BindErase(OCI_Bind bd);
BOOL				OCI_BindAllErase(OCI_Bind bd,...);
注：
		只是抹去数据，即字节上的零值。

功能：
		释放多个绑定变量，必须以NULL结尾
void	OCI_FreeBind(OCI_Bind bd,...);

功能：
		重用一个或多个绑定变量。
void	OCI_BindReuse(OCI_Bind bd);
void	OCI_BindAllReuse(OCI_Bind bd,...);
注：
		该功能会间接的调用OCI_BindResetIter()函数。

功能：
		绑定一个或多个绑定变量
参数：
		err：分配好的错误存储变量，可以为NULL
		bd：待绑定的已分配的绑定变量，必须与分配函数对应，且不能为NULL
BOOL	OCI_BindByName(OCI_Err err,OCI_Bind bd,...);
BOOL	OCI_BindByPos(OCI_Err err,OCI_Bind bd,...);
注：
		请参见分配绑定变量函数说明

***********************************绑定句柄辅助属性操作***************************************
功能：
		得到当前适配器或指定位置的指示器值
参数：
		idx：变量在数组中的位置，不能大于绑定最数据量，否则异常退出
short	OCI_BindGetIdct(OCI_Bind bd);
short	OCI_BindGetIdctIdx(OCI_Bind bd,ushort idx);
注：
		1.对绑定变量数组或动态绑定调用OCI_BindGetIdct()前，必须调用OCI_BindOutNext()且返回必须为真，否则异常退出。
		2.对于数组或动态绑定，调用OCI_BindGetIdctIdx()时idx必须小于OCI_BindGetCur()的返回值，否则异常退出。
		3.仅当绑定变量作为输出时有意义，如PL/SQL的输出绑定、return into 子句动态绑定时。
		4.返回OCI_IDCT_INVALID时，表示当前数据无效。

功能：
		得到当前适配器或指定位置的返回码值
参数：
		idx：变量在数组中的位置，不能大于绑定最大有效数据量，否则异常退出
ushort	OCI_BindGetRecode(OCI_Bind bd);
ushort	OCI_BindGetRecodeIdx(OCI_Bind bd,ushort idx);
注：
		1.对绑定变量数组或动态绑定调用OCI_BindGetRecode()前，必须调用OCI_BindOutNext()且返回必须为真，否则异常退出。
		2.对于数组或动态绑定，调用OCI_BindGetRecodeIdx()时idx必须小于OCI_BindGetCur()的返回值，否则异常退出。
		3.仅当绑定变量作为输出时，且在执行后获取才有意义，如PL/SQL的输出绑定、return into 子句动态绑定时。
		4.返回OCI_RCODE_INVALID时，表示当前数据无效。

功能：
		得到绑定变量数组的当前有效数据量
ushort	OCI_BindGetCur(OCI_Bind bd);
注：
		如果静态绑定用于PL/SQL的标量输出时，无意义，且不能用此值判断数据有效性。必须通过OCI_Bind*Getval*()系列函数的返回来判断有效性。

功能：
		得到绑定变量数组的当前最大数据量
ushort	OCI_BindGetMax(OCI_Bind bd);
注：
		如果作用于动态绑定，与OCI_BindGetCur()效果一致


***********************************绑定句柄适配器操作***************************************
功能：
		对用于输入的绑定，移动一个或同时移动多个数据适配器
BOOL	OCI_BindInNext(OCI_Bind bd);
BOOL	OCI_BindInAllNext(OCI_Bind bd,...);
注：
		1.数据适配器用于绑定变量数组，当作用于标量时永远返回TRUE；
		2.调用OCI_Bind*Get*()系列函数之前，必须调用此函数。

功能：
		对用于输出的绑定，移动一个或同时移动多个数据适配器
BOOL	OCI_BindOutNext(OCI_Bind bd);
BOOL	OCI_BindOutAllNext(OCI_Bind bd,...);
注：
		1.数据适配器用于绑定变量数组，当作用于标量时永远返回TRUE；
		2.调用OCI_Bind*Get*()系列函数之前，必须调用此函数。

功能：
		重置一个或多个绑定变量的数据适配器
void	OCI_BindResetIter(OCI_Bind bd);
void	OCI_BindAllResetIter(OCI_Bind bd,...);
注：
		1.重置后，可以再次利用OCI_BindInNext()、OCI_BindOutNext()和OCI_Bind*Get*()、OCI_Bind*Set*()函数循环对绑定句柄进行操作。

功能：
		得到绑定的数据类型
ushort	OCI_BindGetDataType(OCI_Bind bd);

*************************************数据操作***************************************

普通数据操作只处理，数值型，普通字符型，普通二进制：
		OCI_INT、
		OCI_FLT、
		OCI_STR、
		OCI_LNG、
		OCI_LGB、
		OCI_BIN
		数据类型

功能：
		用于设置绑定变量的当前适配器位置和指定位置的值
参数：
		1.idx：要设置的绑定变量的位置，当绑定标量时，忽略该参数
		2.data：当为NULL或UNDEF值时，该位置的数据被设置成ORACLE中的NULL值。
		3.size：设置的数据字节大小
			1).绑定数值类型时，忽略
			2).绑定OCI_STR类型时且为0时，位置字节大小由字符串长度len决定
			3).绑定二进制类型时，拷贝size个字节到该位置
BOOL	OCI_BindDataSetval(OCI_Bind bd,const void *data,size_t size);
BOOL	OCI_BindDataSetvalIdx(OCI_Bind bd,ushort idx,const void *data,size_t size);
注：
		1.当绑定数组用于PL/SQL时，单个数据字节不能超过64k。
		2.用于DML语句且要数据设置为ORACLE空值时，可以不调用OCI_BindDataSetval*()；
		3.用于PL/SQL的过程调用时必须调用OCI_BindDataSetval*()，否则报OCI_NO_DATA错误。
		4.对绑定变量数组调用OCI_BindDataSetval()前，必须调用OCI_Bind*Next()且返回必须为真，否则异常退出。
		5.对于静态绑定idx必须小于OCI_BindGetMax()的返回值，才能调用OCI_BindDataSetvalIdx()，否则异常退出。
		6.对于动态绑定idx必须小于OCI_BindGetCur()的返回值，才能调用OCI_BindDataSetvalIdx()，否则异常退出。
		

功能：
		用于获取绑定变量的当前适配器位置和指定位置的值
	
参数：
		1.idx:要设置的绑定变量的位置，当绑定标量时，忽略该参数
		2.data：必须位一个可用的数据块空间
		3.size：data数据块空间的字节大小，调用后返回获取的数据字节数
OCI_GETSTAT OCI_BindDataGetval(OCI_Bind bd,void *data,size_t *size);
OCI_GETSTAT OCI_BindDataGetvalIdx(OCI_Bind bd,ushort idx,void *data,size_t *size);
注：
		1.成功时返回OCI_GETSTAT_SUCCESS；
		2.返回值为OCI_GETSTAT_ERR时，表示获取出错，*size为0；返回值为OCI_GETSTAT_NONDATA时，表示绑定变量没有有效数据（绑定输出时，没有对输出变量赋值），*size为0。
		3.返回值为OCI_GETSTAT_NULL时，指定获取数据为ORACLE空值。
		4.对绑定变量数组调用OCI_BindDataGetval()前，必须调用OCI_Bind*Next()且返回必须为真，否则异常退出。

*************************************LOB数据类型操作***************************************
	LOB数据类型：
		OCI_CLOB、
		OCI_BLOB、
	
	将LOB数据类型看成一个带偏移指示器的2G容量的管道来进行操作。
注：
	1.对于OCI_CLOB，偏移值对应的是从起始位置开始的字符数，而对于OCI_BLOB是字节数。
	2.OCI_CLOB的一个字符所占的字节数与服务器的字符集有关。
	3.如果服务器是多字节字符集，而OCI_CLOB的读取是按字符数读取，故分配的缓冲区要尽量的大。

功能：
		用于设置当前适配器位置和指定位置处的指定偏移处进行的值
参数：
		1.off：从LOB定位的起始位置起的偏移值。为0时，使用内部偏移，成功后内部偏移指示器改变
		2.data：当为NULL或UNDEF值时，该位置的数据被设置成ORACLE中的NULL值。
		3.size：设置的数据字节大小
			1).绑定OCI_CLOB类型，且为0时，设置字节大小由字符串长度len决定
BOOL	OCI_BindLobSetval(OCI_Bind bd,size_t off,const void *data,size_t len);
BOOL	OCI_BindLobSetvalIdx(OCI_Bind bd,ushort idx,size_t off,const void *data,size_t len);
注：
		1.绑定变量用于DML要将数据设置为ORACLE空值时，可以不调用OCI_BindDataSetvalIdx()或OCI_BindDataSetval()；
		2.绑定数组变量在用于PL/SQL的过程调用时，必须调用OCI_BindDataSetvalIdx()或OCI_BindDataSetval()，否则报OCI_NO_DATA错误。
		3.对绑定变量数组或动态绑定调用OCI_BindDataSetval()前，必须调用OCI_Bind*Next()且返回必须为真，否则异常退出。
		4.对于LOB类型的数据，请及时处理事务和清理绑定句柄。
		5.对于静态数组绑定idx必须小于OCI_BindGetMax()的返回值，才能调用OCI_BindLobSetvalIdx()。否则异常退出。
		6.对于动态绑定idx必须小于OCI_BindGetCur()的返回值，才能调用OCI_BindLobSetvalIdx()。否则异常退出。
		7.对于动态绑定，以及OCI_Res转换，赋值来的绑定句柄，必须在OCI_Exec*()之后且在提交之前设置数据。

功能：
		用于获取绑定变量的当前适配器位置和指定位置的值
	
参数：
		1.idx：要获取的绑定变量的位置，当绑定标量时，忽略该参数
		2.data：必须位一个可用的数据块空间
		3.size：data数据块空间的字节大小，调用后返回获取的数据字节数
		4.off：从LOB定位的起始位置起的偏移值。为0时，使用内部偏移，成功后内部偏移指示器改变
OCI_GETSTAT OCI_BindLobGetval(OCI_Bind bd,size_t off,void *data,size_t *len);
OCI_GETSTAT OCI_BindLobGetvalIdx(OCI_Bind bd,ushort idx,size_t off,void *data,size_t *len);
注：
		1.成功时返回OCI_GETSTAT_SUCCESS；
		2.返回值为OCI_GETSTAT_ERR时，表示获取出错，*size为0；返回值为OCI_GETSTAT_NONDATA时，表示绑定变量没有有效数据（绑定输出时，没有对输出变量赋值），*size为0。
		3.返回值为OCI_GETSTAT_NULL时，指定获取数据为ORACLE空值。
		4.对绑定变量数组或动态绑定调用OCI_BindDataGetval()前，必须调用OCI_Bind*Next()且返回必须为真，否则异常退出。
		5.对于数组或动态绑定idx必须小于OCI_BindGetCur()的返回值，才能调用OCI_BindLobGetvalIdx()，否则异常退出。
		6.如果OCI_CLOB读取的数据存在双字节，则读取字符数要比缓冲字节数的二分之一小，所有要分配大一点的缓冲空间，以提高读取效率。
		7.对于动态绑定，以及OCI_Res转换，赋值来的绑定句柄，必须在OCI_Exec*()之后且在提交之前设置数据。

功能：
		重置当前数据器位置或指定位置的LOB偏移
参数：
		idx：必须小于绑定变量数组的最大数据量，且大于0
void	OCI_BindLobResetOff(OCI_Bind bd);
void	OCI_BindLobResetOffIdx(OCI_Bind bd,ushort idx);
void	OCI_BindAllLobResetOffIdx(ushort idx,OCI_Bind bd,...);
注：
		1.idx必须小于OCI_BindGetMax()的返回值，才能调用OCI_BindLobResetOffIdx()、OCI_BindAllLobResetOffIdx()，否则异常退出。
		2.对绑定变量数组或动态绑定调用OCI_BindLobResetOff()前，必须调用OCI_Bind*Next()且返回必须为真，否则异常退出。


功能：
		得到当前适配器或指定位置的LOB值长度
BOOL	OCI_BindLobGetLen(OCI_Bind bd,size_t *size);
BOOL	OCI_BindLobGetLenIdx(OCI_Bind bd,ushort idx,size_t *size);
注：
		1.对于静态绑定idx必须小于OCI_BindGetMax()的返回值，才能调用OCI_BindLobGetLenIdx()，否则异常退出。
		2.对于动态绑定idx必须小于OCI_BindGetCur()的返回值，才能调用OCI_BindLobGetLenIdx()，否则异常退出。
		3.对绑定变量数组或动态绑定调用OCI_BindLobGetLen()前，必须调用OCI_Bind*Next()且返回必须为真，否则异常退出。
		4.对于OCI_CLOB类型，size返回的是字符数，而对于OCI_BLOB返回字节数。

功能：
		在当前适配器或指定位置，从内部偏移或指定偏移的位置起，抹去指定长度的LOB数据
参数：
		off:偏移位置，当为0时，使用内部偏移位置。
		len:抹去的数据长度，当为0时，抹去该LOB数据当前off偏移位置起的剩余数据。
BOOL	OCI_BindLobErase(OCI_Bind bd,size_t off,size_t len);
BOOL	OCI_BindLobEraseIdx(OCI_Bind bd,ushort idx,size_t off,size_t len);
注：
		1.对于静态绑定idx必须小于OCI_BindGetMax()的返回值，才能调用OCI_BindLobEraseIdx()，否则异常退出。
		2.对于动态绑定idx必须小于OCI_BindGetCur()的返回值，才能调用OCI_BindLobEraseIdx()，否则异常退出。
		3.对绑定变量数组或动态绑定调用OCI_BindLobErase()前，必须调用OCI_BindInNext()且返回必须为真，否则异常退出。

功能：
		在当前适配器或指定位置，从内部偏移或指定偏移的位置起，截断到指定大小的LOB数据
参数：
		len:截断到指定大小。
BOOL	OCI_BindLobTrunc(OCI_Bind bd,size_t len);
BOOL	OCI_BindLobTruncIdx(OCI_Bind bd,ushort idx,size_t len);

*************************************错误集合************************************************
功能：
		分配一个错误句柄
OCI_Err	OCI_NewErr();
功能：
		从错误句柄得到出错的信息
string	OCI_ErrGetmsg(OCI_Err err);
功能：
		从错误句柄得到出错的函数
string	OCI_ErrGetfunc(OCI_Err err);
功能：
		返回值不为0则出错或出现警告
short	OCI_ErrGetStat(OCI_Err err);

*************************************日期操作集合************************************************
功能：
		使用指定格式，OCIDate转换到字符串，字符串转换到CIData。
参数：
		format：指定的日期格式，如果为0，则使用默认OCI_DATEFULL_FORMAT格式。
		1.OCI_DATEFULL_FORMAT为"YYYY/MM/DD HH24:MI:SS"
        2.OCI_DATE_FORMAT为"YYYY/MM/DD"
		3.OCI_TIME_FORMAT为"HH24:MI:SS"
BOOL	OCI_DateToString(const OCIDate *date,string buf,size_t len,const string format,OCI_Err err);
BOOL	OCI_StringToDate(const string buf,OCIDate *date,const string format,OCI_Err err);
注：
		格式如何定义，请参考ORACLE的to_date()，to_char()函数中的格式。

功能：
		分别获取或设置OCIDate变量中的年月日，时分秒。
BOOL	OCI_DateSetDate(OCIDate *date,short year,char mon,char day);
BOOL	OCI_DateSetTime(OCIDate *date,char hour,char min,char sec);
BOOL	OCI_DateGetDate(const OCIDate *date,short *year,char *mon,char *day);
BOOL	OCI_DateGetTime(const OCIDate *date,char *hour,char *min,char *sec);

功能：
		对OCIDate变量进行加减天数或月数，以获取一个新的日期。
参数：
		days、mons:为负数时，为减操作。为正数时，为加操作。
BOOL	OCI_DateAddDays(const OCIDate *old_dt,OCIDate *new_dt,int days);
BOOL	OCI_DateAddMons(const OCIDate *old_dt,OCIDate *new_dt,int mons);

功能：
		对给定日期，使用指定星期数计算下一个星期数的日期。
BOOL	OCI_DateNextDay(const OCIDate *old_dt,OCIDate *new_dt,const string day);

功能：
		使用给定日期，计算出本月的最后一天的日期。
BOOL	OCI_DateLastDay(const OCIDate *old_dt,OCIDate *new_dt);

功能：
		计算两个日期相差的天数
BOOL	OCI_DateDaysBetween(const OCIDate *dt1,const OCIDate *dt2,int *days);

功能：
		获取本地日期时间。
BOOL	OCI_DateLocalDate(OCIDate *date);

*************************************数值操作集合************************************************
功能：
		将一个OCINumber变量置零
void	OCI_NumberSetZero(OCINumber *num);
功能：
		对两个给点的OCINumber变量进行加、减、乘、除、模、乘方，以得到一个新的变量。
BOOL	OCI_NumberAdd(const OCINumber *num1,const OCINumber *num2,OCINumber *num);
BOOL	OCI_NumberSub(const OCINumber *num1,const OCINumber *num2,OCINumber *num);
BOOL	OCI_NumberMul(const OCINumber *num1,const OCINumber *num2,OCINumber *num);
BOOL	OCI_NumberDiv(const OCINumber *num1,const OCINumber *num2,OCINumber *num);
BOOL	OCI_NumberMod(const OCINumber *num1,const OCINumber *num2,OCINumber *num);
BOOL	OCI_NumberIntPower(const OCINumber *num1,int exponent,OCINumber *num);

功能：
		使用指定格式，OCINumber转换到字符串，字符串转换到OCINumber。
参数：
		format：指定的数字格式，如果为0，则使用默认OCI_NUMBER_FORMAT格式。
BOOL	OCI_NumberToString(const OCINumber *num1,string buf,size_t len,const string format,OCI_Err err);
BOOL	OCI_StringToNumber(const string buf,OCINumber *num1,const string format,OCI_Err err);
注：
		参考ORACLE的to_char()、to_number()的格式字符串。

功能：
		将OCINumber转换到C整型，C整型转换到OCINumber
参数：
		flag：
			1.OCI_NUMBER_UNSIGNED，C整型（unsigned int *）无符号指针，默认值。
			2.OCI_NUMBER_SIGNED、C整型（int *）有符号指针
BOOL	OCI_NumberToInt(const OCINumber *num1,void *result,ushort flag);
BOOL	OCI_IntToNumber(const void *result,OCINumber *num1,ushort flag);

功能：
		将OCINumber转换到C浮点型，C浮点型转换到OCINumber
参数：
		flag：
			1.OCI_NUMBER_FLOAT，C单精度（float *）指针，默认值。
			2.OCI_NUMBER_DOUBLE，C双精度（double *）指针。
			3.OCI_NUMBER_LDOUBLE，C长双精度（long double *）指针。
BOOL	OCI_NumberToReal(const OCINumber *num1,void *result,ushort flag);
BOOL	OCI_RealToNumber(const void *result,OCINumber *num1,ushort flag);

**************************************时间戳类型操作函数*******************************************
功能：
		分配、释放一个新的时间句柄
参数：
		type：OCI_DATETIME_TS:普通时间戳
					OCI_DATETIME_TS_TZ:带时区的时间戳
					OCI_DATETIME_TS_LTZ:本地时区的时间戳
OCI_DateTime	OCI_NewDateTime(uint type);
void	OCI_FreeDateTime(OCI_DateTime dt);
注：
		时间句柄主要用于时间戳的操作。

功能：
		设置时间句柄的年月日，时分秒，以及秒的小数部分。
BOOL	OCI_DateTimeGetDate(const OCI_DateTime dt,short *year,uchar *mon,uchar *day);
BOOL	OCI_DateTimeGetTime(const OCI_DateTime dt,uchar *hour,uchar *min,uchar *sec,uint *fsec);
注：
		fsec的指针指向的值必须小于或等于一个最大9位数值。

功能：
		根据指定的转换格式，将一个时间句柄转换为字符串
BOOL	OCI_DateTimeToString(const OCI_DateTime dt,string buf,size_t len,const string format,OCI_Err err);
BOOL	OCI_StringToDateTime(const string buf,OCI_DateTime dt,const string format,OCI_Err err);
注：
		当格式字符串为空时，使用当前会话（环境）的默认格式。

功能：
		使用给定值赋值一个时间句柄。
BOOL	OCI_DateTimeConstruct(OCI_DateTime dt,short year,uchar month,uchar day,uchar hour,uchar min,uchar sec,uint fsec,string timezone);
注：
		fsec的值必须小于或等于一个最大9位数值。

功能：
		获取本地时间戳
BOOL	OCI_DateTimeLocalTimeStamp(OCI_DateTime dt);

功能：
		获取时区偏移时间、时区的名字
BOOL	OCI_DateTimeGetTimeZone(const OCI_DateTime dt,char *hour,char *min);
BOOL	OCI_DateTimeGetTimeZoneName(const OCI_DateTime dt,string name,size_t len);
注：
		只能对OCI_DATETIME_TS_TZ,OCI_DATETIME_TS_LTZ的时间句柄使用。

*************************************结果集操作集合**********************************************
结果集的注意事项：
		1.只能用OCI_ExecQuery()从已分析的OCI_Stmt句柄中获取。
		2.在使用索引对OCI_Res变量进行获取操作时，必须相应的小于OCI_ResGetCols()和(或)OCI_ResGetFetchedRows()返回值。
		3.在使用适配器对OCI_Res变量进行获取操作时，必须先相应调用的OCI_ResNextRow()和(或)OCI_ResNextCol()。
		4.暂时只能对Oracle的number、date、char、varchar2、clob、blob列类型进行提取操作。
		
功能：
		在光标的当前列开始（包括当前列）提取一组数据到OCI_Res中，数据行数由OCI_ExecQuery()的max参数决定。
OCI_GETSTAT	OCI_FetchNext(OCI_Res rs,OCI_Err err);

功能：
		提取当前光标指针的上一行数据到OCI_Res中
OCI_GETSTAT	OCI_FetchPrior(OCI_Res rs,OCI_Err err);

功能：
		提取当前光标指针的当前行数据到OCI_Res中
OCI_GETSTAT	OCI_FetchCurrent(OCI_Res rs,OCI_Err err);

功能：
		提取光标中的前几行数据，成功后，撤销提取光标的操作性。
BOOL	OCI_FetchTop(OCI_Res rs,ub2 top,OCI_Err err);

功能：
		释放一个结果集
void	OCI_FreeRes(OCI_Res rs);

功能：
		得到一个结果集的列数、提取行数、最大行容积，
ushort	OCI_ResGetCols(OCI_Res rs);
ushort	OCI_ResGetFetchedRows(OCI_Res rs);
ushort	OCI_ResGetMaxRows(OCI_Res rs);

功能：
		移动当前适配器到下一行，对OCI_Res指针进行无索引的数据操作之前，必须调用
BOOL	OCI_ResNextRow(OCI_Res rs);

功能：
		移动当前适配器到下一列，对OCI_Res指针进行无索引的数据和属性操作之前，必须调用
BOOL	OCI_ResNextCol(OCI_Res rs);

功能：
		重置行、列适配器
void	OCI_ResResetColIter(OCI_Res rs);
void	OCI_ResResetRowIter(OCI_Res rs);
注：
		重置行适配器时，列适配器也会被重置。

功能：
		得到当前列适配器或指定位置的列名
string	OCI_ResGetColName(OCI_Res rs);
string	OCI_ResGetColNameIdx(OCI_Res rs,ushort col);

功能：
		得到当前列适配器或指定位置的精度与刻度。仅对数值型的列有效。参见ORACLE的number类型的列属性。
BOOL	OCI_ResGetColPreScale(OCI_Res rs,int *pre,int *scale);
BOOL	OCI_ResGetColPreScaleIdx(OCI_Res rs,ushort col,int *pre,int *scale);

功能：
		得到当前列适配器或指定位置的列类型。
ushort	OCI_ResGetOutType(OCI_Res rs);
ushort	OCI_ResGetOutTypeIdx(OCI_Res rs,ushort col);

功能：
		得到当前列适配器或指定位置的列的最大值。
size_t	OCI_ResGetColMaxSize(OCI_Res rs);
size_t	OCI_ResGetColMaxSizeIdx(OCI_Res rs,ushort col);

功能：
		得到当前行、列适配器或指定行列位置的普通数据。
参数：
		size：当为0时，表示无数据返回，或获取出错。
返回值：
		OCI_GETSTAT_SUCCESS：返回成功
		OCI_GETSTAT_ERROR：获取出错
		OCI_GETSTAT_NULL：数据为oracle的NULL值
		OCI_GETSTAT_NONDATA：当前数据没有被赋值过。
OCI_GETSTAT OCI_ResDataGetval(OCI_Res rs,const void *data,size_t *size);
OCI_GETSTAT OCI_ResDataGetvalIdx(OCI_Res rs,ushort row,ushort col,const void *data,size_t *size);
注：
		当列类型为number、date时，size可以为空。

功能：
		得到当前行、列适配器或指定行列位置的LOB数据。
参数：
		size：当为0时，表示无数据返回，或获取出错。
返回值：
		OCI_GETSTAT_SUCCESS：返回成功
		OCI_GETSTAT_ERROR：获取出错
		OCI_GETSTAT_NULL：数据为oracle的NULL值
		OCI_GETSTAT_NONDATA：表示当前偏移处没有数据。
OCI_GETSTAT OCI_ResLobGetval(OCI_Res rs,size_t off,void *data,size_t *len);
OCI_GETSTAT OCI_ResLobGetvalIdx(OCI_Res rs,ushort row,ushort col,size_t off,void *data,size_t *len);
注；
		len指针返回获取的字节数。

功能：
		得到当前行、列适配器或指定行列位置的LOB数据的长度。
BOOL	OCI_ResLobGetLen(OCI_Res rs,size_t *len);
BOOL	OCI_ResLobGetLenIdx(OCI_Res rs,ushort row,ushort col,size_t *len);
注：
		当列类型为CLOB时，len指针返回字符数，否则为字节数。

功能：
		转换成一个临时的数组绑定句柄，可绑定到其他语句句柄，用于插入其他表；对于FOR UPDATA的子句，可以用于修改LOB字段
OCI_Bind	OCI_ResConvertToBindByPos(const OCI_Res rs,ushort col,ushort pos,OCI_Stmt cs);
OCI_Bind	OCI_ResConvertToBindByName(const OCI_Res rs,ushort col,const string name,OCI_Stmt cs);
注：
		当转换它的结果集合被释放时，对临时绑定句柄的操作将出现异常。

功能：
		设置临时的数组绑定句柄的当前有效值，仅对转换来的绑定有效
void		OCI_BindSetCur(OCI_Bind bd,ushort cur);
void		OCI_BindAllSetCur(ushort cur,OCI_Bind bd,...);

功能：
		把集合复制到另一个绑定句柄，可绑定到其他语句句柄，用于插入其他表；对于FOR UPDATA的子句，可以用于修改LOB字段
BOOL		OCI_ResAssignToBind(const OCI_Res rs,ushort col,OCI_Bind bd);
注：
		1.当赋值给它的结果集合被释放时，对该绑定句柄的操作不会出现异常。
		2.要被赋值的结果集合的列输出类型必须要与绑定句柄的绑定类型一致。
*/