#include "stdafx.h"
#include "zwCcbElockHdr.h"
//#include "zwwsClient.h"
#include "zwPocoLog.h"
#include "jcSerialPort.h"
#include <stdio.h>
using namespace boost::property_tree;
using Poco::AutoPtr;
using Poco::Util::IniFileConfiguration;

namespace zwccbthr {
	boost::mutex thr_mutex;
	//���и��Ľӿڣ�û���������Ӳ����ĵط���Ҳ����˵����ȫ���Զ˿ڣ��ֻ��Ǵ������ļ���ȡ
	jcSerialPort *zwComPort = NULL;
	string s_dbgReturn = "";
	boost::mutex recv_mutex;
	std::string s_ComPort;

	void wait(int milliseconds) {
		boost::this_thread::sleep(boost::posix_time::
					  milliseconds(milliseconds));
	} string zwGetLockIP(void) {
		string myLockIp = "COM3";	//Ĭ��ֵ����ʵ����IP
		if (s_ComPort.length() > 0) {
			myLockIp = s_ComPort;	//����������ļ���IPֵ����ʹ��֮��
		}
		ZWNOTICE("��������IPʹ��");
		ZWNOTICE(myLockIp.c_str());
		return myLockIp;
	}

	//������֮���ͨѶ�߳�
	void ThreadLockComm() {
		ZWFUNCTRACE boost::mutex::scoped_lock lock(thr_mutex);
		string myLockIp;
		try {
			myLockIp = zwGetLockIP();
			jcSerialPort jcsp(myLockIp.c_str());
			zwComPort=&jcsp;
			const int BLEN=1024;
			char recvBuf[BLEN+1];
			memset(recvBuf,0,BLEN+1);
			int outLen=0;
			ZWNOTICE("�������߳ɹ�");
			while (1) {
				ZWINFO("ͨ���̵߳���һ�ֵȴ���������ѭ����ʼ");
				//string recstr;
				{
					try {
						//wsconn.ReceiveString(recstr);
						memset(recvBuf,0,BLEN);
						jcsp.RecvData(recvBuf, BLEN,&outLen);
						ZWNOTICE
						    ("�ɹ������߽����������£�");
					}
					catch(...) {
						ZWFATAL
						    ("�����ߵ�WS�����쳣�Ͽ������ݽ����߳̽���ֹ");
						return;
					}
					ZWNOTICE(recvBuf);
				}

				string outXML;
				jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,
								  outXML);
				ZWINFO("�������߻ش���Json��ת��Ϊ����XML�ɹ�");
				assert(outXML.length() > 42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
				ZWDBGMSG(outXML.c_str());

				{
					boost::mutex::
					    scoped_lock lock(recv_mutex);
					s_dbgReturn = outXML;
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
			ZWFATAL
			    ("��ͨ�����ݽ����̴߳��������쳣�Ͽ����������ݽ����߳̽�����");
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
		catch(Poco::Exception e) {
			pocoLog->
			    warning() << "ini config file" << cfgFileName <<
			    " not found" << endl;
		}
	}

//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

//����ͨ���߳�
boost::thread thr(zwccbthr::ThreadLockComm);


void zwPushString(const string & str)
{
	ZWFUNCTRACE assert(str.length() > 0);
	if (str.length() == 0) {
		return;
	}
	if (NULL == zwccbthr::zwComPort) {
		ZWFATAL("���ڶ���ָ��Ϊ�գ�");
		return;
	}
	try {
		zwccbthr::zwComPort->SendData(str.data(),str.length());
	}
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
