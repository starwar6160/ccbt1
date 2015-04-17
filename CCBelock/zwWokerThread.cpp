#include "stdafx.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"
#include "zwHidSplitMsg.h"
#include "zwHidComm.h"
#include <stdio.h>
using namespace boost::property_tree;

int myOpenElock1503(JCHID *jcElock);
void myCloseElock1503(void);

namespace zwccbthr {
	boost::mutex thr_mutex;
	//建行给的接口，没有设置连接参数的地方，也就是说，完全可以端口，抑或是从配置文件读取
	boost::mutex recv_mutex;
	std::string s_ComPort;
	std::deque < string > dqOutXML;
	JCHID hidHandle;

	void wait(int milliseconds) {
		boost::this_thread::sleep(boost::
					  posix_time::milliseconds
					  (milliseconds));
	} string zwGetLockIP(void) {
		string myLockIp = "COM2";	//默认值是COM2
		if (s_ComPort.length() > 0) {
			myLockIp = s_ComPort;	//如果有配置文件的串口值，就使用之；
		}
		ZWWARN("连接锁具IP使用");
		ZWWARN(myLockIp.c_str());
		return myLockIp;
	}

	//与锁具之间的通讯线程
	void ThreadLockComm() {
		//ZWFUNCTRACE 
		ZWWARN("与锁具之间的通讯线程启动")
		boost::mutex::scoped_lock lock(thr_mutex);

		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];			
			int outLen = 0;
			myOpenElock1503(&zwccbthr::hidHandle);
			time_t lastOpenElock=time(NULL);
			//Open(1);
			while (1) {
				memset(recvBuf, 0, BLEN + 1);
				//printf("###############JCCOMMTHREAD 327 RUNNING\n");
				//每隔多少秒才重新检测并打开电子锁一次
				if (time(NULL)-lastOpenElock>6)
				{
					lastOpenElock=time(NULL);
					myOpenElock1503(&zwccbthr::hidHandle);
				}
				
				time_t thNow=time(NULL);
				if (thNow % 3 ==0)
				{
					OutputDebugStringA(("电子锁数据接收线程仍在运行中"));
				}
				if (thNow % 60 ==0)
				{
					ZWINFO("电子锁数据接收线程仍在运行中");
				}
					
				try {
					boost::this_thread::interruption_point();
					if (NULL==zwccbthr::hidHandle.hid_device)
					{
						Sleep(700);
						continue;
					}
					JCHID_STATUS sts=JCHID_STATUS_FAIL;
					if (NULL!=zwccbthr::hidHandle.hid_device)
					{						
						{
							//boost::mutex::scoped_lock lock(recv_mutex);
							OutputDebugStringA("415接收一条锁具返回消息开始\n");
							sts=jcHidRecvData(&zwccbthr::hidHandle,
								recvBuf, BLEN, &outLen,JCHID_RECV_TIMEOUT);
							OutputDebugStringA("415接收一条锁具返回消息结束\n");
						}						
						//myCloseElock1503();
					}
					//zwCfg::s_hidOpened=true;	//算是通信线程的一个心跳标志					
					//要是什么也没收到，就直接进入下一个循环
					if (JCHID_STATUS_OK!=sts)
					{
						Sleep(900);
						continue;
					}
					printf("\n");
					//////////////////////////////////////////////////////////////////////////
				}
				catch(boost::thread_interrupted &)
				{
					ZWERROR	("RecvData从电子锁接收数据时到遇到线程收到终止信号，数据接收线程将终止");
					return;
				}
				catch(...) {
					ZWFATAL ("RecvData从电子锁接收数据时到遇到线路错误或者未知错误，数据接收线程将终止");
					return;
				}
				

				boost::this_thread::interruption_point();
				string outXML;
				if (strlen(recvBuf)>0){
					jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,
						outXML);
					//ZWINFO("分析锁具回传的Json并转换为建行XML成功");
					//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
					assert(outXML.length() > 42);
					//ZWDBGMSG(outXML.c_str());
					{
						boost::mutex::scoped_lock lock(recv_mutex);
						//收到的XML存入队列
						dqOutXML.push_back(outXML);
					}
				}

				if (NULL==zwCfg::g_WarnCallback)
				{
					const char *err1="回调函数指针为空，无法调用回调函数返回从电子锁收到的报文";
					ZWERROR(err1);
					MessageBoxA(NULL,err1,"严重警告",MB_OK);
				}
				if (outXML.size()==0)
				{
					ZWERROR("收到的锁具返回内容为空，无法返回有用信息给回调函数");
				}
				if (NULL != zwCfg::g_WarnCallback && outXML.size()>0) {
					//调用回调函数传回信息，
					//20150415.1727.为了万敏的要求，控制上传消息速率最多每2秒一条防止ATM死机
					Sleep(2000);
					zwCfg::g_WarnCallback(outXML.c_str());
#ifdef _DEBUG401
					ZWINFO("成功把从锁具接收到的数据传递给回调函数");
#endif // _DEBUG401
				} 					
				boost::this_thread::interruption_point();
			}
			ZWINFO("金储通信数据接收线程正常退出");

		}		//try
		catch(...) {			
			//异常断开就设定该标志为FALSE,以便下次Open不要再跳过启动通信线程的程序段
			zwCfg::s_hidOpened=false;
			ZWFATAL("金储通信数据接收线程数据连接异常断开，现在数据接收线程将结束");
			return;
		}	
	}


//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

CCBELOCK_API int zwPushString( const char *str )
{
	//ZWFUNCTRACE 
	assert(NULL != str && strlen(str) > 0);
	if (NULL == str || strlen(str) == 0) {
		return JCHID_STATUS_FAIL;
	}

		JCHID_STATUS sts=JCHID_STATUS_FAIL;
		static time_t lastPrint=time(NULL);
		{			
			//20150415.1727.为了万敏的要求，控制下发消息速率最多每秒一条防止下位机死机
			//Sleep(1000);
			boost::mutex::scoped_lock lock(zwccbthr::recv_mutex);
			sts=jcHidSendData(&zwccbthr::hidHandle, str, strlen(str));
		}
		
		if (time(NULL)-lastPrint>6)
		{
			if (JCHID_STATUS_OK==sts)
			{
				ZWINFO("检测到锁具在线")
			}
			else
			{
				ZWINFO("检测到锁具离线")
			}
			lastPrint=time(NULL);
		}
	
		return sts;
}

