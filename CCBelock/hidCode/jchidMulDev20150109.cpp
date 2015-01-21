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
#include <vector>
using std::map;
using std::vector;

#define MY114FUNCTRACK	VLOG(4)<<__FUNCTION__<<endl;
const int G_RECV_TIMEOUT=2500;

//将TCHAR转为char   
//*tchar是TCHAR类型指针，*_char是char类型指针   
static void TcharToChar (const TCHAR * tchar, char * _char)  
{  
    int iLength ;  
//获取字节长度   
iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);  
//将tchar值赋给_char    
WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);   
}  

//同上   
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
	//为了防止HID可以重复读取问题，在发送和接收的时候区别一下，只读取一次之用；
	bool s_curCmdRecved=false;	

	void jcMulHidEnum( const int hidPid,string &jcDevListJson )
	{
		LOG(INFO)<<__FUNCTION__<<"hidPid="<<hidPid<<endl;		
		ptree pt;
		hid_device_info *jclock_List= hid_enumerate(JCHID_VID_2014,hidPid);
		VLOG_IF(2,NULL!=jclock_List)<<"Header of jclock_List="<<jclock_List->serial_number<<endl;
		
		while (NULL!=jclock_List)
		{
			char serial[32];
			memset(serial,0,32);
			TcharToChar(jclock_List->serial_number,serial);
			VLOG(2)<<"vid="<<jclock_List->vendor_id<<" pid="<<jclock_List->product_id<<" serial="<<serial<<endl;

			//注意，此处put的话，每次都替代，add的话，才能新增项目
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
		hid_free_enumeration(jclock_List);
		std::ostringstream ss;
		write_json(ss, pt);
		jcDevListJson=ss.str();
		LOG(INFO)<<"jcDevListJson=\n"<<jcDevListJson<<endl;
	}


	uint32_t myJcHidHndFromStrSerial( const char* DrivesTypePID, const char * DrivesIdSN)
	{
		assert(NULL!=DrivesTypePID );
		assert(strlen(DrivesTypePID)>0 );
		assert(strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_LOCK)==0 
			|| strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_SECBOX)==0);
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
		
		VLOG_IF(4,hidPidAndSerial.length()>0)<<"ZW0120 hidPidAndSerial=["<<hidPidAndSerial.c_str()<<"] Length="<<hidPidAndSerial.length()<<endl;
		//inDevId=crc8Short(hidPidAndSerial.c_str(),hidPidAndSerial.length());
		inDevId=Crc32_ComputeBuf(0,hidPidAndSerial.c_str(),hidPidAndSerial.length());
		VLOG(4)<<__FUNCTION__<<"serial="<<inDevId<<endl;
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
/*			VLOG(2)<<"jcLockJsonCmd_t2015a::G_JCDEV_MAP find status of item hashId="
				<<inDevId<<" "<<DrivesTypePID<<" current not Open\n";	*/		
			*jcHidDev=NULL;
			return;
		}
		else
		{
			VLOG(4)<<"jcLockJsonCmd_t2015a::G_JCDEV_MAP find item "<<inDevId<<" "<<DrivesTypePID<<" SUCCESS!\n";
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
		LOG(INFO)<<__FUNCTION__<<"jcHidDev "<<
			((JCHID *)hidHandle)->HidSerial<<" Push "<<str<<endl;

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
			LOG(ERROR)<<__FUNCTION__<<" 通过线路发送数据到锁具异常，可能是未连接\n";
		}

	}


	typedef struct my_zjy_dbg_20150115_t{
		uint32_t devHash;
		JCHID devCtx;
		bool isGood;
	}ZJYDBG;

