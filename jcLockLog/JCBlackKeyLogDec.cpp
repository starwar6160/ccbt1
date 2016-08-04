#include "stdafx.h"
#include "jcLockLog.h"
#include "myLXLockDecHdr.h"

static ReturnTouchKeyLog	g_TouchKeyLogCallback=NULL;
//��Կ����־����

//���ú�Կ�׻ص������ĺ�����
void SetReturnTouchKeyLog(ReturnTouchKeyLog _TouchKeyLogHandleFun)
{
	assert(NULL!=_TouchKeyLogHandleFun);
	if (NULL==_TouchKeyLogHandleFun)
	{
		return ;
	}
	g_TouchKeyLogCallback=_TouchKeyLogHandleFun;
}

int SwapTouchKeyLog(const char * TouchKeyLogItem)
{
	assert(NULL!=TouchKeyLogItem);
	assert(strlen(TouchKeyLogItem)>0);
	if ((NULL==TouchKeyLogItem) || (strlen(TouchKeyLogItem)==0))
	{
		return -1;
	}
	if (NULL==g_TouchKeyLogCallback)
	{
		MessageBox(NULL,TEXT("��Կ����־����ص�����δ����"),TEXT("����"),MB_OK);
		return -4;
	}
	string blackKeyLogBin;
	myHex2Bin(TouchKeyLogItem,blackKeyLogBin);
	assert(blackKeyLogBin.length()>0);
	jclxBlackKeyLog1608_t stuBlk;
	memset(&stuBlk,0,sizeof(stuBlk));
	assert(blackKeyLogBin.length()==sizeof(stuBlk));
	memcpy(&stuBlk,blackKeyLogBin.data(),blackKeyLogBin.length());
	string outStr=jclxBlackKeyLog2Str(&stuBlk);

	g_TouchKeyLogCallback(TouchKeyLogItem,outStr.c_str());
	return 0;
}

string jclxBlackKeyLog2Str(const struct jclxBlackKeyLog1608_t *inBlackLog)
{
	//����5�ֽڴ�˴洢��ʽ�����кţ�Ӧ����5150��ͷ��64λ�޷�������
	int64_t lockSN=beBin2int(inBlackLog->lxSerialNo,5);
	string openLockTime,closeLockTime;
	myGetTimeStr1607(inBlackLog->openLockTime,openLockTime);
	myGetTimeStr1607(inBlackLog->closeLockTime,closeLockTime);
	std::ostringstream oStr;
	//����״̬��֪Ϊ��Ҫת��Ϊint��������ͨ��ostringstream���;
	oStr<<"�����������к�=["<<lockSN<<"];����ʱ��=["<<openLockTime
		<<"]����״̬=["<<static_cast<int>(inBlackLog->openLockStatus)
		<<"]������=["<<inBlackLog->closeCode
		<<"]����ʱ��=["<<closeLockTime<<"]";
	string resStr=oStr.str();
	return resStr;
}


/////////////////////////////////���Դ���/////////////////////////////////////////
	void CDECL blackKeyLogDecCallback(const char* TouchKeyLogItem, const char* TouchKeyLogFactor)
	{
		cout<<"��Կ����־����ԭʼHEX�ַ�����"<<endl<<TouchKeyLogItem<<endl;
		cout<<"��Կ����־�����ַ�����"<<endl<<TouchKeyLogFactor<<endl;
	}

	void myBlackKeyTest804(void)
	{
		const char * blackKeyHexData="727653EC77DDC8FD5600A0FB7603E5C8FD56";
		SetReturnTouchKeyLog(blackKeyLogDecCallback);
		SwapTouchKeyLog(blackKeyHexData);
	}
