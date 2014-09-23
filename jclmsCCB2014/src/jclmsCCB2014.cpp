// jclmsCCB2014.cpp : 定义 DLL 应用程序的导出函数。
//
#include "stdafx.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "jclmsCCB2014.h"
#include "sm3.h"
#include "dCodeHdr.h"

const int ZW_SM3_DGST_SIZE=(256/8)	;
const int ZW_CLOSECODE_STEP=12;	//闭锁码的计算步长时间精度
//从当前时间偏移到将来方向这么多秒，以防止生成密码的加密服务器时间比较快，结果下位机匹配
//的时候，从当前时间开始匹配，始终无法匹配到对应于“将来”某个时间点的动态码；
//这是20140821在建行广开中心发现的问题；
const int JC_DCODE_MATCH_FUTURE_SEC=60*3;	

void mySm3Process(SM3 *ctx,const char *data,const int len);
void mySm3Process(SM3 *ctx,const int data);
//从包含二进制数据的字符串输入，获得一个8位整数的输出
unsigned int zwBinString2Int32(const char *data,const int len);
//获取闭锁码的3个可变条件的“固定值”
void myGetCloseCodeVarItem(int *mdatetime,int *mvalidity,int *mclosecode);


 	int JCLMSCCB2014_API JcLockGetDynaCode( const int handle )
	{
		return myGetDynaCodeImplCCB201407a(handle);
	}

	//生成各种类型的动态码
	int myGetDynaCodeImplCCB201407a( const int handle )
	{		
		zwJcLockDumpJCINPUT(handle);
		const JCINPUT *lock=(const JCINPUT *)handle;
		SM3 sm3;
		char outHmac[ZW_SM3_DGST_SIZE];

		//规格化时间到G_TIMEMOD这么多秒
		int l_datetime=myGetNormalTime(lock->m_datetime,
			lock->m_stepoftime);
			//60*5);	//20140804.1717.应张靖钰的测试需求，暂时改为5分钟取整
		//有效期和闭锁码需要根据不同情况分别处理
		int l_validity=lock->m_validity;
		int l_closecode=lock->m_closecode;	
		//计算初始闭锁码时，采用十天半月大致固定的时间，有效期，闭锁码的值
		//以便对于特定的锁具和PSK来说，初始闭锁码是一个十天半月内的恒定值
		if (JCCMD_INIT_CLOSECODE==lock->m_cmdtype)
		{
			//l_datetime=myGetNormalTime(time(NULL),ZWMEGA);	//初始闭锁码采用1M秒(大约12天)的取整时间
			//l_validity=1000;	//初始有效期取一个有效范围内的规整值
			//l_closecode=1000000;	//初始闭锁码特选一个有效范围内的规整值
			myGetInitCloseCodeVarItem(&l_datetime,&l_validity,&l_closecode);
		}		
		if (JCCMD_CCB_CLOSECODE==lock->m_cmdtype)
		{//计算真正的闭锁码，采用3个固定条件，外加特定的取整步长的时间，以及固定的有效期和“闭锁码”作为输入
			myGetCloseCodeVarItem(&l_datetime,&l_validity,&l_closecode);
		}
		JCERROR err=JcLockCheckInput((const int)lock);
		if (EJC_SUSSESS!=err)
		{
			return err;
		}


		SM3_init(&sm3);

		//限度是小于14开头的时间(1.4G秒)或者快要超出2048M秒的话就是非法了
		/////////////////////////////逐个元素进行HASH运算/////////////////////////////////////////////
		//首先处理固定字段的HASH值输入
		mySm3Process(&sm3,lock->m_atmno,sizeof(lock->m_atmno));
		mySm3Process(&sm3,lock->m_lockno,sizeof(lock->m_lockno));		
		mySm3Process(&sm3,lock->m_psk,sizeof(lock->m_psk));

		//继续输入各个可变字段的HASH值
		mySm3Process(&sm3,l_datetime);
		mySm3Process(&sm3,l_validity);
		mySm3Process(&sm3,l_closecode);
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
		zwJcLockDumpJCINPUT(handle);
		JCINPUT *jcp=(JCINPUT *)handle;
		const int MIN_OF_HOUR=60;	//一小时的分钟数
		JCMATCH jcoff;
		//填入默认的失败返回值
		jcoff.s_datetime=0;
		jcoff.s_validity=0;

		//根据建行广开中心发现的问题，从“将来”几分钟的时间开始往过去方向
		//匹配，以防密码服务器和锁具之间有时间误差；
		int l_datetime=time(NULL)+JC_DCODE_MATCH_FUTURE_SEC;		
		int l_closecode=jcp->m_closecode;
		int l_timestep=jcp->m_stepoftime;
		if (JCCMD_CCB_CLOSECODE==jcp->m_cmdtype)
		{
			int l_validity=jcp->m_validity;	//此输入参数验证时无用，只是为了满足函数输入要求
			//如果是验证闭锁码，就换一套参数
			//验证闭锁码的时候，是否需要搜索更长时间呢？2014.0729.1509周伟
			myGetCloseCodeVarItem(&l_datetime,&l_validity,&l_closecode);
			l_timestep=ZW_CLOSECODE_STEP;
			assert(ZW_CLOSECODE_STEP>0 && ZW_CLOSECODE_STEP<60);
		}

		//搜索时间的起始点必须落在m_stepoftime的整倍数上，否则就无法匹配
		l_datetime=myGetNormalTime(l_datetime,l_timestep);
		int tail=l_datetime % l_timestep;
		l_datetime-=tail;	//取整到数据结构中指定的步长
		//结束时间，往前推数据结构所指定的一段时间，几分钟到一整天不等
		int tend=l_datetime-jcp->m_reverse_time_length;

		for (int tdate=l_datetime;tdate>=tend;tdate-=l_timestep)			
		{			
			//printf("%d\t",tdate);
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
				mySm3Process(&sm3,l_closecode);
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

