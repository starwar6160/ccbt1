// emuLockSrv.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "jcSerialPort.h"
#include "zwCcbElockHdr.h"
#include "zwHidSplitMsg.h"
#define ZWFUNCTRACK	cout<<__FUNCTION__<<endl;
int zwjclms_command_proc(const string &inJson,string &outJson);
jcSerialPort g_jcsp("COM3");

int main(int argc,char *argv[])
{
	ZWFUNCTRACK
	const int RECV_BUF_LEN=1024;
	int outLen=0;
	char buffer[RECV_BUF_LEN];	
	char partBuf[JC_HID_TRANS_BYTES];
	JC_MSG_MULPART s_mpSplit[JC_HIDMSG_SPLIT_NUM];
		do
		{
			memset(buffer,0,RECV_BUF_LEN);
			memset(partBuf,0,JC_HID_TRANS_BYTES);
			outLen=0;
			g_jcsp.RecvData(partBuf,JC_HID_TRANS_BYTES,&outLen);
			assert(outLen>0);
			//////////////////////////////////////////////////////////////////////////
			JC_MSG_MULPART *tmm=(JC_MSG_MULPART *)partBuf;
			int rIndex=NtoHs(tmm->nIndex);
			int rTotal=NtoHs(tmm->nTotalBlock);
			assert(rIndex>=0 && rIndex <256);
			assert(rTotal>0 && rTotal <256);
			assert(rIndex<rTotal);
			memcpy(s_mpSplit+rIndex,partBuf,sizeof(JC_MSG_MULPART));
			if (rIndex==(rTotal-1))
			{
				memset(buffer,0,RECV_BUF_LEN);
				jcMsgMulPartMerge(s_mpSplit,rTotal,buffer,RECV_BUF_LEN);
			}
			else
			{
				//尚未结束一个完整包的处理的话就继续接受下一个分片的消息
				continue;
			}
			//////////////////////////////////////////////////////////////////////////
			string cmdRecv=buffer;
			string cmdSend;				
			int m_type=zwjclms_command_proc(cmdRecv,cmdSend);				
			int sendCount=1;
			if (JCMSG_GET_LOCK_LOG==m_type)
			{
				sendCount=3;
			}

			JC_MSG_MULPART s_mpSplit[JC_HIDMSG_SPLIT_NUM];
			for (int i=0;i<sendCount;i++)
			{
				jcMsgMulPartSplit(cmdSend.c_str(),cmdSend.length(),s_mpSplit,JC_HIDMSG_SPLIT_NUM);
				for (int i=0;i<NtoHs(s_mpSplit[0].nTotalBlock);i++)
				{
					g_jcsp.SendData((char *)(s_mpSplit+i),sizeof(JC_MSG_MULPART));
				}
			}
		}
		while (1);
							

}

