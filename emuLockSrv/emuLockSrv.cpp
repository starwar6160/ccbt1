// emuLockSrv.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "jcSerialPort.h"
#include "zwCcbElockHdr.h"
#define ZWFUNCTRACK	cout<<__FUNCTION__<<endl;
int zwjclms_command_proc(const string &inJson,string &outJson);
jcSerialPort g_jcsp("COM3");

int main(int argc,char *argv[])
{
	ZWFUNCTRACK
	const int RECV_BUF_LEN=1024;
	int outLen=0;
	char buffer[RECV_BUF_LEN];	
		do
		{
			memset(buffer,0,RECV_BUF_LEN);
			outLen=0;
			g_jcsp.RecvData(buffer,RECV_BUF_LEN,&outLen);
			string cmdRecv=buffer;
			string cmdSend;				
			int m_type=zwjclms_command_proc(cmdRecv,cmdSend);				
			int sendCount=1;
			if (JCMSG_GET_LOCK_LOG==m_type)
			{
				sendCount=3;
			}
			for (int i=0;i<sendCount;i++)
			{
				g_jcsp.SendData(cmdSend.data(),cmdSend.size());
			}
		}
		while (1);
							

}

