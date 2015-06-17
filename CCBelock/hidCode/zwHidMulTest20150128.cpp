#include "stdafx.h"
#include <gtest/gtest.h>
#include "hidapi.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"

#include "zwHidMulHeader.h"
#include "zwHidGtest130.h"
#include "ATMCMsgConvert\\myConvIntHdr.h"
using jcAtmcConvertDLL::zwGetJcxmlMsgType;

#define ZWUSEGTEST
//#define _DEBUG_LOCK2TEST
//#define _DEBUG_DEV2
//#define _DEBUG_ZJYBAD20150325
//#define _DEBUG326
//#define _DEBUG401JCHIDENUM
//#define _DEBUG331
//#define _DEBUG401ELOCKSTATUS
void cdecl myATMCRecvMsgRotine(const char *pszMsg);


#ifdef ZWUSEGTEST
int G_TESTCB_SUCC=0;	//是否成功调用了回调函数的一个标志位，仅仅测试用

namespace zwHidGTest20150130{
	const int G_BUFSIZE=1024;
	char s_recvMsg[G_BUFSIZE];
	char s_devList[G_BUFSIZE];

	void ZJY1501STD myReturnMessageTest130(const char* DrivesIdSN,char* DrivesMessageJson)
	{
		int inJsonLen=strlen(DrivesMessageJson);
		assert(inJsonLen<G_BUFSIZE);
		memset(s_recvMsg,0,G_BUFSIZE);
		printf("Callback Function %s:\trecvDataLen=%d\t",__FUNCTION__,inJsonLen);
		printf("devSerial=%s\t devReturnJson is:\n%s\n",DrivesIdSN,DrivesMessageJson);
		strcpy(s_recvMsg,DrivesMessageJson);
	}

	void ZJY1501STD myHidListTest130(const char* DrivesType,const char * DrivesIDList)
	{
#ifndef _DEBUG
		printf("USB PlugInOut Callback 20150130.1445\n");
		printf("Json List of enum jcHidDev Type %s Serial is:\n%s\n",DrivesType,DrivesIDList);
#endif // _DEBUG
		memset(s_devList,0,G_BUFSIZE);
		strcpy(s_devList,DrivesIDList);
	}


	//测试套件初始化和结束事件
	class ATMCDLLSelfTest : public testing::Test
	{
	public:
		char cmdBuf[128];	
		char *devSN1;
		char *devSN2;
	protected:
		virtual void SetUp();
		virtual void TearDown();

	};



	void ATMCDLLSelfTest::SetUp()
	{
		//"jcElockSerial": "OQAiAACAAoQL1wAI",
		//"jcElockSerial": "QAAiAACAAoTXuwAI"	
		//devSN1="OQAiAACAAoQL1wAI";
		devSN1="IQAkAACAAoRXuwAI";
		//devSN2="QAAiAACAAoTXuwAI";
		//devSN2="JQA7AACAAoRXuwAI";	//20150209,万敏重刷程序以后的序列号
		devSN2="OQAiAACAAoQL1wAI";
		//devSN1="QAAiAACAAoTXuwAI";
//////////////////////////////////////////////////////////////////////////
		//printf("%02u\n",myHidSerialToInt(devSN1));
		//printf("%02u\n",myHidSerialToInt(devSN2));
		

//////////////////////////////////////////////////////////////////////////
		SetReturnMessage(myReturnMessageTest130);
		const char *msgT5a="{\"command\":\"Test_USB_HID\",\"cmd_id\":\"1234567890\",\"input\":\"TestAnyString";
		const char *msgT5b="\",\"output\":\"\"}";	
		memset(cmdBuf,0,128);
		sprintf(cmdBuf,"%s%d%s",msgT5a,7,msgT5b);
	}

	void ATMCDLLSelfTest::TearDown()
	{

	}
	//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG401JCHIDENUM
	TEST_F(ATMCDLLSelfTest, jcHidDevEnumNormal)
	{
		//枚举设备，正常情况测试
		memset(s_devList,0,G_BUFSIZE);
		SetReturnDrives(myHidListTest130);
		ListDrives("Lock");
		//"{"jcElockSerial": "a"}"这样的最低限度json是22字节长度
		EXPECT_GT(strlen(s_devList),22);	
	}
#endif // _DEBUG401JCHIDENUM


#ifdef _DEBUG326


