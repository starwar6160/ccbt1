#include "stdafx.h"
#include <iostream>
#include <cassert>
using std::cout;
using std::endl;
#include "jclmsCCB2014.h"
#include "zwTools1.h"
using namespace zwTools;
//using namespace jclms;
#include <vector>
#include <algorithm>
#include <set>
using std::set;
using std::vector;
using std::sort;


//基本的填写JcLockInput结构体并输出的测试
void myJcLockInputTest1()
{
	JcLockInput aa;
	aa.m_atmno="atmnoddddddddsssssssssssssssssssss";
	aa.m_lockno="locknossssssssssssa1";
	aa.m_psk="pskaaaabbbbbccccsssssssssssssssssssssssssssssssssssss";
	//注意现在合法的时间值应该是1.4G以上了，注意位数。20140721.1709
	aa.m_datetime=1400077751;
	aa.m_validity=5;
	aa.m_closecode=87654325;
	aa.m_cmdtype=JCCMD_CCB_DYPASS1;
	aa.DebugPrint();
	set <int> rset;
	//基本上做到了40K个批量生成时重复在个位数，13K个无重复
	//在1到9的字头分布方面，几千个时偏差在10%以内，
	//几万个时偏差3.5%以内，也就是说基本均匀
	const int RCOUNT=900*3;
	int head[10];
	for (int i=0;i<10;i++)
	{
		head[i]=0;
	}
	for (int i=0;i<RCOUNT;i++)
	{
		aa.m_datetime++;
		int dycode=zwGetDynaCode(aa);
		rset.insert(dycode);
		if (i % (RCOUNT/32) ==0)
		{
			cout<<dycode<<"\t";
		}
		int hd=dycode /10000000;
		assert(hd>=0 && hd <=9);
		head[hd]++;
	}
	cout<<endl;
	int realSize=rset.size();
	cout<<"Total Item is "<<RCOUNT<<"\t";
	cout<<"Dups Item is "<<RCOUNT-realSize<<endl;
	for (int i=1;i<10;i++)
	{
		cout<<i<<":"<<head[i]*100.0f/(RCOUNT/9)<<"\n";
	}
}

#ifdef _DEBUG_USE_OLD_SM3HMAC20140703
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
#endif // _DEBUG_USE_OLD_SM3HMAC20140703

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
	cout<<"aa="<<aa<<"len of aa ="<<aa.length()<<endl;
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

#ifdef _DEBUG_BINTEST703
void myBinString2intTest1()
{
	vector <unsigned int>res;
	char  msg[]="zbouweiteststring201407031011c";
	int msglen=sizeof(msg);
	for (int i=0;i<100;i++)
	{
		msg[0]=i;		
		unsigned int rv=zwBinString2Int32(msg,msglen);
		res.push_back(rv);
	}
	sort(res.begin(),res.end());
	cout<<"*******************\n";
	for (size_t i=0;i<res.size();i++)
	{
		cout<<res[i]<<"\t";
	}
	cout<<endl;
}
#endif // _DEBUG_BINTEST703

