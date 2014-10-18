#include "CCBelock.h"
#include "zwSecBoxCSHdr.h"

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

int SecboxOpen(void)
{
	return zwSecboxHidOpen();
}

void SecboxClose(int handleHid)
{
	zwSecboxHidClose(handleHid);
}

void SecboxSendAuthReq(int handleHid)
{
	zwSendAuthReq2SecBox(handleHid);
}

int SecboxVerifyAuthRsp(int handleHid)
{
	return zwVerifyAuthRspFromSecBox(handleHid);
}

void SecboxWriteData(int handleHid,const int index,const char *dataB64)
{
	zwWriteData2SecretBox(handleHid,index,dataB64);
}

const char * SecboxReadData(int handleHid,const int index)
{
	return zwReadDataFromSecretBox(handleHid,index);
}