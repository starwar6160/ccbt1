#include "stdafx.h"
#include "hidapi.h"
#include "zwHidComm.h"
#include "zwCcbElockHdr.h"
#include "CCBelock.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <map>
#include <vector>
using std::map;
using std::vector;
using namespace boost::property_tree;
using jch::zwJcHidDbg15A;
using jch::G_JCHID_RECVMSG_CB;
using jch::G_FAIL;
using jch::G_SUSSESS;

#define MY114FUNCTRACK	VLOG(4)<<__FUNCTION__<<endl;
const int G_RECV_TIMEOUT=2500;
int zwStartHidDevPlugThread(void);
void zwGetHidDevSerial();
void zwGetHidDevSerialTest126();



namespace jcLockJsonCmd_t2015a21{
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
		boost::mutex jcSend_mutex;	//用来限定先要打开设备，启动数据接收线程，然后才能发送数据
		void StartRecvThread();
		void StopRecvThread();
	};
	zwJcHidDbg15A *s_jcHidDev;
//////////////////////////////////变量定义区域////////////////////////////////////////
	
	const int G_RECV_TIMEOUT=700;
	const int G_SUSSESS=0;
	const int G_FAIL=1;
	const int G_NO_CALLBACK=2;
	const char * G_DEV_LOCK="Lock";
	const char * G_DEV_SECBOX="Encryption";
	ReturnDrives G_JCHID_ENUM_DEV2015A=NULL;
	ReturnMessage G_JCHID_RECVMSG_CB=NULL;
	std::map<uint32_t,JCHID> G_JCDEV_MAP;

////////////////////////////////工具函数区域//////////////////////////////////////////
	//将TCHAR转为char   
//*tchar是TCHAR类型指针，*_char是char类型指针   
void TcharToChar (const TCHAR * tchar, char * _char)  
{  
	assert(NULL!=tchar);
	assert(NULL!=_char);
    int iLength ;  
	if (NULL==tchar || NULL==_char)
	{
		return;
	}
//获取字节长度   
iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);  
//将tchar值赋给_char    
WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);   
}  

//同上   
void CharToTchar (const char * _char, TCHAR * tchar)  
{  
	assert(NULL!=tchar);
	assert(NULL!=_char);
    int iLength ;    
	if (NULL==tchar || NULL==_char)
	{
		return;
	}
    iLength = MultiByteToWideChar (CP_ACP, 0, _char, strlen (_char) + 1, NULL, 0) ;  
    MultiByteToWideChar (CP_ACP, 0, _char, strlen (_char) + 1, tchar, iLength) ;  
}  

void zwDumpHidDeviceInfo(const hid_device_info *info)
{
	assert(NULL!=info);
	assert(NULL!=info->vendor_id && NULL!=info->product_id);
	if (NULL==info || NULL==info->vendor_id || NULL==info->product_id)
	{
		return;
	}
	printf("******************START**************************\n");
	printf("path:%s\n",info->path);
	printf("vid:%04X\tpid:%04X\t",info->vendor_id,info->product_id);
	const int BLEN=32;
	char sn[BLEN];
	memset(sn,0,BLEN);
	TcharToChar(info->serial_number,sn);
	printf("serial:%s\t",sn);
	printf("release_number:%u\n",info->release_number);
	memset(sn,0,BLEN);
	TcharToChar(info->manufacturer_string,sn);
	printf("manufacturer_string:%s\t",sn);
	memset(sn,0,BLEN);
	TcharToChar(info->product_string,sn);
	printf("product_string:%s\n",sn);
	printf("usage_page:%u\tusage:%u\n",info->usage_page,info->usage);
	printf("interface_number:%d\tnext:%p\n",info->interface_number,info->next);
	printf("******************END**************************\n");
}

