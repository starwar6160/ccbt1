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

	uint32_t myJcHidHndFromStrSerial( const char* DrivesType, const char * DrivesID)
	{
		assert(NULL!=DrivesType && NULL!=DrivesID);
		assert(strlen(DrivesType)>0 && strlen(DrivesID)>0);
		assert(strcmp(DrivesType,jcLockJsonCmd_t2015a::G_DEV_LOCK)==0 
			|| strcmp(DrivesType,jcLockJsonCmd_t2015a::G_DEV_SECBOX)==0);
		uint32_t inDevId;
		string hidPidAndSerial=DrivesType;
		hidPidAndSerial+=".";
		hidPidAndSerial+=DrivesID;
		inDevId=crc32Short(hidPidAndSerial.c_str(),hidPidAndSerial.length());
		return inDevId;
	}


	void isJcHidDevOpend(const char* DrivesType,const char * DrivesID,uint32_t *inDevIdNum,JCHID **jcHidDev)
	{
		uint32_t inDevId=myJcHidHndFromStrSerial(DrivesType, DrivesID);
		VLOG(4)<<"jcHidHandleFromStrSerial="<<inDevId<<endl;	
		*inDevIdNum=inDevId;
		std::map<uint32_t,JCHID>::iterator it=G_JCDEV_MAP.find(inDevId);
		if (it==jcLockJsonCmd_t2015a::G_JCDEV_MAP.end())
		{
			LOG(ERROR)<<"jcLockJsonCmd_t2015a::G_JCDEV_MAP find item "<<inDevId<<" not found!\n";
			*jcHidDev=NULL;
			return;
		}
		else
		{
			VLOG(4)<<"jcLockJsonCmd_t2015a::G_JCDEV_MAP find item "<<inDevId<<" SUCCESS!\n";
			*jcHidDev=&it->second;
		}

	}

}	//end of namespace jcLockJsonCmd_t2015a{



//////////////////////////////////////////////////////////////////////////
using namespace boost::property_tree;
using jcLockJsonCmd_t2015a::jcMulHidEnum;

int zwStartHidDevPlugThread(void);


//2、设置设备列表返回的回调函数
void SetReturnDrives(ReturnDrives _DrivesListFun)
{
	if (NULL==_DrivesListFun)
	{
		return;
	}
	jcLockJsonCmd_t2015a::G_JCHID_ENUM_DEV2015A=_DrivesListFun;
}

int ListDrives(char * DrivesType)
{
	if (NULL==jcLockJsonCmd_t2015a::G_JCHID_ENUM_DEV2015A)
	{
		return jcLockJsonCmd_t2015a::G_NO_CALLBACK;
	}
	zwStartHidDevPlugThread();

	string jcDevListJson;
	if (0==strcmp(DrivesType,jcLockJsonCmd_t2015a::G_DEV_LOCK))
	{
		jcMulHidEnum(JCHID_PID_LOCK5151,jcDevListJson);
		jcLockJsonCmd_t2015a::G_JCHID_ENUM_DEV2015A(DrivesType,const_cast<char *>(jcDevListJson.c_str()));
		return jcLockJsonCmd_t2015a::G_SUSSESS;
	}
	if (0==strcmp(DrivesType,jcLockJsonCmd_t2015a::G_DEV_SECBOX))
	{
		jcMulHidEnum(JCHID_PID_SECBOX,jcDevListJson);
		jcLockJsonCmd_t2015a::G_JCHID_ENUM_DEV2015A(DrivesType,const_cast<char *>(jcDevListJson.c_str()));
		return jcLockJsonCmd_t2015a::G_SUSSESS;
	}		
	return jcLockJsonCmd_t2015a::G_FAIL;
}

//1、打开设备
int OpenDrives(const char* DrivesType,const char * DrivesID)
{
	MY114FUNCTRACK
	assert(NULL!=DrivesType && NULL!=DrivesID);
	assert(strlen(DrivesType)>0 && strlen(DrivesID)>0);
	assert(strcmp(DrivesType,jcLockJsonCmd_t2015a::G_DEV_LOCK)==0 
		|| strcmp(DrivesType,jcLockJsonCmd_t2015a::G_DEV_SECBOX)==0);
	uint32_t inDevId=jcLockJsonCmd_t2015a::myJcHidHndFromStrSerial(DrivesType, DrivesID);
	VLOG(4)<<"jcHidHandleFromStrSerial="<<inDevId<<endl;
	JCHID jcHandle;
	JCHID *hnd=&jcHandle;
	memset(hnd, 0, sizeof(JCHID));
	hnd->vid = JCHID_VID_2014;
	int serialLen=strlen(DrivesID);
	if (serialLen>0)
	{
		//复制serialLen和JCHID_SERIAL_LENGTH中较小的一个字节数到序列号里面
		strncpy(hnd->HidSerial,DrivesID,serialLen<JCHID_SERIAL_LENGTH?serialLen:JCHID_SERIAL_LENGTH);
	}	
	if (0==strcmp(jcLockJsonCmd_t2015a::G_DEV_LOCK,DrivesType))
	{
		VLOG(4)<<"will Open G_DEV_LOCK\n";
		hnd->pid = JCHID_PID_LOCK5151;
	}
	if (0==strcmp(jcLockJsonCmd_t2015a::G_DEV_SECBOX,DrivesType))
	{
		VLOG(4)<<"will Open G_DEV_SECBOX\n";
		hnd->pid = JCHID_PID_SECBOX;
	}
	
	if (JCHID_STATUS_OK != jcHidOpen(hnd)) {
		LOG(ERROR)<<"HID Device Open ERROR 1225 !";
		return ELOCK_ERROR_PARAMINVALID;
	}
	else
	{
		VLOG(3)<<"jcHid Device "<<DrivesType<<" "<<DrivesID<<" Open Success"<<endl;
	}

	jcLockJsonCmd_t2015a::G_JCDEV_MAP.insert(std::map<uint32_t,JCHID>::value_type(inDevId,*hnd));

	return jcLockJsonCmd_t2015a::G_SUSSESS;
}



//	2、关闭设备
int CloseDrives(const char* DrivesType,const char * DrivesID)
{
	MY114FUNCTRACK
	assert(NULL!=DrivesType && NULL!=DrivesID);
	assert(strlen(DrivesType)>0 && strlen(DrivesID)>0);
	assert(strcmp(DrivesType,jcLockJsonCmd_t2015a::G_DEV_LOCK)==0 
		|| strcmp(DrivesType,jcLockJsonCmd_t2015a::G_DEV_SECBOX)==0);

	JCHID *hnd=NULL;
	uint32_t inDevid=0;
	jcLockJsonCmd_t2015a::isJcHidDevOpend(DrivesType,DrivesID,&inDevid,&hnd);
	//&jcLockJsonCmd_t2015a::G_JCDEV_MAP[inDevId];
	if (NULL!=hnd->hid_device)
	{
		jcHidClose(hnd);
		VLOG(3)<<"jcHid Device "<<DrivesType<<" "<<DrivesID<<" Close Success"<<endl;
	}
	return jcLockJsonCmd_t2015a::G_SUSSESS;
}

//2、设置设备消息返回的回调函数
void SetReturnMessage(ReturnMessage _MessageHandleFun)
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
int inputMessage(char * DrivesID,char * AnyMessage)
{

	return jcLockJsonCmd_t2015a::G_SUSSESS;
}


//////////////////////////////////////////////////////////////////////////