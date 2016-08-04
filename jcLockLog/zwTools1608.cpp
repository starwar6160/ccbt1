#include "stdafx.h"


namespace zwTools2015{
//十六进制字符串转换为二进制
int myHex2Bin(const string &inHexStr,string &outBinStr)
{
	assert(inHexStr.length()>0);
	if (inHexStr.length()==0)
	{
		return -1;
	}
	int inLen=inHexStr.length();
	for (int i=0;i<inLen;i+=2)
	{
		string tch=inHexStr.substr(i,2);
		unsigned int x = std::stoul(tch, nullptr, 16);
		outBinStr.push_back(x);
	}
	return inLen;
}

//把8字节以内的大端结尾二进制数据转换为十进制整数
int64_t beBin2int(const char *inBeBinData,int inDataLen)
{
	assert(NULL!=inBeBinData);
	assert(inDataLen>0);
	if ((NULL==inBeBinData) || (inDataLen==0))
	{
		return -1;
	}
	int64_t dstNum=0;
	for (int i=inDataLen-1;i>=0;i--)
	{
		dstNum=dstNum*256;
		dstNum+=static_cast<uint8_t>(inBeBinData[i]);
	}
	return dstNum;
}

void myGetTimeStr1607(time_t inTime,string &outTimeStr)
{
	time_t rawtime=inTime;
	struct tm* timeinfo;
	char myTimeStr[80];
	memset(myTimeStr,0,80);
	timeinfo=localtime(&rawtime);
	strftime(myTimeStr,80,"%Y/%m/%d %H:%M:%S",timeinfo);
	outTimeStr=myTimeStr;
}


}	//namespace zwTools2015{