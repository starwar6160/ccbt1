// emuLockSrv.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "jcSerialPort.h"
#include "zwCcbElockHdr.h"
#include "zwHidSplitMsg.h"
#include "zwHidComm.h"
#define ZWFUNCTRACK	cout<<__FUNCTION__<<endl;
//#define ZWUSE_HID_MSG_SPLIT		//�Ƿ�ʹ��HID��64�ֽ���Ϣ�зַ���
int zwjclms_command_proc(const string &inJson,string &outJson);
jcSerialPort g_jcsp("COM3");

int main(int argc,char *argv[])
{
	ZWFUNCTRACK
	const int RECV_BUF_LEN=1024;
	int outLen=0;
	char buffer[RECV_BUF_LEN];	
#ifdef ZWUSE_HID_MSG_SPLIT
	char partBuf[JC_HID_TRANS_BYTES];
	JC_MSG_MULPART s_mpSplit[JC_HIDMSG_SPLIT_NUM];
#endif // ZWUSE_HID_MSG_SPLIT
		do
		{
			outLen=0;
			memset(buffer,0,RECV_BUF_LEN);		

#ifdef ZWUSE_HID_MSG_SPLIT
			memset(partBuf,0,JC_HID_TRANS_BYTES);
			g_jcsp.RecvData(partBuf,JC_HID_TRANS_BYTES,&outLen);
#else
			g_jcsp.RecvData(buffer,RECV_BUF_LEN,&outLen);
#endif // ZWUSE_HID_MSG_SPLIT
			assert(outLen>0);
			//////////////////////////////////////////////////////////////////////////
#ifdef ZWUSE_HID_MSG_SPLIT
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
				//��δ����һ���������Ĵ���Ļ��ͼ���������һ����Ƭ����Ϣ
				continue;
			}
#endif // ZWUSE_HID_MSG_SPLIT
			//////////////////////////////////////////////////////////////////////////
			string cmdRecv=buffer;
			string cmdSend;				
			int m_type=zwjclms_command_proc(cmdRecv,cmdSend);				
			int sendCount=1;
			if (JCMSG_GET_LOCK_LOG==m_type)
			{
				sendCount=3;
			}

#ifdef ZWUSE_HID_MSG_SPLIT
			JC_MSG_MULPART s_mpSplit[JC_HIDMSG_SPLIT_NUM];
#endif // ZWUSE_HID_MSG_SPLIT
			for (int i=0;i<sendCount;i++)
			{
#ifdef ZWUSE_HID_MSG_SPLIT
				jcMsgMulPartSplit(cmdSend.c_str(),cmdSend.length(),s_mpSplit,JC_HIDMSG_SPLIT_NUM);
				for (int i=0;i<NtoHs(s_mpSplit[0].nTotalBlock);i++)
				{
					g_jcsp.SendData((char *)(s_mpSplit+i),sizeof(JC_MSG_MULPART));
				}
#else
				g_jcsp.SendData(cmdSend.data(),cmdSend.size());
#endif // ZWUSE_HID_MSG_SPLIT
			}
		}
		while (1);
							

}