void zwGetHidDevSerial(char *jcHidSerial)
{
	assert(NULL!=jcHidSerial);
	assert(strlen(jcHidSerial)>0);
	if (NULL==jcHidSerial || strlen(jcHidSerial)==0)
	{
		return;
	}
	struct hid_device_info *devs=NULL, *cur_dev=NULL;
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
//////////////////////////////////////////////////////////////////////////


zwJcHidDbg15A::zwJcHidDbg15A()
{
	thr=NULL;
	jcSend_mutex.lock();
	VLOG(4)<<__FUNCTION__<<"\njcDevInit_mutex.lock();"<<endl;
	memset(&m_dev,0,sizeof(m_dev));
	m_dev.vid=0x0483;
	m_dev.pid=0x5710;
	JCHID_STATUS sts= jcHidOpen(&m_dev);
	if (JCHID_STATUS_OK==sts)
	{
		LOG(WARNING)<<" Open jcHid Device "<<m_dev.hid_device<<" SUCCESS"<<endl;
		StartRecvThread();
	}
	else
	{
		LOG(ERROR)<<" Open jcHid Device Error"<<endl;
	}
}

zwJcHidDbg15A::~zwJcHidDbg15A()
{
	assert(NULL!=m_dev.hid_device);	
	StopRecvThread();

	if (NULL!=m_dev.hid_device)
	{
		jcHidClose(&m_dev);
		LOG(WARNING)<<"Close jcHid Device "<<m_dev.hid_device<<" SUCCESS"<<endl;
	}
	else
	{
		LOG(ERROR)<<"try to Close NULL jchid Device Handle"<<endl;
	}
};

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
	LOG(WARNING)<<"金储下发 "<<" Json On jcHidDev "<<
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
		LOG(ERROR)<<__FUNCTION__<<" 通过线路发送数据到锁具异常，可能是未连接\n";
	}
	return sts;
}

