#include "stdafx.h"
#include <direct.h>
#include <WinBase.h>
#include "zwCcbElockHdr.h"

string zwGetConfigFileName(void);
string zwGetLogFileName(void);


namespace zwccbthr{
	void myLoadConfig(const string &cfgFileName);
}	//namespace zwccbthr{


string zwGetConfigFileName(void)
{
	HMODULE hdl=GetModuleHandleA("HidProtocol.dll");
	char dllName[256];
	memset(dllName,0,256);
	GetModuleFileNameA(hdl,dllName,256);
	char * tt=strstr(dllName,".dll");
	strcpy(tt,".ini");
	string cfgFileName=dllName;
	return cfgFileName;
}

string zwGetLogFileName(void)
{
	const string jcLogPath=".\\JinChuLog2014";
	string logdate,logtime;
	_mkdir(jcLogPath.c_str());
	time_t now=time(NULL);
	time_t tail=now % 10;
	now=now-tail;
	zwGetLocalDateTimeString(now,logdate,logtime);
	return jcLogPath+"\\"+logdate+"."+logtime+"JinChuLog.txt";
}



void myLoadCfgs()
{
	//在DLL的同一个目录下寻找同名的INI文件载入
	zwccbthr::myLoadConfig(zwGetConfigFileName());
}

namespace zwsTest1{
	int zwsTestAdd(int a,int b);
}

void zw901StaticLibTest1(void)
{
	cout<<"Result from zw Static Lib is "<<zwsTest1::zwsTestAdd(11,22)<<endl;
}
