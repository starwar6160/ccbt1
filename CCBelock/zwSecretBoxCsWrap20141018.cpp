//#include "CCBelock.h"
#include "zwSecretBoxCCBcsWrap.h"
#include "zwSecBoxCSHdr.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <windows.h>
#define PRFN	printf("%s\n",__FUNCTION__);OutputDebugStringA(__FUNCTION__);

static int g_hidHandle;	//����ģʽ�ܺ�HID�����new���ٸ��඼�����ȫ�ֱ�������HIDͨ�ž��������һ���ࣻ

typedef enum jc_secret_box_status_t{
	JC_SECBOX_SUCCESS=0,
	JC_SECBOX_FAIL=1
}JC_SECBOX_STATUS;

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
