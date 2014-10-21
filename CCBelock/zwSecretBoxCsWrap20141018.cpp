//#include "CCBelock.h"
#include "zwSecretBoxCCBcsWrap.h"
#include "zwSecBoxCSHdr.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <windows.h>
#define PRFN	printf("%s\n",__FUNCTION__);OutputDebugStringA(__FUNCTION__);

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

//最简单的不是单例的单例模式，实际上达到了单例的效果，HID HANDLE用一个全局变量来保存；
//无论多少个JcSecBox类对象，打开的都是同一个密盒端口，而且不会重复打开；
int g_hidHandle=0;


JcSecBox::JcSecBox()
{
	PRFN	
	if (0==g_hidHandle)
	{
		g_hidHandle=zwSecboxHidOpen();
		//assert(NULL!=g_hidHandle);
		if (NULL==g_hidHandle)
		{
			printf("%s OPEN HID JINCHU SECRET BOX FAIL!\n");
		}
	}	
}

JcSecBox::~JcSecBox()
{
	PRFN
	assert(NULL!=g_hidHandle);
	//zwSecboxHidClose(m_hidHandle);
}



JC_SECBOX_STATUS JcSecBox::SecboxAuth(void)
{
	PRFN
	if (0==g_hidHandle)
	{
		return JC_SECBOX_FAIL ;
	}
	printf("*****************************SecretBox zwSendAuthReq2SecBox\n");
	zwSendAuthReq2SecBox(g_hidHandle);
	printf("*****************************SecretBox zwVerifyAuthRspFromSecBox\n");
	int AuthRes=zwVerifyAuthRspFromSecBox(g_hidHandle);
	
	if (0==AuthRes)
	{
		printf("1021.1355.****************************************** *SecretBox Auth SUCCESS\n");
		return JC_SECBOX_SUCCESS;
	}
	else
	{
		printf("************************************************** ***SecretBox Auth FAIL\n");
		return JC_SECBOX_FAIL;
	}
}

void JcSecBox::SecboxWriteData( const int index,const char *dataB64 )
{
	PRFN
	if (0==g_hidHandle)
	{
		return ;
	}
	assert(index>=0 && index<=16);
	assert(NULL!=dataB64 && strlen(dataB64)>0);
	if (index<0 || index>16)
	{
		printf("Data Index out of range! must in 0 to 16\n");
	}
	if (NULL==dataB64 || strlen(dataB64)==0)
	{
		printf("input must base64 encoded string!\n");
		return ;
	}
	JC_SECBOX_STATUS status=SecboxAuth();
	if (JC_SECBOX_SUCCESS==status)
	{
		zwWriteData2SecretBox(g_hidHandle,index,dataB64);
	}	
}

const char * JcSecBox::SecboxReadData( const int index )
{
	PRFN
	if (0==g_hidHandle)
	{
		return "";
	}
	assert(index>=0 && index<=16);
	if (index<0 || index>16)
	{
		printf("Data Index out of range! must in 0 to 16\n");
	}
	const char *retStr=NULL;
	JC_SECBOX_STATUS status=SecboxAuth();
	if (JC_SECBOX_SUCCESS==status)
	{
		retStr=zwReadDataFromSecretBox(g_hidHandle,index);
	}	
	return retStr;
}
