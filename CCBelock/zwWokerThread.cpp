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
using jchidDevice2015::jcHidDevice;

jcHidDevice *g_jhc=NULL;	//实际的HID设备类对象

namespace zwccbthr {
	//建行给的接口，没有设置连接参数的地方，也就是说，完全可以端口，抑或是从配置文件读取
	boost::mutex thrhid_mutex;
	void pushToCallBack( const char * recvBuf );

	void wait(int milliseconds) {
		boost::this_thread::sleep(boost::
					  posix_time::milliseconds
					  (milliseconds));
	} 
	
	//与锁具之间的通讯线程
	void ThreadLockComm() {
		//ZWFUNCTRACE 
		ZWWARN("与锁具之间的通讯线程启动v718")
		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];			
			//每隔几秒钟重新打开一次
			time_t lastOpenElock=time(NULL);
			//每隔一二十分钟，最多不出半小时自动强制关闭一次
			//因为HID连接似乎到一个小时就会有时候失去反应；
			while (1) {		
								
				try {
					boost::this_thread::interruption_point();
					JCHID_STATUS sts=JCHID_STATUS_FAIL;
					{
						boost::mutex::scoped_lock lock(thrhid_mutex);
#ifdef _DEBUG
						ZWINFO("thrhid_mutex START")
#endif // _DEBUG
							//每隔多少秒才重新检测并打开电子锁一次
							if ((time(NULL)-lastOpenElock)>(60))
							{									
								if (ELOCK_ERROR_SUCCESS!=g_jhc->getConnectStatus())
								{
									ZWWARN("1551每隔1分钟定期检测和重新连接电子锁防止异常断线\n");
									ZWWARN("真正关闭连接后再次打开连接508")
									g_jhc->CloseJc();
									g_jhc->OpenJc();							
								}					
								lastOpenElock=time(NULL);							
							}						
						memset(recvBuf, 0, BLEN + 1);
						sts=g_jhc->RecvJson(recvBuf,BLEN);							
						if (strlen(recvBuf)>0)
						{
							ZWWARN("收到锁具返回消息=")
							ZWWARN(recvBuf)
							string outXML;
							jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);							
							pushToCallBack(outXML.c_str());	//传递给回调函数
						}
			
#ifdef _DEBUG
						ZWINFO("thrhid_mutex END")
#endif // _DEBUG
					}	//end if (s_jsonCmd.length()>0)
					//要是什么也没收到，就直接进入下一个循环
					if (JCHID_STATUS_OK!=sts)
					{
						Sleep(900);
						continue;
					}
					printf("\n");
					//////////////////////////////////////////////////////////////////////////
				}	//end of try {
				catch(boost::thread_interrupted &)
				{
					g_jhc->CloseJc();
					ZWERROR	("RecvData从电子锁接收数据时到遇到线程收到终止信号，数据接收线程将终止");
					return;
				}
				catch(...) {
					g_jhc->CloseJc();
					ZWFATAL ("RecvData从电子锁接收数据时到遇到线路错误或者未知错误，数据接收线程将终止");
					return;
				}
			}	//end of while (1) {	
			ZWINFO("金储通信数据接收线程正常退出");

		}		//try
		catch(...) {			
			//异常断开就设定该标志为FALSE,以便下次Open不要再跳过启动通信线程的程序段
			g_jhc->CloseJc();
			ZWFATAL("金储通信数据接收线程数据连接异常断开，现在数据接收线程将结束");
			return;
		}	
	}

	void pushToCallBack( const char * recvConvedXML )
	{
		if (NULL==recvConvedXML || strlen(recvConvedXML)==0)
		{
			ZWERROR("收到的锁具返回内容为空，无法返回有用信息给回调函数");
			ZWWARN(recvConvedXML);
		}

		
		if (strlen(recvConvedXML)>0){		
			//ZWINFO("分析锁具回传的Json并转换为建行XML成功");
			//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
			assert(strlen(recvConvedXML) > 42);
		}

		if (NULL==zwCfg::g_WarnCallback)
		{
			const char *err1="回调函数指针为空，无法调用回调函数返回从电子锁收到的报文";
			ZWERROR(err1);
			MessageBoxA(NULL,err1,"严重警告",MB_OK);
		}
		if (NULL != zwCfg::g_WarnCallback && strlen(recvConvedXML)>0) {
			//调用回调函数传回信息，
			//20150415.1727.为了万敏的要求，控制上传消息速率最多每2秒一条防止ATM死机
			Sleep(920);
			zwCfg::g_WarnCallback(recvConvedXML);
#ifdef _DEBUG401
			ZWINFO("成功把从锁具接收到的数据传递给回调函数");
#endif // _DEBUG401
		}
	}



//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

