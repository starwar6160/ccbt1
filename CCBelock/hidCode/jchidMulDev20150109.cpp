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
			LOG(ERROR)<<__FUNCTION__<<" 通过线路发送数据到锁具异常，可能是未连接\n";
		}

	}

//#ifdef _DEBUG_114A
	//从void ThreadLockComm() 修改而来
	//无限循环接收锁具返回值，通过回调函数返回收到的JSON数据
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
				s_hidJsonRecvThrRunning=true;	//算是通信线程的一个心跳标志					
				if (G_JCDEV_MAP.size()==0)
				{
					return "NoOpend HidDevice";
				}
				for (iter=G_JCDEV_MAP.begin();iter!=G_JCDEV_MAP.end();iter++)
				{
					assert(NULL!=iter->second.hid_device);				
					//VLOG(4)<<"hid_device="<<iter->second.hid_device<<endl;
					//根据目前的经验，锁具需要300-400毫秒才能返回数据，所以超时设置为500
					JCHID_STATUS sts=
						jcHidRecvData(&iter->second,
						recvBuf, BLEN, &recvLen,500);				
					//要是某个设备什么数据也没收到，就直接进入下一个设备
					if (JCHID_STATUS_OK!=sts)
					{					
						//LOG(WARNING)<<"JCHID_STATUS No DATA RECVED Status Code "<<sts;
						//return "JCHID_STATUS No DATA RECVED";						
						continue;
					}
					printf("\n");
					LOG(INFO)<<"成功从锁具"<<iter->second.HidSerial<<"接收JSON数据如下："<<endl;
					LOG(INFO)<<recvBuf<<endl;
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

//1、打开设备
CCBELOCK_API int ZJY1501STD OpenDrives( const char* DrivesTypePID,const char * DrivesIdSN )
{
	MY114FUNCTRACK
	assert(NULL!=DrivesTypePID && NULL!=DrivesIdSN);
	assert(strlen(DrivesTypePID)>0 && strlen(DrivesIdSN)>0);
	assert(strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_LOCK)==0 
		|| strcmp(DrivesTypePID,jcLockJsonCmd_t2015a::G_DEV_SECBOX)==0);
	//////////////////////////////////////////////////////////////////////////
	//探测该设备是否已经打开了
	JCHID *hndTc=NULL;
	uint32_t inDevidTc=0;
	jcLockJsonCmd_t2015a::isJcHidDevOpend(DrivesTypePID,DrivesIdSN,&inDevidTc,&hndTc);
	if (NULL!=hndTc)
	{
		//该设备已经被打开，直接返回
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



//	2、关闭设备
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

//2、设置设备消息返回的回调函数
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
//3、向设备发送指令的函数
CCBELOCK_API int ZJY1501STD InputMessage( const char * DrivesTypePID,const char * DrivesIdSN,const char * AnyMessageJson )
{
	//通过在Notify函数开始检测是否端口已经打开，没有打开就直接返回，避免
	//2014年11月初在广州遇到的没有连接锁具时，ATMC执行0002报文查询锁具状态，
	//反复查询，大量无用日志产生的情况。
	//现在换用更可靠的方式，可以处理多个同样VID和PID的设备，不同序列号的情况
	JCHID *hnd=NULL;
	uint32_t inDevid=0;
	jcLockJsonCmd_t2015a::isJcHidDevOpend(DrivesTypePID,DrivesIdSN,&inDevid,&hnd);
	if (NULL==hnd)
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
		LOG(INFO)<< "JinChu下发JSON=" << endl << AnyMessageJson <<
			endl;
	}
	//boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);

	try {
		int inlen = strlen(AnyMessageJson);
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


//////////////////////////////////////////////////////////////////////////