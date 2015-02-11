#ifndef zwSecretBoxAuth_h__
#define zwSecretBoxAuth_h__
#include "zwSecBoxCSHdr.h"
//认证协议：所有整数都是网络字节序
//本协议中只使用short int，因为数据量很小；
//为了避免产生就结构体空洞，所以所有数字都是2字节的short int,特意安排的不会产生空洞的结构体成员分布
//协议类型：使用枚举JC_SECBOX_TYPE，具体参见定义
//所有5类数据包都有一个类型+长度的合计4字节头部；
//然后，认证请求和应答的有效载荷部分都是20字节+20字节二进制随机数据,实际上都是SHA1的HASH算法生成的
//数据读写的3类型的包，读取请求包没有有效载荷，只有头部；
//读取和写入的包，后面都是无格式二进制数据(当然文本数据也完全可以被当作二进制数据来读写)
//为了编程方便，定义了下列结构体作为上述数据格式的具体实现方式，当然也可以直接按照偏移值来读写该协议格式的数据


#define SHA1DGSTLEN	(160/8)	//SHA1的结果长度是160bit也就是20字节
typedef struct _jc_secbox_data_info_2014{
	unsigned char msg_type;			//包类型
	unsigned char data_index;	//数据编号
	short int data_len;	//后续不定长数据的长度
}SECBOX_DATA_INFO;

typedef struct _jc_secbox_auth_201410{
	char rand_data[SHA1DGSTLEN];		//一个160bit随机数
	char sha1_dgst[SHA1DGSTLEN];		//前者和PSK合起来的SHA1值
}SECBOX_AUTH;


typedef enum _jc_secbox_package_t{
	JC_SECBOX_AUTH_REQUEST,		//认证请求
	JC_SECBOX_AUTH_RESPONE,		//认证回复
	JC_SECBOX_DATA_READ_REQUEST,//数据读取请求
	JC_SECBOX_DATA_READ_PAYLOAD,//数据读取回复真正数据	
	JC_SECBOX_DATA_WRITE,		//数据写入
	//JC_SECBOX_ALG_3DES_ENC,		//3DES加密
	//JC_SECBOX_ALG_3DES_DEC,		//3DES解密
	//JC_SECBOX_DYCODE_INITCLOSECODE,	//初始闭锁码
	//JC_SECBOX_DYCODE_CLOSECODE,		//闭锁码
	//JC_SECBOX_DYCODE_DYPASS1,	//第一开锁码
	//JC_SECBOX_DYCODE_VERCODE,	//校验码
	//JC_SECBOX_DYCODE_DYPASS2,	//第二开锁码
	JC_SECBOX_LMS_GENDYCODE,	//LMS生成动态码数据包
	JC_SECBOX_LMS_VERDYCODE		//LMS验证动态码数据包

}JC_SECBOX_TYPE;



extern const unsigned char  SECBOXPSK2014D[SHA1DGSTLEN];	//一个160bit的PSK，上下位机共同秘密持有,上位机使用
extern const unsigned char  SECBOXPSK2014Z[SHA1DGSTLEN];	//一个160bit的PSK，上下位机共同秘密持有，下位机使用

//HID帧的格式：
//头2个字节是一个SHORT INT，实际上是JC_SECBOX_TYPE枚举的值，肯定不会溢出
//接下来，后面根据该值的指示，是不同类型的报文结构体

#ifdef __cplusplus
extern "C" {
#endif
	//生成随机的认证请求,以及认证回复，输出到SECBOX_AUTH结构体，最长为HID帧长度64字节
	//原理在于，上下位机都采用一个随机数+共同持有并保密的PSK生成一个SHA1结果，
	//然后把随机数和SHA1结果发给对方作为验证的方式
	void zwSecboxAuthDataGen(SECBOX_AUTH *req);
	//生成随机的回复认证应答,输出到SECBOX_AUTH结构体，最长为HID帧长度64字节
	//返回0代表认证成功，其他值代表失败；
	//输入和应答都通过同一个结构体req输入输出
	int zwSecboxAuthVerify(SECBOX_AUTH *req);
	//生成包括前导的2字节“包类型”在内的密盒授权请求和应答字节串用于HID发送
	void zwSecboxAuthByteGen(char *outReq,int *outLen,JC_SECBOX_TYPE type);
//////////////////////////////////////////////////////////////////////////
	//生成PC到密盒的数据写入请求，请保证输出缓冲区至少有"SECBOX_DATA_INFO+实际数据长度"这么长
	void pc2BoxDataWriteRequest(const unsigned char index,const char *inData,const int inLen,
		char *outBuf,int *outLen);
	//生成PC到密盒的数据读取请求，请保证输出缓冲区至少有SECBOX_DATA_INFO这么长
	//index指定读取多少号数据
	void pc2BoxDataReadRequest(const unsigned char index, char *outBuf,int *outLen);
	//分析密盒发回的读取数据，并输出数据内容和长度
	void box2PcDataParse(const char *inData,const int inLen,
		char *outBuf,int *outLen);



//////////////////////////////////////////////////////////////////////////
	void zwSecretBoxAuthTest1015A(void);
	void myDumpPsk1016(void);

#ifdef __cplusplus
}
#endif

#endif // zwSecretBoxAuth_h__

