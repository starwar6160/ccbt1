//2016/7/6 17:20:05 [星期三] 当前ATMC DLL的收据收发线程处理逻辑：
//	Notify函数下发报文，用一个互斥锁thrhid_mutex和数据收发线程互斥，避免在收发过程中
//	把报文加入下发队列s_jcNotify，同时还有一个标志位s_bPendingNotify用来标志当前有
//	已经下发但是由于互斥尚未加入下发队列s_jcNotify的报文(用于避免下发报文和锁具
//	单向上传报文撞车造成上层ATMC系统“报文解析错误”)
//	数据收发线程my706LockRecvThr循环处理报文，从下发队列s_jcNotify取出一条报文，
//	发送给锁具，然后看看该报文类型是不是锁具主动上送报文的确认报文，如果是，
//	?就不去读取锁具的返回报文了；
//	如果不是，则读取锁具返回报文，获取返回报文类型upType，将返回报文转换为
//	outXML建行要求的XML格式作为outXML，获取下发报文的报文类型downType；
//	如果upType和downType相等说明这是一条正常下发报文的匹配返回报文(潘飞保证
//	按照顺序处理和返回报文，而且不会丢失报文),则将其通过回调函数返回给
//	上层ATMC应用；否则说明这是一条锁具主动上送报文，需要延迟上传以免打破
//	一问一答，造成上层应用“报文解析错误",所以将其放到单独的上传队列s_jcLockToC里面；
//	读取完毕返回报文后，下发队列s_jcNotify可以弹出刚才处理完毕的报文了；
//	然后开始上传单向上传报文，条件是下发队列为空，最后一条下发消息的回应消息
//	已经上传后过了500毫秒，而且Notify函数那里没有由于线程锁的缘故等待进入
//	下发队列的下发报文，那么就可以通过回调函数把单向上传报文传递给上层应用了；

#include "stdafx.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
#include "zwHidSplitMsg.h"
#include "zwHidComm.h"
#include "zwHidDevClass2015.h"
#include ".\\ATMCMsgConvert\\myConvIntHdr.h"
#include <stdio.h>
#include <deque>
#include <iomanip>
using namespace boost::property_tree;
using boost::condition_variable;
using boost::condition_variable_any;
using jchidDevice2015::jcHidDevice;
using jcAtmcConvertDLL::jcLockMsg1512_t;
using std::setprecision;

#define MYFD1	std::setprecision(0)<<std::setiosflags(std::ios::fixed)

jcHidDevice *g_jhc=NULL;	//实际的HID设备类对象

namespace zwccbthr {	
	//建行给的接口，没有设置连接参数的地方，也就是说，完全可以端口，抑或是从配置文件读取
	boost::mutex thrhid_mutex;
	void pushToCallBack( const char * recvConvedXML,RecvMsgRotine pRecvMsgFun );	
	deque<jcLockMsg1512_t *> s_jcNotify;		//下发命令队列
	deque<jcLockMsg1512_t *> s_dbgMatchNotify;		//下发命令队列复制品，用于调试匹配
	RecvMsgRotine s_CallBack=NULL;
	double s_LastNormalMsgUpTimeMs=0.0;	//最后一次正常循环报文上传的时间，毫秒计算
	//消息下发延迟，防止下发过快导致下位机处理不过来，经过测试100毫秒的话
	// 连续下发1500条就会有220条的下发队列；
	int s_MsgNotifyDelay=150;	
	time_t s_LastUpload=0;	//记录最后一次上传报文的时间，检测锁具挂掉之用

	void wait(int milliseconds) {
		assert(milliseconds>0);
		boost::this_thread::sleep(boost::
					  posix_time::milliseconds
					  (milliseconds));
	} 

	time_t zwNormalTime521(time_t inTime,time_t gapSeconds)
	{
		assert(inTime>0);
		assert(gapSeconds>1);
		time_t tail=inTime % gapSeconds;
		return inTime-tail;

	}

	//根据下发队列大小调整延迟
	int myAdjustNotifyDelay(int dqNotifySize)
	{
		if (dqNotifySize>=0 && dqNotifySize<10)
		{
			return 100;
		}
		if (dqNotifySize>=10 && dqNotifySize<20)
		{
			return 200;
		}
		if (dqNotifySize>=20)
		{
			return 300;
		}
		return 120;	//其他非法输入返回120毫秒还算比较安全
	}


