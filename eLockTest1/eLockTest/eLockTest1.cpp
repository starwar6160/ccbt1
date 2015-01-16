// eLockTest1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "jcElockTestHdr.h"
#include "CCBelock.h"

int eLockGoogleTest2014( int argc, _TCHAR ** argv );
void eLockJsonTest20150106();
void eLockJsonTestLua107(void);

const char *jcHidJsonMsg0005="{ \"command\": \"Lock_System_Journal\",\"State\": \"get\"}";

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

void ZJY1501STD myHidListTest113(const char* DrivesType,const char * DrivesIDList)
{
	//printf("jcHidDevType=%s\n",DrivesType);
	printf("USB PlugInOut Callback 20150116.0952\n");
	printf("Json List of enum jcHidDev Type %s Serial is:\n%s\n",DrivesType,DrivesIDList);
}

void ZJY1501STD myReturnMessageTest115(const char* DrivesIdSN,char* DrivesMessageJson)
{
	printf("Callback Function %s:\n",__FUNCTION__);
	printf("devSerial=%s\n devReturnJson is:\n%s\n",DrivesIdSN,DrivesMessageJson);
}

void myMulHidDevJsonTest20150116A();
void myMulHidDevJsonTest20150116B();

int _tmain(int argc, TCHAR * argv[])
{
	const char *devSN3="PAAbAAAAAAAAgAKE";
	const char *jcHidJsonMsg116t1="{\"command\": \"Test_Motor_Open\",\"cmd_id\": \"1234567890\",\"State\": \"test\"}";

	//return eLockGoogleTest2014(argc, argv);

	//eLockJsonTest20150106();
	//luaSendJsonTest1(argc, argv);
	myMulHidDevJsonTest20150116A();
	//myMulHidDevJsonTest20150116B();
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
	jcLockJsonCmd_t2015a::SendToLockJson(jcHidJsonMsg0005);
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

void myMulHidDevJsonTest20150116A()
{
	const char *hidType="Lock";
	const char *devSN1="00000000011C";
	const char *devSN2="00000000022C";
	SetReturnDrives(myHidListTest113);
	ListDrives("Lock");
	//Sleep(9000);
	//exit(1);

	OpenDrives(hidType,	devSN2);
	OpenDrives(hidType,	devSN1);
	SetReturnMessage(myReturnMessageTest115);

	InputMessage(hidType,devSN2,jcHidJsonMsg0005);
	Sleep(3000);
	InputMessage(hidType,devSN1,jcHidJsonMsg0005);

	getchar();
	CloseDrives(hidType,	devSN1);
	CloseDrives(hidType,	devSN2);
}



void myMulHidDevJsonTest20150116B()
{
	const char *devSN3="PAAbAAAAAAAAgAKE";
	//"jcElockSerial": "PAAbAOgSACDAnwEg"
	//const char *devSN3="PAAbAOgSACDAnwEg";	
	const char *jcHidJsonMsg116t1="{\"command\": \"Test_Motor_Open\",\"cmd_id\": \"1234567890\",\"State\": \"test\"}";

	const char *hidType="Lock";
	SetReturnDrives(myHidListTest113);
	ListDrives("Lock");
	//Sleep(9000);
	//exit(1);

	OpenDrives(hidType,	devSN3);
	SetReturnMessage(myReturnMessageTest115);

	InputMessage(hidType,devSN3,jcHidJsonMsg116t1);
	Sleep(3000);
	
	CloseDrives(hidType,devSN3);
	getchar();
}
