// jclmsCCB2014.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "jclmsCCB2014.h"

//namespace jclms{
	JcLockInput::JcLockInput()
	{
		m_atmno="";
		m_lockno="";
		m_psk="";
		m_datetime=-1;
		m_validity=-1;
		m_closecode=-1;
		m_cmdtype=-1;
	}

	void JcLockInput::print()
	{
		if (EJC_SUSSESS!=check())
		{
			printf("JcLock Input Para Error!\n");
		}
		 
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
		allItems=allItems+buf;
		printf("All Items = %s \n",allItems.c_str());
	}

	JCERROR JcLockInput::check()
	{
		JCERROR status=EJC_SUSSESS;
		if (m_atmno=="")
		{
			status=EJC_INPUT_NULL;
		}
		if (m_lockno=="")
		{
			status=EJC_INPUT_NULL;
		}
		if (m_psk=="")
		{
			status=EJC_INPUT_NULL;
		}
		if (m_datetime<0)
		{
			status=EJC_INPUT_NULL;
		}
		if (m_validity<0)
		{
			status=EJC_INPUT_NULL;
		}
		if (m_closecode<0)
		{
			status=EJC_INPUT_NULL;
		}
		if (m_cmdtype<0)
		{
			status=EJC_INPUT_NULL;
		}


		return status;
	}
//}