	void pushToCallBack( const char * recvConvedXML,RecvMsgRotine pRecvMsgFun )
	{
		static double lastUpMsg=0;	//最后一条上传报文的时间戳，用于检测过长的间隔
		static int nUpCount=0;	//第多少条上传报文
		static int nExceedCount=0;	//超过2秒间隔的计数器
		static double nExceedMsTotal=0;	//超过2秒间隔的共有多少毫秒
		static double nExceedMaxMs=0;	//超过2秒间隔的最大多少毫秒
		assert(NULL!=recvConvedXML);
		assert(strlen(recvConvedXML)>0);
		if (NULL==recvConvedXML || strlen(recvConvedXML)==0)
		{
			ZWERROR("收到的锁具返回内容为空，无法返回有用信息给回调函数");
			return;
		}		

		if (strlen(recvConvedXML)>0){		
			LOG_IF(ERROR,strlen(recvConvedXML) <= 42)<<"XML长度不足42字符，不是合法XML"<<endl;
			//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
			assert(strlen(recvConvedXML) > 42);
		}

		if (NULL==pRecvMsgFun)
		{
			const char *err1="回调函数指针为空，无法调用回调函数返回从电子锁收到的报文20151221";
			ZWERROR(err1);
			MessageBoxA(NULL,err1,"严重警告",MB_OK);
		}
		if (NULL != pRecvMsgFun && strlen(recvConvedXML)>0) {
			//调用回调函数传回信息，
			//20150415.1727.为了万敏的要求，控制上传消息速率最多每2秒一条防止ATM死机
			//Sleep(2920);
			pRecvMsgFun(recvConvedXML);
			s_LastUpload=time(NULL);
			LOG(WARNING)<<"上位机回调函数已经收到 "<<recvConvedXML<<endl;

#ifdef MY_CCBXML_MODE1609
			string upMsgType= jcAtmcConvertDLL::zwGetJcxmlMsgType(recvConvedXML);
			if (s_dbgMatchNotify.size()>0)
			{
				VLOG(3)<<"s_dbgMatchNotify.size()="<<s_dbgMatchNotify.size()<<endl;
				for(size_t i=0;i<s_dbgMatchNotify.size();i++)
				{
					
					string downType=s_dbgMatchNotify.front()->getNotifyNumType();
					LOG_IF(ERROR,downType!=upMsgType)<<"下行报文类型"<<downType<<"不匹配上行报文类型"<<upMsgType<<endl;
					s_dbgMatchNotify.pop_front();
					if (downType==upMsgType)
					{
						LOG(INFO)<<"pushToCallBack中上下行报文匹配了一对,现在跳出逐个匹配s_dbgMatchNotify队列的循环"<<endl;
						break;
					}
				}				
			}

			nUpCount++;
			double curMs=zwccbthr::zwGetMs();
			double diffMs=curMs-lastUpMsg;
			
			if (diffMs>30000 && lastUpMsg>0)
			{
				nExceedCount++;
				nExceedMsTotal+=diffMs;
				VLOG_IF(3,diffMs>nExceedMaxMs)<<"报文间隔异常达到"<<setprecision(0)<<diffMs<<"毫秒"<<endl;
				if (diffMs>nExceedMaxMs)
				{
					nExceedMaxMs=diffMs;
				}				
			}
			LOG_IF(WARNING,(nUpCount%20==0))<<"报文之间间隔时间"<<setprecision(0)<<diffMs<<"毫秒，当前第"<<nUpCount<<"条报文"<<endl;				
			VLOG(3)<<"报文之间间隔时间"<<setprecision(0)<<diffMs<<"毫秒，当前第"<<nUpCount<<"条报文"<<endl;				
			LOG_IF(WARNING,(nUpCount%3==0) && nExceedMaxMs>100)<<setprecision(0)<<"最大"<<nExceedMaxMs<<"毫秒 平均"<<setprecision(2)<<nExceedMsTotal/(nExceedCount+0.001)<<"毫秒"<<endl;
			lastUpMsg=curMs;
#endif // MY_CCBXML_MODE1609
		}
#ifdef MY_CCBXML_MODE1609
		for (size_t i=0;i<s_dbgMatchNotify.size();i++)
		{
			LOG(WARNING)<<"s_dbgMatchNotify."<<s_dbgMatchNotify[i]->getNotifyType()<<endl;
		}
#endif // MY_CCBXML_MODE1609
	}
	
