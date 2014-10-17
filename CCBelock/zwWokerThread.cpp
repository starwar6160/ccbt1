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
	//���и��Ľӿڣ�û���������Ӳ����ĵط���Ҳ����˵����ȫ���Զ˿ڣ��ֻ��Ǵ������ļ���ȡ
	jcSerialPort *zwComPort = NULL;
	boost::mutex recv_mutex;
	std::string s_ComPort;
	std::deque<string> dqOutXML;
	JCHID hidHandle;

	void wait(int milliseconds) {
		boost::this_thread::sleep(boost::posix_time::
					  milliseconds(milliseconds));
	} 
	string zwGetLockIP(void) 
	{
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
			OutputDebugStringA("20141017.1116.Thread.MaHaoTest1");
			const int BLEN=1024;
			char recvBuf[BLEN+1];			
			memset(recvBuf,0,BLEN+1);
			int outLen=0;			
			while (1) {
#ifndef ZWUSE_HID_MSG_SPLIT
				if (NULL==zwComPort)
				{
					ZWNOTICE("�����Ѿ��رգ�ͨ���߳̽��˳�");
					return;
				}
#endif // ZWUSE_HID_MSG_SPLIT
				OutputDebugStringA("20141017.1116.Thread.MaHaoTest2");
				ZWNOTICE("�������߳ɹ�");
				ZWINFO("ͨ���̵߳���һ�ֵȴ���������ѭ����ʼ");			
				try {					
#ifdef ZWUSE_HID_MSG_SPLIT
					OutputDebugStringA("20141017.1116.Thread.MaHaoTest3");
					jcHidRecvData(&zwccbthr::hidHandle,recvBuf,BLEN,&outLen);
					printf("\n");
					OutputDebugStringA("20141017.1116.Thread.MaHaoTest4");
#else
					zwComPort->RecvData(recvBuf, BLEN,&outLen);
#endif // ZWUSE_HID_MSG_SPLIT

					
					//////////////////////////////////////////////////////////////////////////

					ZWNOTICE
						("�ɹ������߽����������£�");
				}
				catch(...) {
					OutputDebugStringA("20141017.1116.Thread.MaHaoTest5");
					ZWFATAL
						("RecvData��������ʱ�����ߵĴ��������쳣�Ͽ������ݽ����߳̽���ֹ");
					return;
				}
				ZWNOTICE(recvBuf);

				string outXML;
				OutputDebugStringA("20141017.1116.Thread.MaHaoTest6");
				jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);
				OutputDebugStringA("20141017.1116.Thread.MaHaoTest7");
				ZWINFO("�������߻ش���Json��ת��Ϊ����XML�ɹ�");
				//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
				assert(outXML.length() > 42);	
				ZWDBGMSG(outXML.c_str());
				OutputDebugStringA("20141017.1116.Thread.MaHaoTest8");
				{
					boost::mutex::
					    scoped_lock lock(recv_mutex);
					//�յ���XML�������
					dqOutXML.push_back(outXML);
				}

				if (NULL != zwCfg::g_WarnCallback) {
					OutputDebugStringA("20141017.1116.Thread.MaHaoTest9");
					//���ûص�����������Ϣ��Ȼ��͹ر����ӣ�����ͨ���̣߳�
					zwCfg::g_WarnCallback(outXML.c_str());
					OutputDebugStringA("20141017.1116.Thread.MaHaoTest10");
					ZWINFO("�ɹ��Ѵ����߽��յ������ݴ��ݸ��ص�����");
				} else {
					OutputDebugStringA("20141017.1116.Thread.MaHaoTest11");
					ZWWARN("�ص�����ָ��Ϊ�գ��޷����ûص�����")
				}
			}
			OutputDebugStringA("20141017.1116.Thread.MaHaoTest12");
			ZWINFO("��ͨ�����ݽ����߳������˳�");

		}		//try
		catch(...) {
			OutputDebugStringA("20141017.1116.Thread.MaHaoTest13");
			ZWFATAL("��ͨ�����ݽ����̴߳��������쳣�Ͽ����������ݽ����߳̽�����");
			return;
		}
		OutputDebugStringA("20141017.1116.Thread.MaHaoTest14");
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
			string errMsg="Load JinChuELock Config file\t"+cfgFileName+"\tfail.now exit !";
			OutputDebugStringA(errMsg.c_str());
			cout<<errMsg<<endl;
			exit(2);
		}
	}

//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{




CCBELOCK_API void zwPushString(const char *str)
{
	ZWFUNCTRACE assert(NULL!=str && strlen(str) > 0);
	if (NULL==str || strlen(str) == 0) {
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
		jcHidSendData(&zwccbthr::hidHandle,str,strlen(str));
#else
zwccbthr::zwComPort->SendData(str,strlen(str));
#endif // ZWUSE_HID_MSG_SPLIT		
		}	//try
	catch(...) {
		ZWDBGMSG(__FUNCTION__);
		ZWDBGMSG("\t SerialPort Send String Exeception!20140805.1626");
		ZWFATAL("ͨ�����ڷ������ݵ������쳣��������δ����")
	}

}

int sptest905a17(void)
{
	//	������ʮ�����ƵĴ��ڵ������ַ������ݣ���ͷ8���ֽ��Ǵ�˽�β������50(0x32)��HEX��ʾ��������5��"0123456789"��HEX��ʾ
	//	000000323031323334353637383930313233343536373839303132333435363738393031323334353637383930313233343536373839

	jcSerialPort jcsp("COM2");
	string msg1="ZhouWei20140909.0858\n";
	jcsp.SendData(msg1.c_str(),msg1.size());
	const int BLEN=500;
	char recvBuf[BLEN+1];
	memset(recvBuf,0,BLEN+1);
	int outLen=0;
	Sleep(5000);
	for (int i=0;i<20;i++)
	{
		memset(recvBuf,0,BLEN);
		jcsp.RecvData(recvBuf, BLEN,&outLen);
		//cout<<"R "<<recvBuf<<endl;
		Sleep(500);
	}

	//Sleep(6000);
	//jcsp.RecvData(recvBuf,&outLen);

	return 0;
}
