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
		int SetElock(const char *ElockSerial);
		int SetSecBox(const char *SecBoxSerial);
		uint32_t PushJson(const char *strJsonCmd);		
		uint32_t GetHash(void);
		void OpenHidDevice();
	private:
		JCHID m_dev;
		uint32_t m_hashId;	//���豸PID�ַ���("Lock"��)�����к�HASH������ID
		boost::thread *thr;
		boost::mutex jcSend_mutex;	//�����޶���Ҫ���豸���������ݽ����̣߳�Ȼ����ܷ�������
		int RecvThread(JCHID *hidHandle);
		void StartRecvThread();
		void StopRecvThread();		
	};

}	//namespace jcLockJsonCmd_t2015a21{

