#include "stdafx.h"
#include "hidapi.h"
#include "zwHidComm.h"
#include "zwCcbElockHdr.h"
#include "CCBelock.h"

namespace jcLockJsonCmd_t2015a{
class zwJcHidDbg15A
{
public:
	zwJcHidDbg15A();
	~zwJcHidDbg15A();
	uint32_t getDevPtr();
private:
	JCHID m_dev;
};

zwJcHidDbg15A::zwJcHidDbg15A()
{
	VLOG(4)<<__FUNCTION__<<endl;
	memset(&m_dev,0,sizeof(m_dev));
	m_dev.vid=0x0483;
	m_dev.pid=0x5710;
	JCHID_STATUS sts= jcHidOpen(&m_dev);
	VLOG_IF(2,JCHID_STATUS_OK==sts)<<" Open jcHid Device "<<m_dev.hid_device<<" SUCCESS"<<endl;
	LOG_IF(ERROR,JCHID_STATUS_OK!=sts)<<" Open jcHid Device Error"<<endl;
}

zwJcHidDbg15A::~zwJcHidDbg15A()
{
	VLOG(4)<<__FUNCTION__<<endl;
	if (NULL!=m_dev.hid_device)
	{
		jcHidClose(&m_dev);
		VLOG(2)<<"Close jcHid Device "<<m_dev.hid_device<<" SUCCESS"<<endl;
	}
	else
	{
		LOG(WARNING)<<"try to Close NULL jchid Device Handle"<<endl;
	}
	
	
};

uint32_t zwJcHidDbg15A::getDevPtr()
{
	return reinterpret_cast<uint32_t>(m_dev.hid_device);
}

}	//end of namespace jcLockJsonCmd_t2015a{



CCBELOCK_API void ZJY1501STD zwTest121a1(void)
{
	using jcLockJsonCmd_t2015a::zwJcHidDbg15A;
	for (int i=0;i<3;i++)
	{
		zwJcHidDbg15A lock1;
		cout<<"DevPtr="<<lock1.getDevPtr()<<endl;
		//Sleep(5000);
	}
}