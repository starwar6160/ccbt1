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
	long jcSendJson2Lock(const char *pszJson)
	{
		//通过在Notify函数开始检测是否端口已经打开，没有打开就直接返回，避免
		//2014年11月初在广州遇到的没有连接锁具时，ATMC执行0002报文查询锁具状态，
		//反复查询，大量无用日志产生的情况。
		if (false == zwCfg::s_hidOpened) {
			return ELOCK_ERROR_CONNECTLOST;
		}
		ZWFUNCTRACE 
		//输入必须有内容，但是最大不得长于下位机内存大小，做合理限制
		assert(NULL != pszJson);
		if (NULL == pszJson) {
			ZWERROR("Notify输入为空")
				return ELOCK_ERROR_PARAMINVALID;
		}
		if (NULL != pszJson && strlen(pszJson) > 0) {
			pocoLog->information() << "JinChu下发JSON=" << endl << pszJson <<
				endl;
		}
		boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
		
		try {
			int inlen = strlen(pszJson);
			assert(inlen > 0 && inlen < JC_MSG_MAXLEN);
			if (inlen == 0 || inlen >= JC_MSG_MAXLEN) {
				ZWWARN("notify输入超过最大最小限制");
				return ELOCK_ERROR_PARAMINVALID;
			}
			//////////////////////////////////////////////////////////////////////////
			ZWNOTICE(pszJson);
			Sleep(50);
			zwPushString(pszJson);
			return ELOCK_ERROR_SUCCESS;
		}
		catch(...) {		//一切网络异常都直接返回错误。主要是为了捕捉未连接时
			//串口对象为空造成访问NULL指针的的SEH异常  
			//为了使得底层Poco库与cceblock类解耦，从我的WS类
			//发现WS对象因为未连接而是NULL时直接throw一个枚举
			//然后在此，也就是上层捕获。暂时不知道捕获精确类型
			//所以catch所有异常了
			ZWFATAL(__FUNCTION__);
			ZWFATAL("NotifyJson通过线路发送数据异常，可能网络故障")
				return ELOCK_ERROR_CONNECTLOST;
		}
	}

	//从void ThreadLockComm() 修改而来
	//阻塞接受锁具返回值，3秒超时返回；直接返回收到的JSON数据
	void jcRecvJsonFromLock(char *outJson,const int outMaxLen) {
		ZWFUNCTRACE boost::mutex::scoped_lock lock(zwccbthr::thr_mutex);

		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];
			memset(recvBuf, 0, BLEN + 1);
			int recvLen = 0;
			//while (1) {
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
						recvBuf, BLEN, &recvLen,JCHID_RECV_TIMEOUT);
					zwCfg::s_hidOpened=true;	//算是通信线程的一个心跳标志					
					//要是什么也没收到，就直接进入下一个循环
					if (JCHID_STATUS_OK!=sts)
					{
						printf("JCHID_STATUS No DATA RECVED %d\n",sts);
						return;						
					}
					printf("\n");
#else
					zwComPort->RecvData(recvBuf, BLEN,
						&recvLen);
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
			//返回数据
				//收到的数据长度大于输出缓冲区大小就截断输出
				if (recvLen>outMaxLen)
				{
					strncpy(outJson,recvBuf,outMaxLen);
				}
				else
				{
					strncpy(outJson,recvBuf,recvLen);
				}
				
			//}	//while (1) {
			ZWINFO("金储通信数据接收过程正常结束");

		}		//try
		catch(...) {			
			//异常断开就设定该标志为FALSE,以便下次Open不要再跳过启动通信线程的程序段
			zwCfg::s_hidOpened=false;
			ZWFATAL
				("金储通信数据接收过程中数据连接异常断开，现在数据接收过程异常结束");
			return;
		}	
	}

}	//end of namespace jcLockJsonCmd_t2015a{