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
		//���к����ΪNULL��ʹ�����кŴ��豸
		int OpenElock(const char *ElockSerial);
		int OpenSecBox(const char *SecBoxSerial);
		uint32_t PushJson(const char *strJsonCmd);		
		uint32_t GetHash(void);
	private:
		JCHID m_dev;
		uint32_t m_hashId;	//���豸PID�ַ���("Lock"��)�����к�HASH������ID
		boost::thread *thr;
		boost::mutex jcSend_mutex;	//�����޶���Ҫ���豸���������ݽ����̣߳�Ȼ����ܷ�������
		int RecvThread(JCHID *hidHandle);
		void StartRecvThread();
		void StopRecvThread();
		void OpenHidDevice();
	};

}	//namespace jcLockJsonCmd_t2015a21{


#ifdef ZWUSEGTEST
//�����׼���ʼ���ͽ����¼�
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