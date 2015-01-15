#include "stdafx.h"
#include <windows.h>
#include "hidapi.h"
#include "zwHidComm.h"
#include "zwCcbElockHdr.h"
#include "CCBelock.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <map>

#define MY114FUNCTRACK	VLOG(4)<<__FUNCTION__<<endl;

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

//ͬ��   
static void CharToTchar (const char * _char, TCHAR * tchar)  
{  
    int iLength ;  
  
    iLength = MultiByteToWideChar (CP_ACP, 0, _char, strlen (_char) + 1, NULL, 0) ;  
    MultiByteToWideChar (CP_ACP, 0, _char, strlen (_char) + 1, tchar, iLength) ;  
}  



//////////////////////////////////////////////////////////////////////////

namespace jcLockJsonCmd_t2015a{
	const int G_SUSSESS=0;
	const int G_FAIL=1;
	const int G_NO_CALLBACK=2;
	const char * G_DEV_LOCK="Lock";
	const char * G_DEV_SECBOX="Encryption";
	ReturnDrives G_JCHID_ENUM_DEV2015A=NULL;
	ReturnMessage G_JCHID_RECVMSG_CB=NULL;
	std::map<uint32_t,JCHID> G_JCDEV_MAP;
	bool s_hidJsonRecvThrRunning=false;

	void jcMulHidEnum( const int hidPid,string &jcDevListJson )
	{
		ptree pt;
		hid_device_info *jclock_List= hid_enumerate(JCHID_VID_2014,hidPid);
		while (NULL!=jclock_List)
		{
			char serial[32];
			memset(serial,0,32);
			TcharToChar(jclock_List->serial_number,serial);
			VLOG(2)<<"vid="<<jclock_List->vendor_id<<" pid="<<jclock_List->product_id<<" serial="<<serial<<endl;

			//ע�⣬�˴�put�Ļ���ÿ�ζ������add�Ļ�������������Ŀ
			if (JCHID_PID_LOCK5151==hidPid)
			{
				pt.add("jcElockSerial",serial);
			}
			if (JCHID_PID_SECBOX==hidPid)
			{
				pt.add("jcSecretBoxSerial",serial);
			}
			
			jclock_List=jclock_List->next;
		}
		std::ostringstream ss;
		write_json(ss, pt);
		jcDevListJson=ss.str();
	}

	uint32_t myJcHidHndFromStrSerial( const char* DrivesTypePID, const char * DrivesIdSN)
	{
		assert(NULL!=DrivesTypePID && NULL!=DrivesIdSN);
		assert(strlen(DrivesTypePID)>0 && strlen(DrivesIdSN)>0);
		assert(strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_LOCK)==0 
			|| strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_SECBOX)==0);
		uint32_t inDevId;
		string hidPidAndSerial=DrivesTypePID;
		hidPidAndSerial+=".";
		hidPidAndSerial+=DrivesIdSN;
		inDevId=crc32Short(hidPidAndSerial.c_str(),hidPidAndSerial.length());
		return inDevId;
	}


	void isJcHidDevOpend(const char* DrivesTypePID,const char * DrivesIdSN,uint32_t *inDevHashId,JCHID **jcHidDev)
	{
		uint32_t inDevId=myJcHidHndFromStrSerial(DrivesTypePID, DrivesIdSN);
		VLOG(4)<<"jcHidHandleFromStrSerial="<<inDevId<<endl;	
		*inDevHashId=inDevId;
		std::map<uint32_t,JCHID>::iterator it=G_JCDEV_MAP.find(inDevId);
		if (it==jcLockJsonCmd_t2015a::G_JCDEV_MAP.end())
		{
			LOG(WARNING)<<"jcLockJsonCmd_t2015a::G_JCDEV_MAP find item hashId="
				<<inDevId<<" "<<DrivesTypePID<<":"<<DrivesIdSN<<" not Open now!\n";
			*jcHidDev=NULL;
			return;
		}
		else
		{
			VLOG(4)<<"jcLockJsonCmd_t2015a::G_JCDEV_MAP find item "<<inDevId<<" "<<DrivesTypePID<<":"<<DrivesIdSN<<" SUCCESS!\n";
			*jcHidDev=&it->second;
		}

	}

	void zwPushString1501(const JCHID *hidHandle,const char *str)
	{
		assert(NULL!=hidHandle && NULL!=hidHandle->hid_device);
		assert(NULL != str && strlen(str) > 0);
		if (NULL == str || strlen(str) == 0
			|| NULL==hidHandle || NULL==hidHandle->hid_device
			) {
				LOG(ERROR)<<__FUNCTION__<<"input Data error!\n";
			return;
		}
		try {
			JCHID_STATUS sts=JCHID_STATUS_FAIL;
			int count=0;
			do 
			{
				sts=jcHidSendData(hidHandle, str, strlen(str));
				if (JCHID_STATUS_OK==sts)
				{
					VLOG(4)<<__FUNCTION__<<" Send Data Success\n";
					break;
				}
				Sleep(1000);
				count++;
				if (count>10)
				{
					break;
				}
			} while (sts!=JCHID_STATUS_OK);
		}			//try
		catch(...) {
			LOG(ERROR)<<__FUNCTION__<<" ͨ����·�������ݵ������쳣��������δ����\n";
		}

	}

