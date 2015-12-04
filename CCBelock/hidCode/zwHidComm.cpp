#include "stdafx.h"
#include <windows.h>
#include "hidapi.h"
#include "zwHidSplitMsg.h"
#include "zwHidComm.h"
#include "zwTimerHdr.h"
//#define _DEBUG_HID_1010
//#define _DEBUG_ZWHIDCOMM

//#define _DEBUG_PRINT_HID_RECV_RAWDATA20141015
const unsigned short int JC_UNINIT=0xCCCC;	//VC的Debug模式下未初始化的内存值0xCC；
#define PRFN	OutputDebugStringA(__FUNCTION__);
//printf("%s\n",__FUNCTION__);

namespace zwTools2015{
	//将TCHAR转为char   
	//*tchar是TCHAR类型指针，*_char是char类型指针   
	void TcharToChar (const TCHAR * tchar, char * _char);
		//同上   
	void CharToTchar (const char * _char, TCHAR * tchar);
}	//namespace zwTools2015{


//这是复制从hid.c，正常使用不需要，调试需要
struct hid_device_ {
	HANDLE device_handle;
	BOOL blocking;
	USHORT output_report_length;
	size_t input_report_length;
	void *last_error_str;
	DWORD last_error_num;
	BOOL read_pending;
	char *read_buf;
	OVERLAPPED ol;
};



//检查输入的结构体内容的正确性
JCHID_STATUS myCheckHidPara( const JCHID * hid )
{
#ifdef _DEBUG401ZW
	assert(NULL!=hid);
	assert(0!=hid->vid && 0!=hid->pid);
	assert(JC_UNINIT!=hid->vid	&& JC_UNINIT!=hid->pid);
#endif // _DEBUG401ZW
	if (NULL==hid || hid->vid==0 || hid->pid==0)
	{
		return JCHID_STATUS_FAIL;
	}
	return JCHID_STATUS_OK;
}

JCHID_STATUS jcHidOpen(JCHID *hid)
{	
	ZWTRC
	JCHID_STATUS sts=myCheckHidPara(hid);	//入口参数合法性检查	
	//OutputDebugStringA("jcHidOpen 20141027 START");
	if (JCHID_STATUS_OK!=sts)
	{
		return sts;
	}
	assert(sizeof(JC_MSG_MULPART)<=JCHID_FRAME_LENGTH);
	// Open the device using the VID, PID,
	// and optionally the Serial number.
	////handle = hid_open(0x4d8, 0x3f, L"12345");
	hid->hid_device=0;
	wchar_t hidSerialW[JCHID_SERIAL_LENGTH*2];
	memset(hidSerialW,0,JCHID_SERIAL_LENGTH*2*sizeof(wchar_t));	
	if (strlen(hid->HidSerial)>0)
	{
		zwTools2015::CharToTchar(hid->HidSerial,hidSerialW);
	}
	//printf("HOZ\n");
	//assert(hid->hid_device!=NULL);
	int openCount=0;
	do
	{
		if (strlen(hid->HidSerial)>0)
		{
			hid->hid_device = hid_open(hid->vid, hid->pid, hidSerialW);
		}
		else
		{
			hid->hid_device = hid_open(hid->vid, hid->pid, NULL);
		}
		
		if (hid->hid_device!=0)
		{
			break;
		}
		printf("Wait for Reconnect USB Line\n");
		Sleep(200);	//等待Open设备
		VLOG(3)<<__FUNCTION__<<"\tSleep 200 ms"<<endl;
		openCount++;
#ifdef _DEBUG
		if (openCount>1)
#else
		if (openCount>2)
#endif // _DEBUG
		{
			break;
		}
	}while(0==hid->hid_device);

	if (!hid->hid_device) {
		OutputDebugStringA("ZW1023JCHIDOPEN_FAIL");
		printf("%s unable to open device\n",__FUNCTION__);
		return JCHID_STATUS_FAIL;
	}
	//hid_set_nonblocking((hid_device *)hid->hid_device, 1);
	//OutputDebugStringA("jcHidOpen 20141027 END");
	return JCHID_STATUS_OK;
}

