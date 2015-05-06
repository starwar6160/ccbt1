#include "stdafx.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"
#include "zwHidSplitMsg.h"
#include "zwHidComm.h"
#include "zwHidDevClass2015.h"
#include <stdio.h>
#include <deque>
using namespace boost::property_tree;

int myOpenElock1503(JCHID *jcElock);
void myCloseElock1503(void);

namespace jcAtmcConvertDLL {
	//为了匹配上下行报文避免答非所问做的报文类型标志位
	extern string s_pipeJcCmdDown;	
	extern string s_pipeJcCmdUp;
}

namespace zwccbthr {
	//建行给的接口，没有设置连接参数的地方，也就是说，完全可以端口，抑或是从配置文件读取
	boost::mutex recv_mutex;
	JCHID hidHandle;

	void wait(int milliseconds) {
		boost::this_thread::sleep(boost::
					  posix_time::milliseconds
					  (milliseconds));
	} 
	
	//与锁具之间的通讯线程
	void ThreadLockComm() {
		//ZWFUNCTRACE 
		ZWWARN("与锁具之间的通讯线程启动")

		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];			
			int outLen = 0;
			myOpenElock1503(&zwccbthr::hidHandle);
			//每隔几秒钟重新打开一次
			time_t lastOpenElock=time(NULL);
			//每隔一二十分钟，最多不出半小时自动强制关闭一次
			//因为HID连接似乎到一个小时就会有时候失去反应；
			time_t lastCloseElock=time(NULL);
			//Open(1);
			while (1) {
				memset(recvBuf, 0, BLEN + 1);
				//printf("###############JCCOMMTHREAD 327 RUNNING\n");
				 
				{
#ifdef _DEBUG430
					//强制关闭连接会导致后续收不到数据，暂时不这么做了
					boost::mutex::scoped_lock lock(recv_mutex);
					//15分钟强制关闭一次,防止一个小时以上HID连接失效
					if ((time(NULL)-lastCloseElock)>19)
					{			
						//myCloseElock1503();
						lastCloseElock=time(NULL);						
						//ZWWARN("20150430.每隔4分钟左右自动强制断开HID连接防止连接失效，3秒以后恢复")
						//Sleep(3000);
						continue;
					}
#endif // _DEBUG430


					//每隔多少秒才重新检测并打开电子锁一次
					if ((time(NULL)-lastOpenElock)>10)
					{					
						myOpenElock1503(&zwccbthr::hidHandle);
						lastOpenElock=time(NULL);
					}
					
				}
				
				time_t thNow=time(NULL);
#ifdef _DEBUG417
				if (thNow % 6 ==0)
				{
					OutputDebugStringA(("电子锁数据接收线程仍在运行中"));
				}
				if (thNow % 60 ==0)
				{
					ZWINFO("电子锁数据接收线程仍在运行中");
				}
#endif // _DEBUG417
					
				try {
					boost::this_thread::interruption_point();
					if (NULL==zwccbthr::hidHandle.hid_device)
					{
						Sleep(300);
						continue;
					}
					JCHID_STATUS sts=JCHID_STATUS_FAIL;
					if (NULL!=zwccbthr::hidHandle.hid_device)
					{						
						{
							boost::mutex::scoped_lock lock(recv_mutex);
							//OutputDebugStringA("415接收一条锁具返回消息开始\n");
							sts=jcHidRecvData(&zwccbthr::hidHandle,
								recvBuf, BLEN, &outLen,JCHID_RECV_TIMEOUT);
							//OutputDebugStringA("415接收一条锁具返回消息结束\n");
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
					if (jcAtmcConvertDLL::s_pipeJcCmdDown!=jcAtmcConvertDLL::s_pipeJcCmdUp)
					//if(jcAtmcConvertDLL::s_pipeJcCmdDown.size()>0)
					{
						ZWERROR("%%%%430jcAtmcConvertDLL::s_pipeJcCmdDown!=jcAtmcConvertDLL::s_pipeJcCmdUp\n");
						ZWWARN(jcAtmcConvertDLL::s_pipeJcCmdDown)
						ZWWARN(jcAtmcConvertDLL::s_pipeJcCmdUp)
					}
					//ZWINFO("分析锁具回传的Json并转换为建行XML成功");
					//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
					assert(outXML.length() > 42);
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
					Sleep(920);
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
		ZWERROR("zwPushString input string is NULL")
		return JCHID_STATUS_FAIL;
	}

		JCHID_STATUS sts=JCHID_STATUS_FAIL;
		//Sleep(1000);		
		static time_t lastPrint=time(NULL);
		{			
			//20150415.1727.为了万敏的要求，控制下发消息速率最多每秒一条防止下位机死机			
			//20150421.0935.应万敏的要求，下发消息延迟放到互斥加锁内部
			Sleep(1000);		
			sts=jcHidSendData(&zwccbthr::hidHandle, str, strlen(str));
		}
		
		if (time(NULL)-lastPrint>6)
		{
			if (JCHID_STATUS_OK==sts)
			{
				//ZWINFO("检测到锁具在线")
			}
			else
			{
				ZWINFO("检测到锁具离线")
			}
			lastPrint=time(NULL);
		}
	
		return sts;
}