//#ifdef _DEBUG_114A
	//��void ThreadLockComm() �޸Ķ���
	//����ѭ���������߷���ֵ��ͨ���ص����������յ���JSON����
	const char * RecvFromLockJsonThr(void)
	{				 
		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];
			memset(recvBuf, 0, BLEN + 1);
			int recvLen = 0;

			std::map<uint32_t,JCHID>::iterator iter;
			assert(G_JCDEV_MAP.size()>0);		
			VLOG(4)<<"G_JCDEV_MAP.size()="<<G_JCDEV_MAP.size()<<endl;
			while (1) {			
			try {
				s_hidJsonRecvThrRunning=true;	//����ͨ���̵߳�һ��������־					
				if (G_JCDEV_MAP.size()==0)
				{
					return "NoOpend HidDevice";
				}
				for (iter=G_JCDEV_MAP.begin();iter!=G_JCDEV_MAP.end();iter++)
				{
					assert(NULL!=iter->second.hid_device);				
					//VLOG(4)<<"hid_device="<<iter->second.hid_device<<endl;
					//����Ŀǰ�ľ��飬������Ҫ300-400������ܷ������ݣ����Գ�ʱ����Ϊ500
					JCHID_STATUS sts=
						jcHidRecvData(&iter->second,
						recvBuf, BLEN, &recvLen,500);				
					//Ҫ��ĳ���豸ʲô����Ҳû�յ�����ֱ�ӽ�����һ���豸
					if (JCHID_STATUS_OK!=sts)
					{					
						//LOG(WARNING)<<"JCHID_STATUS No DATA RECVED Status Code "<<sts;
						//return "JCHID_STATUS No DATA RECVED";						
						continue;
					}
					printf("\n");
					LOG(INFO)<<"�ɹ�������"<<iter->second.HidSerial<<"����JSON�������£�"<<endl;
					LOG(INFO)<<recvBuf<<endl;
				}				
			}	//try {
			catch(...) {
				LOG(ERROR)<<
					"RecvData����JSON����ʱ�����ߵ����������쳣�Ͽ������ݽ��ս���ֹ";
				return "RecvData����JSON����ʱ�����ߵ����������쳣�Ͽ������ݽ��ս���ֹ";
			}	//catch(...) {
			
			}	//while (1) {
			LOG(INFO)<<"��ͨ�����ݽ���JSON������������";
		}		//try
		catch(...) {			
			//�쳣�Ͽ����趨�ñ�־ΪFALSE,�Ա��´�Open��Ҫ����������ͨ���̵߳ĳ����
			s_hidJsonRecvThrRunning=false;
			LOG(ERROR)<<
				"��ͨ�����ݽ��չ��������������쳣�Ͽ����������ݽ���JSON�����쳣����";
			return "��ͨ�����ݽ��չ��������������쳣�Ͽ����������ݽ���JSON�����쳣����";
		}	
	}
//#endif // _DEBUG_114A


}	//end of namespace jcLockJsonCmd_t2015a{



//////////////////////////////////////////////////////////////////////////
using namespace boost::property_tree;
using jcLockJsonCmd_t2015a::jcMulHidEnum;
using jcLockJsonCmd_t2015a::G_FAIL;
using jcLockJsonCmd_t2015a::G_SUSSESS;