	//是否是锁具主动上送报文这样的反向循环报文
	bool myIsJsonMsgFromLockFirstUp(const string &jcMsg)
	{
		if(						
			jcMsg=="Lock_Open_Ident"	  ||
			jcMsg=="Lock_Close_Code_Lock" ||
			jcMsg=="Lock_Time_Sync_Lock" ||
			jcMsg== "Lock_Alarm_Info")
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//搜索字符串判断是否是锁具主动上送字符串
	bool myIsJsonMsgFromLockFirstUpByStr(const string &jcUpJson)
	{
		if(std::string::npos!=jcUpJson.find("Lock_Open_Ident") ||
		std::string::npos!=jcUpJson.find("Lock_Close_Code_Lock") ||
		std::string::npos!=jcUpJson.find("Lock_Time_Sync_Lock") ||
		std::string::npos!=jcUpJson.find("Lock_Alarm_Info"))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	double zwGetUs(void)
	{
		LARGE_INTEGER frq,cur;
		QueryPerformanceCounter(&cur);
		QueryPerformanceFrequency(&frq);
		return 1.0e6*cur.QuadPart/frq.QuadPart;
	}

	double zwGetMs(void)
	{
		return zwGetUs()/1000.0;
	}

	void my912LockRecvXMLThr(void)
	{
	try{
		ZWWARN("与锁具之间的XML透传数据接收线程启动.20160912")
		deque<jcLockMsg1512_t *> s_jcLockToC;		//锁具单向上传队列
		const int BLEN = 1024;
		char recvBuf[BLEN];					
		using zwccbthr::s_jcNotify;
		while (1)
		{	
			boost::this_thread::interruption_point();
			{
//////////////////////////////////////////////////////////////////////////
				boost::mutex::scoped_lock lock(thrhid_mutex);				
				boost::this_thread::interruption_point();
				//下发报文
				if (s_jcNotify.size()>0)
				{
					jcLockMsg1512_t *nItem=s_jcNotify.front();					
					string sCmd=nItem->getNotifyMsg();
					LOG(WARNING)<<"下发给锁具的消息内容是"<<sCmd;
					JCHID_STATUS sts=static_cast<JCHID_STATUS>( g_jhc->SendJson(sCmd.c_str()));
					//断线重连探测机制
					if (JCHID_STATUS_OK!=static_cast<JCHID_STATUS>(sts))
					{						
						g_jhc->OpenJc();
						LOG(WARNING)<<"发生了一次断线重连"<<endl;
					}
					s_jcNotify.pop_front();
					VLOG(3)<<"下发队列头部元素弹出"<<"\t下发队列大小="<<s_jcNotify.size()<<endl;
				}	//if (s_jcNotify.size()>0)	
				//读取返回值
				boost::this_thread::interruption_point();					
				int nReadCount=0;
				while(nReadCount<9)
				{				
				memset(recvBuf,0,BLEN);
				JCHID_STATUS sts=static_cast<JCHID_STATUS>(g_jhc->RecvJson(recvBuf,BLEN));
				LOG_IF(ERROR,JCHID_STATUS_HANDLE_NULL==sts)<<"JCHID_STATUS_HANDLE_NULL 错误发生了，JC锁具打开失败"<<endl;
				if (JCHID_STATUS_HANDLE_NULL==sts)
				{
					LOG(WARNING)<<"等待5秒等待锁具恢复正常"<<endl;
					Sleep(5000);
				}
				if (strlen(recvBuf)>0)
				{										
					LOG(INFO)<<"收到锁具返回消息= "<<recvBuf<<endl;
					RecvMsgRotine pRecvMsgFun=zwccbthr::s_CallBack;
					pushToCallBack(recvBuf,pRecvMsgFun);																
				}
				else
				{
					break;
				}
				}	//while(nReadCount<9)
//////////////////////////////////////////////////////////////////////////
			}	//boost::mutex::scoped_lock 
		}	//while (1)

	}
	catch(boost::property_tree::json_parser_error &e)
	{
		LOG(ERROR)<<"my706LockRecvThr json_parser_error异常:"<<e.what()<<endl;
	}
	catch(boost::property_tree::xml_parser_error &e)
	{
		LOG(ERROR)<<"my706LockRecvThr xml_parser_error异常:"<<e.what()<<endl;
	}
	catch(boost::property_tree::file_parser_error &e)
	{
		LOG(ERROR)<<"my706LockRecvThr file_parser_error异常:"<<e.what()<<endl;
	}
	}


	void my706LockRecvThr(void)
	{
		try{
		ZWWARN("与锁具之间的数据接收线程启动.20160714.v874")
		deque<jcLockMsg1512_t *> s_jcLockToC;		//锁具单向上传队列
		double s_lastNotifyMs=0;
		const int BLEN = 1024;
		char recvBuf[BLEN];					
		using zwccbthr::s_jcNotify;
		while (1)
		{	
			boost::this_thread::interruption_point();
			{
				boost::mutex::scoped_lock lock(thrhid_mutex);		
				zw_trace zntr("报文收发线程同步锁");
//////////////////////////////先处理单向上传队列////////////////////////////////////////////
				double curMs=zwccbthr::zwGetMs();
				VLOG_IF(3,s_jcNotify.size()>0)<<"处理单向上传队列时.下发队列头部元素="<<s_jcNotify.front()->getNotifyType()<<endl;
				VLOG_IF(2,s_jcLockToC.size()>0)<<"单向上传队列大小="<<s_jcLockToC.size()
					<<" 下发队列大小="<<s_jcNotify.size()
					<<" 最后一次上传报文后已经过了="<<(curMs-s_lastNotifyMs)<<setprecision(0)<<"毫秒"<<endl;						
				LOG_IF(WARNING,s_jcLockToC.size()>0)<<"s_jcLockToC.size()="<<s_jcLockToC.size()<<endl;
				while (s_jcLockToC.size()>0)
				{
					zw_trace zntr("单向上传队列处理");
					string sUpMsg;
					jcAtmcConvertDLL::zwJCjson2CCBxml(s_jcLockToC.front()->getNotifyMsg(),sUpMsg);					
					double curMs=zwccbthr::zwGetMs();
					double diffMs= curMs- s_jcLockToC.front()->getNotifyMs();
					RecvMsgRotine pRecvMsgFun=zwccbthr::s_CallBack;
					pushToCallBack(sUpMsg.c_str(),pRecvMsgFun);					
					LOG(WARNING)<<"单向上传报文"<<s_jcLockToC.front()->getNotifyType()
						<<"延迟了"<<setprecision(0)<<diffMs
						<<"毫秒才上传,上传后单向上传队列头部元素弹出,弹出前队列大小是"<<s_jcLockToC.size()<<endl;					
					s_jcLockToC.pop_front();
				}			
				boost::this_thread::interruption_point();
///////////////////////////////下发报文///////////////////////////////////////////
				JCHID_STATUS sts=JCHID_STATUS_FAIL;		
				VLOG_IF(3,s_jcNotify.size()>0)<<"开始下发报文时.下发队列大小="<<s_jcNotify.size()
					<<"\t头部元素="<<s_jcNotify.front()->getNotifyType()<<endl;
				LOG_IF(WARNING,s_jcNotify.size()>20)<<"开始下发报文时.下发队列太长，长度达到了"
					<<s_jcNotify.size()<<"条，需要加大下发延迟了"<<endl;
				LOG_IF(ERROR,s_jcNotify.size()>30)<<"开始下发报文时.下发队列太长，长度达到了"
					<<s_jcNotify.size()<<"条，下发太快锁具忙不过来了"<<endl;
				if (s_jcNotify.size()>50)
				{
					MessageBoxA(NULL,"锁具已经对50条消息失去反应了","FATAL错误",MB_OK);
					break;
				}
				LOG_IF(FATAL,s_jcNotify.size()>50)<<"开始下发报文时.下发队列太长，长度达到了"
					<<s_jcNotify.size()<<"条,锁具应该已经挂了"<<endl;
				if (s_jcNotify.size()>0)
				{
					zw_trace zntr("下发队列处理");
					jcLockMsg1512_t *nItem=s_jcNotify.front();					
					//记录真正下发的时间
					nItem->setInitNotifyMs();
					//放入用于调试匹配的队列
					s_dbgMatchNotify.push_back(nItem);
					string sCmd=nItem->getNotifyMsg();
					
					LOG(INFO)<<"下发给锁具的消息内容是"<<sCmd;
					sts=static_cast<JCHID_STATUS>( g_jhc->SendJson(sCmd.c_str()));
					//断线重连探测机制
					if (JCHID_STATUS_OK!=static_cast<JCHID_STATUS>(sts))
					{						
						g_jhc->OpenJc();
						LOG(WARNING)<<"发生了一次断线重连"<<endl;
					}
					//如果是锁具主动上送报文的返回确认报文，那么下发完毕后不用读取锁具的返回报文
					if(true==myIsJsonMsgFromLockFirstUp(nItem->getNotifyType())) 
					{
						zw_trace zntr("单向上传报文的确认报文的处理");
						if (s_jcNotify.size()>0){
							s_jcNotify.pop_front();
							VLOG(3)<<"锁具单向上送报文的返回确认报文下发之后continue 下发队列头部元素弹出"
								<<"\t下发队列大小="<<s_jcNotify.size()<<endl;
						}
						continue;
					}
				}	//if (s_jcNotify.size()>0)				
				////////////////////////////////读取返回值//////////////////////////////////////////
				
				double msgReadStart=zwccbthr::zwGetMs();
				do 
				{
					boost::this_thread::interruption_point();					
					VLOG(4)<<"读取返回值循环进行中"<<endl;
					memset(recvBuf,0,BLEN);
					sts=static_cast<JCHID_STATUS>(g_jhc->RecvJson(recvBuf,BLEN));
					LOG_IF(ERROR,JCHID_STATUS_HANDLE_NULL==sts)<<"JCHID_STATUS_HANDLE_NULL 错误发生了，JC锁具打开失败"<<endl;
					if (JCHID_STATUS_HANDLE_NULL==sts)
					{
						LOG(WARNING)<<"等待5秒等待锁具恢复正常"<<endl;
						Sleep(5000);
					}
					if (strlen(recvBuf)>0)
					{										
					zw_trace zntr("读到一条回应报文的处理");
					jcLockMsg1512_t *nUpItem=new jcLockMsg1512_t(recvBuf);
					string upType=jcAtmcConvertDLL::zwGetJcJsonMsgType(recvBuf);
					if (myIsJsonMsgFromLockFirstUp(upType)==true)
					{
						LOG(WARNING)<<"收到锁具单向上传消息 "<<recvBuf<<endl;
					}
					else{
						LOG(INFO)<<"收到锁具返回消息= "<<recvBuf<<endl;
					}
					
					string outXML;
					jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);
					VLOG(3)<<"回应XML报文大小="<<outXML.size()<<"\t下发队列大小="<<s_jcNotify.size()<<endl;
					
					if (outXML.size()==0)
					{
						//只有上行json报文非法，而且不是锁具主动上传报文时，才弹出下发队列第一项
						//这样才是正确的
						if (s_jcNotify.size()>0 && false==myIsJsonMsgFromLockFirstUpByStr(recvBuf))
						{
							s_jcNotify.pop_front();
							continue;
						}
					}
						if (outXML.size()>0)
						{							
							zw_trace zntr("回应报文成功转换出回应XML报文的处理");
							VLOG_IF(3,s_jcNotify.size()>0)<<"读取返回值时.下发队列头部元素="<<s_jcNotify.front()->getNotifyType()<<endl;
							if (s_jcNotify.size()>0 && s_jcNotify.front()->matchResponJsonMsg(recvBuf)==true)
							{								
								zw_trace zntr("上下行报文正确匹配的处理");
								RecvMsgRotine pRecvMsgFun=zwccbthr::s_CallBack;
								pushToCallBack(outXML.c_str(),pRecvMsgFun);								
								s_lastNotifyMs=zwccbthr::zwGetMs();
								VLOG(3)<<upType<<"\tJC正常下发消息处理时间"
									<<s_lastNotifyMs-s_jcNotify.front()->getNotifyMs()<<setprecision(1)<<"毫秒"<<endl;
								if (s_jcNotify.size()>0){
									s_jcNotify.pop_front();
									VLOG(3)<<"下发队列头部元素弹出"<<"\t下发队列大小="<<s_jcNotify.size()<<endl;
								}
								VLOG(3)<<"上下行报文匹配正确，现在回应报文传给了回调函数，现在读取循环break"<<endl;
								break;
							}							
							if (myIsJsonMsgFromLockFirstUp(upType)==true)
							{	
								zw_trace zntr("锁具主动上送报文的处理");
								//锁具主动上送报文，暂且放到单独的队列里面有待于延迟处理
								nUpItem->setInitNotifyMs();
								s_jcLockToC.push_back(nUpItem);
								LOG(WARNING)<<" upType="<<upType<<"该报文将会放入另一个队列s_jcLockToC延迟上传,该队列当前大小是"
									<<s_jcLockToC.size()<<",现在读取循环continue"<<endl;
								continue;
							}																																			
						}		
					}	//if (strlen(recvBuf)>0)					
					double curMs=zwccbthr::zwGetMs();
					int nMaxReadMs=900;
					if (s_jcNotify.size()>0)
					{
						string downType= s_jcNotify.front()->getNotifyType();
						if (downType==jcAtmcConvertDLL::JCSTR_LOCK_INIT)
						{
							//20160719.1725.李钺发现初始化时会下发2次1号报文，锁具不是普通的滴滴两声初始完毕，而是4声，我发现
							//原因是预设的1800毫秒读取超时不够，我观察到的2次分别是2300和2500毫秒之后才回应报文的，所以在此
							// 检测如果是初始化报文的1号报文，就延长超时时间
							nMaxReadMs=3000;
						}
#ifdef _JINCHU_DEV1608
						if (downType==jcAtmcConvertDLL::JCSTR_PRV_LOCKUNINSTALL)
						{
							//卸载锁具，测试性5秒超时不够，潘飞说实际大约8秒左右
							// 我就设定在12秒应该够了
							//20160802.1612
							nMaxReadMs=12000;
						}
#endif // _JINCHU_DEV1608
						
						if (downType==jcAtmcConvertDLL::JCSTR_LOCK_ACTIVE_REQUEST ||
							downType==jcAtmcConvertDLL::JCSTR_QUERY_LOCK_STATUS 
							)
						{
							//0号报文大约是需要1200-1650毫秒，1号报文需要2300-2500毫秒，2号报文需要1050-1650毫秒；
							nMaxReadMs=1800;
						}

					}
					if (curMs-msgReadStart>nMaxReadMs && strlen(recvBuf)==0)
					{
						VLOG(4)<<"读取循环开始以后已经过了"<<nMaxReadMs<<"毫秒并且没有读取到任何内容，现在读取循环将break";
						float MYLOCKEXP=(nMaxReadMs*2+1000)/1000;	//锁具最大处理时间目前是2.7秒左右，翻倍大约6秒还没反应就认为锁具挂了
						VLOG_IF(1,(time(NULL)-s_LastUpload>MYLOCKEXP) && MYLOCKEXP>5.0f)
							<<"锁具已经"<<MYLOCKEXP<<"秒没有反应了，应该是挂了801"<<endl;
						break;
					}
				} while (1);
					
			}//boost::mutex::scoped_lock lock(thrhid_mutex);		
			//在互斥锁范围以外留出一点时间给Notify添加新的报文到下发队列
			Sleep(s_MsgNotifyDelay);
			s_MsgNotifyDelay=myAdjustNotifyDelay(s_jcNotify.size());
		}	//end of while(1)
		}
		catch(boost::property_tree::json_parser_error &e)
		{
			LOG(ERROR)<<"my706LockRecvThr json_parser_error异常:"<<e.what()<<endl;
		}
		catch(boost::property_tree::xml_parser_error &e)
		{
			LOG(ERROR)<<"my706LockRecvThr xml_parser_error异常:"<<e.what()<<endl;
		}
		catch(boost::property_tree::file_parser_error &e)
		{
			LOG(ERROR)<<"my706LockRecvThr file_parser_error异常:"<<e.what()<<endl;
		}
	}

//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

