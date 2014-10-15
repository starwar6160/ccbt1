// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "zwCcbElockHdr.h"
void myLoadCfgs(const char *DLLPath);
extern Poco::LogStream * pocoLog;

HMODULE G_DLL_HMODULE=NULL;



#ifdef _DEBUG
string zwTest912(string now)
{
	time_t logGenData=boost::lexical_cast<time_t>(now);
	string exDate, exTime;
	zwGetLocalDateTimeString(logGenData, exDate, exTime);
	return exDate+"."+exTime;
}
#endif // _DEBUG

void zwGetDLLPath(HMODULE hDLL,char *pDllPath,const int dllPathLen)
{
	//assert(	NULL!=hDLL );
	assert(NULL!=pDllPath);
	assert(dllPathLen>0);
	if (NULL==hDLL)
	{
		return;
	}
	GetModuleFileNameA(hDLL,pDllPath,dllPathLen);
	int dllNameLen=strlen(pDllPath);
	//�����һ���ֽ�����,�ҵ�"HidProtocol.dll"֮ǰ��'\\'�ַ��ͽ���׶�
	//֮���Բ���strstr����DLL�ļ�������Ϊ��ȷ�����ж����ִ�Сд�����ʽ
	//Ҫ��ͳһ��Ϊ��д�����ƻ���·�����Ĵ�Сд�����ܻ����Ǳ�����⣬
	//����windows�ļ�ϵͳ�����ִ�Сд
	for (int i=dllNameLen-1;i>0;i--)
	{
		if (pDllPath[i]=='\\')
		{
			pDllPath[i]=NULL;
			break;
		}
	}
}

BOOL APIENTRY DllMain(HMODULE hModule,
		      DWORD ul_reason_for_call, LPVOID lpReserved)
{
	G_DLL_HMODULE=hModule;	//����������Ҫ��ȡDLL����·���ĵط�ʹ�ã�
	char myDllPath[256];
	memset(myDllPath,0,256);
	zwGetDLLPath(hModule,myDllPath,256);
	myLoadCfgs(myDllPath);	
#ifdef _DEBUG
	//cout<<"TEST912 LEXICAST.1557.\t"<<zwTest912("1409023024")<<endl;
#endif // _DEBUG
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		pocoLog->warning("JINCHU ELOCK DLL_PROCESS_ATTACH");
		break;
	case DLL_THREAD_ATTACH:
		pocoLog->notice("JINCHU ELOCK DLL_THREAD_ATTACH");
		break;
	case DLL_THREAD_DETACH:
		pocoLog->notice("JINCHU ELOCK DLL_THREAD_DETACH");
		break;
	case DLL_PROCESS_DETACH:
		pocoLog->warning("JINCHU ELOCK DLL_PROCESS_DETACH");
		break;
	}
	return TRUE;
}