int zwStartHidDevPlugThread(void);


//2�������豸�б��صĻص�����
CCBELOCK_API void ZJY1501STD SetReturnDrives( ReturnDrives _DrivesListFun )
{
	if (NULL==_DrivesListFun)
	{
		return;
	}
	jcLockJsonCmd_t2015a::G_JCHID_ENUM_DEV2015A=_DrivesListFun;
}

CCBELOCK_API int ZJY1501STD ListDrives( const char * DrivesTypePID )
{
	if (NULL==jcLockJsonCmd_t2015a::G_JCHID_ENUM_DEV2015A)
	{
		return jcLockJsonCmd_t2015a::G_NO_CALLBACK;
	}
	zwStartHidDevPlugThread();

	string jcDevListJson;
	if (0==strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_LOCK))
	{
		jcMulHidEnum(JCHID_PID_LOCK5151,jcDevListJson);
		jcLockJsonCmd_t2015a::G_JCHID_ENUM_DEV2015A(jcLockJsonCmd_t2015a::G_DEV_LOCK,
			const_cast<char *>(jcDevListJson.c_str()));
		return G_SUSSESS;
	}
	if (0==strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_SECBOX))
	{
		jcMulHidEnum(JCHID_PID_SECBOX,jcDevListJson);
		jcLockJsonCmd_t2015a::G_JCHID_ENUM_DEV2015A(jcLockJsonCmd_t2015a::G_DEV_SECBOX,
			const_cast<char *>(jcDevListJson.c_str()));
		return G_SUSSESS;
	}		
	return G_FAIL;
}

//1�����豸
CCBELOCK_API int ZJY1501STD OpenDrives( const char* DrivesTypePID,const char * DrivesIdSN )
{
	MY114FUNCTRACK
	assert(NULL!=DrivesTypePID && NULL!=DrivesIdSN);
	assert(strlen(DrivesTypePID)>0 && strlen(DrivesIdSN)>0);
	assert(strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_LOCK)==0 
		|| strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_SECBOX)==0);
	//////////////////////////////////////////////////////////////////////////
	//̽����豸�Ƿ��Ѿ�����
	JCHID *hndTc=NULL;
	uint32_t inDevidTc=0;
	jcLockJsonCmd_t2015a::isJcHidDevOpend(DrivesTypePID,DrivesIdSN,&inDevidTc,&hndTc);
	if (NULL!=hndTc)
	{
		//���豸�Ѿ����򿪣�ֱ�ӷ���
		LOG(WARNING)<<"JcHid Device "<<DrivesTypePID<<":"<<DrivesIdSN<<" already Opened!\n";
		return G_SUSSESS;
	}
	//////////////////////////////////////////////////////////////////////////

	uint32_t inDevId=jcLockJsonCmd_t2015a::myJcHidHndFromStrSerial(DrivesTypePID, DrivesIdSN);
	VLOG(4)<<"jcHidHandleFromStrSerial="<<inDevId<<endl;
	JCHID jcHandle;
	JCHID *hnd=&jcHandle;
	memset(hnd, 0, sizeof(JCHID));

	hnd->vid = JCHID_VID_2014;
	int serialLen=strlen(DrivesIdSN);
	if (serialLen>0)
	{
		//����serialLen��JCHID_SERIAL_LENGTH�н�С��һ���ֽ��������к�����
		strncpy(hnd->HidSerial,DrivesIdSN,serialLen<JCHID_SERIAL_LENGTH?serialLen:JCHID_SERIAL_LENGTH);
	}	
	if (0==strcmp(jcLockJsonCmd_t2015a::G_DEV_LOCK,DrivesTypePID))
	{
		VLOG(4)<<"will Open G_DEV_LOCK\n";
		hnd->pid = JCHID_PID_LOCK5151;
	}
	if (0==strcmp(jcLockJsonCmd_t2015a::G_DEV_SECBOX,DrivesTypePID))
	{
		VLOG(4)<<"will Open G_DEV_SECBOX\n";
		hnd->pid = JCHID_PID_SECBOX;
	}
	
	if (JCHID_STATUS_OK != jcHidOpen(hnd)) {
		LOG(ERROR)<<"jcHid Device "<<DrivesTypePID<<" "<<DrivesIdSN<<" Open ERROR"<<endl;
		return G_FAIL;
	}
	else
	{
		VLOG(3)<<"jcHid Device "<<DrivesTypePID<<" "<<DrivesIdSN<<" Open Success"<<endl;
	}

	jcLockJsonCmd_t2015a::G_JCDEV_MAP.insert(std::map<uint32_t,JCHID>::value_type(inDevId,*hnd));
	if (false==jcLockJsonCmd_t2015a::s_hidJsonRecvThrRunning)
	{
		boost::thread thr(jcLockJsonCmd_t2015a::RecvFromLockJsonThr);
	}
	
	return G_SUSSESS;
}



