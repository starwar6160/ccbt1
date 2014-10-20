//#include "CCBelock.h"
#include "zwSecretBoxCCBcsWrap.h"
#include "zwSecBoxCSHdr.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#ifdef _DEBUG_1018
////////////////////////////////C#��װ����//////////////////////////////////////////
//���ܺ�HIDͨ��,����0����ʧ�ܣ���������ɹ�
int zwSecboxHidOpen(void);
//�ر��ܺ�HIDͨ��������ΪsecboxHidOpen�ķ���ֵ���
void zwSecboxHidClose(int handleHid);
//ͨ��HID������Ȩ��֤�����ܺ�
void zwSendAuthReq2SecBox(int handleHid);
//ͨ��HID�����ܺз�Ӧ����֤���ɹ�����0������ֵ����ʧ��
int zwVerifyAuthRspFromSecBox(int handleHid);

//д�����ݵ��ܺУ����400���ֽڣ������ʽΪbase64������ַ�������Ҫָ��zwSecboxHidOpen�����ľ�����Լ�������
//�����Ŵ�ԼΪ1-8���ң����廹��Ҫ���Թ�ȷ����
void zwWriteData2SecretBox(int handleHid,const int index,const char *dataB64);
//ָ���ܺ�HID������Լ������ţ���ȡ�ܺе����ݣ�����base64����������ַ���
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

//���ڶ�д�����и�������֤����������HID�ӿ��ˣ����������
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
