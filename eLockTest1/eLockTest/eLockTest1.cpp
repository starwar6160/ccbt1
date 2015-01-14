// eLockTest1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "jcElockTestHdr.h"
#include "CCBelock.h"

int eLockGoogleTest2014( int argc, _TCHAR ** argv );

void eLockJsonTest20150106();
void eLockJsonTestLua107(void);


//��TCHARתΪchar   
//*tchar��TCHAR����ָ�룬*_char��char����ָ��   
void TcharToChar (const TCHAR * tchar, char * _char)  
{  
    int iLength ;  
//��ȡ�ֽڳ���   
iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);  
//��tcharֵ����_char    
WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);   
}  

//ͬ��   
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
	//�����˼���Json���Ŀǰֻ����һ���з�Ӧ
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


