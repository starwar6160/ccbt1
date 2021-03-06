// eLockTest1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "jcElockTestHdr.h"
#include "CCBelock.h"
#include "zwHidMulHeader.h"

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
	printf("Callback Function %s:\t",__FUNCTION__);
	printf("devSerial=%s\t devReturnJson is:\n%s\n",DrivesIdSN,DrivesMessageJson);
}

void myMulHidDevJsonTest20150116A();
void myMulHidDevJsonTest20150116B();
void myMulHidDevJsonTest20150120A();
void myMulHidDevJsonTest20150120A1();



//void zwCCBDateTime2UTC(const char *ccbDateLocal,const char *ccbTimeLocal,time_t *outUTC);
	
	


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
	int msts=jlua::OpenJson(25);
	//试验了几个Json命令，目前只有这一个有反应
	//{ \"command\": \"Lock_System_Journal\",\"State\": \"get\"}
	jlua::SendToLockJson(jcHidJsonMsg0005);
	std::string recvBuf;
	recvBuf= jlua::RecvFromLockJson(2000);	
	jlua::CloseJson();
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

	//OpenDrives(hidType,	devSN2);
	//OpenDrives(hidType,	devSN1);
	SetReturnMessage(myReturnMessageTest115);

	InputMessage(hidType,devSN2,jcHidJsonMsg0005);
	Sleep(3000);
	InputMessage(hidType,devSN1,jcHidJsonMsg0005);

	getchar();
	//CloseDrives(hidType,	devSN1);
	//CloseDrives(hidType,	devSN2);
}



void myMulHidDevJsonTest20150116B()
{
	const char *devSN3=
		//"OQAiAACAAoTb1gAI";
		//"OQAiAAAAAAAAgAKE";
		//"";
		//"PAAbAAAAAAAAgAKE";
		"OQAiAACAAoQL1wAI";
	//"jcElockSerial": "PAAbAOgSACDAnwEg"
	//const char *devSN3="PAAbAOgSACDAnwEg";	
	const char *jcHidJsonMsg116t1="{\"command\": \"Test_Motor_Open\",\"cmd_id\": \"1234567890\",\"State\": \"test\"}";
	const char *jcHidJsonMsg116t2="{\"Command\": \"Lock_System_Journal\",\"Begin_No\": \"0\",\"End_No\": \"3\"}";
	const char *jcHidJsonMsg116t3="{\"Command\": \"Lock_Now_Info\"}";
	const char *hidType="Lock";
	const char *msgT5a="{\"command\":\"Test_USB_HID\",\"cmd_id\":\"1234567890\",\"input\":\"TestAnyString";
	const char *msgT5b="\",\"output\":\"\"}";

	SetReturnDrives(myHidListTest113);
	ListDrives("Lock");
	//Sleep(9000);
	//exit(1);

	//OpenDrives(hidType,	devSN3);
	SetReturnMessage(myReturnMessageTest115);

	char buf[128];
	memset(buf,0,128);
	sprintf(buf,"%s%d%s",msgT5a,7,msgT5b);

	InputMessage(hidType,devSN3,buf);
	Sleep(5000);
	
	//CloseDrives(hidType,devSN3);
	//getchar();
}

void myMulHidDevJsonTest20150120A()
{
	const char *devSN3=
		//"OQAiAACAAoTb1gAI";
		//"OQAiAAAAAAAAgAKE";
		//"";
		"PAAbAAAAAAAAgAKE";
	//"jcElockSerial": "PAAbAOgSACDAnwEg"
	//const char *devSN3="PAAbAOgSACDAnwEg";	
	const char *jcHidJsonMsg116t1="{\"command\": \"Test_Motor_Open\",\"cmd_id\": \"1234567890\",\"State\": \"test\"}";
	const char *jcHidJsonMsg116t2="{\"command\": \"Test_Motor_Close\",\"cmd_id\": \"1234567890\",\"State\": \"test\"}";
	const char *jcHidJsonMsg116t3="{\"Command\": \"Lock_System_Journal\",\"Begin_No\": \"0\",\"End_No\": \"3\"}";
	const char *jcHidJsonMsg116t4="{\"Command\": \"Lock_Now_Info\"}";
	const char *hidType="Lock";
	SetReturnDrives(myHidListTest113);
	ListDrives("Lock");
	//Sleep(9000);
	//exit(1);

	//////////////////////////////////////////////////////////////////////////
	//OpenDrives(hidType,	devSN3);
	SetReturnMessage(myReturnMessageTest115);

	//InputMessage(hidType,devSN3,jcHidJsonMsg116t1);
	//printf("CLOSE GATE BLOCK LOCKHEAD TEST 20150121 wait 5 sec\n");
	//Sleep(5000);
	//InputMessage(hidType,devSN3,jcHidJsonMsg116t2);
	//Sleep(3000);
	//for (int i=0;i<1;i++)
	//{
	//	InputMessage(hidType,devSN3,jcHidJsonMsg116t1);
	//	Sleep(1000);
	//	InputMessage(hidType,devSN3,jcHidJsonMsg116t2);
	//	Sleep(1000);
	//}
	//printf("Press any key to continue########################################\n");	
	Sleep(2000);
	//CloseDrives(hidType,devSN3);	
	//////////////////////////////////////////////////////////////////////////
	printf("Press any key to continue########################################\n");	
	//Sleep(1000);
	//getchar();

	Sleep(2000);
	//OpenDrives(hidType,	devSN3);
	SetReturnMessage(myReturnMessageTest115);
	//for (int i=0;i<1;i++)
	//{
	//	InputMessage(hidType,devSN3,jcHidJsonMsg116t1);
	//	Sleep(1000);
	//	InputMessage(hidType,devSN3,jcHidJsonMsg116t2);
	//	Sleep(1000);
	//}
	Sleep(2000);
	//CloseDrives(hidType,devSN3);

	//getchar();
}

