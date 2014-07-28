// jclmsCCB2014.cpp : 定义 DLL 应用程序的导出函数。
//
#include "stdafx.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "jclmsCCB2014.h"
#include "sm3.h"

typedef struct JcLockInput
{
	//固定因素部分
	char m_atmno[JC_ATMNO_MAXLEN+1];		//ATM号
	char m_lockno[JC_LOCKNO_MAXLEN+1];	//锁号
	char m_psk[JC_PSK_LEN+1];			//PSK，上下位机共同持有的唯一机密因素
	//可变因素部分
	int m_datetime;		//日期时间
	int m_validity;		//有效期
	int m_closecode;	//闭锁码		
	JCCMD m_cmdtype;		//模式代码，比如开锁模式，远程重置模式，建行的流程要求的各种模式等等
	///////////////////////////////////以下为配置算法运作模式的数据///////////////////////////////////////
	//反推时间步长秒数，默认为在线模式，精度1分钟，值为60，离线模式请自己设置为3600秒或者其他数值
	int m_stepoftime;	
	//往前反推的时间长度秒数，默认为在线模式，10分钟，值为600，其他值比如离线24小时请自己设置
	int m_reverse_time_length;					
	//有效期，共有NUM_VALIDITY个,默认值是从5分钟到24小时那一系列，单位是分钟；可以自己设定
	//可以把最常用的有效期设置在更靠近开始处加快匹配速度
	int m_validity_array[NUM_VALIDITY];
	//	void DebugPrint(void);	//
}JCINPUT;


//namespace jclms{
	const int G_TIMEMOD=60;	//默认按照60秒取整进入的数据，用于防止一些1-3秒钟的错误
	//实际上不限于AES,只是作为一个基本的块规整大小单位方便处理
	//#define ZW_AES_BLOCK_SIZE	(128/8)	
	//#define ZW_SM3_DGST_SIZE	(256/8)
	const int ZW_AES_BLOCK_SIZE=(128/8)	;
	const int ZW_SM3_DGST_SIZE=(256/8)	;	

