#ifndef dCodeHdr_h__
#define dCodeHdr_h__
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

//生成各种类型的动态码
int myGetDynaCodeImplCCB201407a( const int handle );
//获得规格化的时间，也就是按照某个值取整的时间
int myGetNormalTime(int gmtTime,const int TIMEMOD);
//获取初始闭锁码的3个可变条件的“固定值”
void myGetInitCloseCodeVarItem(int *mdatetime,int *mvalidity,int *mclosecode);
void mySm3Process(SM3 *ctx,const char *data,const int len);
//获取闭锁码的3个可变条件的“固定值”
void myGetCloseCodeVarItem(int *mdatetime,int *mvalidity,int *mclosecode);
void mySm3Process(SM3 *ctx,const char *data,const int len);
void mySm3Process(SM3 *ctx,const int data);
//从包含二进制数据的字符串输入，获得一个8位整数的输出
unsigned int zwBinString2Int32(const char *data,const int len);

extern const int ZW_CLOSECODE_STEP;	//闭锁码的计算步长时间精度
extern const int ZW_CLOSECODE_BASEINPUT;	//计算正常的闭锁码时，m_closecode字段的固定值
extern const int ZW_LOWEST_DATE;	//考虑到取整运算可能使得时间值低于1400M，所以把最低点时间提前一整天该足够了
extern const int ZW_DIGI8_LOW;
extern const int ZW_DIGI8_HIGH;
extern const int ZW_MAXDATA32;	//32位有符号整数可能表示的最大时间值


#endif // dCodeHdr_h__
