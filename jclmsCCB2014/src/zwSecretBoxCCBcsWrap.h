#ifndef zwSecretBoxCCBcsWrap_h__
#define zwSecretBoxCCBcsWrap_h__

//#ifdef _ZWUSE_AS_JNI
//#define JCLMSCCB2014_API
//#else
//#ifdef CCBELOCK_EXPORTS
//#define JCLMSCCB2014_API __declspec(dllexport)
//#else
//#define JCLMSCCB2014_API __declspec(dllimport)
//#endif
//#endif
#include "jclmsCCB2014.h"

#ifdef __cplusplus
extern "C" {
#endif

	////////////////////////////////C#封装函数//////////////////////////////////////////
	////向密盒发送认证请求，返回成功或者结果
	//JCLMSCCB2014_API JC_SECBOX_STATUS SecboxAuth(void);
	////写入数据到密盒，最大400多字节，输入格式为base64编码的字符串。需要指定zwSecboxHidOpen给出的句柄，以及索引号
	////索引号大约为1-8左右，具体还需要和赵工确定；
	//JCLMSCCB2014_API void SecboxWriteData(const int index,const char *dataB64);
	////指定密盒HID句柄，以及索引号，读取密盒的数据，返回base64编码的数据字符串
	//JCLMSCCB2014_API const char * SecboxReadData(const int index);

#ifdef __cplusplus
}
#endif
#endif				// zwSecretBoxCCBcsWrap_h__
