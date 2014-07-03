// ccbTest702.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
using std::cout;
using std::endl;

void myJcLockInputTest1();
void zwSm3HmacTest2();
void myStringTest1();
void myBigNumModTest1();

//从包含二进制数据的字符串输入，获得一个8位整数的输出
unsigned int zwBinString2Int32By8(const string &inStr)
{
	//比1开头的8位数稍微小一些的质数
	const int dyLow=10000019;
	//比开头的8位数稍微小一些的质数
	const int dyMod=89999981;	
	const int dyMul=257;	//随便找的一个质数作为相乘的因子
	int len=inStr.length();
	//int tail=len % (sizeof(int));
	const char *data=inStr.data();
	unsigned int sum=0;
	for (int i=0;i<len;i++)
	{
		unsigned char t=*(data+i);
		sum*=257;
		sum+=t;		
	}
	//这两个数字结合使用，产生肯定是8位数的动态码
	sum %=89999981;
	sum +=dyLow;
	return sum;
}

void myBinString2intTest1();

int _tmain(int argc, _TCHAR* argv[])
{
	myJcLockInputTest1();
	//zwSm3HmacTest2();
	//myStringTest1();
	//myBigNumModTest1();
	//myBinString2intTest1();


	return 0;
}

