#include "CCBelock.h"
#include "zwHidMulHeader.h"
#include "zwHidComm.h"

namespace jcLockJsonCmd_t2015a21{
	class zwJcHidDbg15A
	{
	public:
		zwJcHidDbg15A();			
		//序列号如果为NULL则不使用序列号打开设备
		int SetElock(const char *ElockSerial);
		int SetSecBox(const char *SecBoxSerial);
		//先设置参数，然后可以获得HASH值
		uint32_t GetHash(void);
		//然后打开设备
		int OpenHidDevice();
		//然后发送命令
		uint32_t PushJson(const char *strJsonCmd);				
		//设备由构造函数自动关闭
		~zwJcHidDbg15A();		
	private:
		JCHID m_dev;
		uint32_t m_hashId;	//由设备PID字符串("Lock"等)和序列号HASH出来的ID
		boost::thread *thr;
		boost::mutex jcSend_mutex;	//用来限定先要打开设备，启动数据接收线程，然后才能发送数据
		int RecvThread(JCHID *hidHandle);
		void StartRecvThread();
		void StopRecvThread();		
	};

	extern vector<zwJcHidDbg15A *> vecJcHid;
	void CharToTchar (const char * _char, TCHAR * tchar);
	uint32_t myHidSerialToInt(char *hidSerial);
}	//namespace jcLockJsonCmd_t2015a21{

