#ifndef zwSecretBoxCCBcsWrap_h__
#define zwSecretBoxCCBcsWrap_h__


#ifdef __cplusplus
extern "C" {
#endif

	////////////////////////////////C#封装函数//////////////////////////////////////////
	//打开密盒HID通道,返回0代表失败，其他代表成功
	int SecboxHidOpen(void);
	//关闭密盒HID通道，参数为secboxHidOpen的返回值句柄
	void SecboxHidClose(int handleHid);
	//通过HID发送授权验证请求到密盒
	void SendAuthReq2SecBox(int handleHid);
	//通过HID接收密盒反应并验证，成功返回0，其他值代表失败
	int VerifyAuthRspFromSecBox(int handleHid);

	//写入数据到密盒，最大400多字节，输入格式为base64编码的字符串。需要指定zwSecboxHidOpen给出的句柄，以及索引号
	//索引号大约为1-8左右，具体还需要和赵工确定；
	void WriteData2SecretBox(int handleHid,const int index,const char *dataB64);
	//指定密盒HID句柄，以及索引号，读取密盒的数据，返回base64编码的数据字符串
	const char * ReadDataFromSecretBox(int handleHid,const int index);
#ifdef __cplusplus
}
#endif
#endif // zwSecretBoxCCBcsWrap_h__