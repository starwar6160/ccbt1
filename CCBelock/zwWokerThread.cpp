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

namespace jcAtmcConvertDLL {
	//为了匹配上下行报文避免答非所问做的报文类型标志位
	extern string s_pipeJcCmdDown;	
	extern string s_pipeJcCmdUp;
}

namespace zwccbthr {
	//建行给的接口，没有设置连接参数的地方，也就是说，完全可以端口，抑或是从配置文件读取
	boost::mutex thrhid_mutex;
	void pushToCallBack( const char * recvBuf );
	deque<string> g_dqLockUpMsg;	//锁具主动上送的答非所问消息的临时队列
	bool myDownUpLoopIng=false;	//为了维持一个下发/上行循环的完整

	void wait(int milliseconds) {
		boost::this_thread::sleep(boost::
					  posix_time::milliseconds
					  (milliseconds));
	} 
	
	//与锁具之间的通讯线程
	void ThreadLockComm() {
		//ZWFUNCTRACE 
		ZWWARN("与锁具之间的通讯线程启动v731")
		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];			
			//每隔几秒钟重新打开一次
			time_t lastOpenElock=time(NULL);
			time_t lastPopUpMsg=time(NULL);
			
			//每隔一二十分钟，最多不出半小时自动强制关闭一次
			//因为HID连接似乎到一个小时就会有时候失去反应；
			while (1) {		
								
				try {
					boost::this_thread::interruption_point();
					JCHID_STATUS sts=JCHID_STATUS_FAIL;
					{
						boost::mutex::scoped_lock lock(thrhid_mutex);
#ifdef _DEBUG
						VLOG(4)<<"thrhid_mutex START"<<endl;
#endif // _DEBUG
							//每隔多少秒才重新检测并打开电子锁一次
							if ((time(NULL)-lastOpenElock)>(60*1))
							{									
								//if (ELOCK_ERROR_SUCCESS!=g_jhc->getConnectStatus())
								{
									ZWINFO("0908每隔1分钟定期检测和重新连接电子锁防止异常断线\n");
									ZWWARN("真正关闭连接后再次打开连接512")
									g_jhc->CloseJc();
									g_jhc->OpenJc();							
								}												
								lastOpenElock=time(NULL);							
							}						

							//定期上传报警信息，如果有的话.并且要此时不在一问一答周期内
							if ((time(NULL)-lastPopUpMsg)>(20) && zwccbthr::myDownUpLoopIng==false)
							{
								VLOG_IF(3,g_dqLockUpMsg.size()>0)<<"g_dqLockUpMsg.size()="<<g_dqLockUpMsg.size()<<endl;
								for (auto iter=g_dqLockUpMsg.begin();iter!=g_dqLockUpMsg.end();iter++)
								{
									ZWWARN("每隔20秒弹出前面暂存的锁具主动上送信息给回调函数")
										ZWWARN((*iter).c_str())
										pushToCallBack((*iter).c_str());
								}
								g_dqLockUpMsg.clear();
								lastPopUpMsg=time(NULL);
							}
//////////////////////////////////////////////////////////////////////////
						//假定锁具主动上送报文不超过3条。一般应该也就2条
						int upMsgCount=0;
						do
						{
							upMsgCount++;
							if (upMsgCount>19)
							{
								break;
							}
							memset(recvBuf, 0, BLEN + 1);
							sts=g_jhc->RecvJson(recvBuf,BLEN);							
							if (strlen(recvBuf)>0)
							{
								ZWINFO("收到锁具返回消息=")
								ZWINFO(recvBuf)
								string outXML;
								jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);							
								//单条锁具主动上送报文不涉及答非所问问题的,以及锁具上送闭锁码和验证码两条都放行
								if(zwccbthr::myDownUpLoopIng==false 
									&& (jcAtmcConvertDLL::s_pipeJcCmdUp=="Lock_Open_Ident" ||
									jcAtmcConvertDLL::s_pipeJcCmdUp=="Lock_Close_Code_Lock" )	)
								{
									//Lock_Open_Ident这条报文是开锁的关键部分验证码报文，不该被压下
									//所以在这里提前上送
									ZWWARN("不在一问一答期间的单条锁具主动上送报文不涉及答非所问问题")
										pushToCallBack(outXML.c_str());	//传递给回调函数
								}

								//正确的对应下发报文的回应报文
								if(jcAtmcConvertDLL::s_pipeJcCmdDown==jcAtmcConvertDLL::s_pipeJcCmdUp)
								{									
									DLOG(WARNING)<<"返回正确对口报文给上层"<<endl;
									//传递给回调函数
									pushToCallBack(outXML.c_str());	
									jcAtmcConvertDLL::s_pipeJcCmdDown="";
									//一问一答周期完成
									zwccbthr::myDownUpLoopIng=false;	
									break;
								}								

								//锁具主动上送报文答非所问,以及特别处理报警信息及时压下
								if((jcAtmcConvertDLL::s_pipeJcCmdDown!=jcAtmcConvertDLL::s_pipeJcCmdUp
									&& jcAtmcConvertDLL::s_pipeJcCmdDown!="") ||
									jcAtmcConvertDLL::s_pipeJcCmdUp=="Lock_Alarm_Info")
								{									
									ZWWARN("答非所问,暂存起来晚些时候再传给回调函数")	
									ZWWARN(jcAtmcConvertDLL::s_pipeJcCmdDown)
									ZWWARN(jcAtmcConvertDLL::s_pipeJcCmdUp)
									g_dqLockUpMsg.push_back(outXML);
								}
							}
							else
							{
								//收不到什么东西就立刻跳出循环
								VLOG(4)<<"HID读取没有任何数据，跳出循环"<<endl;
								break;
							}
						}while(strlen(recvBuf)>0);						

//////////////////////////////////////////////////////////////////////////
			
#ifdef _DEBUG
						VLOG(4)<<"thrhid_mutex END"<<endl;
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
			return;
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
			Sleep(2920);
			zwCfg::g_WarnCallback(recvConvedXML);
			VLOG_IF(3,strlen(recvConvedXML)>0)<<"回调函数收到以下内容\n"<<recvConvedXML<<endl;
#ifdef _DEBUG401
			ZWINFO("成功把从锁具接收到的数据传递给回调函数");
#endif // _DEBUG401
		}
	}



//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

