#ifndef DES_H
#define DES_H

#include <stdint.h>

#define ui64 uint64_t
#define ui32 uint32_t
#define ui8  uint8_t

class DES
{
public:
    DES(ui64 key);
    ui64 des(ui64 block, bool mode);

    ui64 encrypt(ui64 block);
    ui64 decrypt(ui64 block);

    static ui64 encrypt(ui64 block, ui64 key);
    static ui64 decrypt(ui64 block, ui64 key);

protected:
    void keygen(ui64 key);

    ui64 ip(ui64 block);
    ui64 fp(ui64 block);

    void feistel(ui32 &L, ui32 &R, ui32 F);
    ui32 f(ui32 R, ui64 k);

private:
    ui64 sub_key[16]; // 48 bits each
};

class DES3
{
public:
	DES3(ui64 k1, ui64 k2, ui64 k3);
	ui64 encrypt(ui64 block);
	ui64 decrypt(ui64 block);

private:
	DES des1;
	DES des2;
	DES des3;
};
#endif // DES_H

#ifndef DESCBC_H
#define DESCBC_H

//#include "des.h"

class DESCBC
{
public:
	DESCBC(ui64 key, ui64 iv);
	ui64 encrypt(ui64 block);
	ui64 decrypt(ui64 block);
	void reset();

private:
	DES des;
	ui64 iv;
	ui64 last_block;
};

#ifdef  __cplusplus
extern "C" {
#endif

enum JC3DES_ERROR{
	JC3DES_OK,	//成功返回
	JC3DES_KEY_INVALID_LENGTH,	//密钥长度非法
	JC3DES_KEY_NONHEX,			//密钥不是HEX字符串
	JC3DES_KEY_WEAKKEY,			//密钥是DES弱密钥
	JC3DES_DYCODE_OUTOFRANGE,	//动态码范围超出有效范围
	JC3DES_OUTBUF_NULL			//输出缓冲区为空
};

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

#endif // DESCBC_H