//#ifdef _DEBUG_114A
	//从void ThreadLockComm() 修改而来
	//无限循环接收锁具返回值，通过回调函数返回收到的JSON数据
	const char * RecvFromLockJsonThr(void)
	{				 
		LOG(WARNING)<<__FUNCTION__<<" Started"<<endl;
		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];
			memset(recvBuf, 0, BLEN + 1);
			int recvLen = 0;

			std::map<uint32_t,JCHID>::iterator iter;
			assert(G_JCDEV_MAP.size()>0);		
			VLOG(4)<<"G_JCDEV_MAP.size()="<<G_JCDEV_MAP.size()<<endl;

			std::vector<ZJYDBG> jcDevVec;
			for(iter=G_JCDEV_MAP.begin();iter!=G_JCDEV_MAP.end();iter++)
			{
				ZJYDBG t1;
				t1.isGood=true;
				t1.devHash=iter->first;
				memcpy(&t1.devCtx,&iter->second,sizeof(JCHID));
				jcDevVec.push_back(t1);
			}
			//LOG(ERROR)<<"jcDecVec[0] is "<<jcDevVec[0].devHash<<endl;
			static uint32_t recvDataSum=0;
			int t_thr_runCount=1;
			while (1) {			
			try {
				VLOG(4)<<"RECVTHR.P1"<<endl;
				LOG(WARNING)<<__FUNCTION__<<(t_thr_runCount++)<<endl;
				if (false==s_hidJsonRecvThrRunning)
				{
					//收到退出信号，结束接收数据线程
					LOG(WARNING)<<"JcHid20150121 RecvData Thread Exit by other Func\n";
					return "RecvThr Exit by CloseJcHidDevice";
				}
				//s_hidJsonRecvThrRunning=true;	//算是通信线程的一个心跳标志					
				if (G_JCDEV_MAP.size()==0)
				{
					return "NoOpend HidDevice";
				}
				//for (iter=G_JCDEV_MAP.begin();iter!=G_JCDEV_MAP.end();iter++)
				VLOG(4)<<"RECVTHR.P2"<<endl;
				
				for(int i=0;i<jcDevVec.size();i++)
				{
					VLOG(4)<<"jcDevVec.size()="<<jcDevVec.size()<<endl;
					VLOG_IF(4,NULL==jcDevVec[i].devCtx.hid_device && true==jcDevVec[i].isGood)
						<<"jcHidDev "<<jcDevVec[i].devCtx.HidSerial<<" Not Open"<<endl;
					if (NULL==jcDevVec[i].devCtx.hid_device)
					{
						jcDevVec[i].isGood=false;						
						continue;
					}
					if (jcDevVec[i].isGood==false)
					{
						continue;
					}					
					//VLOG(4)<<"hid_device="<<iter->second.hid_device<<endl;
					//根据目前的经验，锁具需要300-400毫秒才能返回数据，所以超时设置为500
					LOG(WARNING)<<"RECVTHR.P3.1,Before RecvHidData"<<endl;
					JCHID_STATUS sts=
						jcHidRecvData(&jcDevVec[i].devCtx,
						recvBuf, BLEN, &recvLen,G_RECV_TIMEOUT);				
					LOG(WARNING)<<"RECVTHR.P3.2,After RecvHidData"<<endl;
					//要是某个设备什么数据也没收到，就直接进入下一个设备
					if (JCHID_STATUS_OK!=sts &&JCHID_STATUS_RECV_ZEROBYTES!=sts)
					{					
						//LOG(WARNING)<<"JCHID_STATUS No DATA RECVED Status Code "<<sts;
						//return "JCHID_STATUS No DATA RECVED";	
						LOG(WARNING)<<"NoData from "<<jcDevVec[i].devCtx.HidSerial<<endl;
						jcDevVec[i].isGood=false;
						continue;
					}					
					if (recvLen>0 
						//&& false==s_curCmdRecved 这个条件导致除了第一次回调，以后的回调都不会进行，所以去掉这个条件
						)
					{																		
						//uint32_t recvDataNowSum= recvDataSum=Crc32_ComputeBuf(0,recvBuf,recvLen);
						uint32_t recvDataNowSum=Crc32_ComputeBuf(0,recvBuf,recvLen);
						if (recvDataNowSum!=recvDataSum)
						{
							VLOG(4)<<"recvDataSum="<<recvDataSum<<" recvDataNowSum="<<recvDataNowSum<<endl;
							int tRecvLen=strlen(recvBuf);
							LOG_IF(INFO,tRecvLen>9)<<"成功从锁具"<<jcDevVec[i].devCtx.HidSerial<<"接收JSON数据如下："<<endl;
							LOG_IF(WARNING,tRecvLen>9)<<recvBuf<<endl;
							LOG_IF(ERROR,NULL==G_JCHID_RECVMSG_CB)<<"G_JCHID_RECVMSG_CB==NULL"<<endl;
							if (NULL!=G_JCHID_RECVMSG_CB)
							{
								G_JCHID_RECVMSG_CB(jcDevVec[i].devCtx.HidSerial,recvBuf);
							}						
							s_curCmdRecved=true;
							recvDataSum=recvDataNowSum;
						}

					}					
				}				
			}	//try {
			catch(...) {
				LOG(ERROR)<<
					"RecvData接收JSON数据时到锁具的数据连接异常断开，数据接收将终止";
				return "RecvData接收JSON数据时到锁具的数据连接异常断开，数据接收将终止";
			}	//catch(...) {
			
			}	//while (1) {
			LOG(INFO)<<"金储通信数据接收JSON过程正常结束";
		}		//try
		catch(...) {			
			//异常断开就设定该标志为FALSE,以便下次Open不要再跳过启动通信线程的程序段
			s_hidJsonRecvThrRunning=false;
			LOG(ERROR)<<
				"金储通信数据接收过程中数据连接异常断开，现在数据接收JSON过程异常结束";
			return "金储通信数据接收过程中数据连接异常断开，现在数据接收JSON过程异常结束";
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


//2、设置设备列表返回的回调函数
CCBELOCK_API void ZJY1501STD SetReturnDrives( ReturnDrives _DrivesListFun )
{
	VLOG(4)<<__FUNCTION__<<endl;
	if (NULL==_DrivesListFun)
	{
		return;
	}
	jcLockJsonCmd_t2015a::G_JCHID_ENUM_DEV2015A=_DrivesListFun;
	zwStartHidDevPlugThread();
}

CCBELOCK_API int ZJY1501STD ListDrives( const char * DrivesTypePID )
{
	VLOG(4)<<__FUNCTION__<<" DrivesTypePID="<<DrivesTypePID<<endl;
	if (NULL==jcLockJsonCmd_t2015a::G_JCHID_ENUM_DEV2015A)
	{
		return jcLockJsonCmd_t2015a::G_NO_CALLBACK;
	}
	zwStartHidDevPlugThread();

	string jcDevListJson;
	if (0==strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_LOCK))
	{
		VLOG(4)<<__FUNCTION__<<" for G_DEV_LOCK Callback"<<endl;
		jcMulHidEnum(JCHID_PID_LOCK5151,jcDevListJson);
		jcLockJsonCmd_t2015a::G_JCHID_ENUM_DEV2015A(jcLockJsonCmd_t2015a::G_DEV_LOCK,
			const_cast<char *>(jcDevListJson.c_str()));
		return G_SUSSESS;
	}
	if (0==strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_SECBOX))
	{
		VLOG(4)<<__FUNCTION__<<" for G_DEV_SECBOX Callback"<<endl;
		jcMulHidEnum(JCHID_PID_SECBOX,jcDevListJson);
		jcLockJsonCmd_t2015a::G_JCHID_ENUM_DEV2015A(jcLockJsonCmd_t2015a::G_DEV_SECBOX,
			const_cast<char *>(jcDevListJson.c_str()));
		return G_SUSSESS;
	}		
	LOG(WARNING)<<__FUNCTION__<<" FAIL"<<endl;
	return G_FAIL;
}

