#include "sha2.h"
//#include <time.h>
#include <stdio.h>
#include <string.h>
#include "zwEcies529.h"
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

//此处static变量可能会有多线程安全问题，以后再说；20140716.1600.周伟
char g_zwPskBuf[SHA256_DIGEST_SIZE];
char g_zwPskAsc[SHA256_DIGEST_SIZE*2+1];
ZWECIES_API const char * zwMergePsk(const char *pskInput)
{
	time_t rnd;
	memset(g_zwPskBuf,0,sizeof(g_zwPskBuf));
	memset(g_zwPskAsc,0,sizeof(g_zwPskAsc));
	sha256_ctx shactx;
	sha256_init(&shactx);
	//在此,特地不初始化rnd,使用其内部可能遗留的垃圾数据作为"随机"种子,
	//这是为了消除对于时间的依赖.20141128.周伟
	//rnd = time(NULL);	
	sha256_update(&shactx,(unsigned char *)pskInput,strlen(pskInput));
	sha256_update(&shactx,(unsigned char *)&rnd,sizeof(rnd));
	sha256_final(&shactx,(unsigned char *)g_zwPskBuf);
	for (int i=0;i<SHA256_DIGEST_SIZE;i++)
	{
		sprintf(g_zwPskAsc+i*2,"%02X",(unsigned char)g_zwPskBuf[i]);
	}
	return g_zwPskAsc;
}