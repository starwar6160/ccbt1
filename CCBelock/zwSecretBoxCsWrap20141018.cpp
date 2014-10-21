//#include "CCBelock.h"
#include "zwSecretBoxCCBcsWrap.h"
#include "zwSecBoxCSHdr.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <windows.h>
#define PRFN	printf("%s\n",__FUNCTION__);OutputDebugStringA(__FUNCTION__);

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

JcSecBox::JcSecBox()
{
	PRFN	
		if(NULL==g_hidHandle){
			g_hidHandle=zwSecboxHidOpen();
		}		
		assert(NULL!=g_hidHandle);
		if (NULL==g_hidHandle)
		{
			OutputDebugStringA("JcSecBox open FAIL");
			printf("%s OPEN HID JINCHU SECRET BOX FAIL!\n");
		}
}

JcSecBox::~JcSecBox()
{
	PRFN
		if (NULL!=g_hidHandle)
		{
			zwSecboxHidClose(g_hidHandle);
		}	
}



int JcSecBox::SecboxAuth( void )
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
		OutputDebugStringA("JcSecBox AUTH FAIL###########################");
		printf("************************************************** ***SecretBox Auth FAIL\n");
		return JC_SECBOX_FAIL;
	}
}

int JcSecBox::SecboxWriteData( const int index,const char *dataB64 )
{
	PRFN
	if (0==g_hidHandle)
	{
		return 1;
	}
	assert(index>=0 && index<=16);
	assert(NULL!=dataB64 && strlen(dataB64)>0);
	if (index<0 || index>16)
	{
		printf("Data Index out of range! must in 0 to 16\n");
		return 1;
	}
	if (NULL==dataB64 || strlen(dataB64)==0)
	{
		printf("input must base64 encoded string!\n");
		return 1;
	}
	int status=SecboxAuth();
	if (JC_SECBOX_SUCCESS==status)
	{
		zwWriteData2SecretBox(g_hidHandle,index,dataB64);		
	}	
	return 0;
}

const char * JcSecBox::SecboxReadData( const int index )
{
	PRFN
	const char *retStr="";
	if (0==g_hidHandle)
	{
		return retStr;
	}
	assert(index>=0 && index<=16);
	if (index<0 || index>16)
	{
		printf("Data Index out of range! must in 0 to 16\n");
		return retStr;
	}
	
	int status=SecboxAuth();
	if (JC_SECBOX_SUCCESS==status)
	{
		retStr=zwReadDataFromSecretBox(g_hidHandle,index);
	}	
	return retStr;
}
