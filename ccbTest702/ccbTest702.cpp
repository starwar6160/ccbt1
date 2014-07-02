// ccbTest702.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "jclmsCCB2014.h"
//using namespace jclms;

int _tmain(int argc, _TCHAR* argv[])
{
	JcLockInput aa;
	aa.m_atmno="atmno";
	aa.m_lockno="lockno";
	aa.m_psk="pskaaaabbbbbcccc";
	aa.m_datetime=140007775;
	aa.m_validity=240;
	aa.m_closecode=87654321;
	aa.m_cmdtype=0;
	aa.DebugPrint();
	return 0;
}