#ifdef _DEBUG0121
//1、打开设备
CCBELOCK_API int ZJY1501STD OpenDrives( const char* DrivesTypePID,const char * DrivesIdSN )
{
	MY114FUNCTRACK
	VLOG(4)<<__FUNCTION__<<" DrivesTypePID="<<DrivesTypePID<<endl;
	VLOG_IF(4,NULL!=DrivesIdSN && strlen(DrivesIdSN)>0)<<" DrivesIdSN="<<DrivesIdSN<<endl;
	assert(NULL!=DrivesTypePID );
	assert(strlen(DrivesTypePID)>0);
	assert(strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_LOCK)==0 
		|| strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_SECBOX)==0);
	//////////////////////////////////////////////////////////////////////////
	//探测该设备是否已经打开了
	JCHID *hndTc=NULL;
	uint32_t inDevidTc=0;
	jcLockJsonCmd_t2015a::isJcHidDevOpend(DrivesTypePID,DrivesIdSN,&inDevidTc,&hndTc);
	VLOG(4)<<"jcHidDevHash="<<inDevidTc<<" JCHID1="<<hndTc<<endl;
	if (NULL!=hndTc && NULL!=hndTc->hid_device)
	{
		//该设备已经被打开，直接返回
		LOG(WARNING)<<"JcHid Device "<<DrivesTypePID<<" devPointer="<<hndTc<<" already Opened!\n";
		VLOG_IF(4,NULL!=DrivesIdSN && strlen(DrivesIdSN)>0)<<":"<<DrivesIdSN<<endl;
		return G_SUSSESS;
	}
	//////////////////////////////////////////////////////////////////////////

	uint32_t inDevId=jcLockJsonCmd_t2015a::myJcHidHndFromStrSerial(DrivesTypePID, DrivesIdSN);
	VLOG(4)<<"jcHidHandleFromStrSerial="<<inDevId<<endl;
	LOG_IF(ERROR,inDevId!=inDevidTc)<<"inDevId!=inDevidTc "<<inDevId<<"!="<<inDevidTc<<endl;
	JCHID jcHandle;
	JCHID *hnd=&jcHandle;
	memset(hnd, 0, sizeof(JCHID));

	hnd->vid = JCHID_VID_2014;
	if (NULL!=DrivesIdSN &&strlen(DrivesIdSN)>0)
	{
		int serialLen=strlen(DrivesIdSN);
		VLOG(4)<<"DrivesIdSN length="<<serialLen<<endl;
		//复制serialLen和JCHID_SERIAL_LENGTH中较小的一个字节数到序列号里面
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
		LOG(ERROR)<<"jcHid Device "<<DrivesTypePID<<" Open ERROR"<<endl;
		LOG_IF(ERROR,NULL!=DrivesIdSN && strlen(DrivesIdSN)>0)<<"SN:"<<DrivesIdSN<<endl;
		return G_FAIL;
	}
	else
	{
		LOG(WARNING)<<"jcHid Device "<<DrivesTypePID<<" Open Success"<<endl;
		LOG_IF(WARNING,NULL!=DrivesIdSN &&strlen(DrivesIdSN)>0)<<"SN:"<<DrivesIdSN<<endl;
	}

	jcLockJsonCmd_t2015a::G_JCDEV_MAP.insert(std::map<uint32_t,JCHID>::value_type(inDevId,*hnd));
	if (false==jcLockJsonCmd_t2015a::s_hidJsonRecvThrRunning)
	{		
		jcLockJsonCmd_t2015a::s_hidJsonRecvThrRunning=true;
		boost::thread thr(jcLockJsonCmd_t2015a::RecvFromLockJsonThr);
	}
	jcLockJsonCmd_t2015a::s_curCmdRecved=false;	//清理“已经接收”的标志；
	return G_SUSSESS;
}



