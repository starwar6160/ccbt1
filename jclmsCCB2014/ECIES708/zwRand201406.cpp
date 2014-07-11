#include "sha2.h"
#ifdef WIN32
unsigned int RdtscRand()
{
	unsigned int iRand = 0;
	__asm
	{
		rdtsc;0
			mov iRand, eax;
	}
	return iRand;
}
#else	//ARM
extern unsigned int RdtscRand();
//如果是ARM，请使用其硬件随机数生成器写一个如上原型的函数，放在单独文件中；
#endif // WIN32

void zwRandSeedGen603(char *randBuf,const int randBufLen)
{
	int rndCount=(randBufLen)/sizeof(unsigned int);
	unsigned int rnd;
	sha512_ctx shactx;
	sha512_init(&shactx);
	for (int i=0;i<rndCount;i++)
	{
		rnd = RdtscRand();
		sha512_update(&shactx,(unsigned char *)&rnd,sizeof(rnd));
	}
	unsigned char sha512out[SHA512_DIGEST_SIZE];
	sha512_final(&shactx,sha512out);
	//不管输出缓冲区有多大，反复复制SHA512的结果到输出缓冲区
	for (int i=0;i<randBufLen;i++)
	{
		randBuf[i]=sha512out[(i)%SHA512_DIGEST_SIZE];
	}

}
