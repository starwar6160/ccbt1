#include "stdafx.h"
#include <gtest/gtest.h>
#include "hidapi.h"
#include "base64arduino.h"
#include "CCBelock.h"
#include "zwHidMulHeader.h"
#include "zwHidGtest130.h"
#define ZWUSEGTEST

#ifdef ZWUSEGTEST

CCBELOCK_API int zwStartGtestInDLL(void)
{
	int argc=1;
	char *argv[1];
	argv[0]=NULL;
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

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
		printf("USB PlugInOut Callback 20150130.1445\n");
		printf("Json List of enum jcHidDev Type %s Serial is:\n%s\n",DrivesType,DrivesIDList);
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

	int myHidSerialToInt(char *hidSerial)
	{
		const int BLEN=13;
		unsigned char serialBin[BLEN];
		memset(serialBin,0,BLEN);
		base64_decode((char *)serialBin,hidSerial,strlen(hidSerial));
		for (int i=0;i<BLEN-1;i++)
		{			
			if (i==1 || i==8 ||i==10)
			{
				printf(" ");
			}
			printf("%02X",serialBin[i]& 0xFF);
		}
		printf("\t");
		printf("%02X %02X %02X",serialBin[0],serialBin[8],serialBin[9]);
		printf("\n");
		//把二进制形式的字节0，9，10这3个有效字节组合为一个32bit数字的低24位，作为该序列号的实际有效成分返回
		
		uint32_t serialPayLoad=serialBin[0];
		serialPayLoad=serialPayLoad<<8;
		serialPayLoad+=serialBin[8];
		serialPayLoad=serialPayLoad<<8;
		serialPayLoad+=serialBin[9];
		return serialPayLoad;
	}

	void ATMCDLLSelfTest::SetUp()
	{
		//"jcElockSerial": "OQAiAACAAoQL1wAI",
		//"jcElockSerial": "QAAiAACAAoTXuwAI"	
		devSN1="OQAiAACAAoQL1wAI";
		devSN2="QAAiAACAAoTXuwAI";
		//devSN2="OQAiAACAAoQL1wAI";
		//devSN1="QAAiAACAAoTXuwAI";
//////////////////////////////////////////////////////////////////////////
		printf("%02u\n",myHidSerialToInt(devSN1));
		printf("%02u\n",myHidSerialToInt(devSN2));
		

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
	TEST_F(ATMCDLLSelfTest, jcHidDevEnumNormal)
	{
		//枚举设备，正常情况测试
		memset(s_devList,0,G_BUFSIZE);
		SetReturnDrives(myHidListTest130);
		ListDrives("Lock");
		//"{"jcElockSerial": "a"}"这样的最低限度json是22字节长度
		EXPECT_GT(strlen(s_devList),22);	
	}

	TEST_F(ATMCDLLSelfTest, zjydbgNormal)
	{
		//正常使用流程		
		const char *hidType="Lock";
		EXPECT_EQ(jch::G_SUSSESS,OpenDrives(hidType,	devSN1));
		SetReturnMessage(myReturnMessageTest130);
		EXPECT_EQ(jch::G_SUSSESS,InputMessage(hidType,devSN1,cmdBuf));
		Sleep(3000);
		EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN1));
		Sleep(2000);
		//清空向量
		jch::vecJcHid.clear();
	}

#ifdef _DEBUG_DEV2
	TEST_F(ATMCDLLSelfTest, zjydbgNorma2)
	{
		const char *hidType="Lock";
		EXPECT_EQ(jch::G_SUSSESS,OpenDrives(hidType,	devSN2));
		SetReturnMessage(myReturnMessageTest130);
		EXPECT_EQ(jch::G_SUSSESS,InputMessage(hidType,devSN2,cmdBuf));
		Sleep(3000);
		EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN2));
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
			dv1=	hid_open(0x0483,0x5710,sn1);
			dv2=	hid_open(0x0483,0x5710,sn2);
			EXPECT_NE(NULL,(int)dv1);
			EXPECT_NE(NULL,(int)dv2);
		}
		catch(...)
		{
			//EXPECT_NE(NULL,static_cast<void *>(dv2));
			if (NULL!=dv1)
			{
				hid_close(dv1);
			}

			if (NULL!=dv2)
			{
				hid_close(dv2);
			}
		}
		Sleep(1000);
	}

	TEST_F(ATMCDLLSelfTest, zjydbgNormaMulOpen2)
	{
		const char *hidType="Lock";
		EXPECT_EQ(jch::G_SUSSESS,OpenDrives(hidType,	devSN1));
		EXPECT_EQ(jch::G_SUSSESS,OpenDrives(hidType,	devSN2));
		SetReturnMessage(myReturnMessageTest130);
		EXPECT_EQ(jch::G_SUSSESS,InputMessage(hidType,devSN1,cmdBuf));
		EXPECT_EQ(jch::G_SUSSESS,InputMessage(hidType,devSN2,cmdBuf));
		Sleep(3000);
		EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN1));
		EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN2));
		Sleep(2000);
		//清空向量
		jch::vecJcHid.clear();
		Sleep(1000);
	}
#endif // _DEBUG_DEV2


	TEST_F(ATMCDLLSelfTest, zjydbgBad1)
	{
		//测试没有回调函数的时候
		const char *hidType="Lock";
		EXPECT_EQ(jch::G_SUSSESS,OpenDrives(hidType,	devSN1));
		SetReturnMessage(NULL);
		EXPECT_EQ(jch::G_SUSSESS,InputMessage(hidType,devSN1,cmdBuf));
		Sleep(3000);
		EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN1));
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

		EXPECT_EQ(jch::G_FAIL,OpenDrives(hidType,	devSN1p));
		SetReturnMessage(myReturnMessageTest130);
		EXPECT_EQ(jch::G_FAIL,CloseDrives(hidType,devSN1p));
		Sleep(2000);
		//清空向量
		jch::vecJcHid.clear();
	}


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
		//清理s_recvMsg，期待收到的长度大于0
		memset(s_recvMsg,0,G_BUFSIZE);
		hid1.PushJson(cmdBuf);
		Sleep(2000);
		EXPECT_GT(strlen(s_recvMsg),0);
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


}	//namespace zwHidGTest20150130{
#endif // ZWUSEGTEST