	//监测正常Open以后没有正常Close而是直接拔掉电子锁，之后Open是否还是成功
	TEST_F(ATMCDLLSelfTest, jcLockPlugOuted)
	{
		//测试多次Open和Close设备
		char devSN1p[48];
		memset(devSN1p,0,48);
		strcpy(devSN1p,devSN1);
		const char *hidType="Lock";

		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Open(22));
		//EXPECT_EQ(jch::G_SUSSESS,OpenDrives(hidType,	devSN1p));
		SetReturnMessage(myReturnMessageTest130);
		//EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN1p));
		//EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN1p));
		printf("########################1拔下USB线，暂时不要插上，期待Open返回失败327\n");				
		Sleep(9200);
		EXPECT_NE(ELOCK_ERROR_SUCCESS,Open(22));
		Sleep(1200);
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Close());
		printf("***********************再次插上USB线，期待Open成功\n");
		Sleep(9200);
		
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Open(22));
		printf("等待9秒看看数据接收线程继续运行的效果");
		Sleep(9000);
		//清空向量
		jch::vecJcHid.clear();
	}
#endif // _DEBUG326

#ifdef _DEBUG327B
	TEST_F(ATMCDLLSelfTest, jcLockPlugOuted327)
	{
		//测试多次Open和Close设备
		char devSN1p[48];
		memset(devSN1p,0,48);
		strcpy(devSN1p,devSN1);
		const char *hidType="Lock";

		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Open(22));
		SetReturnMessage(myReturnMessageTest130);		
		Sleep(5200);
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Open(22));
		printf("等待9秒看看数据接收线程继续运行的效果");
		Sleep(9000);
		//清空向量
		jch::vecJcHid.clear();
	}
#endif // _DEBUG327B


#ifdef _DEBUG_ZJYBAD20150325
	TEST_F(ATMCDLLSelfTest, zjydbgBad3)
	{
		//测试多次Open和Close设备
		char devSN1p[48];
		memset(devSN1p,0,48);
		strcpy(devSN1p,devSN1);
		const char *hidType="Lock";

		//EXPECT_EQ(jch::G_SUSSESS,OpenDrives(hidType,	devSN1p));
		//EXPECT_EQ(jch::G_SUSSESS,OpenDrives(hidType,	devSN1p));
		SetReturnMessage(myReturnMessageTest130);
		//EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN1p));
		//EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN1p));
		Sleep(2000);
		//清空向量
		jch::vecJcHid.clear();
		//jch::zwCleanJchidVec();
	}

	TEST_F(ATMCDLLSelfTest, zjydbgBad1)
	{
		//测试没有回调函数的时候
		const char *hidType="Lock";
		//EXPECT_EQ(jch::G_SUSSESS,OpenDrives(hidType,	devSN1));
		SetReturnMessage(NULL);
		EXPECT_EQ(jch::G_SUSSESS,InputMessage(hidType,devSN1,cmdBuf));
		Sleep(3000);
		//EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN1));
		Sleep(2000);
		//清空向量
		jch::vecJcHid.clear();
	}

	TEST_F(ATMCDLLSelfTest, zjydbgBad2)
	{
		//测试没有序列号错误的时候无法打开HID设备
		char devSN1p[48];
		memset(devSN1p,0,48);
		strcpy(devSN1p,devSN1);
		devSN1p[0]='Z';
		const char *hidType="Lock";

		//EXPECT_EQ(jch::G_FAIL,OpenDrives(hidType,	devSN1p));
		SetReturnMessage(myReturnMessageTest130);
		//关闭不存在的设备可以成功
		//EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN1p));
		Sleep(2000);
		//清空向量
		jch::vecJcHid.clear();
	}
#endif // _DEBUG_ZJYBAD20150325

#ifdef _DEBUG_DEV2

	TEST_F(ATMCDLLSelfTest, zjydbgNormal)
	{
		//正常使用流程		
		const char *hidType="Lock";
		//EXPECT_EQ(jch::G_SUSSESS,OpenDrives(hidType,	devSN1));
		SetReturnMessage(myReturnMessageTest130);
		EXPECT_EQ(jch::G_SUSSESS,InputMessage(hidType,devSN1,cmdBuf));
		Sleep(3000);
		//EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN1));
		Sleep(2000);
		//清空向量
		jch::vecJcHid.clear();
	}

