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

#include "des.h"

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

//把进来的64比特信息转换为64比特无符号整型
ui64 myChar2Ui64(const char *inStr);
void myui64sprintf(ui64 n64,char *outHex);
//使用建行的通讯加密密钥ccbComm3DESKeyHex把8位动态码dyCode加密，返回在出参outEncDyCodeHex中
//其中通讯加密密钥，以及加密结果都是HEX字符串，动态码是整数
void zwCCB3DESEncryptDyCode(const char *ccbComm3DESKeyHex,const int dyCode,char *outEncDyCodeHex);

#endif // DESCBC_H

