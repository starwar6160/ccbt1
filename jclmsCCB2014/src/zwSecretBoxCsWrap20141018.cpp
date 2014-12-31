//#include "CCBelock.h"
//#include "stdafx.h"
//#include "zwSecretBoxCCBcsWrap.h"
#include "jclmsCCB2014.h"
#include "zwSecBoxCSHdr.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <windows.h>
//extern Poco::LogStream * pocoLog;
//#define ZWTRC	zwTrace1027 zwtrace(__FUNCTION__);
#define ZWTRC
//printf("%s\n",__FUNCTION__);



class zwTrace1027 {
	char *m_strClass;
	char m_buf[64];
	LARGE_INTEGER nStart, nEnd;
      public:
	zwTrace1027(const char *strClassName);
	~zwTrace1027();
};

static int g_hidHandle;		//单例模式密盒HID句柄，new多少个类都是这个全局变量保存HID通信句柄，等于一个类；

typedef enum jc_secret_box_status_t {
	JC_SECBOX_SUCCESS = 0,
	JC_SECBOX_FAIL = 1
} JC_SECBOX_STATUS;

#ifdef _DEBUG_1018
////////////////////////////////C#封装函数//////////////////////////////////////////
//打开密盒HID通道,返回0代表失败，其他代表成功
int zwSecboxHidOpen(void);
//关闭密盒HID通道，参数为secboxHidOpen的返回值句柄
void zwSecboxHidClose(int handleHid);
//通过HID发送授权验证请求到密盒
void zwSendAuthReq2SecBox(int handleHid);
//通过HID接收密盒反应并验证，成功返回0，其他值代表失败
int zwVerifyAuthRspFromSecBox(int handleHid);

//写入数据到密盒，最大400多字节，输入格式为base64编码的字符串。需要指定zwSecboxHidOpen给出的句柄，以及索引号
//索引号大约为1-8左右，具体还需要和赵工确定；
void zwWriteData2SecretBox(int handleHid, const int index, const char *dataB64);
//指定密盒HID句柄，以及索引号，读取密盒的数据，返回base64编码的数据字符串
const char *zwReadDataFromSecretBox(int handleHid, const int index);
#endif // _DEBUG_1018

JCLMSCCB2014_API JcSecBox::JcSecBox()
{
	ZWTRC if (NULL == g_hidHandle) {
#ifdef _DEBUG_USEPOCOLOG1027
		pocoLog->information("jcSecBox OpenFirst");
#endif // _DEBUG_USEPOCOLOG1027
		g_hidHandle = zwSecboxHidOpen();
	}
	if (NULL == g_hidHandle) {
		OutputDebugStringA("JcSecBox open FAIL");
		printf("%s OPEN HID JINCHU SECRET BOX FAIL!\n", __FUNCTION__);
#ifdef _DEBUG_USEPOCOLOG1027
		pocoLog->error("jcSecBox OpenNULL");
#endif // _DEBUG_USEPOCOLOG1027
	}
}

JCLMSCCB2014_API JcSecBox::~JcSecBox()
{
	ZWTRC 
	CloseHid();
}

//20141222.0941.周伟.JcSecBox添加一个明确的CloseHid接口以后，解决了先密盒认证读写
//之后再发送lms密盒请求就会打不开设备的问题。根源还是C#无法控制对象析构的时机，
//导致JcSecBox没有及时调用释放HID设备；
JCLMSCCB2014_API void JcSecBox::CloseHid()
{
	if (NULL != g_hidHandle) {
		//pocoLog->information()<<"jcSecBox Closed handle="<<g_hidHandle<<endl;
		zwSecboxHidClose(g_hidHandle);
		g_hidHandle=NULL;
	}
}

