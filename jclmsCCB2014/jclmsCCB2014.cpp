// jclmsCCB2014.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "jclmsCCB2014.h"


// 这是导出变量的一个示例
JCLMSCCB2014_API int njclmsCCB2014=0;

// 这是导出函数的一个示例。
JCLMSCCB2014_API int fnjclmsCCB2014(void)
{
	return 42;
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 jclmsCCB2014.h
CjclmsCCB2014::CjclmsCCB2014()
{
	return;
}

namespace jclms{
	void JcLockInput::print()
	{
		string conn=".";	//连字符号
		//三个固定条件组合在一起
		string allItems=m_atmno+conn+m_lockno+conn+m_psk+conn;
		//可变条件逐个化为字符串，组合到一起
#define BLEN (16)
		char buf[BLEN];
		memset(buf,0,BLEN);
		sprintf(buf,"%d",m_datetime);
		allItems=allItems+buf+conn;
		memset(buf,0,BLEN);
		sprintf(buf,"%d",m_validity);
		allItems=allItems+buf+conn;
		memset(buf,0,BLEN);
		sprintf(buf,"%d",m_closecode);
		allItems=allItems+buf+conn;
		memset(buf,0,BLEN);
		sprintf(buf,"%d",m_cmdtype);
		allItems=allItems+buf+conn;
		cout<<allItems<<endl;			
	}
}
