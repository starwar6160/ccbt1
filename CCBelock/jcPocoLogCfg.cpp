#include "stdafx.h"
#include <direct.h>
#include <WinBase.h>
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"

string zwGetConfigFileName(void);
string zwGetLogFileName(void);
int PocoLogInit(void);

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


#ifdef _DEBUG_POCOLOG910
Poco::LogStream *pocoLog=NULL;
int PocoLogInit(void)  
{  
	Poco::AutoPtr<Poco::Channel> channel;  
	{  

		Poco::AutoPtr<Poco::Channel> fileChannel(new Poco::FileChannel());  
		fileChannel->setProperty("compress", "true");  
		fileChannel->setProperty("path", 
			zwGetLogFileName()); 
		fileChannel->setProperty("archive", "number");  
		fileChannel->setProperty("purgeAge", "30 days");  
		fileChannel->setProperty("rotation", "6 hours");  
		fileChannel->setProperty("times", "local"); 

		Poco::AutoPtr<Poco::PatternFormatter> patternFormatter(new Poco::PatternFormatter());  
		patternFormatter->setProperty("pattern", "%H:%M:%S:%i %s(%l): %t");  
		channel = new Poco::FormattingChannel(patternFormatter, fileChannel);  
	}  


	Poco::Logger::root().setChannel(channel.get());  
	Poco::Logger& logger = Poco::Logger::get("LockDLL");  
	logger.setLevel(Poco::Message::PRIO_TRACE);
	pocoLog=new Poco::LogStream(logger); 
	return 0;  
}  
#endif // _DEBUG_POCOLOG910
void zwLogInit();
void myLoadCfgs()
{
	//��DLL��ͬһ��Ŀ¼��Ѱ��ͬ����INI�ļ�����
	zwccbthr::myLoadConfig(zwGetConfigFileName());
	//PocoLogInit();
	zwLogInit();
}

namespace zwsTest1{
	int zwsTestAdd(int a,int b);
}

void zw901StaticLibTest1(void)
{
	cout<<"Result from zw Static Lib is "<<zwsTest1::zwsTestAdd(11,22)<<endl;
}

void zwFatalExec(void)
{
	cout<<"ZWGLOG FATAL ERROR FOUND!"<<endl;
	exit(10000);
}

static bool g_glog_inited=false;

void zwLogInit()
{
	if (true==g_glog_inited)
	{
		return;
	}
	g_glog_inited=true;
	google::InitGoogleLogging("JCELOCK.DLL");    // ��ʼ��
	google::InstallFailureFunction(zwFatalExec);	//��װĬ��FATAL�����������FATALʱֱ�ӷ�����ֹͣ����	
	//FLAGS_minloglevel=google::GLOG_WARNING;
	FLAGS_minloglevel=google::GLOG_FATAL;
	//FLAGS_minloglevel=google::GLOG_INFO;

	//����һ����ʹ�õ��Կ��Կ�����־�����ǲ�������־
	FLAGS_logtostderr=true;	//�����stderr����������ļ�
	//////////////////////////////////////////////////////////////////////////
	FLAGS_log_dir=".";	//�趨�ڵ�ǰĿ¼�¼�¼��־
	//FLAGS_alsologtostderr=true;	//ͬʱ�����stderr���ļ�;
	//FLAGS_max_log_size=4;	//�����־�ļ���С4M
	FLAGS_colorlogtostderr=true;	//����ն�֧�֣����ò�ɫ���
	FLAGS_logbufsecs=1;	//��־����1������������ڴ�����־���ʱ�����ܣ�����Ϊ
	FLAGS_v=4;	//ֻ��VLOGֵС�ڵ��ڸ����֣��Żᱻ����������ʺ��ڵ�����־
	//
	//google::SetLogDestination(google::GLOG_INFO,"./zwjclmstmp");  
	LOG(WARNING)<<"Zhou Wei 20140616.1349b.JCLMSCORE.DLL Loaded!";
#ifdef _DEBUG_TESTGLOG411
	LOG(INFO)<<"zhouwei use google glog 20140411.1146.info";
	LOG(WARNING)<<"zhouwei use google glog 20140411.1146.warn";
	LOG(ERROR)<<"zhouwei use google glog 20140411.1146.ERROR";
	//ʹ��FATAL�����ֱ�ӵ��³����˳��������������FATAL�ĺ�����
	LOG(FATAL)<<"zhouwei use google glog 20140411.1146.FATAL";
#endif // _DEBUG_TESTGLOG411

}
