//#include "CCBelock.h"
#include "stdafx.h"
#include "zwSecretBoxCCBcsWrap.h"
#include "zwSecBoxCSHdr.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <windows.h>
extern Poco::LogStream * pocoLog;
#define PRFN	OutputDebugStringA(__FUNCTION__);pocoLog->information()<<__FUNCTION__<<endl;
//printf("%s\n",__FUNCTION__);

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

CCBELOCK_API JcSecBox::JcSecBox()
{
	PRFN	
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
	PRFN
		if (NULL!=g_hidHandle)
		{
			//pocoLog->information()<<"jcSecBox Closed handle="<<g_hidHandle<<endl;
			//zwSecboxHidClose(g_hidHandle);
		}	
}



CCBELOCK_API int JcSecBox::SecboxAuth( void )
{
	PRFN	
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
	PRFN
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
	PRFN
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
