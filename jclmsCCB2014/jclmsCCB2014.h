#ifndef jclmsCCB2014_h__
#define jclmsCCB2014_h__
//#include "zwstdafx.h"

// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 JCLMSCCB2014_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// JCLMSCCB2014_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
//在ARM上使用请打开该宏定义以便消除windows的DLL相关定义的编译错误
//#define _USEON_NONWIN32
#ifdef _ZWUSE_AS_JNI
#define JCLMSCCB2014_API
#else
#ifdef JCLMSCCB2014_EXPORTS
#define JCLMSCCB2014_API __declspec(dllexport)
#else
#define JCLMSCCB2014_API __declspec(dllimport)
#endif
#endif	//_ZWUSE_AS_JNI

//////////////////////////////////////////////////////////////////////////
//namespace jclms{
	extern const int ZW_AES_BLOCK_SIZE;
	extern const int ZW_SM3_DGST_SIZE;

//////////////////////////////////////////////////////////////////////////
typedef enum jc_error_code{
	EJC_SUSSESS,	//成功
	EJC_FAIL,		//失败
	EJC_INPUT_NULL,	//输入条件缺少
	EJC_DATETIME_INVALID,	//日期时间非法
	EJC_VALIDRANGE_INVALID,	//有效期非法
	EJC_CLOSECODE_INVALID,	//闭锁码非法
	EJC_CMDTYPE_TIMESTEP_INVALID,	//时间搜索步长非法
	EJC_CMDTYPE_TIMELEN_INVALID,	//时间搜索长度非法
	EJC_CMDTYPE_INVALID		//命令类型非法
} JCERROR;

typedef enum jc_cmd_type{
	JCCMD_INVALID_START,			//无效命令
	//初始闭锁码，此时仅有ATM编号，锁编号，PSK三者决定，其余可变因素为定值
	JCCMD_INIT_CLOSECODE,	
	JCCMD_CCB_DYPASS1,		//上位机第一开锁密码.此时“闭锁码”字段填写真正的闭锁码
	JCCMD_CCB_LOCK_VERCODE,	//下位机验证码.产生验证码时，“闭锁码”字段必须填写第一开锁密码
	JCCMD_CCB_DYPASS2,		//上位机第二开锁密码.产生第二开锁密码时，“闭锁码”字段填写验证码
	JCCMD_RESET_NFCKEY,		//重置NFC钥匙
	JCCMD_RESET_LOCKTIME,	//重设锁体时间
	JCCMD_RESET_LOCKSYSTEM,	//锁系统重置
	JCCMD_GET_LOCKLOG,		//提取锁体日志	
	JCCMD_INVALID_END
} JCCMD;

typedef enum jc_input_type{
	JCI_ATMNO,
	JCI_LOCKNO,
	JCI_PSK,
	JCI_DATETIME,
	JCI_VALIDITY,
	JCI_CLOSECODE
};

//离线匹配的返回值
typedef struct jcLockReverseMatchResult{
	int s_datetime;		//匹配结果秒数
	int s_validity;		//匹配结果有效期分钟数
	int s_matchTimes;	//匹配所用计算次数
}JCMATCH;

//有效期数组大小；更改此处以后请对应更改源代码中JcLockInput类初始化代码中
//为有效期数组m_validity_array赋予初值的相应语句
#define NUM_VALIDITY (8)
#define JC_ATMNO_MAXLEN (16)	//ATM编号长度最大值
#define JC_LOCKNO_MAXLEN (16)	//LOCK编号长度最大值
#define JC_PSK_LEN (256/4)	//256bit HEX+NULL,这是定长值
#define JC_INVALID_VALUE	(-1)

typedef struct JcLockInput
{
	//固定因素部分
	char m_atmno[JC_ATMNO_MAXLEN+1];		//ATM号
	char m_lockno[JC_LOCKNO_MAXLEN+1];	//锁号
	char m_psk[JC_PSK_LEN+1];			//PSK，上下位机共同持有的唯一机密因素
	//可变因素部分
	int m_datetime;		//日期时间
	int m_validity;		//有效期
	int m_closecode;	//闭锁码		
	JCCMD m_cmdtype;		//模式代码，比如开锁模式，远程重置模式，建行的流程要求的各种模式等等
	///////////////////////////////////以下为配置算法运作模式的数据///////////////////////////////////////
	//反推时间步长秒数，默认为在线模式，精度1分钟，值为60，离线模式请自己设置为3600秒或者其他数值
	int m_stepoftime;	
	//往前反推的时间长度秒数，默认为在线模式，10分钟，值为600，其他值比如离线24小时请自己设置
	int m_reverse_time_length;					
	//有效期，共有NUM_VALIDITY个,默认值是从5分钟到24小时那一系列，单位是分钟；可以自己设定
	//可以把最常用的有效期设置在更靠近开始处加快匹配速度
	int m_validity_array[NUM_VALIDITY];
//	void DebugPrint(void);	//
}JCINPUT;


	//	JcLockInput(void);
int JCLMSCCB2014_API JcLockNew(void);
	//	JCERROR CheckInput(void);
JCERROR JCLMSCCB2014_API JcLockCheckInput(const int jchandle);
	//lock结构体内部m_cmdtype决定了生成哪一类动态码；
int JCLMSCCB2014_API JcLockGetDynaCode(const int handle);
	//验证动态码，返回反推出来的时间和有效期结果，失败的话，两者均为0；
JCMATCH JCLMSCCB2014_API JcLockReverseVerifyDynaCode( const int handle,const int dstCode );
	//指明该算法是哪一天出的，当算法有运算结果上的变更时这个版本改变，一天最多只出一个版本；
	int JCLMSCCB2014_API JcLockGetVersion(void);
	void JCLMSCCB2014_API JcLockDebugPrint(const JCINPUT *jc);


//}	//end of namespace jclms
//////////////////////////////新设计的C接口////////////////////////////////////////////

#endif // jclmsCCB2014_h__
