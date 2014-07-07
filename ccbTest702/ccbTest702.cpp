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
void myBinString2intTest1();

int _tmain(int argc, _TCHAR* argv[])
{
	myJcLockInputTest1();
	//zwSm3HmacTest2();
	//myStringTest1();
	//myBigNumModTest1();
	//myBinString2intTest1();
	const int MYHOUR=60;
	int valarr[]={MYHOUR*1,MYHOUR*4,MYHOUR*8,MYHOUR*12,MYHOUR*24};
	cout<<"items of valarr is "<<sizeof(valarr)/sizeof(int)<<endl;

	return 0;
}

