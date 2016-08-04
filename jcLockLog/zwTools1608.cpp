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



}	//namespace zwTools2015{