void myMulHidDevJsonTest20150120A1()
{
	const char *devSN3=
		"PAAbAAAAAAAAgAKE";
	const char *msgT1="{\"command\": \"Test_Motor_Open\",\"cmd_id\": \"1234567890\",\"State\": \"test\"}";
	const char *msgT2="{\"command\": \"Test_Motor_Close\",\"cmd_id\": \"1234567890\",\"State\": \"test\"}";
	const char *msgT3="{\"Command\": \"Lock_System_Journal\",\"Begin_No\": \"0\",\"End_No\": \"3\"}";
	const char *msgT4="{\"Command\": \"Lock_Now_Info\"}";
	const char *msgT5a="{\"command\":\"Test_USB_HID\",\"cmd_id\":\"1234567890\",\"input\":\"TestAnyString";
	const char *msgT5b="\",\"output\":\"\"}";
	
	const char *hidType="Lock";
	SetReturnDrives(myHidListTest113);
	ListDrives("Lock");
	SetReturnMessage(myReturnMessageTest115);
	//////////////////////////////////////////////////////////////////////////
	for (int i=0;i<3;i++)
	{
		//OpenDrives(hidType,	devSN3);
		for (int j=0;j<50;j++)
		{
			char buf[128];
			memset(buf,0,128);
			sprintf(buf,"%s%d%s",msgT5a,7,msgT5b);
			InputMessage(hidType,devSN3,buf);						
			Sleep(3200);
			//InputMessage(hidType,devSN3,jcHidJsonMsg116t2);			
			//Sleep(8000);
		}
		//CloseDrives(hidType,devSN3);	
		
		Sleep(500);
		cout<<"Count "<<i<<" Complete"<<endl;
	}
}

void myDequeTest704(void)
{
	using std::deque;
	deque<int> dqi;
	for(int i=0;i<10;i++)
	{
		dqi.push_back(i);
	}
	printf("deque size=%d\n",dqi.size());
	dqi[2]=22;
	for(int i=0;i<10;i++)
	{		
		printf("D[%d]\t",dqi.front());
		dqi.pop_front();
	}
}

int myGetRunCount(void);

//#include "jcLockLog.h"
#include "zwHidMulHeader.h"

int _tmain(int argc, TCHAR * argv[])
{
	const char *devSN3="PAAbAAAAAAAAgAKE";
	const char *jcHidJsonMsg116t1="{\"command\": \"Test_Motor_Open\",\"cmd_id\": \"1234567890\",\"State\": \"test\"}";
	//string srcHexStr="414243440041424344";
	//string dstBinStr;
	//myHex2Bin(srcHexStr,dstBinStr);
	//cout<<dstBinStr<<endl;

	//zwStartGtestInDLL();


	//eLockJsonTest20150106();
	//luaSendJsonTest1(argc, argv);
	//myMulHidDevJsonTest20150116A();

	//myMulHidDevJsonTest20150120A();
	//myMulHidDevJsonTest20150120A1();
	//zwTest121a1();
	//myHidSerialTest126();
	//myMulHidDevJsonTest20150116B();
	//time_t dstDateTime=0;
	//zwCCBDateTime2UTC("20140515","000000",&dstDateTime);
	//printf("dstDateTime=%u\n",dstDateTime);

	//myDequeTest704();

	eLockGoogleTest2014(argc, argv);	
	//cout<<"输入任何数字结束"<<endl;
	//int tmpaa;
	//cin>>tmpaa;
	//myBlackKeyTest804();
}