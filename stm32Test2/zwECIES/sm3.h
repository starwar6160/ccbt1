/******************************************************
* 
* Copyright (c) 2013,金储自动化技术有限公司
* All rights reserved.
* 
* 文件名称：SM3.h
* 摘    要：国家密码局规定的密码杂凑函数SM3
* 当前版本：1.0
* 作    者：周伟
* 完成日期：2013年12月
*
********************************************************/

#ifndef  SM3_H_
#define  SM3_H_
#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define HASHLEN  32
	typedef struct {
		unsigned int length[2];
		unsigned int h[8];
		unsigned int w[68];
	} SM3;

#define PAD  0x80
#define ZERO 0

#define H0 0x7380166F
#define H1 0x4914B2B9
#define H2 0x172442D7
#define H3 0xDA8A0600
#define H4 0xA96F30BC
#define H5 0x163138AA
#define H6 0xE38DEE4D
#define H7 0xB0FB0E4E

#define Tj_0_to_15       0x79CC4519
#define Tj_16_to_63      0x7A879D8A

	//循环左移
#define L_R(x,n)                     ((x)<<(n)|(x)>>(32-(n)))


	//布尔函数
#define FF_j_0_to_15(X, Y, Z)          ((X) ^ (Y) ^ (Z))
#define FF_j_16_to_63(X, Y, Z)         (((X) & (Y)) | ((X) & (Z)) | ((Y) & (Z)))
#define GG_j_0_to_15(X, Y, Z)          ((X) ^ (Y) ^ (Z))
#define GG_j_16_to_63(X, Y, Z)         (((X) & (Y)) | ((~X) & (Z)))



	//置换函数
#define P0(X)                       ((X) ^  L_R((X), 9 ) ^ L_R((X), 17))
#define P1(X)                       ((X) ^  L_R((X), 15) ^ L_R((X), 23))


	static void SM3_transform(SM3 * sm);

	void SM3_Init(SM3 * sm);

	void SM3_Update(SM3 * sm, int byte);

	void SM3_Final(SM3 * sm, char hash[HASHLEN]);

	//6个参数实际上是3个，密钥，消息，输出的摘要
	int32_t zwSm3Hmac(const char *psk,const int32_t pskLen,
		const char *message,const int32_t msgLen,
		char *outHmac,const int32_t outHmacLen);


#define ZWSM3_BLOCK_LEN (512/8)
#define ZWSM3_DGST_LEN (256/8)


#ifdef  __cplusplus
}	//extern "C" {
#endif

#endif				//SM3 头文件