#ifdef _DEBUG_LOCK2TEST
	TEST_F(ATMCDLLSelfTest, zjydbgNorma2)
	{
		const char *hidType="Lock";
		//EXPECT_EQ(jch::G_SUSSESS,OpenDrives(hidType,	devSN2));
		SetReturnMessage(myReturnMessageTest130);
		EXPECT_EQ(jch::G_SUSSESS,InputMessage(hidType,devSN2,cmdBuf));
		Sleep(3000);
		//EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN2));
		Sleep(2000);
		//清空向量
		jch::vecJcHid.clear();
	}


	TEST_F(ATMCDLLSelfTest, zjydbgNormaMulDirectOpen2)
	{
		hid_device *dv1=NULL,*dv2=NULL;
		try{
			wchar_t sn1[17],sn2[17];
			memset(sn1,0,sizeof(wchar_t)*17);
			memset(sn2,0,sizeof(wchar_t)*17);
			jch::CharToTchar(devSN1,sn1);
			jch::CharToTchar(devSN2,sn2);
			VLOG(4)<<"SN1:"<<devSN1<<"\tSN2:"<<devSN2<<endl;
			dv1=	hid_open(0x0483,0x5710,sn1);
			EXPECT_NE(NULL,(int)dv1);
			Sleep(2000);	//此处起码要1秒-2秒之间的一个数字，曾经秒够了，后来变成2秒才够
			dv2=	hid_open(0x0483,0x5710,sn2);
			EXPECT_NE(NULL,(int)dv2);
			VLOG(4)<<"TWO HID DEVICE OPEN END"<<endl;
		}
		catch(...)
		{
			//EXPECT_NE(NULL,static_cast<void *>(dv2));
			LOG(ERROR)<<"zjydbgNormaMulDirectOpen2 FAIL"<<endl;
		}
		if (NULL!=dv1)
		{
			VLOG(4)<<"Closed hid Device 1"<<endl;
			hid_close(dv1);
		}

		if (NULL!=dv2)
		{
			VLOG(4)<<"Closed hid Device 2"<<endl;
			hid_close(dv2);
		}
		Sleep(1000);
		VLOG(4)<<"End Test of zjydbgNormaMulDirectOpen2"<<endl;
	}

	TEST_F(ATMCDLLSelfTest, zjydbgNormaMulOpen2)
	{
		const char *hidType="Lock";
		//EXPECT_EQ(jch::G_SUSSESS,OpenDrives(hidType,	devSN1));
		Sleep(2000);	//此处至少要等待2秒
		//EXPECT_EQ(jch::G_SUSSESS,OpenDrives(hidType,	devSN2));
		SetReturnMessage(myReturnMessageTest130);
		EXPECT_EQ(jch::G_SUSSESS,InputMessage(hidType,devSN1,cmdBuf));
		EXPECT_EQ(jch::G_SUSSESS,InputMessage(hidType,devSN2,cmdBuf));
		Sleep(3000);
		//EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN1));
		//EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN2));
		Sleep(2000);
		//清空向量
		jch::vecJcHid.clear();
		Sleep(1000);
	}
#endif // _DEBUG_LOCK2TEST