//	2、关闭设备
CCBELOCK_API int ZJY1501STD CloseDrives( const char* DrivesTypePID,const char * DrivesIdSN )
{
	MY114FUNCTRACK
	VLOG(4)<<__FUNCTION__<<" DrivesTypePID="<<DrivesTypePID<<endl;
	VLOG_IF(4,NULL!=DrivesIdSN &&strlen(DrivesIdSN)>0)<<" DrivesIdSN="<<DrivesIdSN<<endl;
	LOG_IF(WARNING,NULL!=DrivesTypePID);
	assert(NULL!=DrivesTypePID );
	assert(strlen(DrivesTypePID)>0);
	assert(strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_LOCK)==0 
		|| strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_SECBOX)==0);

	JCHID *hnd=NULL;
	uint32_t inDevid=0;
	jcLockJsonCmd_t2015a::isJcHidDevOpend(DrivesTypePID,DrivesIdSN,&inDevid,&hnd);
	VLOG(4)<<"jcHidDevHash="<<inDevid<<" JCHID2="<<hnd<<endl;
	//&jcLockJsonCmd_t2015a::G_JCDEV_MAP[inDevId];
	if (NULL!=hnd && NULL!=hnd->hid_device)
	{
		 if(JCHID_STATUS_OK==jcHidClose(hnd))
		 {
			 std::map<uint32_t,JCHID>::iterator it=
			  jcLockJsonCmd_t2015a::G_JCDEV_MAP.find(inDevid);
			 jcLockJsonCmd_t2015a::G_JCDEV_MAP.erase(it);
			 jcLockJsonCmd_t2015a::s_hidJsonRecvThrRunning=false;	//通知通信接收线程退出
			 VLOG(3)<<"jcHid Device "<<DrivesTypePID<<" Close Success"<<endl;
			 VLOG_IF(3,NULL!=DrivesIdSN &&strlen(DrivesIdSN)>0)<<"SN:"<<DrivesIdSN<<endl;
			 Sleep(500);
			 return G_SUSSESS;
		 }				
	}
	else
	{
		LOG(WARNING)<<"jcHid Device "<<DrivesTypePID<<" Not Open,So can't Close"<<endl;
		LOG_IF(WARNING,NULL!=DrivesIdSN &&strlen(DrivesIdSN)>0)<<"SN:"<<DrivesIdSN<<endl;
		return G_FAIL;
	}
	return G_SUSSESS;
}


