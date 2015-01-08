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

/////////////////////////////GLOG INIT START/////////////////////////////////////////////
void zwFatalExec(void)
{
	cout<<"ZWGLOG FATAL ERROR FOUND!"<<endl;
	//exit(10000);
}

#include <direct.h>
void zwGlogInit()
{
	static int runOnce=0;
	if (1==runOnce)
	{
		return;
	}
	runOnce=1;
	google::InitGoogleLogging("JCATMC151");    // ��ʼ��
	google::InstallFailureFunction(zwFatalExec);	//��װĬ��FATAL�����������FATALʱֱ�ӷ�����ֹͣ����	
	//FLAGS_minloglevel=google::GLOG_WARNING;
	//FLAGS_minloglevel=google::GLOG_FATAL;
	FLAGS_minloglevel=google::GLOG_INFO;

	//����һ����ʹ�õ��Կ��Կ�����־�����ǲ�������־
	//FLAGS_logtostderr=true;	//�����stderr����������ļ�
	//////////////////////////////////////////////////////////////////////////
	const char *zwLogDir=".\\JCLOG15";
	_mkdir(zwLogDir);
	FLAGS_log_dir=zwLogDir;	//�趨��ָ��Ŀ¼�¼�¼��־
	//FLAGS_alsologtostderr=true;	//ͬʱ�����stderr���ļ�;
	FLAGS_max_log_size=5;	//�����־�ļ���С����M�ֽڣ�֮��ͻ��Զ������µ���־�ļ����Ա�ǰ�����־���鿴���߱���
	FLAGS_colorlogtostderr=true;	//����ն�֧�֣����ò�ɫ���
	FLAGS_logbufsecs=1;	//��־����1������������ڴ�����־���ʱ�����ܣ�����Ϊ
	FLAGS_v=3;	//ֻ��VLOGֵС�ڵ��ڸ����֣��Żᱻ����������ʺ��ڵ�����־
	//
	//google::SetLogDestination(google::GLOG_INFO,"./zw1231");  
	LOG(WARNING)<<"Zhou Wei 20141231.1353 test1";
	VLOG(4)<<"VLOG 4";
	VLOG(3)<<"VLOG 3";
	VLOG(2)<<"VLOG 2";
	VLOG(1)<<"VLOG 1";
	for (int i=0;i<1000;i++)
	{
		LOG(INFO)<<"zhouwei use google glog 20150108.1635.info "<<i<<" line";
	}

	LOG(FATAL)<<"zhouwei use google glog 20141231.1354.FATAL";
	LOG(WARNING)<<"zhouwei use google glog 20141231.1354.warn";
	LOG(ERROR)<<"zhouwei use google glog 20141231.1354.ERROR";
	//ʹ��FATAL�����ֱ�ӵ��³����˳��������������FATAL�ĺ�����

}

/////////////////////////////GLOG INIT END/////////////////////////////////////////////


BOOL APIENTRY DllMain(HMODULE hModule,
		      DWORD ul_reason_for_call, LPVOID lpReserved)
{//
	try
	{ 
		zwGlogInit();
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
