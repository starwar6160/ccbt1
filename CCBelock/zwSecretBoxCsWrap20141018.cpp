//#include "CCBelock.h"
#include "stdafx.h"
#include "zwSecretBoxCCBcsWrap.h"
#include "zwSecBoxCSHdr.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <windows.h>
extern Poco::LogStream * pocoLog;
#define ZWTRC	zwTrace1027 zwtrace(__FUNCTION__);
//printf("%s\n",__FUNCTION__);

static int g_hidHandle;	//单例模式密盒HID句柄，new多少个类都是这个全局变量保存HID通信句柄，等于一个类；

typedef enum jc_secret_box_status_t{
	JC_SECBOX_SUCCESS=0,
	JC_SECBOX_FAIL=1
}JC_SECBOX_STATUS;

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
void zwWriteData2SecretBox(int handleHid,const int index,const char *dataB64);
//指定密盒HID句柄，以及索引号，读取密盒的数据，返回base64编码的数据字符串
const char * zwReadDataFromSecretBox(int handleHid,const int index);
#endif // _DEBUG_1018

class zwTrace1027
{
	char *m_strClass;
	char m_buf[64];
	LARGE_INTEGER nStart,nEnd;
public:
	zwTrace1027(const char *strClassName)
	{
		m_strClass=(char *)strClassName;
		memset(m_buf,0,64);
		sprintf(m_buf,"%s [START]",m_strClass);		
		OutputDebugStringA(m_buf);
		QueryPerformanceCounter(&nStart);
	}
	~zwTrace1027()
	{
		QueryPerformanceCounter(&nEnd);
		memset(m_buf,0,64);				
		LARGE_INTEGER nPerf;
		QueryPerformanceFrequency(&nPerf);
		//实际高精度计时器精度一般都在1.2M到2.8M左右，也就是都可以精确到0.9-0.4微秒左右；
		double fLifeMs=(nEnd.QuadPart-nStart.QuadPart)*1000.0/nPerf.QuadPart;
		sprintf(m_buf,"%s [END] elps %.1f ms",m_strClass,fLifeMs);		
		OutputDebugStringA(m_buf);
		pocoLog->information()<<m_buf<<endl;
	}
};


CCBELOCK_API JcSecBox::JcSecBox()
{
	ZWTRC
		if(NULL==g_hidHandle){
			pocoLog->information("jcSecBox OpenFirst");
			g_hidHandle=zwSecboxHidOpen();
		}		
		if (NULL==g_hidHandle)
		{
			OutputDebugStringA("JcSecBox open FAIL");
			printf("%s OPEN HID JINCHU SECRET BOX FAIL!\n",__FUNCTION__);
			pocoLog->error("jcSecBox OpenNULL");
		}
}

CCBELOCK_API JcSecBox::~JcSecBox()
{
	ZWTRC
		if (NULL!=g_hidHandle)
		{
			//pocoLog->information()<<"jcSecBox Closed handle="<<g_hidHandle<<endl;
			//zwSecboxHidClose(g_hidHandle);
		}	
}



CCBELOCK_API int JcSecBox::SecboxAuth( void )
{
	ZWTRC
	if (0==g_hidHandle)
	{
		pocoLog->error("jcSecbox Error HandleZero");
		return JC_SECBOX_FAIL ;
	}
	//printf("*****************************SecretBox zwSendAuthReq2SecBox\n");
	pocoLog->information()<<"zwSendAuthReq2SecBox"<<g_hidHandle<<endl;
	zwSendAuthReq2SecBox(g_hidHandle);
	//printf("*****************************SecretBox zwVerifyAuthRspFromSecBox\n");
	pocoLog->information()<<"zwVerifyAuthRspFromSecBox g_hidHandle="<<g_hidHandle<<endl;
	int AuthRes=zwVerifyAuthRspFromSecBox(g_hidHandle);
	
	if (0==AuthRes)
	{
		pocoLog->information()<<"SecboxAuth SUCCESSJC"<<endl;
		//printf("1021.1355.****************************************** *SecretBox Auth SUCCESS\n");
		return JC_SECBOX_SUCCESS;
	}
	else
	{
		pocoLog->error()<<"SecboxAuth FAILJC"<<endl;
		OutputDebugStringA("JcSecBox AUTH FAIL###########################");
		printf("************************************************** ***SecretBox Auth FAIL\n");
		return JC_SECBOX_FAIL;
	}

}

CCBELOCK_API int JcSecBox::SecboxWriteData( const int index,const char *dataB64 )
{
	ZWTRC
	if (0==g_hidHandle)
	{
		pocoLog->error("jcSecbox Error HandleZero");
		return 1;
	}
	assert(index>=0 && index<=16);
	assert(NULL!=dataB64 && strlen(dataB64)>0);
	if (index<0 || index>16)
	{
		pocoLog->error("jcSecbox Error IndexOutofRange");
		printf("Data Index out of range! must in 0 to 16\n");
		return 1;
	}
	if (NULL==dataB64 || strlen(dataB64)==0)
	{
		pocoLog->error("jcSecbox Error InputDataZero");
		printf("input must base64 encoded string!\n");
		return 1;
	}
	pocoLog->information()<<__FUNCTION__<<"index="<<index<<" input Data="<<dataB64<<endl;
	//pocoLog->information("WriteData Auth Start");
	int status=SecboxAuth();
	//pocoLog->information("WriteData Auth End");
	if (JC_SECBOX_SUCCESS==status)
	{
		//pocoLog->information()<<"WriteData Authed,start write"<<endl;
		zwWriteData2SecretBox(g_hidHandle,index,dataB64);		
		//pocoLog->information()<<"WriteData end"<<endl;
	}	
	return 0;
}

CCBELOCK_API const char * JcSecBox::SecboxReadData( const int index )
{
	ZWTRC
	const char *retStr="";
	if (0==g_hidHandle)
	{
		pocoLog->error("jcSecbox Error HandleZero");
		return retStr;
	}
	assert(index>=0 && index<=16);
	if (index<0 || index>16)
	{
		pocoLog->error("jcSecbox Error IndexOutofRange");
		printf("Data Index out of range! must in 0 to 16\n");
		return retStr;
	}
	pocoLog->information()<<__FUNCTION__<<" index="<<index<<endl;
	//pocoLog->information("ReadData Auth Start");
	int status=SecboxAuth();
	//pocoLog->information("ReadData Auth End");
	if (JC_SECBOX_SUCCESS==status)
	{
		//pocoLog->information()<<"ReadData Authed,start read"<<endl;
		retStr=zwReadDataFromSecretBox(g_hidHandle,index);
		//pocoLog->information()<<"ReadData end"<<endl;
	}	
	pocoLog->information()<<__FUNCTION__<<"Return "<<retStr<<endl;
	return retStr;
}
