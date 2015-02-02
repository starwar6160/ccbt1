#include "CCBelock.h"
#include "zwHidMulHeader.h"
#include "zwHidComm.h"

namespace jcLockJsonCmd_t2015a21{
	class zwJcHidDbg15A
	{
	public:
		zwJcHidDbg15A();			
		//���к����ΪNULL��ʹ�����кŴ��豸
		int SetElock(const char *ElockSerial);
		int SetSecBox(const char *SecBoxSerial);
		//�����ò�����Ȼ����Ի��HASHֵ
		uint32_t GetHash(void);
		//Ȼ����豸
		int OpenHidDevice();
		//Ȼ��������
		uint32_t PushJson(const char *strJsonCmd);				
		//�豸�ɹ��캯���Զ��ر�
		~zwJcHidDbg15A();
	private:
		JCHID m_dev;
		uint32_t m_hashId;	//���豸PID�ַ���("Lock"��)�����к�HASH������ID
		boost::thread *thr;
		boost::mutex jcSend_mutex;	//�����޶���Ҫ���豸���������ݽ����̣߳�Ȼ����ܷ�������
		int RecvThread(JCHID *hidHandle);
		void StartRecvThread();
		void StopRecvThread();		
	};

	extern vector<zwJcHidDbg15A *> vecJcHid;
	void CharToTchar (const char * _char, TCHAR * tchar);
}	//namespace jcLockJsonCmd_t2015a21{