JCHID_STATUS jcHidClose( const JCHID *hid )
{	
	ZWTRC
	if (NULL==hid || NULL==hid->hid_device)
	{
		return JCHID_STATUS_FAIL;
	}
	const wchar_t *jcerr=hid_error(static_cast<hid_device *>(hid->hid_device));
	if (NULL!=jcerr)
	{
		return JCHID_STATUS_FAIL;
	}

	JCHID_STATUS sts=myCheckHidPara(hid);	//入口参数合法性检查
	//OutputDebugStringA("jcHidClose 20141027 START");
	if (JCHID_STATUS_OK!=sts)
	{
		return sts;
	}
	assert(sizeof(JC_MSG_MULPART)<=JCHID_FRAME_LENGTH);
	hid_close((hid_device *)hid->hid_device);
	//printf("HCZ\n");
	//OutputDebugStringA("jcHidClose 20141027 END");
	return JCHID_STATUS_OK;
}

JCHID_STATUS jcHidSendData(const JCHID *hid,const char *inData,const int inDataLen)
{
	ZWTRC
	char zwbuf[256];
	JCHID_STATUS sts=myCheckHidPara(hid);	//入口参数合法性检查
	JC_MSG_MULPART s_mpSplit[JC_HIDMSG_SPLIT_NUM];	//保存切分结果
	int SplitedBlockNum=0;
	char sendBuf[JCHID_FRAME_LENGTH+1];//65字节，用于开头添加一个无用一个0字节满足HID底层协议的需求
	int i=0;
	//OutputDebugStringA("jcHidSendData 20141027 START");
	if (JCHID_STATUS_OK!=sts)
	{
		OutputDebugStringA("jcHidSendData myCheckHidPara Fail");
		return sts;
	}
	if (NULL==hid->hid_device)
	{
		OutputDebugStringA("jcHidSendData NULL==hid->hid_device");
		//return JCHID_STATUS_HANDLE_NULL;
	}
//////////////////////////////////////////////////////////////////////////
	assert(sizeof(JC_MSG_MULPART)<=JCHID_FRAME_LENGTH);
	//切分输入
	jcMsgMulPartSplit(inData,inDataLen,s_mpSplit,sizeof(s_mpSplit)/sizeof(JC_MSG_MULPART));
	//一块一块发送切分好的输入数据	
	SplitedBlockNum=NtoHs(s_mpSplit[0].nTotalBlock);
	for (i=0;i<SplitedBlockNum;i++)
	{	
		int nc=0;
		unsigned char *tc=(unsigned char *)(s_mpSplit+i);
		hid_device *hdev=(hid_device *)hid->hid_device;
		//20141010.周伟.由于HID协议底层会把第一个字节挪到命令内部，为了便于处理，
		//我决定使用单独一个缓冲区，65字节大小，初始化为0，然后从偏移值1开始把结构体复制进去
		//然后发送这个缓冲区,这样，下位机收到后，第一个未用字节被HID协议挪走了，
		//余下的就是有用的64字节了
		memset(sendBuf,0,(JCHID_FRAME_LENGTH+1));
		memcpy(sendBuf+1,(unsigned char *)(s_mpSplit+i),sizeof(JC_MSG_MULPART));
#ifdef _DEBUG_ZWHIDCOMM
		sprintf(zwbuf,"jcHidSendData Sending Hid Data Block #%d\n",i);
		OutputDebugStringA(zwbuf);				
		printf(zwbuf);
		printf("%s HEX DATA is:\n",__FUNCTION__);
		for (nc=1;nc<(JCHID_FRAME_LENGTH+1);nc++)
		{
			printf("%02X ",*(unsigned char *)(sendBuf+nc) & 0xFF);
			if (nc % 16 ==0 && nc >0)
			{
				printf("\n");
			}
		}
#endif // _DEBUG_ZWHIDCOMM
		//printf("\n");
		if (NULL==hdev)
		{
			OutputDebugStringA("NULL==hdev Line 126");
			return JCHID_STATUS_FAIL;
		}
		int bytes_write=hid_write((hid_device *)hid->hid_device,
			(unsigned char *)sendBuf,(JCHID_FRAME_LENGTH+1));
		if (0==bytes_write)
		{
			printf("hid_write BYTES==0,return\n");
			return JCHID_STATUS_FAIL;
		}
		//printf("HWZ\t");
		{	//hid_error错误检测
			wchar_t *errmsg=(wchar_t *)hid_error((hid_device *)hid->hid_device);
			if(NULL!=errmsg)
			{
				return JCHID_STATUS_FAIL;
				//OutputDebugStringA("hid_write FAIL,now Close Hid Device and reOpen it");
				//jcHidClose(hid);
				//jcHidOpen(hid);
				//hid_write((hid_device *)hid->hid_device,
				//	(unsigned char *)sendBuf,(JCHID_FRAME_LENGTH+1));
				//printf("%s JCHID PLUGOUT/IN RECONNECT_SEND %s",__FUNCTION__,errmsg);
			}
		}
	}
	//printf("\n");
	
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG_ZWHIDCOMM
	printf("%s Send Total %d Blocks\n",__FUNCTION__,SplitedBlockNum);
	printf("S%d ",SplitedBlockNum);
#endif // _DEBUG_ZWHIDCOMM
	
	//OutputDebugStringA("jcHidSendData 20141027 END");
	return JCHID_STATUS_OK;
}

