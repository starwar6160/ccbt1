#include "stdafx.h"
#include <gtest/gtest.h>
#include "hidapi.h"
#include "CCBelock.h"
#include "zwHidMulHeader.h"
#include "zwHidGtest130.h"
#define ZWUSEGTEST
#define _DEBUG_LOCK2TEST
#define _DEBUG_DEV2


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
#ifndef _DEBUG
		printf("USB PlugInOut Callback 20150130.1445\n");
		printf("Json List of enum jcHidDev Type %s Serial is:\n%s\n",DrivesType,DrivesIDList);
#endif // _DEBUG
		memset(s_devList,0,G_BUFSIZE);
		strcpy(s_devList,DrivesIDList);
	}


	//�����׼���ʼ���ͽ����¼�
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
		//devSN2="JQA7AACAAoRXuwAI";	//20150209,������ˢ�����Ժ�����к�
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

	TEST_F(ATMCDLLSelfTest, jcHidDevEnumNormal)
	{
		//ö���豸�������������
		memset(s_devList,0,G_BUFSIZE);
		SetReturnDrives(myHidListTest130);
		ListDrives("Lock");
		//"{"jcElockSerial": "a"}"����������޶�json��22�ֽڳ���
		EXPECT_GT(strlen(s_devList),22);	
	}

	TEST_F(ATMCDLLSelfTest, zjydbgBad3)
	{
		//���Զ��Open��Close�豸
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
		//�������
		jch::vecJcHid.clear();
		//jch::zwCleanJchidVec();
	}

	TEST_F(ATMCDLLSelfTest, zjydbgBad1)
	{
		//����û�лص�������ʱ��
		const char *hidType="Lock";
		//EXPECT_EQ(jch::G_SUSSESS,OpenDrives(hidType,	devSN1));
		SetReturnMessage(NULL);
		EXPECT_EQ(jch::G_SUSSESS,InputMessage(hidType,devSN1,cmdBuf));
		Sleep(3000);
		//EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN1));
		Sleep(2000);
		//�������
		jch::vecJcHid.clear();
	}

	TEST_F(ATMCDLLSelfTest, zjydbgBad2)
	{
		//����û�����кŴ����ʱ���޷���HID�豸
		char devSN1p[48];
		memset(devSN1p,0,48);
		strcpy(devSN1p,devSN1);
		devSN1p[0]='Z';
		const char *hidType="Lock";

		//EXPECT_EQ(jch::G_FAIL,OpenDrives(hidType,	devSN1p));
		SetReturnMessage(myReturnMessageTest130);
		//�رղ����ڵ��豸���Գɹ�
		//EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN1p));
		Sleep(2000);
		//�������
		jch::vecJcHid.clear();
	}

#ifdef _DEBUG_DEV2

	TEST_F(ATMCDLLSelfTest, zjydbgNormal)
	{
		//����ʹ������		
		const char *hidType="Lock";
		//EXPECT_EQ(jch::G_SUSSESS,OpenDrives(hidType,	devSN1));
		SetReturnMessage(myReturnMessageTest130);
		EXPECT_EQ(jch::G_SUSSESS,InputMessage(hidType,devSN1,cmdBuf));
		Sleep(3000);
		//EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN1));
		Sleep(2000);
		//�������
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
		//�������
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
			Sleep(2000);	//�˴�����Ҫ1��-2��֮���һ�����֣������빻�ˣ��������2��Ź�
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
		Sleep(2000);	//�˴�����Ҫ�ȴ�2��
		//EXPECT_EQ(jch::G_SUSSESS,OpenDrives(hidType,	devSN2));
		SetReturnMessage(myReturnMessageTest130);
		EXPECT_EQ(jch::G_SUSSESS,InputMessage(hidType,devSN1,cmdBuf));
		EXPECT_EQ(jch::G_SUSSESS,InputMessage(hidType,devSN2,cmdBuf));
		Sleep(3000);
		//EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN1));
		//EXPECT_EQ(jch::G_SUSSESS,CloseDrives(hidType,devSN2));
		Sleep(2000);
		//�������
		jch::vecJcHid.clear();
		Sleep(1000);
	}
#endif // _DEBUG_LOCK2TEST






//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////

	TEST_F(ATMCDLLSelfTest, LockNormalUse)
	{
		//���Դ��ⲿ�ַ���ID���������豸HASH�Ƿ���ȷ
		jch::zwJcHidDbg15A hid1;
		//�˴�������Ǳ�׼���ߵ�HASH
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
		//����2��������ͬһ���豸�����
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
		//����û��ִ��OpenHidDevice�Ķ���
		jch::zwJcHidDbg15A hdv;
		hdv.SetElock(NULL);
		EXPECT_EQ(jch::G_FAIL,hdv.PushJson(cmdBuf));
		Sleep(700);
	}

	TEST_F(ATMCDLLSelfTest, LockPushNULL)
	{
		//���Է��Ϳյ������ַ����Ĳ���
		jch::zwJcHidDbg15A hdv;
		hdv.SetElock(NULL);
		hdv.OpenHidDevice();
		//����s_recvMsg���ڴ��յ��ĳ��ȵ���0����Ϊ���Ϳ������ַ���
		memset(s_recvMsg,0,G_BUFSIZE);
		EXPECT_EQ(jch::G_FAIL,hdv.PushJson(NULL));
		EXPECT_EQ(jch::G_FAIL,hdv.PushJson(""));
		Sleep(700);
		EXPECT_EQ(strlen(s_recvMsg),0);
	}

	TEST_F(ATMCDLLSelfTest, LockNotSetPara)
	{
		//����û�����ò�����ִ��OpenHidDevice�Ķ���
		jch::zwJcHidDbg15A hdv;
		hdv.OpenHidDevice();
		EXPECT_EQ(jch::G_FAIL,hdv.PushJson(cmdBuf));
		Sleep(700);
	}

	//////////////////////////////////////////////////////////////////////////


	TEST_F(ATMCDLLSelfTest, jcHidDevEnumBad1)
	{
		//ö���豸����������������
		memset(s_devList,0,G_BUFSIZE);
		SetReturnDrives(myHidListTest130);
		ListDrives("Lock1");
		//"{"jcElockSerial": "a"}"����������޶�json��22�ֽڳ���
		EXPECT_EQ(strlen(s_devList),0);	
	}

	TEST_F(ATMCDLLSelfTest, jcHidDevEnumBad2)
	{
		//ö���豸���޻ص������������
		memset(s_devList,0,G_BUFSIZE);
		SetReturnDrives(NULL);
		ListDrives("Lock");
		//"{"jcElockSerial": "a"}"����������޶�json��22�ֽڳ���
		EXPECT_EQ(strlen(s_devList),0);	
	}

	TEST_F(ATMCDLLSelfTest, jcHidDevEnumBad3)
	{
		//ö���豸������������޻ص������������
		memset(s_devList,0,G_BUFSIZE);
		SetReturnDrives(NULL);
		ListDrives("Lock1");
		//"{"jcElockSerial": "a"}"����������޶�json��22�ֽڳ���
		EXPECT_EQ(strlen(s_devList),0);	
	}
	//////////////////////////////////////////////////////////////////////////

#endif // _DEBUG_DEV2

}	//namespace zwHidGTest20150130{
#endif // ZWUSEGTEST