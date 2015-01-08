// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "zwCcbElockHdr.h"
void myLoadCfgs(const char *DLLPath);
extern Poco::LogStream * pocoLog;

HMODULE G_DLL_HMODULE = NULL;

#ifdef _DEBUG
string zwTest912(string now)
{
	time_t logGenData = boost::lexical_cast < time_t > (now);
	string exDate, exTime;
	zwGetLocalDateTimeString(logGenData, exDate, exTime);
	return exDate + "." + exTime;
}
#endif // _DEBUG

BOOL APIENTRY DllMain(HMODULE hModule,
		      DWORD ul_reason_for_call, LPVOID lpReserved)
{//
	try
	{ 
		//���ô���ҳΪ�������ģ�936�Ǽ������ĵĴ���ҳ��
		std::locale loc1 = std::locale::global(std::locale(".936"));
		OutputDebugStringA("JinChu Elock DllMain Loaded.20141017.1023");
		G_DLL_HMODULE = hModule;	//����������Ҫ��ȡDLL����·���ĵط�ʹ�ã�
		char myDllPath[256];
		memset(myDllPath, 0, 256);
		zwGetDLLPath(hModule, myDllPath, 256);
		OutputDebugStringA(myDllPath);
		myLoadCfgs(myDllPath);
#ifdef _DEBUG
		//cout<<"TEST912 LEXICAST.1557.\t"<<zwTest912("1409023024")<<endl;
#endif // _DEBUG
		switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			OutputDebugStringA("JINCHU ELOCK DLL_PROCESS_ATTACH");
			pocoLog->warning("JINCHU ELOCK DLL_PROCESS_ATTACH");
			break;
		case DLL_THREAD_ATTACH:
			OutputDebugStringA("JINCHU ELOCK DLL_THREAD_ATTACH");
			pocoLog->notice("JINCHU ELOCK DLL_THREAD_ATTACH");
			break;
		case DLL_THREAD_DETACH:
			OutputDebugStringA("JINCHU ELOCK DLL_THREAD_DETACH");
			pocoLog->notice("JINCHU ELOCK DLL_THREAD_DETACH");
			break;
		case DLL_PROCESS_DETACH:
			OutputDebugStringA("JINCHU ELOCK DLL_PROCESS_DETACH");
			pocoLog->warning("JINCHU ELOCK DLL_PROCESS_DETACH");
			break;
		}
		OutputDebugStringA("JinChu Elock DllMain Success End.20141017.1023");
		//�ָ�ԭ���Ĵ���ҳ
		std::locale::global(std::locale(loc1));
	}
	catch(...)
	{
		//Ϊ�˻�ȡ����Ŀ¼��Poco����־����쳣ʱ������
		printf("PocoLog in Chinese PathName Execption!20150108\n");
		printf("��ѱ�ATMC DLL�ŵ���Ӣ�ģ��޿ո��·���²�����ȷ�����ã�лл��Zhou Wei.20150108.\n");
		exit(2015);
	}

	return TRUE;
}
