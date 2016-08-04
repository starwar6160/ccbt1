#include "stdafx.h"
#include "jcLockLog.h"
#include "myLXLockDecHdr.h"

static ReturnTouchKeyLog	g_TouchKeyLogCallback=NULL;
//黑钥匙日志处理

//设置黑钥匙回调函数的函数：
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
		MessageBox(NULL,TEXT("黑钥匙日志解码回调函数未设置"),TEXT("错误"),MB_OK);
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
	//计算5字节大端存储格式的序列号，应该是5150打头的64位无符号整数
	int64_t lockSN=beBin2int(inBlackLog->lxSerialNo,5);
	string openLockTime,closeLockTime;
	myGetTimeStr1607(inBlackLog->openLockTime,openLockTime);
	myGetTimeStr1607(inBlackLog->closeLockTime,closeLockTime);
	std::ostringstream oStr;
	//开锁状态不知为何要转换为int才能正常通过ostringstream输出;
	oStr<<"离线锁据序列号=["<<lockSN<<"];开锁时间=["<<openLockTime
		<<"]开锁状态=["<<static_cast<int>(inBlackLog->openLockStatus)
		<<"]闭锁码=["<<inBlackLog->closeCode
		<<"]关锁时间=["<<closeLockTime<<"]";
	string resStr=oStr.str();
	return resStr;
}


/////////////////////////////////测试代码/////////////////////////////////////////
	void CDECL blackKeyLogDecCallback(const char* TouchKeyLogItem, const char* TouchKeyLogFactor)
	{
		cout<<"黑钥匙日志数据原始HEX字符串是"<<endl<<TouchKeyLogItem<<endl;
		cout<<"黑钥匙日志解码字符串是"<<endl<<TouchKeyLogFactor<<endl;
	}

	void myBlackKeyTest804(void)
	{
		const char * blackKeyHexData="727653EC77DDC8FD5600A0FB7603E5C8FD56";
		SetReturnTouchKeyLog(blackKeyLogDecCallback);
		SwapTouchKeyLog(blackKeyHexData);
	}
