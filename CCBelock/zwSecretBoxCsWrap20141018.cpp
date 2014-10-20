//#include "CCBelock.h"
#include "zwSecretBoxCCBcsWrap.h"
#include "zwSecBoxCSHdr.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

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



JC_SECBOX_STATUS SecboxAuth(void)
{
	int hidHandle=0;
	printf("*****************************SecretBox zwSecboxHidOpen\n");
	hidHandle=zwSecboxHidOpen();
	printf("*****************************SecretBox zwSendAuthReq2SecBox\n");
	zwSendAuthReq2SecBox(hidHandle);
	printf("*****************************SecretBox zwVerifyAuthRspFromSecBox\n");
	int AuthRes=zwVerifyAuthRspFromSecBox(hidHandle);
	
	if (0==AuthRes)
	{
		printf("*****************************SecretBox Auth SUCCESS\n");
		return JC_SECBOX_SUCCESS;
	}
	else
	{
		printf("*****************************SecretBox Auth FAIL\n");
		return JC_SECBOX_FAIL;
	}
	zwSecboxHidClose(hidHandle);
}

//用于读写过程中附带的认证，不做开关HID接口了，由外层来做
static JC_SECBOX_STATUS mySecboxMiniAuth(int hidHandle)
{	
	assert(NULL!=hidHandle);
	if (NULL==hidHandle)
	{
		printf("HID HANDLE IS NULL!\n");
		return JC_SECBOX_FAIL;
	}
	zwSendAuthReq2SecBox(hidHandle);
	int AuthRes=zwVerifyAuthRspFromSecBox(hidHandle);
	if (0==AuthRes)
	{
		printf("*****************MINI SecretBox Auth SUCCESS\n");
		return JC_SECBOX_SUCCESS;
	}
	else
	{
		printf("*****************MINI SecretBox Auth FAIL\n");
		return JC_SECBOX_FAIL;
	}
}

CCBELOCK_API void SecboxWriteData( const int index,const char *dataB64 )
{
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
	int hidHandle=zwSecboxHidOpen();
	JC_SECBOX_STATUS status=mySecboxMiniAuth(hidHandle);
	if (JC_SECBOX_SUCCESS==status)
	{
		zwWriteData2SecretBox(hidHandle,index,dataB64);
	}	
	zwSecboxHidClose(hidHandle);
}

CCBELOCK_API const char * SecboxReadData( const int index )
{
	assert(index>=0 && index<=16);
	if (index<0 || index>16)
	{
		printf("Data Index out of range! must in 0 to 16\n");
	}
	const char *retStr=NULL;
	int hidHandle=zwSecboxHidOpen();	
	JC_SECBOX_STATUS status=mySecboxMiniAuth(hidHandle);
	if (JC_SECBOX_SUCCESS==status)
	{
		retStr=zwReadDataFromSecretBox(hidHandle,index);
	}	
	zwSecboxHidClose(hidHandle);
	return retStr;
}
