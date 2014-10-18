#include "CCBelock.h"
#include "zwSecBoxCSHdr.h"

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

int SecboxHidOpen(void)
{
	return zwSecboxHidOpen();
}

void SecboxHidClose(int handleHid)
{
	zwSecboxHidClose(handleHid);
}

void SendAuthReq2SecBox(int handleHid)
{
	zwSendAuthReq2SecBox(handleHid);
}

int VerifyAuthRspFromSecBox(int handleHid)
{
	return zwVerifyAuthRspFromSecBox(handleHid);
}

void WriteData2SecretBox(int handleHid,const int index,const char *dataB64)
{
	zwWriteData2SecretBox(handleHid,index,dataB64);
}

const char * ReadDataFromSecretBox(int handleHid,const int index)
{
	return zwReadDataFromSecretBox(handleHid,index);
}