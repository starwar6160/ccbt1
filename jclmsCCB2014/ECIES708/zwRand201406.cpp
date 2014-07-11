
#include "sha2.h"
#ifdef WIN32
#include <Windows.h>
#endif	//end of WIN32

void zwRandSeedGen603(char *randBuf,const int randBufLen)
{
	int rndCount=(randBufLen)/sizeof(LARGE_INTEGER);
	LARGE_INTEGER rnd;
	sha512_ctx shactx;
	sha512_init(&shactx);
	for (int i=0;i<rndCount;i++)
	{
		QueryPerformanceCounter(&rnd);
		//*(LONGLONG *)(randBuf)=rnd.QuadPart;
		sha512_update(&shactx,(unsigned char *)&rnd,sizeof(rnd));
		Sleep(1);	//利用进程调度的不确定性增加随机性
	}
	unsigned char sha512out[SHA512_DIGEST_SIZE];
	sha512_final(&shactx,sha512out);
	//不管输出缓冲区有多大，反复复制SHA512的结果到输出缓冲区
	for (int i=0;i<randBufLen;i++)
	{
		randBuf[i]=sha512out[(i)%SHA512_DIGEST_SIZE];
	}
}

