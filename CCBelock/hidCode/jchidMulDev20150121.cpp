#include "stdafx.h"
#include "hidapi.h"
#include "zwHidComm.h"
#include "zwCcbElockHdr.h"
#include "CCBelock.h"

namespace jcLockJsonCmd_t2015a21{
	const int G_RECV_TIMEOUT=700;
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
	uint32_t Push2jcHidDev(const char *strJsonCmd);
	uint32_t RecvJson(string &recvJson);
	const char * RecvFromLockJsonThr(void);
private:
	JCHID m_dev;
	boost::thread *thr;
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

	boost::function<const char * (void)> memberFunctionWrapper(boost::bind(&zwJcHidDbg15A::RecvFromLockJsonThr, this));  	

	thr=new boost::thread(memberFunctionWrapper);	
}

zwJcHidDbg15A::~zwJcHidDbg15A()
{
	VLOG(4)<<__FUNCTION__<<endl;
	if (NULL!=m_dev.hid_device)
	{
		thr->interrupt();
		thr->join();

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

uint32_t zwJcHidDbg15A::Push2jcHidDev(const char *strJsonCmd)
{
	assert(NULL!=m_dev.hid_device);
	assert(NULL != strJsonCmd && strlen(strJsonCmd) > 0);
	if (NULL == strJsonCmd || strlen(strJsonCmd) == 0
		|| NULL==m_dev.hid_device
		) {
			LOG(ERROR)<<__FUNCTION__<<"input Data error!\n";
			return G_FAIL;
	}
	LOG(INFO)<<__FUNCTION__<<"jcHidDev "<<
		m_dev.HidSerial<<" Push "<<strJsonCmd<<endl;
		JCHID_STATUS sts=JCHID_STATUS_FAIL;
	try {
	
		int count=0;
		do 
		{
			sts=jcHidSendData(&m_dev, strJsonCmd, strlen(strJsonCmd));
			if (JCHID_STATUS_OK==sts)
			{
				VLOG(4)<<__FUNCTION__<<" Send Data Success\n";
				break;
			}
			else
			{
				LOG(WARNING)<<__FUNCTION__<<" Push Data 2 jcHid Device Fail"<<endl;
				Sleep(1000);
				count++;
			}
			if (count>10)
			{
				break;
			}
		} while (sts!=JCHID_STATUS_OK);
	}			//try
	catch(...) {
		LOG(ERROR)<<__FUNCTION__<<" ͨ����·�������ݵ������쳣��������δ����\n";
	}
	return sts;
}

uint32_t zwJcHidDbg15A::RecvJson(string &recvJson)
{
	const int BLEN = 1024;
	char recvBuf[BLEN + 1];
	memset(recvBuf, 0, BLEN + 1);
	int recvLen = 0;
	Sleep(500);
	JCHID_STATUS sts=
		jcHidRecvData(&m_dev,
		recvBuf, BLEN, &recvLen,G_RECV_TIMEOUT);				
	if (JCHID_STATUS_OK==sts)
	{
		recvJson=recvBuf;
	}
	else
	{
		recvJson="NoData Recv 20150121";
	}
	return G_SUSSESS;
}


const char * zwJcHidDbg15A::RecvFromLockJsonThr(void)
{				 
	LOG(WARNING)<<__FUNCTION__<<" Started"<<endl;	
		const int BLEN = 1024;
		char recvBuf[BLEN + 1];
		memset(recvBuf, 0, BLEN + 1);
		int recvLen = 0;

		uint32_t recvDataSum=0;
		int t_thr_runCount=1;
		while (1) {	
			//LOG(WARNING)<<"RECV THR "<<t_thr_runCount++<<endl;
			/** �ֶ����߳��м����жϵ㣬�жϵ㲻Ӱ���������ִ�� */  
			boost::this_thread::interruption_point();  
					if (NULL==m_dev.hid_device)
					{
						continue;
					}
					//LOG(WARNING)<<"RECVTHR.P3.1,Before RecvHidData"<<endl;
					JCHID_STATUS sts=
						jcHidRecvData(&m_dev,
						recvBuf, BLEN, &recvLen,G_RECV_TIMEOUT);				
					//LOG(WARNING)<<"RECVTHR.P3.2,After RecvHidData"<<endl;
					//Ҫ��ĳ���豸ʲô����Ҳû�յ�����ֱ�ӽ�����һ���豸
					if (JCHID_STATUS_OK!=sts &&JCHID_STATUS_RECV_ZEROBYTES!=sts)
					{					
						LOG(WARNING)<<"NoData from "<<m_dev.HidSerial<<endl;
						continue;
					}					
					if (recvLen>0 )
					{																		
						//uint32_t recvDataNowSum= recvDataSum=Crc32_ComputeBuf(0,recvBuf,recvLen);
						uint32_t recvDataNowSum=Crc32_ComputeBuf(0,recvBuf,recvLen);
						if (recvDataNowSum!=recvDataSum)
						{
							VLOG(4)<<"recvDataSum="<<recvDataSum<<" recvDataNowSum="<<recvDataNowSum<<endl;
							int tRecvLen=strlen(recvBuf);
							LOG_IF(INFO,tRecvLen>9)<<"�ɹ�������"<<m_dev.HidSerial<<"����JSON�������£�"<<endl;
							LOG_IF(WARNING,tRecvLen>9)<<endl<<recvBuf<<endl;
							LOG_IF(ERROR,NULL==G_JCHID_RECVMSG_CB)<<"G_JCHID_RECVMSG_CB==NULL"<<endl;
							if (NULL!=G_JCHID_RECVMSG_CB)
							{
								G_JCHID_RECVMSG_CB(m_dev.HidSerial,recvBuf);
							}						
							recvDataSum=recvDataNowSum;
						}

					}						

		}	//while (1) {
		LOG(INFO)<<"��ͨ�����ݽ���JSON������������";
}


}	//end of namespace jcLockJsonCmd_t2015a21{

namespace jch=jcLockJsonCmd_t2015a21;

using jch::zwJcHidDbg15A;
using jch::s_jcHidDev;
using jch::G_JCHID_RECVMSG_CB;
using jch::G_FAIL;
using jch::G_SUSSESS;

CCBELOCK_API int ZJY1501STD OpenDrives( const char* DrivesTypePID,const char * DrivesIdSN )
{
	s_jcHidDev=new zwJcHidDbg15A();
	return G_SUSSESS;
}

CCBELOCK_API int ZJY1501STD CloseDrives( const char* DrivesTypePID,const char * DrivesIdSN )
{
	if (NULL!=s_jcHidDev)
	{
		//memset(s_jcHidDev,0,sizeof(s_jcHidDev));
		delete s_jcHidDev;
	}
	return G_SUSSESS;
}

//2�������豸��Ϣ���صĻص�����
CCBELOCK_API void ZJY1501STD SetReturnMessage( ReturnMessage _MessageHandleFun )
{
	if (NULL!=_MessageHandleFun)
	{
		LOG_IF(ERROR,NULL==_MessageHandleFun)<<"G_JCHID_RECVMSG_CB==NULL"<<endl;
		jch::G_JCHID_RECVMSG_CB=_MessageHandleFun;
		VLOG(3)<<"SetReturnMessage set Callback Success\n";
	}	
	else
	{
		LOG(WARNING)<<"SetReturnMessage using NULL CallBack Function Pointer !\n";
	}
}
//3�����豸����ָ��ĺ���
CCBELOCK_API int ZJY1501STD InputMessage( const char * DrivesTypePID,const char * DrivesIdSN,const char * AnyMessageJson )
{
	LOG(INFO)<<"DrivesTypePID"<<DrivesTypePID<<"AnyMessageJson"<<AnyMessageJson<<endl;
	LOG_IF(INFO,NULL!=DrivesIdSN &&strlen(DrivesIdSN)>0)<<"DrivesIdSN"<<DrivesIdSN<<endl;
	if (NULL != AnyMessageJson && strlen(AnyMessageJson) > 0) {
		LOG(WARNING)<< "JinChu�·�JSON=" << endl << AnyMessageJson <<
			endl;
	}

	try {
		s_jcHidDev->Push2jcHidDev(AnyMessageJson);
		return G_SUSSESS;
	}
	catch(...) {		//һ�������쳣��ֱ�ӷ��ش�����Ҫ��Ϊ�˲�׽δ����ʱ
		//���ڶ���Ϊ����ɷ���NULLָ��ĵ�SEH�쳣  
		//Ϊ��ʹ�õײ�Poco����cceblock�������ҵ�WS��
		//����WS������Ϊδ���Ӷ���NULLʱֱ��throwһ��ö��
		//Ȼ���ڴˣ�Ҳ�����ϲ㲶����ʱ��֪������ȷ����
		//����catch�����쳣��
		LOG(FATAL)<<__FUNCTION__<<" NotifyJsonͨ����·���������쳣�������������"<<endl;
		return G_FAIL;
	}

	return G_SUSSESS;
}