//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////

	TEST_F(ATMCDLLSelfTest, LockNormalUse)
	{
		//测试从外部字符串ID计算来的设备HASH是否正确
		jch::zwJcHidDbg15A hid1;
		//此处计算的是标准锁具的HASH
		hid1.SetElock(NULL);
		uint32_t hidHash=hid1.GetHash();
		cout<<"hidHash="<<hidHash<<endl;
		EXPECT_GT(hidHash,0);
		EXPECT_EQ(hidHash,2378562802);
		hid1.OpenHidDevice();
	}


	TEST_F(ATMCDLLSelfTest, LockErrorSerial)
	{
		jch::zwJcHidDbg15A hid1;
		hid1.SetElock("errorserial");
		EXPECT_EQ(jch::G_FAIL,hid1.OpenHidDevice());
	}

	TEST_F(ATMCDLLSelfTest, LockMultiOpen)
	{
		//测试2个类对象打开同一个设备的情况
		jch::zwJcHidDbg15A hid1,hid2;
		hid1.SetElock(NULL);
		hid2.SetElock(NULL);
		cout<<"will SUCCESS Open HID Device 1"<<endl;
		EXPECT_EQ(jch::G_SUSSESS,hid1.OpenHidDevice());
		cout<<"will FAIL Open same HID Device 1"<<endl;
		EXPECT_EQ(jch::G_FAIL,hid2.OpenHidDevice());

	}

	TEST_F(ATMCDLLSelfTest, LockNotOpen)
	{
		//测试没有执行OpenHidDevice的动作
		jch::zwJcHidDbg15A hdv;
		hdv.SetElock(NULL);
		EXPECT_EQ(jch::G_FAIL,hdv.PushJson(cmdBuf));
		Sleep(700);
	}

	TEST_F(ATMCDLLSelfTest, LockPushNULL)
	{
		//测试发送空的命令字符串的操作
		jch::zwJcHidDbg15A hdv;
		hdv.SetElock(NULL);
		hdv.OpenHidDevice();
		//清理s_recvMsg，期待收到的长度等于0，因为发送空命令字符串
		memset(s_recvMsg,0,G_BUFSIZE);
		EXPECT_EQ(jch::G_FAIL,hdv.PushJson(NULL));
		EXPECT_EQ(jch::G_FAIL,hdv.PushJson(""));
		Sleep(700);
		EXPECT_EQ(strlen(s_recvMsg),0);
	}

	TEST_F(ATMCDLLSelfTest, LockNotSetPara)
	{
		//测试没有设置参数就执行OpenHidDevice的动作
		jch::zwJcHidDbg15A hdv;
		hdv.OpenHidDevice();
		EXPECT_EQ(jch::G_FAIL,hdv.PushJson(cmdBuf));
		Sleep(700);
	}

	//////////////////////////////////////////////////////////////////////////


	TEST_F(ATMCDLLSelfTest, jcHidDevEnumBad1)
	{
		//枚举设备，输入错误情况测试
		memset(s_devList,0,G_BUFSIZE);
		SetReturnDrives(myHidListTest130);
		ListDrives("Lock1");
		//"{"jcElockSerial": "a"}"这样的最低限度json是22字节长度
		EXPECT_EQ(strlen(s_devList),0);	
	}

	TEST_F(ATMCDLLSelfTest, jcHidDevEnumBad2)
	{
		//枚举设备，无回调函数情况测试
		memset(s_devList,0,G_BUFSIZE);
		SetReturnDrives(NULL);
		ListDrives("Lock");
		//"{"jcElockSerial": "a"}"这样的最低限度json是22字节长度
		EXPECT_EQ(strlen(s_devList),0);	
	}

	TEST_F(ATMCDLLSelfTest, jcHidDevEnumBad3)
	{
		//枚举设备，输入错误并且无回调函数情况测试
		memset(s_devList,0,G_BUFSIZE);
		SetReturnDrives(NULL);
		ListDrives("Lock1");
		//"{"jcElockSerial": "a"}"这样的最低限度json是22字节长度
		EXPECT_EQ(strlen(s_devList),0);	
	}
	//////////////////////////////////////////////////////////////////////////