//	2���ر��豸
CCBELOCK_API int ZJY1501STD CloseDrives( const char* DrivesTypePID,const char * DrivesIdSN )
{
	MY114FUNCTRACK
	assert(NULL!=DrivesTypePID && NULL!=DrivesIdSN);
	assert(strlen(DrivesTypePID)>0 && strlen(DrivesIdSN)>0);
	assert(strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_LOCK)==0 
		|| strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_SECBOX)==0);

	JCHID *hnd=NULL;
	uint32_t inDevid=0;
	jcLockJsonCmd_t2015a::isJcHidDevOpend(DrivesTypePID,DrivesIdSN,&inDevid,&hnd);
	//&jcLockJsonCmd_t2015a::G_JCDEV_MAP[inDevId];
	if (NULL!=hnd)
	{
		jcHidClose(hnd);
		VLOG(3)<<"jcHid Device "<<DrivesTypePID<<" "<<DrivesIdSN<<" Close Success"<<endl;
		return G_SUSSESS;
	}
	else
	{
		VLOG(3)<<"jcHid Device "<<DrivesTypePID<<" "<<DrivesIdSN<<" Not Open,So can't Close"<<endl;
		return G_FAIL;
	}
	
}

//2�������豸��Ϣ���صĻص�����
CCBELOCK_API void ZJY1501STD SetReturnMessage( ReturnMessage _MessageHandleFun )
{
	if (NULL!=_MessageHandleFun)
	{
		jcLockJsonCmd_t2015a::G_JCHID_RECVMSG_CB=_MessageHandleFun;
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
	//ͨ����Notify������ʼ����Ƿ�˿��Ѿ��򿪣�û�д򿪾�ֱ�ӷ��أ�����
	//2014��11�³��ڹ���������û����������ʱ��ATMCִ��0002���Ĳ�ѯ����״̬��
	//������ѯ������������־�����������
	//���ڻ��ø��ɿ��ķ�ʽ�����Դ�����ͬ��VID��PID���豸����ͬ���кŵ����
	JCHID *hnd=NULL;
	uint32_t inDevid=0;
	jcLockJsonCmd_t2015a::isJcHidDevOpend(DrivesTypePID,DrivesIdSN,&inDevid,&hnd);
	if (NULL==hnd)
	{
		return G_FAIL;
	}

	ZWFUNCTRACE 
		//������������ݣ�������󲻵ó�����λ���ڴ��С������������
		assert(NULL != AnyMessageJson);
	if (NULL == AnyMessageJson) {
		LOG(WARNING)<<("inputMessage����Ϊ��");
			return G_FAIL;
	}
	if (NULL != AnyMessageJson && strlen(AnyMessageJson) > 0) {
		LOG(INFO)<< "JinChu�·�JSON=" << endl << AnyMessageJson <<
			endl;
	}
	//boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);

	try {
		int inlen = strlen(AnyMessageJson);
		assert(inlen > 0 && inlen < JC_MSG_MAXLEN);
		if (inlen == 0 || inlen >= JC_MSG_MAXLEN) {
			LOG(WARNING)<<("notify���볬�������С����");
			return G_FAIL;
		}
		//////////////////////////////////////////////////////////////////////////
		LOG(INFO)<<AnyMessageJson<<endl;
		Sleep(50);
		jcLockJsonCmd_t2015a::zwPushString1501(hnd,AnyMessageJson);
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

	return jcLockJsonCmd_t2015a::G_SUSSESS;
}


//////////////////////////////////////////////////////////////////////////