//2、设置设备消息返回的回调函数
CCBELOCK_API void ZJY1501STD SetReturnMessage( ReturnMessage _MessageHandleFun )
{
	if (NULL!=_MessageHandleFun)
	{
		LOG_IF(ERROR,NULL==_MessageHandleFun)<<"G_JCHID_RECVMSG_CB==NULL"<<endl;
		jcLockJsonCmd_t2015a::G_JCHID_RECVMSG_CB=_MessageHandleFun;
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
	LOG(INFO)<<"DrivesTypePID"<<DrivesTypePID<<"AnyMessageJson"<<AnyMessageJson<<endl;
	LOG_IF(INFO,NULL!=DrivesIdSN &&strlen(DrivesIdSN)>0)<<"DrivesIdSN"<<DrivesIdSN<<endl;
	//通过在Notify函数开始检测是否端口已经打开，没有打开就直接返回，避免
	//2014年11月初在广州遇到的没有连接锁具时，ATMC执行0002报文查询锁具状态，
	//反复查询，大量无用日志产生的情况。
	//现在换用更可靠的方式，可以处理多个同样VID和PID的设备，不同序列号的情况
	JCHID *hnd=NULL;
	uint32_t inDevid=0;
	jcLockJsonCmd_t2015a::isJcHidDevOpend(DrivesTypePID,DrivesIdSN,&inDevid,&hnd);
	VLOG(4)<<"jcHidDevHash="<<inDevid<<" JCHID3="<<hnd<<endl;
	if (NULL==hnd || NULL==hnd->hid_device)
	{
		return G_FAIL;
	}

	ZWFUNCTRACE 
		//输入必须有内容，但是最大不得长于下位机内存大小，做合理限制
		assert(NULL != AnyMessageJson);
	if (NULL == AnyMessageJson) {
		LOG(WARNING)<<("inputMessage输入为空");
			return G_FAIL;
	}
	if (NULL != AnyMessageJson && strlen(AnyMessageJson) > 0) {
		LOG(WARNING)<< "JinChu下发JSON=" << endl << AnyMessageJson <<
			endl;
	}
	//boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);

	try {
		int inlen = strlen(AnyMessageJson);
		LOG_IF(WARNING,inlen<2)<<"AnyMessageJson Length <2"<<endl;
		VLOG(4)<<"AnyMessageJson Length is "<<inlen<<endl;
		assert(inlen > 0 && inlen < JC_MSG_MAXLEN);
		if (inlen == 0 || inlen >= JC_MSG_MAXLEN) {
			LOG(WARNING)<<("notify输入超过最大最小限制");
			return G_FAIL;
		}
		//////////////////////////////////////////////////////////////////////////
		LOG(INFO)<<AnyMessageJson<<endl;
		Sleep(50);
		jcLockJsonCmd_t2015a::zwPushString1501(hnd,AnyMessageJson);
		return G_SUSSESS;
	}
	catch(...) {		//一切网络异常都直接返回错误。主要是为了捕捉未连接时
		//串口对象为空造成访问NULL指针的的SEH异常  
		//为了使得底层Poco库与cceblock类解耦，从我的WS类
		//发现WS对象因为未连接而是NULL时直接throw一个枚举
		//然后在此，也就是上层捕获。暂时不知道捕获精确类型
		//所以catch所有异常了
		LOG(FATAL)<<__FUNCTION__<<" NotifyJson通过线路发送数据异常，可能网络故障"<<endl;
			return G_FAIL;
	}

	return jcLockJsonCmd_t2015a::G_SUSSESS;
}
#endif // _DEBUG0121

//copy from void myMulHidDevJsonTest20150116B()
void ZJY1501STD zjyTest116()
{
	const char *devSN3=NULL;
		//"PAAbAAAAAAAAgAKE";
	//"jcElockSerial": "PAAbAOgSACDAnwEg"
	//const char *devSN3="PAAbAOgSACDAnwEg";	
	const char *jcHidJsonMsg116t1="{\"command\": \"Test_Motor_Open\",\"cmd_id\": \"1234567890\",\"State\": \"test\"}";

	const char *hidType="Lock";
	//SetReturnDrives(myHidListTest113);
	ListDrives("Lock");
	//Sleep(9000);
	//exit(1);

	OpenDrives(hidType,	devSN3);
	//SetReturnMessage(myReturnMessageTest115);

	InputMessage(hidType,devSN3,jcHidJsonMsg116t1);
	Sleep(750);

	CloseDrives(hidType,devSN3);
	getchar();
}


//////////////////////////////////////////////////////////////////////////