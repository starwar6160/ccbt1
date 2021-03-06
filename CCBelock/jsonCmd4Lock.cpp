#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwHidComm.h"
#include "CCBelock.h"
#include "zwHidDevClass2015.h"
using namespace boost::property_tree;
using boost::mutex;

using jchidDevice2015::jcHidDevice;
extern jcHidDevice* g_jhc;	//实际的HID设备类对象，构造时自动被打开


namespace jcLockJsonCmd_t2015a27{
	//为了保护接收json时返回的静态缓冲区在多线程状况下的使用
	boost::mutex recvstr_mutex;


	CCBELOCK_API long OpenJson(long lTimeOut)
	{
		char buf[256];
		memset(buf, 0, 256);
		sprintf(buf, "Open incoming timeout value seconds is %d", lTimeOut);
		OutputDebugStringA(buf);
		assert(lTimeOut >= -1 && lTimeOut <= JC_CCBDLL_TIMEOUT);
		if (lTimeOut < -1 || lTimeOut > JC_CCBDLL_TIMEOUT) {
			memset(buf, 0, 256);
			sprintf(buf,
				"Open Function incoming timeout Value must in -1 to %d seconds",
				JC_CCBDLL_TIMEOUT);
			OutputDebugStringA(buf);
			LOG(ERROR)<< 
				"Open Function incoming timeout Value must in -1 to " <<
				JC_CCBDLL_TIMEOUT << "seconds";
			return ELOCK_ERROR_PARAMINVALID;
		}
		//必须大于0，小于JC_CCBDLL_TIMEOUT，限制在一个合理范围内
		LOG(WARNING)<< "Open Return " << ELOCK_ERROR_SUCCESS << endl;
		string myLockIp;
		try {

		}
		catch(...) {
			string errMsg = "打开端口" + myLockIp + "失败";
			ZWFATAL(errMsg.c_str())
		}

		ZWWARN("成功打开 到锁具的JSON连接")
			return ELOCK_ERROR_SUCCESS;
	}

	CCBELOCK_API long JCAPISTD CloseJson()
	{
			g_jhc->CloseJc();

		ZWWARN("关闭 到锁具的JSON连接")
			return ELOCK_ERROR_SUCCESS;
	}


	//从long Notify(const char *pszMsg)修改而来
	CCBELOCK_API long SendToLockJson(const char *pszJson)
	{
		//通过在Notify函数开始检测是否端口已经打开，没有打开就直接返回，避免
		//2014年11月初在广州遇到的没有连接锁具时，ATMC执行0002报文查询锁具状态，
		//反复查询，大量无用日志产生的情况。
		if (false == zwCfg::s_hidOpened) {
			return ELOCK_ERROR_CONNECTLOST;
		}
		//输入必须有内容，但是最大不得长于下位机内存大小，做合理限制
		assert(NULL != pszJson);
		if (NULL == pszJson) {
			ZWFATAL("Notify输入为空")
				return ELOCK_ERROR_PARAMINVALID;
		}
		if (NULL != pszJson && strlen(pszJson) > 0) {
			LOG(INFO)<< "JinChu下发JSON=" << endl << pszJson <<
				endl;
		}
		
		try {
			int inlen = strlen(pszJson);
			assert(inlen > 0 && inlen < JC_MSG_MAXLEN);
			if (inlen == 0 || inlen >= JC_MSG_MAXLEN) {
				ZWERROR("notify输入超过最大最小限制");
				return ELOCK_ERROR_PARAMINVALID;
			}
			//////////////////////////////////////////////////////////////////////////
			ZWWARN(pszJson);
			Sleep(50);
			VLOG(3)<<__FUNCTION__<<"\tSleep 50 ms"<<endl;
			g_jhc->SendJson(pszJson);
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
	CCBELOCK_API const char * RecvFromLockJson( const int timeoutMs )
	{
		//超时值默认值
		int realTimeOut=JCHID_RECV_TIMEOUT;
		//如果超时值在一个合理范围内，就采用
		if (timeoutMs>0 && timeoutMs <(3600*1000))
		{
			realTimeOut=timeoutMs;
		}
		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];
			memset(recvBuf, 0, BLEN + 1);
			int recvLen = 0;
			//while (1) {
#ifndef ZWUSE_HID_MSG_SPLIT
				if (NULL == zwComPort) {
					ZWERROR	("线路已经关闭，通信线程将退出");
					return;
				}
#endif // ZWUSE_HID_MSG_SPLIT
				ZWWARN("连接锁具JSON成功 jsonCmd4Lock.cpp");			
				try {
#ifdef ZWUSE_HID_MSG_SPLIT
					JCHID_STATUS sts=static_cast<JCHID_STATUS>(g_jhc->RecvJson(recvBuf,BLEN));
					zwCfg::s_hidOpened=true;	//算是通信线程的一个心跳标志					
					//要是什么也没收到，就直接进入下一个循环
					if (JCHID_STATUS_OK!=sts)
					{
						printf("JCHID_STATUS No DATA RECVED %d\n",sts);
						return "JCHID_STATUS No DATA RECVED";						
					}
					printf("\n");
#else
					zwComPort->RecvData(recvBuf, BLEN,
						&recvLen);
#endif // ZWUSE_HID_MSG_SPLIT

					//////////////////////////////////////////////////////////////////////////

					ZWWARN("jsonCmd4Lock.cpp cmd4Lock成功从锁具接收JSON数据如下：");
				}
				catch(...) {
					ZWFATAL("RecvData接收JSON数据时到锁具的数据连接异常断开，数据接收将终止");
					return "RecvData接收JSON数据时到锁具的数据连接异常断开，数据接收将终止";
				}
				ZWWARN(recvBuf);
			//}	//while (1) {
			ZWINFO("金储通信数据接收JSON过程正常结束");
			{
				//返回数据
				//为了保护返回数据用的static std::string retStr,而加锁
				boost::mutex::scoped_lock lockRetStr(recvstr_mutex);
				static std::string retStr=recvBuf;
				return retStr.c_str();
			}
		}		//try
		catch(...) {			
			//异常断开就设定该标志为FALSE,以便下次Open不要再跳过启动通信线程的程序段
			zwCfg::s_hidOpened=false;
			ZWFATAL("金储通信数据接收过程中数据连接异常断开，现在数据接收JSON过程异常结束");
			return "金储通信数据接收过程中数据连接异常断开，现在数据接收JSON过程异常结束";
		}	
	}

}	//end of namespace jcLockJsonCmd_t2015a21{