JCHID_STATUS jcHidRecvData( JCHID *hid,char *outData,const int outMaxLen,int *outLen,const int timeout )
{		
	
	ZWTRC

	JCHID_STATUS sts=myCheckHidPara(hid);	//入口参数合法性检查
	int res=0;
	char partBuf[JC_HID_TRANS_BYTES];
	JC_MSG_MULPART s_mpSplit[JC_HIDMSG_SPLIT_NUM];
	JC_MSG_MULPART *tmm=NULL;
	int rIndex=0;
	int rTotal=0;

	{
		//OutputDebugStringA("jcHidRecvData 20141027 START");
		if (JCHID_STATUS_OK!=sts)
		{
			OutputDebugStringA("jcHidRecvData myCheckHidPara Fail");
			return sts;
		}
		if (NULL==hid->hid_device)
		{
			OutputDebugStringA("jcHidRecvData NULL==hid->hid_device");
			return JCHID_STATUS_HANDLE_NULL;
		}
		assert(NULL!=outData && 0xCCCCCCCC!=(unsigned int)outData);
		assert(outMaxLen>0);
		assert(NULL!=outLen && 0xCCCCCCCC!=(unsigned int)outLen);
		if (NULL==outData || outMaxLen<=0 || NULL==outLen)
		{
			OutputDebugStringA("NULL==outData || outMaxLen<=0 || NULL==outLen 176");
			return JCHID_STATUS_INPUTNULL;
		} 
		assert(sizeof(JC_MSG_MULPART)<=JCHID_FRAME_LENGTH);
	}

	//printf("Receved Data From JCELock is:\n");
	// Read requested state. hid_read() has been set to be
	// non-blocking by the call to hid_set_nonblocking() above.
	// This loop demonstrates the non-blocking nature of hid_read().
	do{
		try{	
		//zwTrace1027 zwt1("jcHidRecvData P1hid_read");		
		memset(partBuf,0,JC_HID_TRANS_BYTES);
		//hid_read_timeout并不存在timeout为0就是无限期等待的规则，所以需要如此区分
		if (0==timeout)
		{
			res=hid_read((hid_device *)hid->hid_device, (unsigned char *)partBuf, JC_HID_TRANS_BYTES);
		}
		else
		{
			res=hid_read_timeout((hid_device *)hid->hid_device, (unsigned char *)partBuf, JC_HID_TRANS_BYTES,timeout);
		}		
		//printf("HRZ\t");
		//////////////////////////////////////////////////////////////////////////
		
#ifdef _DEBUG_PRINT_HID_RECV_RAWDATA20141015
		for(int i=0;i<JC_HID_TRANS_BYTES;i++)
		{
			if (i%16==0 && i>0)
			{
				printf("\n");
			}
			printf("%02X ",static_cast<unsigned char>(partBuf[i]));			
		}
		printf("\n");
#endif // _DEBUG_PRINT_HID_RECV_RAWDATA20141015

		//////////////////////////////////////////////////////////////////////////
		
		{	//hid_error错误检测
			wchar_t *errmsg=NULL;
			if (NULL!=(hid_device *)hid->hid_device)
			{
				errmsg=(wchar_t *)hid_error((hid_device *)hid->hid_device);
			}
			
			if(NULL!=errmsg)
			{
				//return JCHID_STATUS_FAIL;
				OutputDebugStringA("jcHidRecvData hid_read FAIL");
				jcHidClose(hid);
				jcHidOpen(hid);
				if (NULL==(hid_device *)hid->hid_device)
				{
					goto zw116;
				}
				//assert(NULL!=(hid_device *)hid->hid_device);
				res=hid_read_timeout((hid_device *)hid->hid_device, (unsigned char *)partBuf, JC_HID_TRANS_BYTES,JCHID_RECV_TIMEOUT);				
				errmsg=(wchar_t *)hid_error((hid_device *)hid->hid_device);
				zw116:
				if (NULL!=errmsg)
				{
					return JCHID_STATUS_FAIL;
				}
				printf("%s JCHID PLUGOUT/IN RECONNECT_RECV",__FUNCTION__);
			}
		}
		//从下位机那里什么都没读到，也等于某种错误
		if (0==res)
		{
			//printf("READ 0 bytes from HID DEVICE!\n");
			return JCHID_STATUS_RECV_ZEROBYTES;
		}
		}	//try{
		catch(...){
			printf("%s ReadDataFrom JinChu Hid Device Error.20150115.1614\n",__FUNCTION__);
			return JCHID_STATUS_FAIL;
		}

#ifdef _DEBUG_HID_1010
		//printf("\n");
#ifdef _DEBUG_PRINT_HID_RECV_RAWDATA20141015
#ifdef _DEBUG_ZWHIDCOMM
		{
			int i;
			for (i=0;i<JC_HID_TRANS_BYTES;i++)
			{
				if (i>0 && i%16==0)
				{
					printf("\n");
				}
				printf("%02X ",(unsigned char)partBuf[i] & 0xFF);
			}			
		}
		printf("\n******************\n");
#endif // _DEBUG_ZWHIDCOMM
#endif // _DEBUG_PRINT_HID_RECV_RAWDATA20141015
#endif // _DEBUG_HID_1010
		//assert(res>=0);
		assert(res<=JC_HID_TRANS_BYTES);
		if (res < 0)
		{
			OutputDebugStringA("jcHidRecvData hid_read error");
			printf("Unable to read()\n");		
			return JCHID_STATUS_FAIL;
		}
		(*outLen)+=res;
//////////////////////////////////////////////////////////////////////////
		tmm=(JC_MSG_MULPART *)partBuf;
		rIndex=NtoHs(tmm->nIndex);
		rTotal=NtoHs(tmm->nTotalBlock);
		assert(rIndex>=0 && rIndex <256);
		assert(rTotal>0 && rTotal <256);
		assert(rIndex<rTotal);
		if (rIndex<0 || rIndex>=256 ||
			rTotal<=0 || rTotal>=256 ||
			rIndex>=rTotal
			)
		{
			OutputDebugStringA("jcHidRecvData rIndex/rTotal out of range");
			return JCHID_STATUS_FAIL;
		}
		memcpy(s_mpSplit+rIndex,partBuf,sizeof(JC_MSG_MULPART));
		{
			int i=0;
			int c=NtoHs(s_mpSplit[rIndex].nDataLength);
			char * p=(char *)s_mpSplit[rIndex].Data;
			char wmbuf[JCHID_FRAME_LENGTH];
			memset(wmbuf,0,JCHID_FRAME_LENGTH);
			memcpy(wmbuf,p,c);
			//printf("%s",wmbuf);
		} 
		//如果已经收到了最后一块数据，就合并输出
		if (rIndex==(rTotal-1))
		{
			memset(outData,0,outMaxLen);
			jcMsgMulPartMerge(s_mpSplit,rTotal,outData,outMaxLen);					
		}
		else
		{
			//尚未结束一个完整包的处理的话就继续接受下一个分片的消息
			//OutputDebugStringA("jcHidRecvData part msg continue 259");
			continue;
		}	
		//zwtrace.DiffTime();
	}
	while(res>0 && rIndex<(rTotal-1));
	//printf("\n");
#ifdef _DEBUG_ZWHIDCOMM
	printf("%s Recv Total %d Blocks\n",__FUNCTION__,rTotal);
	printf("R%d ",rTotal);
#endif // _DEBUG_ZWHIDCOMM
	
	//OutputDebugStringA("jcHidRecvData Success Complete");
	//OutputDebugStringA("jcHidRecvData 20141027 END");
	return JCHID_STATUS_OK;
}




