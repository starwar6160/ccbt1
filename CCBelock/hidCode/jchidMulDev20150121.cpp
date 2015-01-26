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
	uint32_t Push2jcHidDev(const char *strJsonCmd);
	int RecvFromLockJsonThr(JCHID *hidHandle);
private:
	JCHID m_dev;
	boost::thread *thr;
	boost::mutex jcSend_mutex;	//�����޶���Ҫ���豸���������ݽ����̣߳�Ȼ����ܷ�������
};

zwJcHidDbg15A::zwJcHidDbg15A()
{
	jcSend_mutex.lock();
	VLOG(4)<<__FUNCTION__<<"\njcDevInit_mutex.lock();"<<endl;
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
	//����һ���������󣬼������ڲ���ǰ���Ǻ�������ֵ�������ڲ��Ǻ�����һ�����߶������(�β�)�������Ƕ��ŷָ���
	//������boost::bind�������¸�ʽ�Ѻ���ָ��ͺ���_1��ʽ��һ�����߶������(�β�)�󶨳�Ϊһ����������
	boost::function<int (JCHID *)> memberFunctionWrapper(boost::bind(&zwJcHidDbg15A::RecvFromLockJsonThr, this,_1));  	
	//�ٴ�ʹ��boost::bind�Ѻ���������ʵ�ΰ󶨵�һ�𣬾Ϳ��Դ��ݸ�boost::thread��Ϊ�߳��庯����
	thr=new boost::thread(boost::bind(memberFunctionWrapper,&m_dev));	
	Sleep(5);	//�ȴ��߳�������ϣ���ʵҲ��2����һ��������ˣ�

}

