// jcMultiHID15.cpp : 定义 DLL 应用程序的导出函数。
//20150408.1609.这是为了把支持多个HID设备的工厂用，测试用DLL和
//建行项目ATMC DLL分开而做的单独一个DLL，共用部分提取到zwBaseLib里面

#include "stdafx.h"
#include "jcMultiHID15.h"


// 这是导出变量的一个示例
JCMULTIHID15_API int njcMultiHID15=0;

// 这是导出函数的一个示例。
JCMULTIHID15_API int fnjcMultiHID15(void)
{
	return 42;
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 jcMultiHID15.h
CjcMultiHID15::CjcMultiHID15()
{
	return;
}
