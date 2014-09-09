// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "jcSerialPort.h"
void myLoadCfgs();
extern Poco::LogStream * pocoLog;

BOOL APIENTRY DllMain(HMODULE hModule,
		      DWORD ul_reason_for_call, LPVOID lpReserved)
{
	myLoadCfgs();

	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		pocoLog->warning("JINCHU ELOCK DLL_PROCESS_ATTACH");
		break;
	case DLL_THREAD_ATTACH:
		pocoLog->notice("JINCHU ELOCK DLL_THREAD_ATTACH");
		break;
	case DLL_THREAD_DETACH:
		pocoLog->notice("JINCHU ELOCK DLL_THREAD_DETACH");
		break;
	case DLL_PROCESS_DETACH:
		pocoLog->warning("JINCHU ELOCK DLL_PROCESS_DETACH");
		break;
	}
	return TRUE;
}


int sptest905a17(void)
{
	//	以下是十六进制的串口调试助手发送数据，开头8个字节是大端结尾的数字50(0x32)的HEX表示，后面是5组"0123456789"的HEX表示
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
