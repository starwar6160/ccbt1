#ifndef des_h__
#define des_h__

#include <stdint.h>
#ifdef  __cplusplus
extern "C" {
#endif

#define ui64 uint64_t
#define ui32 uint32_t
#define ui8  uint8_t

typedef enum {
	JC3DES_OK,	//成功返回
	JC3DES_KEY_INVALID_LENGTH,	//密钥长度非法
	JC3DES_KEY_NONHEX,			//密钥不是HEX字符串
	JC3DES_KEY_WEAKKEY,			//密钥是DES弱密钥
	JC3DES_DYCODE_OUTOFRANGE,	//动态码范围超出有效范围
	JC3DES_OUTBUF_NULL			//输出缓冲区为空
}JC3DES_ERROR;

//判断输入字符串是否是HEX字符串，如果是，返回HEX字符串长度，如果不是，返回0
int myHexStringLength(const char *hexStr);
//检测常见DES弱密钥，输入是64bit/16HEX字符的DES密钥
JC3DES_ERROR myIsDESWeakKey(const char *desKey);

//使用建行的通讯加密密钥ccbComm3DESKeyHex把8位动态码dyCode加密，返回在出参outEncDyCodeHex中
//其中通讯加密密钥，以及加密结果都是HEX字符串，动态码是整数
JC3DES_ERROR zwCCB3DESEncryptDyCode(const char *ccbComm3DESKeyHex,const int dyCode,char *outEncDyCodeHex);

#ifdef  __cplusplus
}	//extern "C" {
#endif

#endif // des_h__