#endif // _DEBUG_DEV2
	const char *g_msg00="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0000</TransCode><TransName>CallForActInfo</TransName><TransDate>20150401</TransDate><TransTime>090333</TransTime><DevCode>440600300145</DevCode><SpareString1></SpareString1><SpareString2></SpareString2></root>";
	const char *g_msg01="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0001</TransCode><TransName>SendActInfo</TransName><TransDate>20150401</TransDate><TransTime>090340</TransTime><DevCode>440600300145</DevCode><LockMan>BeiJing.JinChu</LockMan><LockId>22222222</LockId><ActInfo>BFr4af4YvXxtsGmJnDCKsZ3OhmkZimSur0itl6fwuc3fqkiK6j05arPl2on3N4rfVLQkfo9GRceMmbXDebf7rdY=.h/UOVQjtcby5I3wQyUXDdB/uDTeUq1oW.vNDgbpmArJ2CTkBSAJ0NKDeZ6vUifPLKjbLZH3eiFE+QkEBOy5+r2ZJEYEgBmjD1KGIbVfridTa3sjuqaFo0lby7YpqBXTo56v5yIzyz28k=</ActInfo><SpareString1></SpareString1><SpareString2></SpareString2></root>";
	const char *g_msg02="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0002</TransCode><TransName>QueryForLockStatus</TransName><TransDate>20150401</TransDate><TransTime>084539</TransTime><DevCode>440600300145</DevCode><LockMan></LockMan><LockId></LockId><SpareString1></SpareString1><SpareString2></SpareString2></root>";
	const char *g_msg03="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0003</TransCode><TransName>TimeSync</TransName><TransDate>20150401</TransDate><TransTime>085006</TransTime></root>";
	const char *g_msg04="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0004</TransCode><TransName>ReadShutLockCode</TransName><TransDate>20150401</TransDate><TransTime>124932</TransTime><DevCode>440600300145</DevCode><SpareString1></SpareString1><SpareString2></SpareString2></root>";
	


	int myRndFunc(const int funcIndex)
	{
		
		int r=0;
		switch (funcIndex)
		{
		case 0:printf("ZWRND330:Open(1)\n");
			r=Open(1);			
			break;
		case 1:printf("ZWRND330:Close()\n");
			r=Close();			
			break;
		case 2:printf("ZWRND330:Notify(msg03)\n");
			r=Notify(g_msg03);			
			break;
		case 3:printf("ZWRND330:Notify(NULL)\n");
			r=Notify(NULL);			
			break;
		case 4:printf("ZWRND330:SetRecvMsgRotine(myATMCRecvMsgRotine)\n");
			r=SetRecvMsgRotine(myATMCRecvMsgRotine);			
			break;
		case 5:printf("ZWRND330:SetRecvMsgRotine(NULL)\n");
			r=SetRecvMsgRotine(NULL);			
			break;
		case 6:printf("ZWRND330:SetRecvMsgRotine((RecvMsgRotine )0x11112222)\n");
			SetRecvMsgRotine((RecvMsgRotine )0x11112222);			
			break;
		case 7:printf("ZWRND330:Notify((const char *)0x11112222)\n");
			Notify((const char *)0x11112222);			
			break;
		}
		return r;
	}

	//TEST_F(ATMCDLLSelfTest, jcHidDevRandom1)
	//{
	//	int i=1,j=0;		
	//	srand(time(NULL));
	//	for (int i=0;i<10;i++)
	//	{
	//		//EXPECT_EQ(ELOCK_ERROR_SUCCESS,myRndFunc(i));
	//		myRndFunc(rand()%8);
	//	}
	//}

	void myWaitForRecv331()
	{
		printf("%s\n",__FUNCTION__);
		for(int i=0;i<50;i++)
		{
			if (1==G_TESTCB_SUCC)
			{
				break;
			}
			Sleep(300);
			putchar('.');
		}
		putchar('\n');
	}

	//等待最大超时时间，还没有收到数据的话，需要人工按键继续
	void myWaitForRecvKeyPress424()
	{
		//printf("%s\n",__FUNCTION__);
		for(int i=0;i<12;i++)
		{
			if (1==G_TESTCB_SUCC)
			{
				break;
			}
			Sleep(500);
			putchar('B');
		}
		putchar('\n');
		//if (1!=G_TESTCB_SUCC)
		//{
		//	printf("没有从锁具收到有效数据，请在锁具键盘按下几个键测试后，按电脑键盘继续测试\n");
		//	int i=getchar();			
		//}
	}


#ifdef _DEBUG331
	TEST_F(ATMCDLLSelfTest, jcHidDev331Normal_1_Short)
	{			
		Sleep(1000);
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Open(22));
		for (int i=0;i<3;i++)
		{		 
		SetRecvMsgRotine(myATMCRecvMsgRotine);	
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg02));	
		myWaitForRecv331();
		EXPECT_EQ(1,G_TESTCB_SUCC);
		}
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Close());
		
		//Sleep(2000);
	}

	

	//TEST_F(ATMCDLLSelfTest, jcHidDev331Normal_2_Long)
	//{		
	//	SetRecvMsgRotine(myATMCRecvMsgRotine);		
	//	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Open(22));
	//	//Sleep(8200);
	//	for (int i=0;i<5;i++)
	//	{
	//		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg03));	
	//		myWaitForRecv331();
	//		//if (i%2==0)
	//		{
	//			Sleep(3000);
	//		}			
	//		EXPECT_EQ(1,G_TESTCB_SUCC);
	//	}
	//	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Close());		
	//	//Sleep(2000);
	//}
#endif // _DEBUG331

	

#ifdef _DEBUG401ELOCKSTATUS

	TEST_F(ATMCDLLSelfTest, jcHidDev401_ElockStatusA3_ELockPlugIn)
	{
		SetRecvMsgRotine(myATMCRecvMsgRotine);		
		printf("请拔下USB线再插上，测试锁具拔插后是否还能正常工作");
		Sleep(6000);
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg03));	
		myWaitForRecv331();
		EXPECT_EQ(1,G_TESTCB_SUCC);

	}
#endif // _DEBUG401ELOCKSTATUS