int zwJcHidDbg15A::RecvFromLockJsonThr(JCHID *hidHandle)
{				 
	assert(NULL!=hidHandle && NULL!=hidHandle->hid_device);
	if (NULL==hidHandle || NULL==hidHandle->hid_device)
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
		jcSend_mutex.unlock();//这里之后可以开始发送数据了
		VLOG(4)<<__FUNCTION__<<"\njcDevInit_mutex.unlock();"<<endl;
		while (1) {	
#ifdef _DEBUG1
			LOG(WARNING)<<"RECV THR 20150122 "<<t_thr_runCount++<<endl;
#endif // _DEBUG
try{
			/** 手动在线程中加入中断点，中断点不影响其他语句执行 */  
			boost::this_thread::interruption_point();  
					if (NULL==hidHandle->hid_device)
					{
						VLOG(4)<<"NULL==hidHandle->hid_device"<<endl;
						continue;
					}
					//LOG(WARNING)<<"RECVTHR.P3.1,Before RecvHidData"<<endl;
					JCHID_STATUS sts=JCHID_STATUS_FAIL;
					//注意此处如果不每次循环清零，则会导致长度值超过大约4K字节时，计算CRC或者其他读取
					//收到的内容数组时，尽管数据可能只有几十字节，但是读取操作越界，程序崩溃；
					int recvLen = 0;	
					sts=jcHidRecvData(hidHandle,recvBuf, BLEN, &recvLen,G_RECV_TIMEOUT);				
					//要是某个设备什么数据也没收到，就直接进入下一个设备
					if (JCHID_STATUS_OK!=sts &&JCHID_STATUS_RECV_ZEROBYTES!=sts)
					{					
						LOG(INFO)<<"NoData from "<<hidHandle->HidSerial<<endl;
						continue;
					}					
					if (JCHID_STATUS_OK==sts && recvLen>0 )
					{																								
						//计算接收到的数据的CRC
						uint32_t recvDataNowSum=Crc32_ComputeBuf(0,recvBuf,recvLen);
							VLOG_IF(4,recvDataNowSum>0)<<" recvDataNowSum="<<recvDataNowSum<<endl;
							int tRecvLen=strlen(recvBuf);
							static int tmpRecvCount=0;
							LOG_IF(WARNING,tRecvLen>1)<<"MulHid成功从锁具 "<<hidHandle->HidSerial<<
								" 接收JSON数据如下：Count "<<tmpRecvCount++<<endl<<recvBuf<<endl;
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
	LOG(WARNING)<<"金储HID数据接收线程被主程序正常终止"<<endl;
	return G_SUSSESS;
}
//////////////////////TRY-CATCH完毕////////////////////////////////////////////////////
		}	//while (1) {
}

void zwJcHidDbg15A::StartRecvThread()
{
	assert(NULL!=m_dev.hid_device);
	if (NULL==m_dev.hid_device)
	{
		LOG(ERROR)<<"jcHidDevice Not Open,cant't Start RecvThread"<<endl;
	}
	//声明一个函数对象，尖括号内部，前面是函数返回值，括号内部是函数的一个或者多个参数(形参)，估计是逗号分隔，
	//后面用boost::bind按照以下格式把函数指针和后面_1形式的一个或者多个参数(形参)绑定成为一个函数对象
	boost::function<int (JCHID *)> memberFunctionWrapper(boost::bind(&zwJcHidDbg15A::RecvFromLockJsonThr, this,_1));  	
	//再次使用boost::bind把函数对象与实参绑定到一起，就可以传递给boost::thread作为线程体函数了
	thr=new boost::thread(boost::bind(memberFunctionWrapper,&m_dev));	
	Sleep(5);	//等待线程启动完毕，其实也就2毫秒一般就启动了；
}

void zwJcHidDbg15A::StopRecvThread()
{
	assert(NULL!=thr);
	assert(NULL!=m_dev.hid_device);
	if (NULL!=thr && NULL!=m_dev.hid_device)
	{
		thr->interrupt();
		thr->join();
	}
}

//////////////////////////////////////////////////////////////////////////
void jcMulHidEnum( const int hidPid,string &jcDevListJson )
{
	assert(hidPid>0);
	LOG(INFO)<<__FUNCTION__<<"hidPid="<<hidPid<<endl;		
	ptree pt;
	hid_device_info *jclock_cur= hid_enumerate(JCHID_VID_2014,hidPid);
	hid_device_info *jclock_head=jclock_cur;
	VLOG_IF(2,NULL!=jclock_cur)<<"Header of jclock_List="<<jclock_cur<<endl;

	while (NULL!=jclock_cur)
	{
		char serial[32];
		memset(serial,0,32);
		TcharToChar(jclock_cur->serial_number,serial);
		//VLOG(2)<<"vid="<<jclock_cur->vendor_id<<" pid="<<jclock_cur->product_id<<" serial="<<serial<<endl;
		zwDumpHidDeviceInfo(jclock_cur);


		//注意，此处put的话，每次都替代，add的话，才能新增项目
		if (JCHID_PID_LOCK5151==hidPid)
		{
			pt.add("jcElockSerial",serial);
		}
		if (JCHID_PID_SECBOX==hidPid)
		{
			pt.add("jcSecretBoxSerial",serial);
		}

		jclock_cur=jclock_cur->next;
	}
	hid_free_enumeration(jclock_head);
	std::ostringstream ss;
	write_json(ss, pt);
	jcDevListJson=ss.str();
	LOG(INFO)<<"jcDevListJson=\n"<<jcDevListJson;
	Sleep(1000);
}

uint32_t myJcHidHndFromStrSerial( const char* DrivesTypePID, const char * DrivesIdSN)
{
	assert(NULL!=DrivesTypePID );
	assert(strlen(DrivesTypePID)>0 );
	assert(strcmp(DrivesTypePID,jch::G_DEV_LOCK)==0 
		|| strcmp(DrivesTypePID,jch::G_DEV_SECBOX)==0);
	if (NULL==DrivesTypePID || strlen(DrivesTypePID)==0)
	{
		LOG(ERROR)<<"DrivesTypePID is NULL"<<endl;
		return G_FAIL;
	}
	if (strcmp(DrivesTypePID,jch::G_DEV_LOCK)!=0 && strcmp(DrivesTypePID,jch::G_DEV_SECBOX)!=0)
	{
		LOG(ERROR)<<"DrivesTypePID is NOT "<<jch::G_DEV_LOCK<<" or "<<jch::G_DEV_SECBOX<<endl;
		return G_FAIL;
	}
	uint32_t inDevId;
	string hidPidAndSerial=DrivesTypePID;
	hidPidAndSerial+=".";
	if (NULL!=DrivesIdSN &&strlen(DrivesIdSN)>0)
	{
		hidPidAndSerial+=DrivesIdSN;
	}
	else
	{
		hidPidAndSerial+="NULLSN";
	}

	VLOG_IF(4,hidPidAndSerial.length()>0)<<"ZW0120 hidPidAndSerial=["<<hidPidAndSerial.c_str()
		<<"] Length="<<hidPidAndSerial.length()<<endl;
	//inDevId=crc8Short(hidPidAndSerial.c_str(),hidPidAndSerial.length());
	inDevId=Crc32_ComputeBuf(0,hidPidAndSerial.c_str(),hidPidAndSerial.length());
	VLOG(4)<<__FUNCTION__<<"serial="<<inDevId<<endl;
	return inDevId;
}

	void isJcHidDevOpend(const char* DrivesTypePID,const char * DrivesIdSN,uint32_t *inDevHashId,JCHID **jcHidDev)
	{
		assert(NULL!=DrivesTypePID && strlen(DrivesTypePID)>0);
		assert(NULL!=inDevHashId);
		assert(NULL!=jcHidDev);
		if (NULL==DrivesTypePID || strlen(DrivesTypePID)==0)
		{
			LOG(ERROR)<<"DrivesTypePID is NULL"<<endl;
			return;
		}
		if (NULL==inDevHashId)
		{
			LOG(ERROR)<<"inDevHashId is NULL"<<endl;
		}
		if (NULL==jcHidDev)
		{
			LOG(ERROR)<<"jcHidDev is NULL"<<endl;
		}

		uint32_t inDevId=myJcHidHndFromStrSerial(DrivesTypePID, DrivesIdSN);
		VLOG(4)<<"jcHidHandleFromStrSerial="<<inDevId<<endl;	
		*inDevHashId=inDevId;
		auto it=G_JCDEV_MAP.find(inDevId);
		if (it==jch::G_JCDEV_MAP.end())
		{
/*			VLOG(2)<<"jcLockJsonCmd_t2015a21::G_JCDEV_MAP find status of item hashId="
				<<inDevId<<" "<<DrivesTypePID<<" current not Open\n";	*/		
			*jcHidDev=NULL;
			return;
		}
		else
		{
			VLOG(4)<<"jcLockJsonCmd_t2015a21::G_JCDEV_MAP find item "<<inDevId<<" "<<DrivesTypePID<<" SUCCESS!\n";
			if (NULL==it->second.hid_device)
			{
				*jcHidDev=NULL;
			}
			else
			{
				*jcHidDev=&it->second;
			}			
		}
		VLOG_IF(2,NULL!=DrivesIdSN &&strlen(DrivesIdSN)>0)<<"SN="<<DrivesIdSN<<endl;

	}

}	//end of namespace jcLockJsonCmd_t2015a21{


