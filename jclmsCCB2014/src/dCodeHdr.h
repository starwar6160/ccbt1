#ifndef dCodeHdr_h__
#define dCodeHdr_h__
#include "sm3.h"
#include "jclmsCCB2014.h"

#ifdef  __cplusplus
extern "C" {
#endif


#pragma pack(1)
typedef struct JcLockInput {
	//固定因素部分
	char AtmNo[JC_ATMNO_MAXLEN + 1];	//ATM号
	char LockNo[JC_LOCKNO_MAXLEN + 1];	//锁号
	char PSK[JC_PSK_LEN + 1];	//PSK，上下位机共同持有的唯一机密因素
	//可变因素部分
	int CodeGenDateTime;		//日期时间
	int Validity;		//有效期
	int CloseCode;	//闭锁码             	
	JCCMD CmdType;	//模式代码，比如开锁模式，远程重置模式，建行的流程要求的各种模式等等
	///////////////////////////////////以下为配置算法运作模式的数据///////////////////////////////////////
	int dbgSearchTimeStart;	//搜索时间起始点，默认值应该是当前时间，但是比如测试时可以额外设定。20141118新增,主要供调试和单元测试使用
	//反推时间步长秒数，默认为在线模式，精度1分钟，值为60，离线模式请自己设置为3600秒或者其他数值
	int SearchTimeStep;
	//往前反推的时间长度秒数，默认为在线模式，10分钟，值为600，其他值比如离线24小时请自己设置
	int SearchTimeLength;
	//有效期，共有NUM_VALIDITY个,默认值是从5分钟到24小时那一系列，单位是分钟；可以自己设定
	//可以把最常用的有效期设置在更靠近开始处加快匹配速度
	int ValidityArray[NUM_VALIDITY];
} JCINPUT;

typedef enum jclmsd_request{
	JCLMS_CCB_CODEGEN,		//动态码生成
	JCLMS_CCB_CODEVERIFY	//动态码反推
}JCLMSOP;

typedef struct jcLmsRequest{
	JCLMSOP op;
	int dstCode;	//反推运算的输入动态码
	JCINPUT inputData;
}JCLMSREQ;

//用于HID等通信接口返回结果，统一在一个结构体里面
typedef union JcLockResult{
	int dynaCode;			//动态码结果
	JCMATCH verCodeMatch;		//验证码匹配日期时间和有效期结果
}JCRESULT;

#pragma pack()

//获得规格化的时间，也就是按照某个值取整的时间
int myGetNormalTime(int gmtTime, const int TIMEMOD);
//获取初始闭锁码的3个可变条件的“固定值”
void myGetInitCloseCodeVarItem(int *mdatetime, int *mvalidity, int *mclosecode);

#ifdef  __cplusplus
}	//extern "C" {
#endif

#endif // dCodeHdr_h__