JCLMSCCB2014_API int JcSecBox::SecboxAuth(void)
{
	ZWTRC if (0 == g_hidHandle) {
#ifdef _DEBUG_USEPOCOLOG1027
		pocoLog->error("jcSecbox Error HandleZero");
#endif // _DEBUG_USEPOCOLOG1027
		return JC_SECBOX_FAIL;
	}
	int AuthRes=0;

	for (int i=0;i<3;i++)
	{
		//printf("*****************************SecretBox zwSendAuthReq2SecBox\n");
#ifdef _DEBUG_USEPOCOLOG1027
		pocoLog->information() << "zwSendAuthReq2SecBox" << g_hidHandle << endl;
#endif // _DEBUG_USEPOCOLOG1027
		zwSendAuthReq2SecBox(g_hidHandle);
		//printf("*****************************SecretBox zwVerifyAuthRspFromSecBox\n");
#ifdef _DEBUG_USEPOCOLOG1027
		pocoLog->
			information() << "zwVerifyAuthRspFromSecBox g_hidHandle=" <<
			g_hidHandle << endl;
#endif // _DEBUG_USEPOCOLOG1027
		AuthRes = zwVerifyAuthRspFromSecBox(g_hidHandle);
		if (0 == AuthRes) {
#ifdef _DEBUG_USEPOCOLOG1027
			pocoLog->information() << "SecboxAuth SUCCESSJC" << endl;
#endif // _DEBUG_USEPOCOLOG1027
			//printf("1021.1355.****************************************** *SecretBox Auth SUCCESS\n");
			//成功的话就直接返回成功
			return JC_SECBOX_SUCCESS;
		} 
		else{
			printf("SECBOX AUTH TEMP FAIL,WAIT 1 SEC FOR RETRY\n");
			Sleep(1000);	//等待1秒再重试认证
		}
	}

//重试N次以后还是认证失败
#ifdef _DEBUG_USEPOCOLOG1027
		pocoLog->error() << "SecboxAuth FAILJC" << endl;
#endif // _DEBUG_USEPOCOLOG1027
		OutputDebugStringA
		    ("JcSecBox AUTH FAIL###########################");
		printf
		    ("************************************************** ***SecretBox Auth FAIL\n");
		return JC_SECBOX_FAIL;
}

JCLMSCCB2014_API int JcSecBox::SecboxWriteData(const int index, const char *dataB64)
{
	ZWTRC if (0 == g_hidHandle) {
#ifdef _DEBUG_USEPOCOLOG1027
		pocoLog->error("jcSecbox Error HandleZero");
#endif // _DEBUG_USEPOCOLOG1027
		//return 1;
	}
	assert(index >= 0 && index <= 16);
	assert(NULL != dataB64 && strlen(dataB64) > 0);
	if (index < 0 || index > 16) {
#ifdef _DEBUG_USEPOCOLOG1027
		pocoLog->error("jcSecbox Error IndexOutofRange");
#endif // _DEBUG_USEPOCOLOG1027
		printf("Data Index out of range! must in 0 to 16\n");
		return 1;
	}
	if (NULL == dataB64 || strlen(dataB64) == 0) {
#ifdef _DEBUG_USEPOCOLOG1027
		pocoLog->error("jcSecbox Error InputDataZero");
#endif // _DEBUG_USEPOCOLOG1027
		printf("input must base64 encoded string!\n");
		return 1;
	}
#ifdef _DEBUG_USEPOCOLOG1027
	pocoLog->
	    information() << __FUNCTION__ << "index=" << index << " input Data="
	    << dataB64 << endl;
#endif // _DEBUG_USEPOCOLOG1027
	//pocoLog->information("WriteData Auth Start");
	int status = SecboxAuth();
	//pocoLog->information("WriteData Auth End");
	if (JC_SECBOX_SUCCESS == status) {
		//pocoLog->information()<<"WriteData Authed,start write"<<endl;
		zwWriteData2SecretBox(g_hidHandle, index, dataB64);
		//pocoLog->information()<<"WriteData end"<<endl;
	}
	return 0;
}

JCLMSCCB2014_API const char *JcSecBox::SecboxReadData(const int index)
{
	ZWTRC const char *retStr = "";
	if (0 == g_hidHandle) {
#ifdef _DEBUG_USEPOCOLOG1027
		pocoLog->error("jcSecbox Error HandleZero");
#endif // _DEBUG_USEPOCOLOG1027
		return retStr;
	}
	assert(index >= 0 && index <= 16);
	if (index < 0 || index > 16) {
#ifdef _DEBUG_USEPOCOLOG1027
		pocoLog->error("jcSecbox Error IndexOutofRange");
#endif // _DEBUG_USEPOCOLOG1027
		printf("Data Index out of range! must in 0 to 16\n");
		return retStr;
	}
#ifdef _DEBUG_USEPOCOLOG1027
	pocoLog->information() << __FUNCTION__ << " index=" << index << endl;
#endif // _DEBUG_USEPOCOLOG1027
	//pocoLog->information("ReadData Auth Start");
	int status = SecboxAuth();
	//pocoLog->information("ReadData Auth End");
	if (JC_SECBOX_SUCCESS == status) {
		//pocoLog->information()<<"ReadData Authed,start read"<<endl;
		retStr = zwReadDataFromSecretBox(g_hidHandle, index);
		//pocoLog->information()<<"ReadData end"<<endl;
	}
#ifdef _DEBUG_USEPOCOLOG1027
	pocoLog->information() << __FUNCTION__ << "Return " << retStr << endl;
#endif // _DEBUG_USEPOCOLOG1027
	return retStr;
}