zwJcHidDbg15A::~zwJcHidDbg15A()
{
	assert(NULL!=m_dev.hid_device);	
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


//////////////////////////////////////////////////////////////////////////

zwJcHidDbg15A *s_jcHidDev=NULL;

uint32_t zwJcHidDbg15A::Push2jcHidDev(const char *strJsonCmd)
{
	assert(NULL!=m_dev.hid_device);
	assert(NULL != strJsonCmd && strlen(strJsonCmd) > 0);
	if (NULL == strJsonCmd || strlen(strJsonCmd) == 0
		|| NULL==m_dev.hid_device
		) {
			LOG(ERROR)<<__FUNCTION__<<"input Data strJsonCmd or m_dev.hid_device error!\n";
			return G_FAIL;
	}
	static int jcPushCountTmp123=0;
	LOG(WARNING)<<"���·� "<<" Json On jcHidDev "<<
		m_dev.hid_device <<" Push Count "<<jcPushCountTmp123++<<endl<<strJsonCmd<<endl;
		JCHID_STATUS sts=JCHID_STATUS_FAIL;
	try {	
		int count=0;
		do 
		{
				boost::mutex::scoped_lock lock(jcSend_mutex);
				//VLOG(4)<<__FUNCTION__<<"\nSendData"<<endl;
				sts=jcHidSendData(&m_dev, strJsonCmd, strlen(strJsonCmd));
			if (JCHID_STATUS_OK==sts)
			{
				//VLOG(4)<<__FUNCTION__<<" Send Data Success\n";
				break;
			}
			else
			{
				LOG(WARNING)<<__FUNCTION__<<" Push Data to jcHid Device "<<m_dev.hid_device<<" Fail"<<endl;
				Sleep(1000);
				count++;
			}
			if (count>3)
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


int zwJcHidDbg15A::RecvFromLockJsonThr(JCHID *hidHandle)
{				 
	assert(NULL!=hidHandle);
	if (NULL==hidHandle)
	{
		LOG(WARNING)<<__FUNCTION__<<"\tInput JCHID * is NULL!"<<endl;
		return G_FAIL;
	}
	LOG(WARNING)<<__FUNCTION__<<"\n JcHidZJYDbg Thread Started"<<endl;	
		const int BLEN = 1024;
		char recvBuf[BLEN + 1];
		memset(recvBuf, 0, BLEN + 1);		
#ifdef _DEBUG1
		int t_thr_runCount=1;
#endif // _DEBUG
		jcSend_mutex.unlock();//����֮����Կ�ʼ����������
		VLOG(4)<<__FUNCTION__<<"\njcDevInit_mutex.unlock();"<<endl;
		while (1) {	
#ifdef _DEBUG1
			LOG(WARNING)<<"RECV THR 20150122 "<<t_thr_runCount++<<endl;
#endif // _DEBUG
try{
			/** �ֶ����߳��м����жϵ㣬�жϵ㲻Ӱ���������ִ�� */  
			boost::this_thread::interruption_point();  
					if (NULL==hidHandle->hid_device)
					{
						continue;
					}
					//LOG(WARNING)<<"RECVTHR.P3.1,Before RecvHidData"<<endl;
					JCHID_STATUS sts=JCHID_STATUS_FAIL;
					//ע��˴������ÿ��ѭ�����㣬��ᵼ�³���ֵ������Լ4K�ֽ�ʱ������CRC����������ȡ
					//�յ�����������ʱ���������ݿ���ֻ�м�ʮ�ֽڣ����Ƕ�ȡ����Խ�磬���������
					int recvLen = 0;	
					sts=jcHidRecvData(hidHandle,recvBuf, BLEN, &recvLen,G_RECV_TIMEOUT);				
					//Ҫ��ĳ���豸ʲô����Ҳû�յ�����ֱ�ӽ�����һ���豸
					if (JCHID_STATUS_OK!=sts &&JCHID_STATUS_RECV_ZEROBYTES!=sts)
					{					
						LOG(INFO)<<"NoData from "<<hidHandle->HidSerial<<endl;
						continue;
					}					
					if (JCHID_STATUS_OK==sts && recvLen>0 )
					{																								
						//������յ������ݵ�CRC
						uint32_t recvDataNowSum=Crc32_ComputeBuf(0,recvBuf,recvLen);
							VLOG_IF(4,recvDataNowSum>0)<<" recvDataNowSum="<<recvDataNowSum<<endl;
							int tRecvLen=strlen(recvBuf);
							static int tmpRecvCount=0;
							LOG_IF(WARNING,tRecvLen>1)<<"MulHid�ɹ������� "<<hidHandle->HidSerial<<
								" ����JSON�������£�Count "<<tmpRecvCount++<<endl<<recvBuf<<endl;
							LOG_IF(ERROR,NULL==G_JCHID_RECVMSG_CB)<<"G_JCHID_RECVMSG_CB==NULL"<<endl;
							if (NULL!=G_JCHID_RECVMSG_CB)
							{								
								G_JCHID_RECVMSG_CB(hidHandle->HidSerial,recvBuf);
								VLOG(2)<<"JinChu RecvMsg Callback Function be Call Success"<<endl;
							}						
					}						
}
catch(boost::thread_interrupted)
{
	LOG(WARNING)<<"��HID���ݽ����̱߳�������������ֹ"<<endl;
	return G_SUSSESS;
}
//////////////////////TRY-CATCH���////////////////////////////////////////////////////
		}	//while (1) {
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
	assert(NULL!=DrivesTypePID);
	s_jcHidDev=new zwJcHidDbg15A();
	return G_SUSSESS;
}

CCBELOCK_API int ZJY1501STD CloseDrives( const char* DrivesTypePID,const char * DrivesIdSN )
{
	assert(NULL!=DrivesTypePID);
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
	assert(NULL!=_MessageHandleFun);
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
	assert(NULL!=DrivesTypePID);
	assert(NULL!=AnyMessageJson && strlen(AnyMessageJson)>0);
	//LOG(INFO)<<"DrivesTypePID:"<<DrivesTypePID<<" AnyMessageJson:"<<endl<<AnyMessageJson<<endl;
	//LOG_IF(INFO,NULL!=DrivesIdSN &&strlen(DrivesIdSN)>0)<<"DrivesIdSN:"<<DrivesIdSN<<endl;
	//if (NULL != AnyMessageJson && strlen(AnyMessageJson) > 0) {
	//	LOG(WARNING)<<__FUNCTION__<< " JinChu�·�JSON=" << endl << AnyMessageJson <<endl;
	//}

	try {
		s_jcHidDev->Push2jcHidDev(AnyMessageJson);
		return G_SUSSESS;
	}
	catch(...) {
		LOG(ERROR)<<__FUNCTION__<<" NotifyJsonͨ����·���������쳣�������������"<<endl;
		return G_FAIL;
	}
}

//��TCHARתΪchar   
//*tchar��TCHAR����ָ�룬*_char��char����ָ��   
static void TcharToChar (const TCHAR * tchar, char * _char)  
{  
    int iLength ;  
//��ȡ�ֽڳ���   
iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);  
//��tcharֵ����_char    
WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);   
}  

void zwGetHidDevSerial();
void zwGetHidDevSerialTest126();

CCBELOCK_API void myHidSerialTest126(void)
{
	VLOG(4)<<__FUNCTION__<<endl;
	JCHID hnd;
	memset(&hnd,0,sizeof(hnd));
	hnd.vid=0x0483;
	hnd.pid=0x5710;
	//strcpy(hnd.HidSerial,"OQAiAAAAAAAAgAKE");
	strcpy(hnd.HidSerial,"OQAiAACAAoQL1wAI");
	std::string lockSerialList;
//////////////////////////////////////////////////////////////////////////
	//zwGetHidDevSerial();
	jcLockJsonCmd_t2015a::jcMulHidEnum(0x5710,lockSerialList);	
//////////////////////////////////////////////////////////////////////////
	printf(lockSerialList.c_str());
	//Sleep(1000);
	//zwGetHidDevSerialTest126();

	JCHID_STATUS sts=jcHidOpen(&hnd);
	if (JCHID_STATUS_OK==sts)
	{
		printf("Open HID OK\n");
	}
	if (JCHID_STATUS_FAIL==sts)
	{
		printf("Open HID FAIL\n");
	}
	jcHidClose(&hnd);
}

void zwGetHidDevSerial(char *jcHidSerial)
{
	struct hid_device_info *devs, *cur_dev;
	hid_device *handle = NULL;
	devs = hid_enumerate(0x0483, 0x5710);
	cur_dev = devs;
	while (cur_dev) {
		if (cur_dev->vendor_id == 0x0483 &&
			cur_dev->product_id == 0x5710) {
				TcharToChar(cur_dev->serial_number,jcHidSerial);
		}
		cur_dev = cur_dev->next;
	}
}

void zwGetHidDevSerialTest126()
{
	struct hid_device_info *devs, *cur_dev;
	hid_device *handle = NULL;
	devs = hid_enumerate(0x0483, 0x5710);
	cur_dev = devs;
	while (cur_dev) {
		if (cur_dev->vendor_id == 0x0483 &&
			cur_dev->product_id == 0x5710) {
				char snc[24];
				memset(snc,0,24);
				TcharToChar(cur_dev->serial_number,snc);
				printf("%s\n",snc);
		}
		cur_dev = cur_dev->next;
	}
	Sleep(1000);
}

