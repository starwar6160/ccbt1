#include "stdafx.h"
#include "hidapi.h"
#include "zwHidComm.h"
#include "zwCcbElockHdr.h"
#include "CCBelock.h"

namespace jcLockJsonCmd_t2015a21{
	const int G_SUSSESS=0;
	const int G_FAIL=1;
	const int G_NO_CALLBACK=2;
	const char * G_DEV_LOCK="Lock";
	const char * G_DEV_SECBOX="Encryption";
	ReturnDrives G_JCHID_ENUM_DEV2015A=NULL;
	ReturnMessage G_JCHID_RECVMSG_CB=NULL;



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
	if (JCHID_STATUS_OK==sts)
	{
		LOG(WARNING)<<" Open jcHid Device "<<m_dev.hid_device<<" SUCCESS"<<endl;
	}
	else
	{
		LOG(ERROR)<<" Open jcHid Device Error"<<endl;
	}
	
	
}

zwJcHidDbg15A::~zwJcHidDbg15A()
{
	VLOG(4)<<__FUNCTION__<<endl;
	if (NULL!=m_dev.hid_device)
	{
		jcHidClose(&m_dev);
		LOG(WARNING)<<"Close jcHid Device "<<m_dev.hid_device<<" SUCCESS"<<endl;
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

//////////////////////////////////////////////////////////////////////////

zwJcHidDbg15A *s_jcHidDev=NULL;
}	//end of namespace jcLockJsonCmd_t2015a21{

using jcLockJsonCmd_t2015a21::zwJcHidDbg15A;
using jcLockJsonCmd_t2015a21::s_jcHidDev;


CCBELOCK_API void ZJY1501STD zwTest121a1(void)
{
	
	for (int i=0;i<3;i++)
	{
		zwJcHidDbg15A lock1;
		cout<<"DevPtr="<<lock1.getDevPtr()<<endl;
		//Sleep(5000);
	}
}

CCBELOCK_API int ZJY1501STD OpenDrives( const char* DrivesTypePID,const char * DrivesIdSN )
{
	s_jcHidDev=new zwJcHidDbg15A();
	return jcLockJsonCmd_t2015a21::G_SUSSESS;
}

CCBELOCK_API int ZJY1501STD CloseDrives( const char* DrivesTypePID,const char * DrivesIdSN )
{
	if (NULL!=s_jcHidDev)
	{
		delete s_jcHidDev;
	}
	return jcLockJsonCmd_t2015a21::G_SUSSESS;
}