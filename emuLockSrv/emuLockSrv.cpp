// emuLockSrv.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "jcSerialPort.h"
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
			if ('!'==buffer[outLen-1])
			{
				buffer[outLen-1]=0;	//去掉为了万敏的要求添加的末尾的感叹号的干扰
			}
			string cmdRecv=buffer;
			//app.logger().information(Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags)));										
			string cmdSend;				
			zwjclms_command_proc(cmdRecv,cmdSend);				
			g_jcsp.SendData(cmdSend.data(),cmdSend.size());
			//app.logger().information(Poco::format("RECV msg=%s",cmdRecv));		
			//app.logger().information(Poco::format("SEND msg=%s",cmdSend));	
		}
		while (1);
							

}

