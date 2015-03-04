#ifndef jclmsCCB2014_h__
#define jclmsCCB2014_h__

#ifdef  __cplusplus
extern "C" {
#endif
// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 JCLMSCCB2014_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// JCLMSCCB2014_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
//在ARM上使用请打开该宏定义以便消除windows的DLL相关定义的编译错误
//#define _USEON_NONWIN32
#ifndef _WIN32	
//20141203.1709.为了便于ARM移植时减少修改工作量而添加
#define _ZWUSE_AS_JNI
#endif

#ifdef _ZWUSE_AS_JNI
#define JCLMSCCB2014_API
#else
#ifdef JCLMSCCB2014_EXPORTS
#define JCLMSCCB2014_API __declspec(dllexport)
#else
#define JCLMSCCB2014_API __declspec(dllimport)
#endif
#endif //_ZWUSE_AS_JNI

//注意此处这个包含指令要放在JCLMSCCB2014_API的定义之后，才能使得该导出的DLL函数正确导出
#include "jclmsCCB2014AlgCore.h"

//////////////////////////////////////////////////////////////////////////
extern const int ZW_SYNCALG_BLOCK_SIZE;
extern const int ZW_SM3_DGST_SIZE;

//////////////////////////////////////////////////////////////////////////

typedef struct jcLmsRequest{
	JCLMSOP Type;
	int dstCode;	//反推运算的输入动态码
	JCINPUT inputData;
}JCLMSREQ;

//用于HID等通信接口返回结果，统一在一个结构体里面
typedef union JcLockResult{
	int dynaCode;			//动态码结果
	JCMATCH verCodeMatch;		//验证码匹配日期时间和有效期结果
}JCRESULT;

//#pragma pack()


const int ZWMEGA = 1000000;	//一百万
//20141125新增，密盒通信函数,上位机部分
//两个zwJclmsReq函数是上位机专用
//填写完毕handle里面的数据结构以后，调用该函数生成动态码，该函数在底层将请求
//做JSON序列化以后通过HID等通信线路发送到密盒，然后阻塞接收密盒返回结果，通过出参返回；
int JCLMSCCB2014_API csJclmsReqGenDyCode( int lmsHandle );
#ifndef _ZWUSE_AS_JNI
int JCLMSCCB2014_API zwJclmsReqGenDyCode( int lmsHandle,int *dyCode);
#endif // _ZWUSE_AS_JNI

//填写完毕handle里面的数据结构以后，调用该函数验证动态码（第一和第二动态码中间，锁具生成的校验码
//也是使用其他两个动态码的同样算法生成的，所以也算一种动态码，该函数在底层将验证请求做JSON序列化
//以后通过HID等通信线路发送到密盒，然后阻塞接收密盒返回结果，通过出参返回；
int JCLMSCCB2014_API zwJclmsReqVerifyDyCode( int lmsHandle,int dstCode,JCMATCH *match );
int JCLMSCCB2014_API zwLmsAlgStandTest20141203(void);
int JCLMSCCB2014_API zwLmsAlgStandTest20141216GenPass1(void);
int JCLMSCCB2014_API zwLmsAlgStandTest20141216VerifyPass1(void);
//输入：接收到的整个合并完毕的HID数据以及该数据的长度；
//输出：JSON格式的返回值，输出缓冲区最大长度由outJsonLen指定
//void JCLMSCCB2014_API zwJclmsRsp( void * inLmsReq,const int inLmsReqLen,char *outJson,const int outJsonLen );
void JCLMSCCB2014_API myPureHidTestDataGen20141216(void);
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

#ifdef  __cplusplus
}	//extern "C" {
#endif


JCLMSCCB2014_API class JcSecBox {
public:
	JCLMSCCB2014_API JcSecBox();
	JCLMSCCB2014_API ~ JcSecBox();
	JCLMSCCB2014_API void CloseHid();
	JCLMSCCB2014_API int SecboxAuth(void);
	JCLMSCCB2014_API int SecboxWriteData(const int index,
		const char *dataB64);
	JCLMSCCB2014_API const char *SecboxReadData(const int index);
private:
};

#endif // jclmsCCB2014_h__
