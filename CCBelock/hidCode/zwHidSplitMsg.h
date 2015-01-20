#ifndef zwHidSplitMsg_h__
#define zwHidSplitMsg_h__
#ifdef __cplusplus
extern "C" {
#endif
//////////////////////////////////////////////////////////////////////////
	//该结构体刚好填满64字节
//JC_HID_TRANS_BYTES等同于JCHID_FRAME_LENGTH，两者必须保持一致
#define JC_HID_TRANS_BYTES		(64)	//HID留给我们的有效载荷大小
//多减去几个字节保证留有HID底层数据的余地
#define JC_HIDMSG_PAYLOAD_LEN	(JC_HID_TRANS_BYTES-3*(sizeof(unsigned short int)))
#define JC_HIDMSG_SPLIT_NUM		(12)		//最大有多少块
	typedef struct jcMulPartMessage_t{		
		unsigned short int nIndex;	//第多少块
		unsigned short int nTotalBlock;	//共有多少块		
		unsigned short int nDataLength;	//本块有效数据长度				
		unsigned char Data[JC_HIDMSG_PAYLOAD_LEN];
	}JC_MSG_MULPART;

typedef enum jcSplitStatus_t{
	JC_SP_OK,
	JC_SP_INPUT_NULL,
	JC_SP_OUTBUF_TOO_SHORT
}JC_SPLIT_STATUS;

//////////////////////////////////////////////////////////////////////////
//调试期间这两个值取值较小,以便迅速发现问题;20140917.1720
#define JC_MSG_MAXSIZE	(64*5)	//JSON消息的最大大小；根据记忆目前遇到的最大大小是272字节，建议设定为64字节的倍数

extern JC_MSG_MULPART s_mpSplit[JC_HIDMSG_SPLIT_NUM];

//计算一个简单的数据校验和
int zwDataSum26(const char *Data,const int DataLen);
// 模拟htons函数，本机字节序转网络字节序
unsigned short int HtoNs(unsigned short int h);
// 模拟ntohs函数，网络字节序转本机字节序
unsigned short int NtoHs(unsigned short int n);
// 模拟htonl函数，本机字节序转网络字节序
unsigned long int HtoNl(unsigned long int h);
// 模拟ntohl函数，网络字节序转本机字节序
unsigned long int NtoHl(unsigned long int n);

//给定输入数据指针inData,长度inDataLen，
//输出结构体数组首个成员指针outMsg，结构体数组成员个数
//把输入切分为一个个输出结构体，正好符合HID要求的64字节大小，可以直接发送
JC_SPLIT_STATUS jcMsgMulPartSplit(const char *inData,const int inLen,JC_MSG_MULPART outMsg[],const int outCount);
JC_SPLIT_STATUS jcMsgMulPartMerge(const JC_MSG_MULPART inMsg[],const int inCount,char *outData,int outLen);

#ifdef __cplusplus
}
#endif
#endif // zwHidSplitMsg_h__
