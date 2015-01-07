// eLockTest1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "jcElockTestHdr.h"
#include "CCBelock.h"

int eLockGoogleTest2014( int argc, _TCHAR ** argv );

void eLockJsonTest20150106();


int _tmain(int argc, _TCHAR * argv[])
{
	//return eLockGoogleTest2014(argc, argv);

	eLockJsonTest20150106();
	return 0;
}

int eLockGoogleTest2014( int argc, _TCHAR ** argv )
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

void eLockJsonTest20150106()
{
	const int BUFLEN=1024;
	//20150106.1710
	printf("eLockJsonTest20150106\n");
	int msts=jcLockJsonCmd_t2015a::OpenJson(25);
	//试验了几个Json命令，目前只有这一个有反应
	//{ \"command\": \"Lock_System_Journal\",\"State\": \"get\"}
	jcLockJsonCmd_t2015a::SendToLockJson("{ \"command\": \"Lock_System_Journal\",\"State\": \"get\"}");
	char recvBuf[BUFLEN];
	memset(recvBuf,0,BUFLEN);
	jcLockJsonCmd_t2015a::RecvFromLockJson(recvBuf,BUFLEN, 2000);	
	jcLockJsonCmd_t2015a::CloseJson();
	printf("Receive form Lock Json 20150106.1713 is \n%s\n",recvBuf);
}
