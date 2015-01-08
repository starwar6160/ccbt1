#include "stdafx.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"
#include "zwHidSplitMsg.h"
#include "zwHidComm.h"
#include <stdio.h>
using namespace boost::property_tree;
using Poco::AutoPtr;
using Poco::Util::IniFileConfiguration;

namespace zwccbthr {
	boost::mutex thr_mutex;
	//���и��Ľӿڣ�û���������Ӳ����ĵط���Ҳ����˵����ȫ���Զ˿ڣ��ֻ��Ǵ������ļ���ȡ
	boost::mutex recv_mutex;
	std::string s_ComPort;
	std::deque < string > dqOutXML;
	JCHID hidHandle;

	void wait(int milliseconds) {
		boost::this_thread::sleep(boost::
					  posix_time::milliseconds
					  (milliseconds));
	} string zwGetLockIP(void) {
		string myLockIp = "COM2";	//Ĭ��ֵ��COM2
		if (s_ComPort.length() > 0) {
			myLockIp = s_ComPort;	//����������ļ��Ĵ���ֵ����ʹ��֮��
		}
		ZWNOTICE("��������IPʹ��");
		ZWNOTICE(myLockIp.c_str());
		return myLockIp;
	}

	//������֮���ͨѶ�߳�
	void ThreadLockComm() {
		ZWFUNCTRACE boost::mutex::scoped_lock lock(thr_mutex);

		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];
			memset(recvBuf, 0, BLEN + 1);
			int outLen = 0;
			while (1) {
#ifndef ZWUSE_HID_MSG_SPLIT
				if (NULL == zwComPort) {
					ZWNOTICE
					    ("��·�Ѿ��رգ�ͨ���߳̽��˳�");
					return;
				}
#endif // ZWUSE_HID_MSG_SPLIT
				ZWNOTICE("�������߳ɹ�");
				ZWINFO("ͨ���̵߳���һ�ֵȴ���������ѭ����ʼ");
				try {
#ifdef ZWUSE_HID_MSG_SPLIT
					JCHID_STATUS sts=
					jcHidRecvData(&zwccbthr::hidHandle,
						      recvBuf, BLEN, &outLen,JCHID_RECV_TIMEOUT);
					zwCfg::s_hidOpened=true;	//����ͨ���̵߳�һ��������־					
					//Ҫ��ʲôҲû�յ�����ֱ�ӽ�����һ��ѭ��
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

					ZWNOTICE("�ɹ������߽����������£�");
				}
				catch(...) {
					ZWFATAL
					    ("RecvData��������ʱ�����ߵ����������쳣�Ͽ������ݽ����߳̽���ֹ");
					return;
				}
				ZWNOTICE(recvBuf);

				string outXML;
				jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,
								  outXML);
				ZWINFO("�������߻ش���Json��ת��Ϊ����XML�ɹ�");
				//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
				assert(outXML.length() > 42);
				ZWDBGMSG(outXML.c_str());
				{
					boost::
					    mutex::scoped_lock lock(recv_mutex);
					//�յ���XML�������
					dqOutXML.push_back(outXML);
				}

				if (NULL != zwCfg::g_WarnCallback) {
					//���ûص�����������Ϣ��Ȼ��͹ر����ӣ�����ͨ���̣߳�
					zwCfg::g_WarnCallback(outXML.c_str());
					ZWINFO
					    ("�ɹ��Ѵ����߽��յ������ݴ��ݸ��ص�����");
				} else {
					ZWWARN
					    ("�ص�����ָ��Ϊ�գ��޷����ûص�����")
				}
			}
			ZWINFO("��ͨ�����ݽ����߳������˳�");

		}		//try
		catch(...) {			
			//�쳣�Ͽ����趨�ñ�־ΪFALSE,�Ա��´�Open��Ҫ����������ͨ���̵߳ĳ����
			zwCfg::s_hidOpened=false;
			ZWFATAL
			    ("��ͨ�����ݽ����߳����������쳣�Ͽ����������ݽ����߳̽�����");
			return;
		}	
	}

	void myLoadConfig(const string & cfgFileName) {
		try {
			// 1. ���������ļ�  
			AutoPtr < IniFileConfiguration >
			    cfg(new IniFileConfiguration(cfgFileName));
			// 2. ��ȡ�ڵ��ֵ  
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
		ZWFATAL("���ڶ���ָ��Ϊ�գ�");
		return;
	}
#endif // ZWUSE_HID_MSG_SPLIT
	try {
#ifdef ZWUSE_HID_MSG_SPLIT
		JCHID_STATUS sts=JCHID_STATUS_FAIL;
		do 
		{
			sts=jcHidSendData(&zwccbthr::hidHandle, str, strlen(str));
			if (JCHID_STATUS_OK==sts)
			{
				break;
			}
			Sleep(1000);
		} while (sts!=JCHID_STATUS_OK);
		
#else
		zwccbthr::zwComPort->SendData(str, strlen(str));
#endif // ZWUSE_HID_MSG_SPLIT
	}			//try
	catch(...) {
		ZWDBGMSG(__FUNCTION__);
		ZWDBGMSG("\t SerialPort Send String Exeception!20140805.1626");
		ZWFATAL("ͨ����·�������ݵ������쳣��������δ����")
	}

}

