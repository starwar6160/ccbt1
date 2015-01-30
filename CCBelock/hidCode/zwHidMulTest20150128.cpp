#include "stdafx.h"
#include <gtest/gtest.h>
#define ZWUSEGTEST
#include "CCBelock.h"
#include "zwHidMulHeader.h"
#include "zwHidComm.h"

namespace jcLockJsonCmd_t2015a21{
	class zwJcHidDbg15A
	{
	public:
		zwJcHidDbg15A();	
		~zwJcHidDbg15A();
		//序列号如果为NULL则不使用序列号打开设备
		int OpenElock(const char *ElockSerial);
		int OpenSecBox(const char *SecBoxSerial);
		uint32_t PushJson(const char *strJsonCmd);		
		uint32_t GetHash(void);
	private:
		JCHID m_dev;
		uint32_t m_hashId;	//由设备PID字符串("Lock"等)和序列号HASH出来的ID
		boost::thread *thr;
		boost::mutex jcSend_mutex;	//用来限定先要打开设备，启动数据接收线程，然后才能发送数据
		int RecvThread(JCHID *hidHandle);
		void StartRecvThread();
		void StopRecvThread();
		void OpenHidDevice();
	};

}	//namespace jcLockJsonCmd_t2015a21{


#ifdef ZWUSEGTEST
//测试套件初始化和结束事件
class ATMCDLLSelfTest : public testing::Test
{
public:
	

protected:
	virtual void SetUp();
	virtual void TearDown();
};

void ATMCDLLSelfTest::SetUp()
{


}

void ATMCDLLSelfTest::TearDown()
{

}

CCBELOCK_API int zwStartGtestInDLL(void)
{
	int argc=1;
	char *argv[1];
	argv[0]=NULL;
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST_F(ATMCDLLSelfTest, LockActiveTest0000)
{
	int age=1;
	EXPECT_EQ(1,age);
	jch::zwJcHidDbg15A hid1;
	hid1.OpenElock(NULL);
	EXPECT_GT(hid1.GetHash(),0);
}
#endif // ZWUSEGTEST