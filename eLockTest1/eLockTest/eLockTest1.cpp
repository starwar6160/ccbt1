// eLockTest1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "jcElockTestHdr.h"
#include "CCBelock.h"

int eLockGoogleTest2014( int argc, _TCHAR ** argv );

void eLockJsonTest20150106();
void eLockJsonTestLua107(void);


//将TCHAR转为char   
//*tchar是TCHAR类型指针，*_char是char类型指针   
void TcharToChar (const TCHAR * tchar, char * _char)  
{  
    int iLength ;  
//获取字节长度   
iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);  
//将tchar值赋给_char    
WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);   
}  

//同上   
void CharToTchar (const char * _char, TCHAR * tchar)  
{  
    int iLength ;  
  
    iLength = MultiByteToWideChar (CP_ACP, 0, _char, strlen (_char) + 1, NULL, 0) ;  
    MultiByteToWideChar (CP_ACP, 0, _char, strlen (_char) + 1, tchar, iLength) ;  
}  

void luaSendJsonTest1( int argc, TCHAR ** argv );

void myHidListTest113(const char* DrivesType,const char * DrivesIDList)
{
	//printf("jcHidDevType=%s\n",DrivesType);
	printf("Json List of enum jcHidDev Serial is:\n%s\n",DrivesIDList);
}

int _tmain(int argc, TCHAR * argv[])
{
	//return eLockGoogleTest2014(argc, argv);

	//eLockJsonTest20150106();
	//luaSendJsonTest1(argc, argv);

	SetReturnDrives(myHidListTest113);
	ListDrives("Lock");

	OpenDrives("Lock",	"00000000022C");
	CloseDrives("Lock",	"00000000022C");
	OpenDrives("Lock",	"00000000011C");
	CloseDrives("Lock",	"00000000011C");

	getchar();
	jcLockJsonCmd_t2015a::CloseJson();
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
	std::string recvBuf;
	recvBuf= jcLockJsonCmd_t2015a::RecvFromLockJson(2000);	
	jcLockJsonCmd_t2015a::CloseJson();
	printf("Receive form Lock Json 20150106.1713 is \n%s\n",recvBuf.c_str());
}

void luaSendJsonTest1( int argc, TCHAR ** argv )
{
	if (argc>=2)
	{
		char aFn[256];
		memset(aFn,0,256);
		TcharToChar(argv[1],aFn);
		printf("argv[1]=%s\n",aFn);
		myLuaBridgeSendJsonAPI(aFn);
	}
	else
	{
		myLuaBridgeSendJsonAPI("E:\\zwWorkSrc\\zwBaseLib\\zwTest201407\\test107.lua");
	}
}


