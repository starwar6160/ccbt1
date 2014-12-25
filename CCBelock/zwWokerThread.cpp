#include "stdafx.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
//#include "zwwsClient.h"
#include "zwPocoLog.h"
#include "jcSerialPort.h"
#include "zwHidSplitMsg.h"
#include "zwHidComm.h"
#include <stdio.h>
using namespace boost::property_tree;
using Poco::AutoPtr;
using Poco::Util::IniFileConfiguration;

namespace zwccbthr {
	boost::mutex thr_mutex;
	//建行给的接口，没有设置连接参数的地方，也就是说，完全可以端口，抑或是从配置文件读取
	jcSerialPort *zwComPort = NULL;
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
		ZWNOTICE("连接锁具IP使用");
		ZWNOTICE(myLockIp.c_str());
		return myLockIp;
	}

	//与锁具之间的通讯线程
	void ThreadLockComm() {
		ZWFUNCTRACE boost::mutex::scoped_lock lock(thr_mutex);

		try {
			OutputDebugStringA("20141017.1116.Thread.MaHaoTest1");
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
				OutputDebugStringA
				    ("20141017.1116.Thread.MaHaoTest2");
				ZWNOTICE("连接锁具成功");
				ZWINFO("通信线程的新一轮等待接收数据循环开始");
				try {
#ifdef ZWUSE_HID_MSG_SPLIT
					OutputDebugStringA
					    ("20141017.1116.Thread.MaHaoTest3");
					JCHID_STATUS sts=
					jcHidRecvData(&zwccbthr::hidHandle,
						      recvBuf, BLEN, &outLen,3000);
					zwCfg::s_hidOpened=true;	//算是通信线程的一个心跳标志
					//要是什么也没收到，就直接进入下一个循环
					if (JCHID_STATUS_RECV_ZEROBYTES==sts)
					{
						continue;
					}
					printf("\n");
					OutputDebugStringA
					    ("20141017.1116.Thread.MaHaoTest4");
#else
					zwComPort->RecvData(recvBuf, BLEN,
							    &outLen);
#endif // ZWUSE_HID_MSG_SPLIT

					//////////////////////////////////////////////////////////////////////////

					ZWNOTICE("成功从锁具接收数据如下：");
				}
				catch(...) {
					OutputDebugStringA
					    ("20141017.1116.Thread.MaHaoTest5");
					ZWFATAL
					    ("RecvData接收数据时到锁具的数据连接异常断开，数据接收线程将终止");
					return;
				}
				ZWNOTICE(recvBuf);

				string outXML;
				OutputDebugStringA
				    ("20141017.1116.Thread.MaHaoTest6");
				jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,
								  outXML);
				OutputDebugStringA
				    ("20141017.1116.Thread.MaHaoTest7");
				ZWINFO("分析锁具回传的Json并转换为建行XML成功");
				//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
				assert(outXML.length() > 42);
				ZWDBGMSG(outXML.c_str());
				OutputDebugStringA
				    ("20141017.1116.Thread.MaHaoTest8");
				{
					boost::
					    mutex::scoped_lock lock(recv_mutex);
					//收到的XML存入队列
					dqOutXML.push_back(outXML);
				}

				if (NULL != zwCfg::g_WarnCallback) {
					OutputDebugStringA
					    ("20141017.1116.Thread.MaHaoTest9");
					//调用回调函数传回信息，然后就关闭连接，结束通信线程；
					zwCfg::g_WarnCallback(outXML.c_str());
					OutputDebugStringA
					    ("20141017.1116.Thread.MaHaoTest10");
					ZWINFO
					    ("成功把从锁具接收到的数据传递给回调函数");
				} else {
					OutputDebugStringA
					    ("20141017.1116.Thread.MaHaoTest11");
					ZWWARN
					    ("回调函数指针为空，无法调用回调函数")
				}
			}
			OutputDebugStringA("20141017.1116.Thread.MaHaoTest12");
			ZWINFO("金储通信数据接收线程正常退出");

		}		//try
		catch(...) {
			OutputDebugStringA("20141017.1116.Thread.MaHaoTest13");
			//异常断开就设定该标志为FALSE,以便下次Open不要再跳过启动通信线程的程序段
			zwCfg::s_hidOpened=false;
			ZWFATAL
			    ("金储通信数据接收线程数据连接异常断开，现在数据接收线程将结束");
			return;
		}
		OutputDebugStringA("20141017.1116.Thread.MaHaoTest14");
	}

	void myLoadConfig(const string & cfgFileName) {
		try {
			// 1. 载入配置文件  
			AutoPtr < IniFileConfiguration >
			    cfg(new IniFileConfiguration(cfgFileName));
			// 2. 获取节点的值  
			s_ComPort = cfg->getString("ELock.ComPort");
			assert(s_ComPort.length() > 0);
		}
		catch(...) {
			string errMsg =
			    "Load JinChuELock Config file\t" + cfgFileName +
			    "\tfail.now exit !";
			OutputDebugStringA(errMsg.c_str());
			cout << errMsg << endl;
			exit(2);
		}
	}

//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

CCBELOCK_API void zwPushString(const char *str)
{
	ZWFUNCTRACE assert(NULL != str && strlen(str) > 0);
	if (NULL == str || strlen(str) == 0) {
		return;
	}
#ifndef ZWUSE_HID_MSG_SPLIT
	if (NULL == zwccbthr::zwComPort) {
		ZWFATAL("串口对象指针为空！");
		return;
	}
#endif // ZWUSE_HID_MSG_SPLIT
	try {
#ifdef ZWUSE_HID_MSG_SPLIT
		jcHidSendData(&zwccbthr::hidHandle, str, strlen(str));
#else
		zwccbthr::zwComPort->SendData(str, strlen(str));
#endif // ZWUSE_HID_MSG_SPLIT
	}			//try
	catch(...) {
		ZWDBGMSG(__FUNCTION__);
		ZWDBGMSG("\t SerialPort Send String Exeception!20140805.1626");
		ZWFATAL("通过线路发送数据到锁具异常，可能是未连接")
	}

}

int sptest905a17(void)
{
	//      以下是十六进制的串口调试助手发送数据，开头8个字节是大端结尾的数字50(0x32)的HEX表示，后面是5组"0123456789"的HEX表示
	//      000000323031323334353637383930313233343536373839303132333435363738393031323334353637383930313233343536373839

	jcSerialPort jcsp("COM2");
	string msg1 = "ZhouWei20140909.0858\n";
	jcsp.SendData(msg1.c_str(), msg1.size());
	const int BLEN = 500;
	char recvBuf[BLEN + 1];
	memset(recvBuf, 0, BLEN + 1);
	int outLen = 0;
	Sleep(5000);
	for (int i = 0; i < 20; i++) {
		memset(recvBuf, 0, BLEN);
		jcsp.RecvData(recvBuf, BLEN, &outLen);
		//cout<<"R "<<recvBuf<<endl;
		Sleep(500);
	}

	//Sleep(6000);
	//jcsp.RecvData(recvBuf,&outLen);

	return 0;
}
