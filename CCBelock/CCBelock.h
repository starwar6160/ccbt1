#ifndef CCBelock_h__
#define CCBelock_h__

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

#ifdef __cplusplus
extern "C" {
#endif

CCBELOCK_API long	JCAPISTD Open(long lTimeOut);
CCBELOCK_API long	JCAPISTD Close();
CCBELOCK_API long	JCAPISTD Notify(const char *pszMsg);
CCBELOCK_API int	JCAPISTD SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun);

//////////////////////////////////////////////////////////////////////////
CCBELOCK_API void zwPushString(const char *str);
CCBELOCK_API const char * dbgGetLockReturnXML(void);

////////////////////////////////C#封装函数//////////////////////////////////////////
//打开密盒HID通道,返回0代表失败，其他代表成功
CCBELOCK_API int SecboxHidOpen(void);
//关闭密盒HID通道，参数为secboxHidOpen的返回值句柄
CCBELOCK_API void SecboxHidClose(int handleHid);
//通过HID发送授权验证请求到密盒
CCBELOCK_API void SendAuthReq2SecBox(int handleHid);
//通过HID接收密盒反应并验证，成功返回0，其他值代表失败
CCBELOCK_API int VerifyAuthRspFromSecBox(int handleHid);

//写入数据到密盒，最大400多字节，输入格式为base64编码的字符串。需要指定zwSecboxHidOpen给出的句柄，以及索引号
//索引号大约为1-8左右，具体还需要和赵工确定；
CCBELOCK_API void WriteData2SecretBox(int handleHid,const int index,const char *dataB64);
//指定密盒HID句柄，以及索引号，读取密盒的数据，返回base64编码的数据字符串
CCBELOCK_API const char * ReadDataFromSecretBox(int handleHid,const int index);



#ifdef __cplusplus
}
#endif
#endif // CCBelock_h__