///////////////////////////////test20150424///////////////////////////////////////////

	vector<string> g_vecTestResultXML;


	TEST_F(ATMCDLLSelfTest, jcHidDev424TestLongStable)
	{			
		Sleep(3000);
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Open(22));
		
		for (int i=0;i<
			//12*60*9;
			2;
			i++)
		{		 
			SetRecvMsgRotine(myATMCRecvMsgRotine);	
			

#define _DEBUG508A1
#ifdef _DEBUG508A1
			EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg00));	
			//EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg02));	
			//Sleep(2000);
			//myWaitForRecvKeyPress424();			
			//EXPECT_EQ(1,G_TESTCB_SUCC);
			EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg01));	
			Sleep(3000);
			//myWaitForRecvKeyPress424();
			//EXPECT_EQ(1,G_TESTCB_SUCC);			
#else
			EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg02));	
			Sleep(2000);
			//myWaitForRecvKeyPress424();
			//EXPECT_EQ(1,G_TESTCB_SUCC);
#endif // _DEBUG508A1
			Sleep(1000);
		}		
		printf("SLEEP 5 SEC BEFORE PROGRAM END\n");
		//测试代码晚一点结束，以便锁具后续较慢报文能收到
		Sleep(15000);
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Close());
		LOG(INFO)<<"LISTOFUPMSG20150604"<<endl;
		string upAllMsg;
		for (auto it=g_vecTestResultXML.begin();it!=g_vecTestResultXML.end();it++)
		{			
			if ("0000"==(*it))
			{
				upAllMsg+="0";
			}
			//upAllMsg+=(*it); 
			if ("0001"==(*it))
			{
				upAllMsg+="1";
			}
			upAllMsg+=" ";
		}
		LOG(INFO)<<upAllMsg<<endl;
		//检查返回报文结果是不是0号和1号返回报文交错的正确顺序
		for (int i=0;i<g_vecTestResultXML.size()-1;i++)
		{
			//cout<<g_vecTestResultXML[i]<<"\t";
			if (g_vecTestResultXML[i]==g_vecTestResultXML[i+1])
			{
				LOG(WARNING)<<"g_vecTestResultXML ERROR on Index "<<i<<endl;
			}
		}
		cout<<endl;
	}

}	//namespace zwHidGTest20150130{


void cdecl myATMCRecvMsgRotine(const char *pszMsg)
{	
	static string lastUpMsg;
	static int nUpErrCount=0;
	static int nUpCount=0;
	//ZWFUNCTRACE 
	//assert(pszMsg != NULL && strlen(pszMsg) > 42);
	//boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	//输入必须有内容，但是最大不得长于下位机内存大小，做合理限制
	assert(NULL != pszMsg);
	int inlen = strlen(pszMsg);
	if (0==inlen)
	{
		ZWERROR("Callback Function myATMCRecvMsgRotine input is NULL")
	}
	VLOG_IF(1,inlen==0)<<"Callback Function myATMCRecvMsgRotine input is NULL";
	assert(
		//inlen > 0 && 
		inlen < JC_MSG_MAXLEN);
	if (NULL == pszMsg 
		//|| inlen == 0 
		|| inlen >= JC_MSG_MAXLEN) {
			VLOG(1)<<"Callback Function myATMCRecvMsgRotine input TOO LONG";
			return;
	}
	if (inlen>0)	
	{
		//boost::mutex::scoped_lock lock(zwccbthr::recv_mutex);
		G_TESTCB_SUCC=1;	//成功调用了回调函数
		//printf("%s\n%s\n",__FUNCTION__,pszMsg);
		VLOG_IF(4,strlen(pszMsg)>0)<<"CALLBACK512 RECV= "<<pszMsg<<endl;
		string msgType=zwGetJcxmlMsgType(pszMsg);
		//VLOG(3)<<"604LastUpMsg Error Count= "<<msgType<<endl;
		if ("1003"!=msgType)
		{
			zwHidGTest20150130::g_vecTestResultXML.push_back(msgType);
			//VLOG(3)<<lastUpMsg<<" UPMSGEQ "<<msgType<<endl;
			if (lastUpMsg==msgType)
			{
				nUpErrCount++;								
			}
			lastUpMsg=msgType;
		}
		nUpCount++;
		LOG(INFO)<<"604LastUpMsg Error Count= "<<nUpErrCount<<" nUpCount="<<nUpCount<<endl;	
		VLOG(3)<<"JcUpMsgType="<<msgType<<endl;
	}	
}


#endif // ZWUSEGTEST