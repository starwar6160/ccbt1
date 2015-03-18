#ifndef jclmsCCB2014AlgCore_h__
#define jclmsCCB2014AlgCore_h__
#include <time.h>
//此处用这个条件定义使得JCLMSCCB2014_API在单独使用时定义为无，便于ARM等使用，
//而在被嵌套在一个更大的jclmsCCB2014.h中使用时使用其DLL导出导入的定义
#ifndef jclmsCCB2014_h__
#define JCLMSCCB2014_API
#endif

#ifdef  __cplusplus
extern "C" {
#endif


	typedef enum jc_error_code {
		EJC_SUSSESS,		//成功
		EJC_FAIL,		//失败
		EJC_INPUT_NULL,		//输入条件缺少
		EJC_DATETIME_INVALID,	//日期时间非法
		EJC_VALIDRANGE_INVALID,	//有效期非法
		EJC_CLOSECODE_INVALID,	//闭锁码非法
		EJC_CMDTYPE_TIMESTEP_INVALID,	//时间搜索步长非法
		EJC_CMDTYPE_TIMELEN_INVALID,	//时间搜索长度非法
		EJC_CMDTYPE_INVALID	//命令类型非法
	} JCERROR;


	typedef enum jc_cmd_type {
		JCCMD_START,		//无效命令
		//初始闭锁码，此时仅有ATM编号，锁编号，PSK三者决定，其余可变因素为定值
		//注意，初始闭锁码的生成，对于每个锁具仅有初始化时仅有一次，所以请仅仅在
		//锁具初始化时生成一次初始闭锁码
		JCCMD_INIT_CLOSECODE,
		JCCMD_CCB_DYPASS1,	//上位机第一开锁密码.此时“闭锁码”字段填写真正的闭锁码
		JCCMD_CCB_LOCK_VERCODE,	//下位机验证码.产生验证码时，“闭锁码”字段必须填写第一开锁密码
		JCCMD_CCB_DYPASS2,	//上位机第二开锁密码.产生第二开锁密码时，“闭锁码”字段填写验证码
		JCCMD_CCB_CLOSECODE,	//生成真正的闭锁码
		JCCMD_RESET_NFCKEY,	//重置NFC钥匙
		JCCMD_RESET_LOCKTIME,	//重设锁体时间
		JCCMD_RESET_LOCKSYSTEM,	//锁系统重置
		JCCMD_GET_LOCKLOG,	//提取锁体日志    
		JCCMD_END
	} JCCMD;

	typedef enum jc_input_type {
		JCI_START,		//无效值，用于界定上下范围
		JCI_ATMNO,		//ATM编号，初始化时设定
		JCI_LOCKNO,		//锁号，初始化时设定
		JCI_PSK,		//PSK，初始化时设定
		JCI_DATETIME,		//时间日期，单位是秒，应该是GMT秒数
		JCI_VALIDITY,		//有效期，单位是分钟
		JCI_CLOSECODE,		//闭锁码
		JCI_CMDTYPE,		//要生成什么类型的动态码
		JCI_TIMESTEP,		//时间往前推算的间隔，单位为秒
		JCI_SEARCH_TIME_LENGTH,	//反推时间长度，单位为秒，默认值是9分钟(从当前时间的过去6分钟到将来3分钟，适应可能的时间有误差的情形
		JCI_SEARCH_TIME_START,		//搜索时间起始值，默认为当前时间，一般不用设置，主要为了调试和单元测试目的设置。20141118新增；
		JCI_END			//无效值，用于界定上下范围
	} JCITYPE;

	//离线匹配的返回值
	typedef struct jcLockReverseMatchResult {
		int s_datetime;		//匹配结果秒数
		int s_validity;		//匹配结果有效期分钟数
	} JCMATCH;

	typedef enum jclmsd_request{
		JCLMS_CCB_INVALID,		//无效值
		JCLMS_CCB_CODEGEN,		//动态码生成
		JCLMS_CCB_CODEVERIFY	//动态码反推
	}JCLMSOP;

	//有效期数组大小；更改此处以后请对应更改源代码中JcLockInput类初始化代码中
	//为有效期数组m_validity_array赋予初值的相应语句
#define NUM_VALIDITY (8)
#define JC_ATMNO_MAXLEN (16)	//ATM编号长度最大值
#define JC_LOCKNO_MAXLEN (16)	//LOCK编号长度最大值
#define JC_PSK_LEN (256/4)	//256bit HEX+NULL,这是定长值
#define JC_INVALID_VALUE	(-1)

	//#pragma pack(1)
	typedef struct JcLockInput {
		//固定因素部分
		char AtmNo[JC_ATMNO_MAXLEN + 1];	//ATM号
		char LockNo[JC_LOCKNO_MAXLEN + 1];	//锁号
		char PSK[JC_PSK_LEN + 1];	//PSK，上下位机共同持有的唯一机密因素
		//可变因素部分
		int CodeGenDateTime;		//日期时间
		int Validity;		//有效期
		int CloseCode;	//闭锁码             	
		JCCMD CmdType;	//模式代码，比如开锁模式，远程重置模式，建行的流程要求的各种模式等等
		///////////////////////////////////以下为配置算法运作模式的数据///////////////////////////////////////
		int SearchTimeStart;	//搜索时间起始点，默认值应该是当前时间，但是比如测试时可以额外设定。20141118新增,主要供调试和单元测试使用
		//反推时间步长秒数，默认为在线模式，精度1分钟，值为60，离线模式请自己设置为3600秒或者其他数值
		int SearchTimeStep;
		//往前反推的时间长度秒数，默认为在线模式，10分钟，值为600，其他值比如离线24小时请自己设置
		int SearchTimeLength;
		//有效期，共有NUM_VALIDITY个,默认值是从5分钟到24小时那一系列，单位是分钟；可以自己设定
		//可以把最常用的有效期设置在更靠近开始处加快匹配速度
		int ValidityArray[NUM_VALIDITY];
	} JCINPUT;


	//分配一个内部数据结构，返回句柄，以后所有操作均以该句柄为对象
	int JCLMSCCB2014_API JcLockNew(void);
	//删除内部数据结构，释放内存空间，请在所有JCLMS相关代码结束前调用
	int JCLMSCCB2014_API JcLockDelete(const int handle);
	//设置整数类型的值
	JCERROR JCLMSCCB2014_API JcLockSetInt(const int handle, const JCITYPE mtype,
		int num);
	//设置字符串类型的值
	JCERROR JCLMSCCB2014_API JcLockSetString(const int handle, const JCITYPE mtype,
		const char *str);
	//设置命令类型(第一开锁码，初始闭锁码等等)
	JCERROR JCLMSCCB2014_API JcLockSetCmdType(const int handle, const JCITYPE mtype,
		const JCCMD cmd);
	//检查输入参数合法性
	JCERROR JCLMSCCB2014_API JcLockCheckInput(const int handle);
	//lock结构体内部m_cmdtype决定了生成哪一类动态码；
	int JCLMSCCB2014_API JcLockGetDynaCode(const int handle);
	//验证动态码，返回反推出来的时间和有效期结果，失败的话，两者均为0；
	JCMATCH JCLMSCCB2014_API JcLockReverseVerifyDynaCode(const int handle,
		const int dstCode);
	//指明该算法是哪一天出的，当算法有运算结果上的变更时这个版本改变，一天最多只出一个版本；
	int JCLMSCCB2014_API JcLockGetVersion(void);
	//输出调试字符串
	void JCLMSCCB2014_API JcLockDebugPrint(const int handle);
	void JCLMSCCB2014_API zwJcLockDumpJCINPUT(const int handle);
	int JCLMSCCB2014_API zwSM3StandardTestVector(void);

//////////////////////////////////////////////////////////////////////////
//生成第一，第二开锁码,验证码，闭锁码，初始闭锁码的共同函数，差异只在于CloseCode那个位置，
//在生成第一开锁码时填写的是前一次的闭锁码，生成验证码时填写的是第一开锁码，生成第二开锁码时填写的是验证码
//atm编号，锁编号都是不超过一定长度限度的随意的字符串，PSK是定长64字节HEX字符串相关长度限制请见头文件
//DyCodeUTCTime为指定动态码的时间UTC秒数，一般都是当前时间，但也可以为将来提前生成动态码而指定将来的时间
	int embSrvGenDyCode(const JCCMD Pass,const time_t DyCodeUTCTime,const int CloseCode,
		const char *AtmNo,const char *LockNo,const char *PSK);

//校验动态码，返回匹配的UTC时间秒数,需要的输入有：
//JCI_ATMNO,JCI_LOCKNO,JCI_PSK等3个基本条件
//以及CloseCode(此处指的是生成该动态码时填写的那个前一环节的输入条件)
//JCCMD指示校验的是哪一类的动态码
//SearchStartTime指定搜索起始时间，一般情况下就是当前时间的UTC秒数
int embSrvReverseDyCode(const JCCMD Pass,const int dyCode, const int CloseCode,const time_t SearchStartTime,
		const char *AtmNo,const char *LockNo,const char *PSK);
//从建行的2个输入因素生成PSK，结果是64字节HEX字符串；
const char * zwGenPSKFromCCB(const char * ccbFact1, const char * ccbFact2);

////////////////////////////////ECIES//////////////////////////////////////////
//从公钥，建行的2个输入因子字符串，输出激活信息字符串，输出缓冲区必须有头文件里面指定的足够大小
void zwGenActiveInfo(const char *pubkey,const char *ccbFact1,const char *ccbFact2,char *ccbActiveInfo);
//生成公钥私钥对,输入缓冲区必须有头文件里面宏定义值所指定的足够大小
void zwGenKeyPair(char *pubKey,char *priKey);
//从私钥，激活信息，获取PSK，输出缓冲区必须有头文件里面指定的足够大小
void zwGetPSK(const char *priKey,const char *ccbActiveInfo,char *PSK);


#ifdef WIN32
#ifdef _DEBUG
//#define ZWDEBUG(format,...) printf(" "__FILE__","__FUNCTION__",LINE:%d:"format"", __LINE__,##__VA_ARGS__)
//注意这里的format外面的双重引号一定要
#define ZWDBG_INFO(format,...) printf(""format"", ##__VA_ARGS__)
#define ZWDBG_NOTICE(format,...) printf(""format"", ##__VA_ARGS__)
#define ZWDBG_WARN(format,...) printf(""format"", ##__VA_ARGS__)
#define ZWDBG_ERROR(format,...) printf(""format"", ##__VA_ARGS__)
#else
#define ZWDBG_INFO(format,...) 
//#define ZWDBG_NOTICE(format,...) 
//#define ZWDBG_WARN(format,...) 
//#define ZWDBG_ERROR(format,...) 
#define ZWDBG_NOTICE(format,...) printf(""format"", ##__VA_ARGS__)
#define ZWDBG_WARN(format,...) printf(""format"", ##__VA_ARGS__)
#define ZWDBG_ERROR(format,...) printf(""format"", ##__VA_ARGS__)

#endif // _DEBUG
#endif // WIN32


#ifdef  __cplusplus
}	//extern "C" {
#endif
#endif // jclmsCCB2014AlgCore_h__
