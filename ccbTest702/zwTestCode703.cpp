#include "stdafx.h"
#include <iostream>
using std::cout;
using std::endl;
#include "jclmsCCB2014.h"
using namespace zwTools;
using namespace jclms;


//基本的填写JcLockInput结构体并输出的测试
void myJcLockInputTest1()
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
}

//二进制的SM3HMAC测试
void zwSm3HmacTest2()
{
	const char *pska="mypskaaabbbcccdddeee";
	const char *msga="myplaintexttest201407021710.myplaintexttest201407021742";
	char tbuf[ZW_SM3_DGST_SIZE];
	memset(tbuf,0,ZW_SM3_DGST_SIZE);
	zwHexTool psk(pska,strlen(pska));
	zwHexTool msg(msga,strlen(msga));
	zwHexTool hmac(tbuf,ZW_SM3_DGST_SIZE);

	printf("Init Value of hmac\n");
	hmac.PrintBin();
	zwSm3Hmac7(psk,msg,hmac);
	printf("Result Value of hmac\n");
	hmac.PrintBin();
}

//测试std::string对于含有0字符的二进制数据的处理能力
void myStringTest1()
{
	const int BUFSIZE=16;
	string aa,bb,cc;
	char buf[BUFSIZE];
	char b2[BUFSIZE];
	memset(buf,'A',BUFSIZE);
	memset(b2,'B',BUFSIZE);
	buf[6]=NULL;
	b2[10]=NULL;
	//assign可以正确赋值含有0字符的二进制数据
	aa.assign(buf,BUFSIZE);
	bb.assign(b2,BUFSIZE);
	//+运算符可以正确处理含有0字符的二进制数据
	cc=aa+bb;
	cout<<"aa="<<aa<<endl;
	cout<<"bb="<<bb<<endl;
	cout<<"cc="<<cc<<endl;
	//=运算符碰到含有0字符的二进制数据会截断
	aa=buf;
	bb=b2;
	cc=aa+bb;
	cout<<"aa="<<aa<<endl;
	cout<<"bb="<<bb<<endl;
	cout<<"cc="<<cc<<endl;
}

//大整数相乘取模结果测试
void myBigNumModTest1()
{
	unsigned int aa=77778888,bb=88889999,cc=99998887;
	unsigned int rs=aa*bb % cc;
	cout<<aa<<"*"<<bb<<"%"<<cc<<"="<<rs<<endl;
}

