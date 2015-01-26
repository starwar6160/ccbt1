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
	//Ϊ�˷�ֹHID�����ظ���ȡ���⣬�ڷ��ͺͽ��յ�ʱ������һ�£�ֻ��ȡһ��֮�ã�
	bool s_curCmdRecved=false;	


	void zwDumpHidDeviceInfo(const hid_device_info *info)
	{
		if (NULL==info)
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


	void jcMulHidEnum( const int hidPid,string &jcDevListJson )
	{
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


			//ע�⣬�˴�put�Ļ���ÿ�ζ������add�Ļ�������������Ŀ
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


//////////////////////////////////////////////////////////////////////////