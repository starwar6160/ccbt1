#include "stdafx.h"
#include <windows.h>
#include "hidapi.h"
#include "zwHidComm.h"
#include "CCBelock.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace boost::property_tree;


static const int G_SUSSESS=0;
static const int G_FAIL=1;
static const char * G_DEV_LOCK="Lock";
static const char * G_DEV_SECBOX="Encryption";



//一、设备列表接口 1、回调函数类型
typedef void (CDECL *ReturnDrives)(char* DrivesType,char * DrivesIDList);
ReturnDrives G_JCHID_ENUM_DEV2015A=NULL;

//2、设置设备列表返回的回调函数
void SetReturnDrives(ReturnDrives _DrivesListFun);

//2、设置设备列表返回的回调函数
void SetReturnDrives(ReturnDrives _DrivesListFun)
{
	if (NULL==_DrivesListFun)
	{
		return;
	}
	G_JCHID_ENUM_DEV2015A=_DrivesListFun;
}

//3、获取设备列表的指令
//说明：
//	（1）DrivesType表示设备类型，具体内容是：锁具=Lock，密盒=Encryption
//	（2）所有int类型函数的返回值中0表示成功，非零表示错误（下同）
//	（3）DrivesIDList为json格式字符，其中列出了所有符合要求设备的DrivesID（HID设备序列号）
//	（4）当调用ListDrives时，接口函数应返回当前所有此类设备的列表
//	（5）当有新设备插入或拔出时，接口函数应该返回插入设备所属类型的最新列表（重复内容由上位机过滤去重）

int ListDrives(char * DrivesType);

int ListDrives(char * DrivesType)
{
	string jcDevListJson;
	if (0==strcmp(DrivesType,G_DEV_LOCK))
	{
		jcMulHidEnum(JCHID_PID_LOCK5151,jcDevListJson);
		return G_SUSSESS;
	}
	if (0==strcmp(DrivesType,G_DEV_SECBOX))
	{
		jcMulHidEnum(JCHID_PID_SECBOX,jcDevListJson);
		return G_SUSSESS;
	}	
	return G_FAIL;
}


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
		pt.put("serial",serial);
		jclock_List=jclock_List->next;
	}
	std::ostringstream ss;
	write_json(ss, pt);
	

}

int jcMulHidOpen(const int vid,const int pid,const char *serial_number)
{
	JCHID hnd;
	hnd.vid=vid;
	hnd.pid=pid;
	strncpy(hnd.HidSerial,serial_number,16);	//16是序列号最大长度
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