#ifndef zwSecretBoxCCBcsWrap_h__
#define zwSecretBoxCCBcsWrap_h__

#ifdef _ZWUSE_AS_JNI
#define CCBELOCK_API
#else
#ifdef CCBELOCK_EXPORTS
#define CCBELOCK_API __declspec(dllexport) 
#else
#define CCBELOCK_API __declspec(dllimport)
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

	////////////////////////////////C#封装函数//////////////////////////////////////////
	//打开密盒HID通道,返回0代表失败，其他代表成功
	CCBELOCK_API int SecboxOpen(void);
	//关闭密盒HID通道，参数为secboxHidOpen的返回值句柄
	CCBELOCK_API void SecboxClose(int handleHid);
	//通过HID发送授权验证请求到密盒
	CCBELOCK_API void SecboxSendAuthReq(int handleHid);
	//通过HID接收密盒反应并验证，成功返回0，其他值代表失败
	CCBELOCK_API int SecboxVerifyAuthRsp(int handleHid);

	//写入数据到密盒，最大400多字节，输入格式为base64编码的字符串。需要指定zwSecboxHidOpen给出的句柄，以及索引号
	//索引号大约为1-8左右，具体还需要和赵工确定；
	CCBELOCK_API void SecboxWriteData(int handleHid,const int index,const char *dataB64);
	//指定密盒HID句柄，以及索引号，读取密盒的数据，返回base64编码的数据字符串
	CCBELOCK_API const char * SecboxReadData(int handleHid,const int index);


#ifdef __cplusplus
}
#endif
#endif // zwSecretBoxCCBcsWrap_h__