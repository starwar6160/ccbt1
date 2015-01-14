#ifndef CCBelock_h__
#define CCBelock_h__
#include <string>
using std::string;
// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 CCBELOCK_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// CCBELOCK_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef _ZWUSE_AS_JNI
#define CCBELOCK_API
typedef void (*RecvMsgRotine)(const char *pszMsg);
#else
#ifdef CCBELOCK_EXPORTS
#define CCBELOCK_API __declspec(dllexport) 
#else
#define CCBELOCK_API __declspec(dllimport)
#endif
typedef void (cdecl *RecvMsgRotine)(const char *pszMsg);
#endif

#define JCAPISTD
//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

CCBELOCK_API long	JCAPISTD Open(long lTimeOut);
CCBELOCK_API long	JCAPISTD Close();
CCBELOCK_API long	JCAPISTD Notify(const char *pszMsg);
CCBELOCK_API int	JCAPISTD SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun);

//////////////////////////////////////////////////////////////////////////
CCBELOCK_API void zwPushString(const char *str);
//CCBELOCK_API const char * dbgGetLockReturnXML(void);
CCBELOCK_API void myLuaBridgeSendJsonAPI(const char *luaScriptFile);

////////////////////////调试用Json命令接口//////////////////////////////////////////////////
//一、设备列表接口 1、回调函数类型
typedef void (*ReturnDrives)(const char* DrivesTypePID,const char * DrivesIDListJson);

CCBELOCK_API void SetReturnDrives(ReturnDrives _DrivesListFun);
//3、获取设备列表的指令
//说明：
//	（1）DrivesType表示设备类型，具体内容是：锁具=Lock，密盒=Encryption
//	（2）所有int类型函数的返回值中0表示成功，非零表示错误（下同）
//	（3）DrivesIDList为json格式字符，其中列出了所有符合要求设备的DrivesID（HID设备序列号）
//	（4）当调用ListDrives时，接口函数应返回当前所有此类设备的列表
//	（5）当有新设备插入或拔出时，接口函数应该返回插入设备所属类型的最新列表（重复内容由上位机过滤去重）
CCBELOCK_API int ListDrives(const char * DrivesTypePID);
//1、打开设备
CCBELOCK_API int OpenDrives(const char* DrivesTypePID,const char * DrivesIdSN);
//	2、关闭设备
CCBELOCK_API int CloseDrives(const char* DrivesTypePID,const char * DrivesIdSN);
//extern ReturnDrives G_JCHID_ENUM_DEV2015A;

//三、设备消息接口 说明：（1）DrivesMessage和AnyMessage均为json格式字符
//1、回调函数类型
	typedef void (*ReturnMessage)(const char* DrivesIdSN,char* DrivesMessageJson);
//2、设置设备消息返回的回调函数
	void SetReturnMessage(ReturnMessage _MessageHandleFun);
//3、向设备发送指令的函数
	int inputMessage(const char * DrivesTypePID,const char * DrivesIdSN,const char * AnyMessageJson);
#ifdef __cplusplus
}
#endif

namespace jcLockJsonCmd_t2015a{

	CCBELOCK_API long OpenJson(long lTimeOut);
	CCBELOCK_API long SendToLockJson(const char *pszJson);
	CCBELOCK_API const char * RecvFromLockJson(const int timeoutMs);
	CCBELOCK_API long CloseJson();	
	CCBELOCK_API void jcMulHidEnum( const int hidPid ,string &jcDevListJson);
	extern ReturnDrives G_JCHID_ENUM_DEV2015A;
}	//end of namespace jcLockJsonCmd_t2015a{


#endif // CCBelock_h__
