// jclmsCCB2014.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
//#include <cassert>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "jclmsCCB2014.h"
#include "hashalg\\sm3.h"

namespace jclms{
	const int G_TIMEMOD=10;	//默认按照10秒取整进入的数据，用于防止一些1-3秒钟的错误
int myGetDynaCodeImpl( const JcLockInput &lock );
//从包含二进制数据的字符串输入，获得一个8位整数的输出
unsigned int zwBinString2Int32(const char *data,const int len);

	int getVersion(void)
	{
		//含义是前8位是日期，第9位一般是0，如果一天出了多个发布版本，最后一位变化
		return 201407090;	
	}

	//获得规格化的时间，也就是按照某个值取整的时间
	static int myGetNormalTime(int gmtTime,const int TIMEMOD) 
	{
		int tail=gmtTime % TIMEMOD;
		return gmtTime-tail;
	}

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
		//比1开头的8位数稍微大一些的质数
		const int dyLow=10000019;
		//比9开头的8位数稍微小一些的质数
		const int dyMod=89999969;	
		const int dyMul=257;	//随便找的一个质数作为相乘的因子

		unsigned __int64 sum=0;
		for (int i=0;i<len;i++)
		{
			unsigned char t=*(data+i);
			sum*=dyMul;
			sum+=t;		
		}
		//这两个数字结合使用，产生肯定是8位数的动态码
		sum %=dyMod;
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
		m_cmdtype=JCCMD_INVALID_START;
		m_status=EJC_FAIL;
	}

	void JcLockInput::DebugPrint()
	{
		if (EJC_SUSSESS!=CheckInput())
		{
			printf("JcLock Input Para Error!\n");
		}
		 
		m_datetime=myGetNormalTime(m_datetime,G_TIMEMOD);
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
		if (m_cmdtype==JCCMD_INVALID_START)
		{
			status=EJC_INPUT_NULL;
		}
		//时间秒数取整到G_TIMEMOD，以便消除一些1-2秒的RTC时钟误差造成无法开锁
		m_datetime=myGetNormalTime(m_datetime,G_TIMEMOD);
		m_status=status;
		return status;
	}

	//生成各种类型的动态码
	int myGetDynaCodeImpl( const JcLockInput &lock )
	{
		const int ZWMEGA=1000*1000;
		SM3 sm3;
		char outHmac[ZW_SM3_DGST_SIZE];
		SM3_init(&sm3);
		//假定这些数字字段在二进制层面都是等同于int的长度的，以便通过一个统一的函数进行HASH运算
		assert(sizeof(JcLockInput.m_datetime)==sizeof(int));
		assert(sizeof(JcLockInput.m_validity)==sizeof(int));
		assert(sizeof(JcLockInput.m_closecode)==sizeof(int));
		assert(sizeof(JcLockInput.m_cmdtype)==sizeof(int));
		assert(lock.m_datetime>(1400*ZWMEGA) && lock.m_datetime<(2<<31));
		assert(lock.m_validity>0 && lock.m_validity<=(24*60));
		assert(lock.m_closecode>=0 && lock.m_closecode<=(100*ZWMEGA));
		assert(lock.m_cmdtype>JCCMD_INVALID_START && lock.m_cmdtype<JCCMD_INVALID_END);
		/////////////////////////////逐个元素进行HASH运算/////////////////////////////////////////////
		//首先处理固定字段的HASH值输入
		mySm3Process(&sm3,lock.m_atmno.data(),lock.m_atmno.size());
		mySm3Process(&sm3,lock.m_lockno.data(),lock.m_lockno.size());
		mySm3Process(&sm3,lock.m_psk.data(),lock.m_psk.size());

		//规格化时间到G_TIMEMOD这么多秒
		int l_datetime=myGetNormalTime(lock.m_datetime,G_TIMEMOD);
		//有效期和闭锁码需要根据不同情况分别处理
		int l_validity=lock.m_validity;
		int l_closecode=lock.m_closecode;	
		//计算初始闭锁码时，采用固定的时间，有效期，闭锁码的值
		//以便对于特定的锁具和PSK来说，初始闭锁码是一个恒定值
		if (JCCMD_INIT_CLOSECODE==lock.m_cmdtype)
		{
			l_datetime=1400000000;	//初始闭锁码采用一个特殊的固定值作为时间
			l_validity=0;	//初始闭锁码特选一个合法有效期之外的值
			l_closecode=0;	//初始闭锁码特选一个非法闭锁码			
		}		
		//继续输入各个可变字段的HASH值
		mySm3Process(&sm3,l_datetime);
		mySm3Process(&sm3,l_validity);
		mySm3Process(&sm3,l_closecode);
		mySm3Process(&sm3,lock.m_cmdtype);
		//////////////////////////////HASH运算结束////////////////////////////////////////////
		memset(outHmac,0,ZWSM3_DGST_LEN);
		SM3_hash(&sm3,(char *)(outHmac));
		//把HASH结果转化为8位数字输出
		unsigned int res=zwBinString2Int32(outHmac,ZWSM3_DGST_LEN);
		return res;
	}

	//离线模式匹配，时间点精度为取整到一个小时的零点，有效期精度为1小时起
	//如果找到了，返回JCOFFLINE中是匹配的时间和有效期，否则其中的值都是0
	JCOFFLINE zwOfflineVerifyDynaCode( const JcLockInput &lock,const int dstCode )
	{
		JCOFFLINE jcoff;
		//填入默认的失败返回值
		jcoff.s_datetime=0;
		jcoff.s_validity=0;
		int l_datetime=time(NULL);
		const int MIN_OF_HOUR=60;	//一小时的分钟数
		const int SEC_OF_HOUR=60*60;		//一小时的秒数
		const int SEC_OF_DAY=24*60*60;//一天的秒数
		int valarr[]={MIN_OF_HOUR*4,MIN_OF_HOUR*8,MIN_OF_HOUR*12,MIN_OF_HOUR*24};

		int tail=l_datetime % SEC_OF_HOUR;
		l_datetime-=tail;	//取整到整点小时
		//结束时间，往前推一整天
		int tend=l_datetime-SEC_OF_DAY;

		for (int tdate=l_datetime;tdate>tend;tdate-=SEC_OF_HOUR)
		{
			//printf("TDATE=\t%d\n",tdate);
			for (int v=0;v<sizeof(valarr)/sizeof(int);v++)
			{
				SM3 sm3;
				char outHmac[ZW_SM3_DGST_SIZE];

				SM3_init(&sm3);
				/////////////////////////////逐个元素进行HASH运算/////////////////////////////////////////////
				mySm3Process(&sm3,lock.m_atmno.data(),lock.m_atmno.size());
				mySm3Process(&sm3,lock.m_lockno.data(),lock.m_lockno.size());
				mySm3Process(&sm3,lock.m_psk.data(),lock.m_psk.size());

				mySm3Process(&sm3,tdate);
				mySm3Process(&sm3,valarr[v]);
				mySm3Process(&sm3,lock.m_closecode);
				mySm3Process(&sm3,lock.m_cmdtype);
				//////////////////////////////HASH运算结束////////////////////////////////////////////
				memset(outHmac,0,ZWSM3_DGST_LEN);
				SM3_hash(&sm3,(char *)(outHmac));
				unsigned int res=zwBinString2Int32(outHmac,ZWSM3_DGST_LEN);
				if (dstCode==res)	//发现了匹配的时间和有效期
				{
					//填写匹配的时间和有效期到结果
					printf("FOUND MATCH %d %d\n",tdate,valarr[v]);
					jcoff.s_datetime=tdate;
					jcoff.s_validity=valarr[v];
					goto foundMatch;
				}
			}	//END OF VALIDITY LOOP
		} //END OF DATE LOOP
foundMatch:

		return jcoff;
	}


}	//end of namespace jclms
