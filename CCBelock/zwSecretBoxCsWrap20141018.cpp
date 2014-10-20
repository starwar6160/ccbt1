//#include "CCBelock.h"
#include "zwSecretBoxCCBcsWrap.h"
#include "zwSecBoxCSHdr.h"
#include <stdio.h>

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

static int g_HidSecretBoxHandle=0;

JC_SECBOX_STATUS SecboxAuth(void)
{
	printf("*****************************SecretBox zwSecboxHidOpen\n");
	g_HidSecretBoxHandle=zwSecboxHidOpen();
	printf("*****************************SecretBox zwSendAuthReq2SecBox\n");
	zwSendAuthReq2SecBox(g_HidSecretBoxHandle);
	printf("*****************************SecretBox zwVerifyAuthRspFromSecBox\n");
	int AuthRes=zwVerifyAuthRspFromSecBox(g_HidSecretBoxHandle);
	
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
	
}

CCBELOCK_API void SecboxWriteData( const int index,const char *dataB64 )
{
	zwWriteData2SecretBox(g_HidSecretBoxHandle,index,dataB64);
}

CCBELOCK_API const char * SecboxReadData( const int index )
{
	return zwReadDataFromSecretBox(g_HidSecretBoxHandle,index);
}

CCBELOCK_API void SecboxReadClose( void )
{
	zwSecboxHidClose(g_HidSecretBoxHandle);
}