#include <stdio.h>
#include <cassert>
#include <cstring>
#include "sm3.h"
#include "jclmsCCB2014.h"
using namespace std;
typedef unsigned char BYTE;

extern "C"
{
	// 模拟ntohl函数，网络字节序转本机字节序
	unsigned long int NtoHl(unsigned long int n);
};

namespace zwTools {
#ifdef _DEBUG_USE_OLD_SM3HMAC20140703
	const uint8_t ZW_INPAD_FILL_CHAR = 0x36;
	const uint8_t ZW_OUTPAD_FILL_CHAR = 0x5C;
//密钥，消息，输出的摘要,都是二进制格式
	int32_t zwSm3Hmac7(zwHexTool & inPsk,
			   zwHexTool & inMessage, zwHexTool & outHmac) {
		SM3 sm3p1, sm3p2;
		uint8_t sm3hash_t1[ZWSM3_DGST_LEN];	//中间HASH结果pass1保存的地方
		//uint8_t sm3hash_t2[ZWSM3_DGST_LEN];   //中间HASH结果pass2保存的地方
		//////////////////////////////////////////////////////////////////////////
		uint8_t inpad[ZWSM3_BLOCK_LEN];
		uint8_t outpad[ZWSM3_BLOCK_LEN];
		int32_t i = 0;

		//////////////////////////////////////////////////////////////////////////
		//组合第一趟HASH所需的内容，psk放在开头，余下部分被ZW_INPAD_FILL_CHAR填充
		 memset(inpad, ZW_INPAD_FILL_CHAR, ZWSM3_BLOCK_LEN);
		 memcpy(inpad, inPsk.getBin(), inPsk.getBinLen());
		 memset(&sm3p1, 0, sizeof(sm3p1));
		//计算第一趟HASH结果
		 SM3_Init(&sm3p1);
		for (i = 0; i < ZWSM3_BLOCK_LEN; i++) {
			SM3_Update(&sm3p1, inpad[i]);
		} for (i = 0; i < inMessage.getBinLen(); i++) {
			SM3_Update(&sm3p1, *(inMessage.getBin() + i));
		}
		memset(sm3hash_t1, 0, ZWSM3_DGST_LEN);
		SM3_Final(&sm3p1, (char *)sm3hash_t1);
		//////////////////////////////////////////////////////////////////////////
		//计算第二趟HASH结果
		memset(outpad, ZW_OUTPAD_FILL_CHAR, ZWSM3_BLOCK_LEN);
		memcpy(outpad, inPsk.getBin(), inPsk.getBinLen());
		memset(&sm3p2, 0, sizeof(sm3p2));
		SM3_Init(&sm3p2);
		for (i = 0; i < ZWSM3_BLOCK_LEN; i++) {
			SM3_Update(&sm3p2, outpad[i]);
		}
		for (i = 0; i < ZWSM3_DGST_LEN; i++) {
			SM3_Update(&sm3p2, sm3hash_t1[i]);
		}
		SM3_Final(&sm3p2, outHmac.getBin());
		return 0;
	}
#endif // _DEBUG_USE_OLD_SM3HMAC20140703

//做一个实际运行的机器上的SM3计算，针对一个已知值A，把结果与PC上的SM3(A)比对，
//返回值为0代表正确，非0代表错误，那么整个算法不用运行下去了；
	int zwSM3SelfTest(void) {
		const char *msg = "mysm3testmessage";
		const int dstCorrectSM3Result = -1026675421;
		SM3 sm3;
		int i = 0;
		uint8_t buf[ZWSM3_BLOCK_LEN];
		uint8_t outHmac[ZWSM3_DGST_LEN];
		int *correctSM3Result = 0;
		memset(&sm3, 0, sizeof(sm3));
		memset(buf, 0, ZWSM3_BLOCK_LEN);
		memcpy(buf, msg, strlen(msg));
		SM3_Init(&sm3);
		for (i = 0; i < ZWSM3_BLOCK_LEN; i++) {
			SM3_Update(&sm3, buf[i]);
		}
		memset(outHmac, 0, ZWSM3_DGST_LEN);
		SM3_Final(&sm3, (char *)(outHmac));
		//强制转换outHmac开头4字节为一个无符号整数，用作检验结果是否正确
		//的一个高概率方案，错误几率是2的32次方之一
		correctSM3Result = (int *)outHmac;
		//返回值是实际结果与正确值之差，如果非零，就说明SM3算法验证失败
		return (*correctSM3Result) - dstCorrectSM3Result;
	}

}				//namespace jclms{

//////////////////////////////////////////////////////////////////////////
//国密办网站给出的SM3算法标准测试向量，函数返回值为0代表成功，也就是说算出来
//的值与标准测试向量没有差距，非零就是失败；尽管只检测了头32位，已经足够了；
int zwSM3StandardTestVector(void) {
	const char *msg = "abc";
	const int sm3TestVecResult1[]={0X66c7f0f4,0X62eeedd9,0Xd1f2d46b,0Xdc10e4e2,
		0X4167c487,0X5cf2f7a2,0X297da02b,0X8f4ba8e0};
	SM3 sm3;
	uint8_t buf[ZWSM3_BLOCK_LEN];
	uint8_t outHmac[ZWSM3_DGST_LEN];

	memset(&sm3, 0, sizeof(sm3));
	memset(buf, 0, ZWSM3_BLOCK_LEN);
	int srcLen=strlen(msg);
	memcpy(buf, msg, srcLen);

	SM3_Init(&sm3);
	for (int i = 0; i < srcLen; i++) {
		SM3_Update(&sm3, buf[i]);
	}
	memset(outHmac, 0, ZWSM3_DGST_LEN);
	SM3_Final(&sm3, (char *)(outHmac));
	int *oTmp=(int *)outHmac;
	for (int i=0;i<8;i++)
	{
		printf("%08X\t",NtoHl(oTmp[i]));
	}
	printf("\n");
	return sm3TestVecResult1[0]-NtoHl(oTmp[0]);
}
