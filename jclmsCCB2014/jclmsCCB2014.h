#ifndef jclmsCCB2014_h__
#define jclmsCCB2014_h__
#include "zwstdafx.h"

// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 JCLMSCCB2014_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// JCLMSCCB2014_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
//在ARM上使用请打开该宏定义以便消除windows的DLL相关定义的编译错误
//#define _USEON_NONWIN32
#ifdef _USEON_NONWIN32
#define JCLMSCCB2014_API
#else
#ifdef JCLMSCCB2014_EXPORTS
#define JCLMSCCB2014_API __declspec(dllexport)
#else
#define JCLMSCCB2014_API __declspec(dllimport)
#endif
#endif	//_ZWUSE_AS_JNI

//////////////////////////////////////////////////////////////////////////
namespace jclms{
//////////////////////////////////////////////////////////////////////////
typedef enum jc_error_code{
	EJC_SUSSESS,	//成功
	EJC_FAIL,		//失败
	EJC_INPUT_NULL,	//输入条件缺少
	EJC_DATETIME_INVALID,	//日期时间非法
	EJC_VALIDRANGE_INVALID,	//有效期非法
	EJC_CLOSECODE_INVALID,	//闭锁码非法
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

//离线匹配的返回值
typedef struct jcOfflineResult{
	int s_datetime;
	int s_validity;
}JCOFFLINE;

	class JCLMSCCB2014_API JcLockInput
	{
	public:
		//固定因素部分
		string m_atmno;			//ATM号
		string m_lockno;		//锁号
		string m_psk;			//PSK，上下位机共同持有的唯一机密因素
		//可变因素部分
		int m_datetime;		//日期时间
		int m_validity;		//有效期
		int m_closecode;	//闭锁码		
		JCCMD m_cmdtype;		//模式代码，比如开锁模式，远程重置模式，建行的流程要求的各种模式等等
	private:		
		JCERROR m_status;
	public:
		JcLockInput(void);
		void DebugPrint(void);	//
		JCERROR CheckInput(void);
	};
	//lock结构体内部m_cmdtype决定了生成哪一类动态码；
	int JCLMSCCB2014_API zwGetDynaCode(const JcLockInput &lock);

//验证动态码，成功返回EJC_SUSSESS，失败返回EJC_FAIL
	JCERROR zwVerifyDynaCode(const JcLockInput &lock,const int dstDyCode);

	JCOFFLINE JCLMSCCB2014_API zwOfflineVerifyDynaCode( const JcLockInput &lock,const int dstCode );
	int JCLMSCCB2014_API getVersion(void);

	JCERROR CheckInputValid( const JcLockInput &lock );

}	//end of namespace jclms

namespace zwTools{
	//////////////////////////////////////////////////////////////////////////

	//实际上不限于AES,只是作为一个基本的块规整大小单位方便处理
#define ZW_AES_BLOCK_SIZE	(128/8)	
#define ZW_SM3_DGST_SIZE	(256/8)
	class JCLMSCCB2014_API zwHexTool
	{
		char *m_bin;
		int m_binLen;
		int m_padLen;
		string m_CArrayStr;
	public:
		zwHexTool(const char *HexInput);
		zwHexTool(const void *msg,const int msgLen);
		~zwHexTool();
		//出参给出内部bin数据区地址,以及长度
		char * getBin(void);
		int getBinLen(void);
		int getPadedLen(void);
		int getXXTEABlockNum(void);
		void PrintBin(void);
		const char * getCArrayStr(void);
	protected:

	private:
	};
	//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG_USE_OLD_SM3HMAC20140703
	//密钥，消息，输出的摘要,都是二进制格式
	int32_t JCLMSCCB2014_API zwSm3Hmac7(zwHexTool &inPsk,
		zwHexTool &inMessage,
		zwHexTool &outHmac);
#endif // _DEBUG_USE_OLD_SM3HMAC20140703
	
}

#endif // jclmsCCB2014_h__
