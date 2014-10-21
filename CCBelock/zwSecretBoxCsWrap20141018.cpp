//#include "CCBelock.h"
#include "zwSecretBoxCCBcsWrap.h"
#include "zwSecBoxCSHdr.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <windows.h>
#define PRFN	printf("%s\n",__FUNCTION__);OutputDebugStringA(__FUNCTION__);

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

//��򵥵Ĳ��ǵ����ĵ���ģʽ��ʵ���ϴﵽ�˵�����Ч����HID HANDLE��һ��ȫ�ֱ��������棻
//���۶��ٸ�JcSecBox����󣬴򿪵Ķ���ͬһ���ܺж˿ڣ����Ҳ����ظ��򿪣�
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
