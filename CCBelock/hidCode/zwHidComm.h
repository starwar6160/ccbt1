#ifndef zwHidComm_h__
#define zwHidComm_h__

//20140922.1545.该宏定义用于在这几天调试期间临时切换HID和原始不切分消息的串口通信，方便万敏和马浩调试
#define ZWUSE_HID_MSG_SPLIT		//是否使用HID的64字节消息切分方案


#ifdef __cplusplus
extern "C" {
#endif

#define JCHID_SERIAL_LENGTH	(16)
#define JCHID_FRAME_LENGTH	(64)
#define JCHID_MESSAGE_MAXLENGTH	(JCHID_FRAME_LENGTH*5)
typedef struct jcHidCommon_t{
	unsigned short int vid;
	unsigned short int pid;
	char HidSerial[JCHID_SERIAL_LENGTH+1];	
	void * hid_device;	//为了避免hid_device等HIDAPI类型定义扩散到外部，将其定义为void
} JCHID;

typedef enum jcHidStatus_t{
	JCHID_STATUS_OK,
	JCHID_STATUS_FAIL,
	JCHID_STATUS_INPUTNULL,
	JCHID_STATUS_HANDLE_NULL,
	JCHID_STATUS_RECV_ZEROBYTES
}JCHID_STATUS;

JCHID_STATUS jcHidOpen(JCHID *hid);
JCHID_STATUS jcHidClose(const JCHID *hid);
JCHID_STATUS jcHidSendData(const JCHID *hid,const char *inData,const int inDataLen);
JCHID_STATUS jcHidRecvData(JCHID *hid,char *outData,const int outMaxLen,int *outLen,const int timeout);
int zwSecboxData104Enc(const void *src,
	const int jcseq,void *outData,const int outLen);
int zwSecboxData104Dec(void *src,int *jcseq,void *inData,const int inLen);
//////////////////////////////////////////////////////////////////////////
//20141230为了解决演示时同一电脑同时连接锁具和密盒而做的PID区分
#define JCHID_VID_2014	(0X0483)
#define JCHID_PID_LOCK5151	(0X5710)
#define JCHID_PID_SECBOX	(0X5712)	
//20150331.1754.广州建行.默认6秒超时.原始值是3000毫秒也就是3秒，经常有报文
// 特别是第二条开始的报文的返回值收不到，跑2条报文的测试，3次内基本必定
// 碰到第二条报文的返回值收不到。将其加倍到6秒后情况极大改善，基本没有
// 遇到过收不到返回报文的情况了；
#define JCHID_RECV_TIMEOUT	(6000)	



#ifdef __cplusplus
}	//extern "C" {
#endif



#endif // zwHidComm_h__