CCBELOCK_API int ZJY1501STD OpenDrives( const char* DrivesTypePID,const char * DrivesIdSN )
{
	assert(NULL!=DrivesTypePID && strlen(DrivesTypePID)>0);
	if (NULL==DrivesTypePID || strlen(DrivesTypePID)==0)
	{
		return G_FAIL;
	}
	jch::s_jcHidDev=new zwJcHidDbg15A();
	return G_SUSSESS;
}

CCBELOCK_API int ZJY1501STD CloseDrives( const char* DrivesTypePID,const char * DrivesIdSN )
{
	assert(NULL!=DrivesTypePID && strlen(DrivesTypePID)>0);
	if (NULL==DrivesTypePID || strlen(DrivesTypePID)==0)
	{
		return G_FAIL;
	}

	if (NULL!=jch::s_jcHidDev)
	{
		//memset(s_jcHidDev,0,sizeof(s_jcHidDev));
		delete jch::s_jcHidDev;
	}
	return G_SUSSESS;
}

//2、设置设备消息返回的回调函数
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
//3、向设备发送指令的函数
CCBELOCK_API int ZJY1501STD InputMessage( const char * DrivesTypePID,const char * DrivesIdSN,const char * AnyMessageJson )
{
	assert(NULL!=DrivesTypePID && strlen(DrivesTypePID)>0);
	assert(NULL!=AnyMessageJson && strlen(AnyMessageJson)>0);
	if (NULL==DrivesTypePID || strlen(DrivesTypePID)==0)
	{
		LOG(ERROR)<<"DrivesTypePID is NULL"<<endl;
		return G_FAIL;
	}
	if (NULL==AnyMessageJson || strlen(AnyMessageJson)==0)
	{
		LOG(ERROR)<<"AnyMessageJson is NULL"<<endl;
		return G_FAIL;
	}
	//LOG(INFO)<<"DrivesTypePID:"<<DrivesTypePID<<" AnyMessageJson:"<<endl<<AnyMessageJson<<endl;
	//LOG_IF(INFO,NULL!=DrivesIdSN &&strlen(DrivesIdSN)>0)<<"DrivesIdSN:"<<DrivesIdSN<<endl;
	//if (NULL != AnyMessageJson && strlen(AnyMessageJson) > 0) {
	//	LOG(WARNING)<<__FUNCTION__<< " JinChu下发JSON=" << endl << AnyMessageJson <<endl;
	//}

	try {
		jch::s_jcHidDev->Push2jcHidDev(AnyMessageJson);
		return G_SUSSESS;
	}
	catch(...) {
		LOG(ERROR)<<__FUNCTION__<<" NotifyJson通过线路发送数据异常，可能网络故障"<<endl;
		return G_FAIL;
	}
}

