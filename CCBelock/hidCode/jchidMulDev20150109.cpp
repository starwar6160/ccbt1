#include "stdafx.h"
#include <windows.h>
#include "hidapi.h"
#include "zwHidComm.h"
#include "CCBelock.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace boost::property_tree;
using jcLockJsonCmd_t2015a::jcMulHidEnum;

int zwStartHidDevPlugThread(void);

static const int G_SUSSESS=0;
static const int G_FAIL=1;
static const int G_NO_CALLBACK=2;
static const char * G_DEV_LOCK="Lock";
static const char * G_DEV_SECBOX="Encryption";

ReturnDrives G_JCHID_ENUM_DEV2015A=NULL;

//2�������豸�б��صĻص�����
void SetReturnDrives(ReturnDrives _DrivesListFun)
{
	if (NULL==_DrivesListFun)
	{
		return;
	}
	G_JCHID_ENUM_DEV2015A=_DrivesListFun;
}

int ListDrives(char * DrivesType)
{
	if (NULL==G_JCHID_ENUM_DEV2015A)
	{
		return G_NO_CALLBACK;
	}
	zwStartHidDevPlugThread();
	
	string jcDevListJson;
	if (0==strcmp(DrivesType,G_DEV_LOCK))
	{
		jcMulHidEnum(JCHID_PID_LOCK5151,jcDevListJson);
		G_JCHID_ENUM_DEV2015A(DrivesType,const_cast<char *>(jcDevListJson.c_str()));
		return G_SUSSESS;
	}
	if (0==strcmp(DrivesType,G_DEV_SECBOX))
	{
		jcMulHidEnum(JCHID_PID_SECBOX,jcDevListJson);
		G_JCHID_ENUM_DEV2015A(DrivesType,const_cast<char *>(jcDevListJson.c_str()));
		return G_SUSSESS;
	}		
	return G_FAIL;
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

//ͬ��   
static void CharToTchar (const char * _char, TCHAR * tchar)  
{  
    int iLength ;  
  
    iLength = MultiByteToWideChar (CP_ACP, 0, _char, strlen (_char) + 1, NULL, 0) ;  
    MultiByteToWideChar (CP_ACP, 0, _char, strlen (_char) + 1, tchar, iLength) ;  
}  



int jcMulHidOpen(const int vid,const int pid,const char *serial_number)
{
	JCHID hnd;
	hnd.vid=vid;
	hnd.pid=pid;
	strncpy(hnd.HidSerial,serial_number,16);	//16�����к���󳤶�
	JCHID_STATUS sts= jcHidOpen(&hnd);
	if (JCHID_STATUS_OK==sts)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void jcMulHidClose(const JCHID *hnd)
{
	if (NULL!=hnd && NULL!=hnd->hid_device)
	{
		jcHidClose(hnd);
	}	
}

//////////////////////////////////////////////////////////////////////////

namespace jcLockJsonCmd_t2015a{
	void jcMulHidEnum( const int hidPid,string &jcDevListJson )
	{
		ptree pt;
		hid_device_info *jclock_List= hid_enumerate(JCHID_VID_2014,hidPid);
		while (NULL!=jclock_List)
		{
			char serial[32];
			memset(serial,0,32);
			TcharToChar(jclock_List->serial_number,serial);
			printf("%s\n",serial);
			pt.put("jcDevSerial",serial);
			jclock_List=jclock_List->next;
		}
		std::ostringstream ss;
		write_json(ss, pt);
		jcDevListJson=ss.str();
	}

}	//end of namespace jcLockJsonCmd_t2015a{