// jclmsCCB2014.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "jclmsCCB2014.h"
#include "hashalg\\sm3.h"

namespace jclms{
int myGetDynaCodeImpl( const JcLockInput &lock );
//从包含二进制数据的字符串输入，获得一个8位整数的输出
unsigned int zwBinString2Int32(const char *data,const int len);

	void mySm3Process(SM3 *ctx,const char *data,const int len)
	{
		assert(ctx!=NULL);
		assert(ctx->length>0);
		assert(data!=NULL);
		assert(len>0);
		for (int i=0;i<len;i++)
		{
			SM3_process(ctx,*(data+i));
		}
	}

	void mySm3Process(SM3 *ctx,const int data)
	{
		assert(ctx!=NULL);
		assert(ctx->length>0);
		assert(data>=0);	//几个整数参数，都是0或者正整数
		int td=data;
		for (int i=0;i<sizeof(data);i++)
		{
			unsigned char t=td & 0xff;
			SM3_process(ctx,t);
			td=td>>8;
		}
		assert(td==0);
	}

	int zwGetDynaCode(const JcLockInput &lock)
	{
		return myGetDynaCodeImpl(lock);
	}

	jclms::JCERROR zwVerifyDynaCode( const JcLockInput &lock,const int dstDyCode )
	{
		int calCode= myGetDynaCodeImpl(lock);
		if (calCode==dstDyCode)
		{
			return EJC_SUSSESS;
		}
		else
		{
			return EJC_FAIL;
		}
	}

	//从包含二进制数据的字符串输入，获得一个8位整数的输出
	unsigned int zwBinString2Int32(const char *data,const int len)
	{
		//比1开头的8位数稍微小一些的质数
		const int dyLow=10000019;
		//比开头的8位数稍微小一些的质数
		const int dyMod=89999981;	
		const int dyMul=257;	//随便找的一个质数作为相乘的因子
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


//////////////////////////////////////////////////////////////////////////
	JcLockInput::JcLockInput()
	{
		m_atmno="";
		m_lockno="";
		m_psk="";
		m_datetime=-1;
		m_validity=-1;
		m_closecode=-1;	
		m_cmdtype=JCCMD_INVALID;
		m_status=EJC_FAIL;
	}

	void JcLockInput::DebugPrint()
	{
		if (EJC_SUSSESS!=CheckInput())
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

	JCERROR JcLockInput::CheckInput()
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
		if (m_cmdtype==JCCMD_INVALID)
		{
			status=EJC_INPUT_NULL;
		}

		m_status=status;
		return status;
	}

	int myGetDynaCodeImpl( const JcLockInput &lock )
	{
		SM3 sm3;
		char outHmac[ZW_SM3_DGST_SIZE];
		SM3_init(&sm3);
		/////////////////////////////逐个元素进行HASH运算/////////////////////////////////////////////
		mySm3Process(&sm3,lock.m_atmno.data(),lock.m_atmno.size());
		mySm3Process(&sm3,lock.m_lockno.data(),lock.m_lockno.size());
		mySm3Process(&sm3,lock.m_psk.data(),lock.m_psk.size());

		int l_datetime=lock.m_datetime;
		int l_validity=lock.m_validity;
		int l_closecode=lock.m_closecode;	
		if (JCCMD_INIT_CLOSECODE==lock.m_cmdtype)
		{
			l_datetime=1400000000;	//初始闭锁码采用一个特殊的固定值作为时间
			l_validity=0;	//初始闭锁码特选一个合法有效期之外的值
			l_closecode=0;	//初始闭锁码特选一个非法闭锁码			
		}
		mySm3Process(&sm3,l_datetime);
		mySm3Process(&sm3,l_validity);
		mySm3Process(&sm3,l_closecode);
		mySm3Process(&sm3,lock.m_cmdtype);
		//////////////////////////////HASH运算结束////////////////////////////////////////////
		memset(outHmac,0,ZWSM3_DGST_LEN);
		SM3_hash(&sm3,(char *)(outHmac));
		unsigned int res=zwBinString2Int32(outHmac,ZWSM3_DGST_LEN);
		return res;
	}

}