//2、设置设备列表返回的回调函数
CCBELOCK_API void ZJY1501STD SetReturnDrives( ReturnDrives _DrivesListFun )
{
	assert(NULL!=_DrivesListFun);
	VLOG(4)<<__FUNCTION__<<endl;
	if (NULL==_DrivesListFun)
	{
		LOG(ERROR)<<"Callback Function Pointer _DrivesListFun is NULL"<<endl;
		return;
	}
	jch::G_JCHID_ENUM_DEV2015A=_DrivesListFun;
	zwStartHidDevPlugThread();
}

CCBELOCK_API int ZJY1501STD ListDrives( const char * DrivesTypePID )
{
	assert(NULL!=DrivesTypePID && strlen(DrivesTypePID)>0);
	if (NULL==DrivesTypePID || strlen(DrivesTypePID)==0)
	{
		LOG(ERROR)<<"DrivesTypePID is NULL"<<endl;
		return G_FAIL;
	}
	VLOG(4)<<__FUNCTION__<<" DrivesTypePID="<<DrivesTypePID<<endl;
	if (NULL==jch::G_JCHID_ENUM_DEV2015A)
	{
		LOG(WARNING)<<"NULL==jch::G_JCHID_ENUM_DEV2015A"<<endl;
		return jch::G_NO_CALLBACK;
	}
	zwStartHidDevPlugThread();

	string jcDevListJson;
	if (0==strcmp(DrivesTypePID,jch::G_DEV_LOCK))
	{
		VLOG(4)<<__FUNCTION__<<" for G_DEV_LOCK Callback"<<endl;
		jch::jcMulHidEnum(JCHID_PID_LOCK5151,jcDevListJson);
		jch::G_JCHID_ENUM_DEV2015A(jch::G_DEV_LOCK,
			const_cast<char *>(jcDevListJson.c_str()));
		return G_SUSSESS;
	}
	if (0==strcmp(DrivesTypePID,jch::G_DEV_SECBOX))
	{
		VLOG(4)<<__FUNCTION__<<" for G_DEV_SECBOX Callback"<<endl;
		jch::jcMulHidEnum(JCHID_PID_SECBOX,jcDevListJson);
		jch::G_JCHID_ENUM_DEV2015A(jch::G_DEV_SECBOX,
			const_cast<char *>(jcDevListJson.c_str()));
		return G_SUSSESS;
	}		
	LOG(WARNING)<<__FUNCTION__<<" FAIL"<<endl;
	return G_FAIL;
}

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
	jch::jcMulHidEnum(0x5710,lockSerialList);	
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
				jch::TcharToChar(cur_dev->serial_number,snc);
				printf("%s\n",snc);
		}
		cur_dev = cur_dev->next;
	}
	Sleep(1000);
}