int myGetDynaCodeImplCCB201407a( const int handle );
//从包含二进制数据的字符串输入，获得一个8位整数的输出
unsigned int zwBinString2Int32(const char *data,const int len);

	int JcLockGetVersion(void)
	{
		//含义是是日期
		return 20140724;	
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

	int JCLMSCCB2014_API JcLockGetDynaCode( const int handle )
	{
		return myGetDynaCodeImplCCB201407a(handle);
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
	int JCLMSCCB2014_API JcLockNew( void )
	{
		JCINPUT *pjc=new JCINPUT;
		assert(pjc!=NULL);
		memset(pjc,0,sizeof(JCINPUT));
		memset(pjc->m_atmno,0,JC_ATMNO_MAXLEN+1);
		memset(pjc->m_lockno,0,JC_LOCKNO_MAXLEN+1);
		memset(pjc->m_psk,0,JC_PSK_LEN+1);
		//为没有可变输入的初始闭锁码指定3个常量
		pjc->m_datetime=1400*1000*1000;
		pjc->m_validity=5;		//用的最多的是5分钟有效期，所以直接初始化为
		pjc->m_closecode=0;		//防备初始闭锁码生成的时候此处未初始化
		pjc->m_cmdtype=JCCMD_INIT_CLOSECODE;
//#ifdef _DEBUG
//		pjc->m_stepoftime=6;	//调试模式采用6秒的步长，快速发现问题
//#else
		pjc->m_stepoftime=60;	//默认在线模式，反推时间步长60秒
//#endif // _DEBUG
		pjc->m_reverse_time_length=10*60;	//默认在线模式，反推10分钟
		////将5分钟，4小时这样最常用到的有效期排列在前面，提高效率
		//int valarr[]={5,MIN_OF_HOUR*4,MIN_OF_HOUR*8,MIN_OF_HOUR*12,15,30,60,MIN_OF_HOUR*24};
		pjc->m_validity_array[0]=5;
		pjc->m_validity_array[1]=60*4;
		pjc->m_validity_array[2]=60*8;
		pjc->m_validity_array[3]=60*12;
		pjc->m_validity_array[4]=15;
		pjc->m_validity_array[5]=30;
		pjc->m_validity_array[6]=60;
		pjc->m_validity_array[7]=60*24;
		return (int)pjc;
	}

	int		JCLMSCCB2014_API JcLockDelete(const int handle)
	{
		JCINPUT *jcp=(JCINPUT *)handle;
		assert(NULL!=jcp);
		if (NULL==jcp)
		{
			return EJC_INPUT_NULL;
		}
		memset(jcp,0xCC,sizeof(JCINPUT));
		delete jcp;
		return EJC_SUSSESS;
	}

	void JCLMSCCB2014_API JcLockDebugPrint( const int handle )
	{
		JCINPUT *jcp=(JCINPUT *)handle;
		if (EJC_SUSSESS!=JcLockCheckInput((const int)jcp))
		{
			printf("JcLock Input Para Error!\n");
		}	 		
		//三个固定条件组合在一起,还要为NULL，连接符等留出余量
		char mainstr[JC_ATMNO_MAXLEN+JC_LOCKNO_MAXLEN+JC_PSK_LEN+5];
		memset(mainstr,0,sizeof(mainstr));		
		sprintf(mainstr,"%s.%s.%s.",jcp->m_atmno,jcp->m_lockno,jcp->m_psk);
		//可变条件逐个化为字符串，组合到一起
		char vstr[11+5+9+3+3];	//大致把各个可变字段的位数估计一下
		sprintf(vstr,"%d.%d.%d.%d#%d.%d",jcp->m_datetime,jcp->m_validity,
			jcp->m_closecode,jcp->m_cmdtype,
			jcp->m_stepoftime,jcp->m_reverse_time_length);
		//allItems=allItems+buf;
		char allStr[128];
		memset(allStr,0,128);
		strncpy(allStr,mainstr,128);
		strcat(allStr,vstr);
		printf("All Items = %s \n",allStr);
	}


	//生成各种类型的动态码
	int myGetDynaCodeImplCCB201407a( const int handle )
	{		
		const JCINPUT *lock=(const JCINPUT *)handle;
		SM3 sm3;
		char outHmac[ZW_SM3_DGST_SIZE];
		SM3_init(&sm3);

		JCERROR err=JcLockCheckInput((const int)lock);
		if (EJC_SUSSESS!=err)
		{
			return err;
		}
		//限度是小于14开头的时间(1.4G秒)或者快要超出2048M秒的话就是非法了
		/////////////////////////////逐个元素进行HASH运算/////////////////////////////////////////////
		//首先处理固定字段的HASH值输入
		mySm3Process(&sm3,lock->m_atmno,sizeof(lock->m_atmno));
		mySm3Process(&sm3,lock->m_lockno,sizeof(lock->m_lockno));		
		mySm3Process(&sm3,lock->m_psk,sizeof(lock->m_psk));

		//规格化时间到G_TIMEMOD这么多秒
		int l_datetime=myGetNormalTime(lock->m_datetime,lock->m_stepoftime);
		//有效期和闭锁码需要根据不同情况分别处理
		int l_validity=lock->m_validity;
		int l_closecode=lock->m_closecode;	
		//计算初始闭锁码时，采用固定的时间，有效期，闭锁码的值
		//以便对于特定的锁具和PSK来说，初始闭锁码是一个恒定值
		if (JCCMD_INIT_CLOSECODE==lock->m_cmdtype)
		{
			//l_datetime=myGetNormalTime(time(NULL),8*60*60);	//初始闭锁码采用8小时的取整时间
			//l_validity=(24*60)*365;	//初始有效期特选一个合法有效期之外的值,一整年
			//l_closecode=100001111;	//初始闭锁码特选一个超范围的9位非法闭锁码			
		}		
		//继续输入各个可变字段的HASH值
		mySm3Process(&sm3,lock->m_datetime);
		mySm3Process(&sm3,lock->m_validity);
		mySm3Process(&sm3,lock->m_closecode);
		mySm3Process(&sm3,lock->m_cmdtype);
		//////////////////////////////HASH运算结束////////////////////////////////////////////
		memset(outHmac,0,ZWSM3_DGST_LEN);
		SM3_hash(&sm3,(char *)(outHmac));
		//把HASH结果转化为8位数字输出
		unsigned int res=zwBinString2Int32(outHmac,ZWSM3_DGST_LEN);
		return res;
	}



	//离线模式匹配，时间点精度为取整到一个小时的零点，有效期精度为1小时起
	//如果找到了，返回JCOFFLINE中是匹配的时间和有效期，否则其中的值都是0
	JCMATCH JCLMSCCB2014_API JcLockReverseVerifyDynaCode( const int handle,const int dstCode )
	{		
		JCINPUT *jcp=(JCINPUT *)handle;
		const int MIN_OF_HOUR=60;	//一小时的分钟数
		JCMATCH jcoff;
		//填入默认的失败返回值
		jcoff.s_datetime=0;
		jcoff.s_validity=0;

		int l_datetime=time(NULL);
		//搜索时间的起始点必须落在m_stepoftime的整倍数上，否则就无法匹配
		l_datetime=myGetNormalTime(l_datetime,jcp->m_stepoftime);
		int tail=l_datetime % jcp->m_stepoftime;
		l_datetime-=tail;	//取整到数据结构中指定的步长
		//结束时间，往前推数据结构所指定的一段时间，几分钟到一整天不等
		int tend=l_datetime-jcp->m_reverse_time_length;

		for (int tdate=l_datetime;tdate>=tend;tdate-=jcp->m_stepoftime)			
		{			
			printf("%d\t",tdate);
			for (int v=0;v<NUM_VALIDITY;v++)
			{
				SM3 sm3;
				char outHmac[ZW_SM3_DGST_SIZE];

				SM3_init(&sm3);
				/////////////////////////////逐个元素进行HASH运算/////////////////////////////////////////////
				mySm3Process(&sm3,jcp->m_atmno,sizeof(jcp->m_atmno));
				mySm3Process(&sm3,jcp->m_lockno,sizeof(jcp->m_lockno));
				mySm3Process(&sm3,jcp->m_psk,sizeof(jcp->m_psk));

				mySm3Process(&sm3,tdate);
				mySm3Process(&sm3,jcp->m_validity_array[v]);
				mySm3Process(&sm3,jcp->m_closecode);
				mySm3Process(&sm3,jcp->m_cmdtype);
				//////////////////////////////HASH运算结束////////////////////////////////////////////
				memset(outHmac,0,ZWSM3_DGST_LEN);
				SM3_hash(&sm3,(char *)(outHmac));
				unsigned int res=zwBinString2Int32(outHmac,ZWSM3_DGST_LEN);
				if (dstCode==res)	//发现了匹配的时间和有效期
				{
					//填写匹配的时间和有效期到结果
					printf("FOUND MATCH %d %d\n",tdate,jcp->m_validity_array[v]);
					jcoff.s_datetime=tdate;
					jcoff.s_validity=jcp->m_validity_array[v];
					goto foundMatch;
				}
			}	//END OF VALIDITY LOOP
		} //END OF DATE LOOP
		foundMatch:
		return jcoff;
	}

	JCERROR JCLMSCCB2014_API JcLockCheckInput( const int handle )
	{
		const int ZWMEGA=1000*1000;
		JCINPUT *jcp=(JCINPUT *)handle;
		//假定这些数字字段在二进制层面都是等同于int的长度的，以便通过一个统一的函数进行HASH运算
		assert(sizeof(jcp->m_datetime)==sizeof(int));
		assert(sizeof(jcp->m_validity)==sizeof(int));
		assert(sizeof(jcp->m_closecode)==sizeof(int));
		assert(sizeof(jcp->m_cmdtype)==sizeof(int));

		assert(jcp->m_datetime>=(1400*ZWMEGA) && jcp->m_datetime<((2048*ZWMEGA)-3));
		assert(jcp->m_cmdtype>JCCMD_START && jcp->m_cmdtype<JCCMD_END);
if (JCCMD_INIT_CLOSECODE!=jcp->m_cmdtype)
{	//生成初始闭锁码时，不检查有效期和闭锁码的值
	assert(jcp->m_validity>=0 && jcp->m_validity<=(24*60));
	assert(jcp->m_closecode>=0 && jcp->m_closecode<=(100*ZWMEGA));
}


		//限度是小于14开头的时间(1.4G秒)或者快要超出2048M秒的话就是非法了
		if (jcp->m_datetime<(1400*ZWMEGA) || jcp->m_datetime>((2048*ZWMEGA)-3))
		{//日期时间秒数在2014年的某个1.4G秒之前的日子，或者超过2038年(32位有符号整数最大值)则无效
			return EJC_DATETIME_INVALID;
		}
		if (JCCMD_INIT_CLOSECODE!=jcp->m_cmdtype)
		{	//生成初始闭锁码时，不检查有效期和闭锁码的值
		if (jcp->m_validity<0 || jcp->m_validity>(24*60))
		{//有效期分钟数为负数或者大于一整天则无效
			return EJC_VALIDRANGE_INVALID;
		}
		if (jcp->m_closecode<0 || jcp->m_closecode>(100*ZWMEGA))
		{//闭锁码为负数或者大于8位则无效
			return EJC_CLOSECODE_INVALID;
		}
		}	//if (JCCMD_INIT_CLOSECODE!=jcp->m_cmdtype)
		if (jcp->m_stepoftime<=0 || jcp->m_stepoftime>=(24*60*60))
		{//搜索步长为负数或者大于一整天则无效
			return EJC_CMDTYPE_TIMESTEP_INVALID;
		}
		if (jcp->m_reverse_time_length<=0 || jcp->m_reverse_time_length>=(365*24*60*60))
		{//往前搜索时间为负数或者大于一整年则无效
			return EJC_CMDTYPE_TIMELEN_INVALID;
		}

		if (jcp->m_cmdtype<=JCCMD_START || jcp->m_cmdtype>=JCCMD_END)
		{
			return EJC_CMDTYPE_INVALID;
		}
		return EJC_SUSSESS;
	}

	//设置整数类型的值
	JCERROR JCLMSCCB2014_API JcLockSetInt( const int handle,const JCITYPE mtype,int num )
	{
		assert(handle>0);
		assert(mtype>JCI_START && mtype<JCI_END );
		assert(num>JC_INVALID_VALUE);
		if (handle<=0 || mtype<=JCI_START || mtype>=JCI_END || num <= JC_INVALID_VALUE)
		{
			return EJC_INPUT_NULL;
		}
		JCINPUT *jcp=(JCINPUT *)handle;
		switch (mtype)
		{
		case JCI_DATETIME:
			jcp->m_datetime=num;
			break;
		case JCI_VALIDITY:
			jcp->m_validity=num;
			break;
		case JCI_CLOSECODE:
			jcp->m_closecode=num;
			break;
		}
		return EJC_SUSSESS;
	}

	//设置字符串类型的值
	JCERROR	JCLMSCCB2014_API JcLockSetString(const int handle,const JCITYPE mtype,const char *str)
	{
		assert(handle>0);
		assert(mtype>JCI_START && mtype<JCI_END );
		assert(str!=NULL && strlen(str)>0);
		if (handle<=0 || mtype<=JCI_START || mtype>=JCI_END 
			|| str==NULL || strlen(str)==0)
		{
			return EJC_INPUT_NULL;
		}
		JCINPUT *jcp=(JCINPUT *)handle;
		switch (mtype)
		{
		case JCI_ATMNO:
			strncpy(jcp->m_atmno,str,sizeof(jcp->m_atmno));
			break;
		case JCI_LOCKNO:
			strncpy(jcp->m_lockno,str,sizeof(jcp->m_lockno));
			break;
		case JCI_PSK:
			strncpy(jcp->m_psk,str,sizeof(jcp->m_psk));
			break;
		}
		return EJC_SUSSESS;

	}

	//设置命令类型(第一开锁码，初始闭锁码等等)
	JCERROR	JCLMSCCB2014_API JcLockSetCmdType(const int handle,const JCITYPE mtype,const JCCMD cmd)
	{
		assert(handle>0);
		assert(mtype>JCI_START && mtype<JCI_END );
		assert(cmd>JCCMD_START && cmd<JCCMD_END);
		if (handle<=0 || mtype<=JCI_START || mtype>=JCI_END 
			|| cmd<=JCCMD_START || cmd>=JCCMD_END)
		{
			return EJC_INPUT_NULL;
		}
		JCINPUT *jcp=(JCINPUT *)handle;

		jcp->m_cmdtype=cmd;

		return EJC_SUSSESS;
	}


//}	//end of namespace jclms
