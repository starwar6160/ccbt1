#ifndef jclmsCCB2014_h__
#define jclmsCCB2014_h__
#include "stdafx.h"

// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 JCLMSCCB2014_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// JCLMSCCB2014_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
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
//////////////////////////////////////////////////////////////////////////
typedef enum jc_error_code{
	EJC_SUSSESS,	//成功
	EJC_FAIL,		//失败
	EJC_INPUT_NULL	//输入条件缺少
} JCERROR;

typedef enum jc_cmd_type{
	JCCMD_GEN_DYNACODE,		//正常生成动态码
	JCCMD_CCB_DYPASS1,		//上位机第一开锁密码
	JCCMD_CCB_LOCK_VERCODE,	//下位机验证码
	JCCMD_CCB_DYPASS2,		//上位机第二开锁密码
	JCCMD_RESET_KEY		//重置钥匙
} JCCMD;


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
		int m_cmdtype;		//模式代码，比如开锁模式，远程重置模式，建行的流程要求的各种模式等等
		//////////////////////////////////////////////////////////////////////////
#ifdef JC_USEKEYINFO_201407
		string m_keyno;
		string m_keypin;
#endif // JC_USEKEYINFO_201407
	private:
		JCERROR m_status;
	public:
		JcLockInput(void);
		void DebugPrint(void);	//
		JCERROR CheckInput(void);
	};
//////////////////////////////////////////////////////////////////////////
//}	//end of namespace jclms

#endif // jclmsCCB2014_h__
