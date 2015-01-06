#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"
#include "zwHidComm.h"
#include "CCBelock.h"
using namespace boost::property_tree;
using Poco::AutoPtr;
using Poco::Util::IniFileConfiguration;
using boost::mutex;
namespace zwccbthr{
	extern boost::mutex thr_mutex;
	extern JCHID hidHandle;
	extern boost::mutex recv_mutex;
	extern std::deque < string > dqOutXML;
}


namespace zwCfg {
	extern const long JC_CCBDLL_TIMEOUT;	//最长超时时间为30秒,用于测试目的尽快达到限制暴露问题
	//定义一个回调函数指针
	extern RecvMsgRotine g_WarnCallback;
	extern boost::mutex ComPort_mutex;	//用于保护串口连接对象
	//线程对象作为一个全局静态变量，则不需要显示启动就能启动一个线程
	extern boost::thread * thr;
	extern bool s_hidOpened;
} //namespace zwCfg{  

namespace jcLockJsonCmd_t2015a{
	//从long Notify(const char *pszMsg)修改而来
	long NotifyJson(const char *pszMsg)
	{
		//通过在Notify函数开始检测是否端口已经打开，没有打开就直接返回，避免
		//2014年11月初在广州遇到的没有连接锁具时，ATMC执行0002报文查询锁具状态，
		//反复查询，大量无用日志产生的情况。
		if (false == zwCfg::s_hidOpened) {
			return ELOCK_ERROR_CONNECTLOST;
		}
		ZWFUNCTRACE assert(pszMsg != NULL);
		assert(strlen(pszMsg) >= 42);	//XML至少42字节utf8
		if (pszMsg == NULL || strlen(pszMsg) < 42) {
			return ELOCK_ERROR_PARAMINVALID;
		}
		if (NULL != pszMsg && strlen(pszMsg) > 0) {
			pocoLog->information() << "CCB下发XML=" << endl << pszMsg <<
				endl;
		}
		boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
		string strJsonSend;
		try {
			//输入必须有内容，但是最大不得长于下位机内存大小，做合理限制
			assert(NULL != pszMsg);
			if (NULL == pszMsg) {
				ZWERROR("Notify输入为空")
					return ELOCK_ERROR_PARAMINVALID;
			}
			int inlen = strlen(pszMsg);
			assert(inlen > 0 && inlen < JC_MSG_MAXLEN);
			//if (inlen == 0 || inlen >= zwCfg::JC_MSG_MAXLEN) {
			//	ZWWARN("Notify输入超过最大最小限制");
			//	return ELOCK_ERROR_PARAMINVALID;
			//}
			//////////////////////////////////////////////////////////////////////////
			string strXMLSend = pszMsg;
			assert(strXMLSend.length() > 42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
			jcAtmcConvertDLL::zwCCBxml2JCjson(strXMLSend, strJsonSend);
			assert(strJsonSend.length() > 9);	//json最基本的符号起码好像要9个字符左右
			ZWNOTICE(strJsonSend.c_str());
			Sleep(50);
			zwPushString(strJsonSend.c_str());
			return ELOCK_ERROR_SUCCESS;
		}
		catch(ptree_bad_path & e) {
			ZWERROR(e.what());
			ZWERROR("CCB下发XML有错误节点路径")
				return ELOCK_ERROR_NOTSUPPORT;
		}
		catch(ptree_bad_data & e) {
			ZWERROR(e.what());
			ZWERROR("CCB下发XML有错误数据内容")
				return ELOCK_ERROR_PARAMINVALID;
		}
		catch(ptree_error & e) {
			ZWERROR(e.what());
			ZWERROR("CCB下发XML有其他未知错误")
				return ELOCK_ERROR_CONNECTLOST;
		}
		catch(...) {		//一切网络异常都直接返回错误。主要是为了捕捉未连接时
			//串口对象为空造成访问NULL指针的的SEH异常  
			//为了使得底层Poco库与cceblock类解耦，从我的WS类
			//发现WS对象因为未连接而是NULL时直接throw一个枚举
			//然后在此，也就是上层捕获。暂时不知道捕获精确类型
			//所以catch所有异常了
			ZWFATAL(__FUNCTION__);
			ZWFATAL("Notify通过线路发送数据异常，可能网络故障")
				return ELOCK_ERROR_CONNECTLOST;
		}
	}

	//从void ThreadLockComm() 修改而来
	//与锁具之间的通讯线程
	void ThreadLockCommJson() {
		ZWFUNCTRACE boost::mutex::scoped_lock lock(zwccbthr::thr_mutex);

		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];
			memset(recvBuf, 0, BLEN + 1);
			int outLen = 0;
			while (1) {
#ifndef ZWUSE_HID_MSG_SPLIT
				if (NULL == zwComPort) {
					ZWNOTICE
						("线路已经关闭，通信线程将退出");
					return;
				}
#endif // ZWUSE_HID_MSG_SPLIT
				ZWNOTICE("连接锁具成功");
				ZWINFO("通信线程的新一轮等待接收数据循环开始");
				try {
#ifdef ZWUSE_HID_MSG_SPLIT
					JCHID_STATUS sts=
						jcHidRecvData(&zwccbthr::hidHandle,
						recvBuf, BLEN, &outLen,JCHID_RECV_TIMEOUT);
					zwCfg::s_hidOpened=true;	//算是通信线程的一个心跳标志					
					//要是什么也没收到，就直接进入下一个循环
					if (JCHID_STATUS_OK!=sts)
					{
						printf("JCHID_STATUS 1225 %d\n",sts);
						Sleep(1000);
						continue;
					}
					printf("\n");
#else
					zwComPort->RecvData(recvBuf, BLEN,
						&outLen);
#endif // ZWUSE_HID_MSG_SPLIT

					//////////////////////////////////////////////////////////////////////////

					ZWNOTICE("成功从锁具接收数据如下：");
				}
				catch(...) {
					ZWFATAL
						("RecvData接收数据时到锁具的数据连接异常断开，数据接收线程将终止");
					return;
				}
				ZWNOTICE(recvBuf);

				string outXML;
				jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,
					outXML);
				ZWINFO("分析锁具回传的Json并转换为建行XML成功");
				//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
				assert(outXML.length() > 42);
				ZWDBGMSG(outXML.c_str());
				{
					boost::
						mutex::scoped_lock lock(zwccbthr::recv_mutex);
					//收到的XML存入队列
					zwccbthr::dqOutXML.push_back(outXML);
				}

				if (NULL != zwCfg::g_WarnCallback) {
					//调用回调函数传回信息，然后就关闭连接，结束通信线程；
					zwCfg::g_WarnCallback(outXML.c_str());
					ZWINFO
						("成功把从锁具接收到的数据传递给回调函数");
				} else {
					ZWWARN
						("回调函数指针为空，无法调用回调函数")
				}
			}
			ZWINFO("金储通信数据接收线程正常退出");

		}		//try
		catch(...) {			
			//异常断开就设定该标志为FALSE,以便下次Open不要再跳过启动通信线程的程序段
			zwCfg::s_hidOpened=false;
			ZWFATAL
				("金储通信数据接收线程数据连接异常断开，现在数据接收线程将结束");
			return;
		}	
	}

}	//end of namespace jcLockJsonCmd_t2015a{