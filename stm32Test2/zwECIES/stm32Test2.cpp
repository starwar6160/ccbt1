// stm32Test2.cpp : 定义控制台应用程序的入口点。
//
#define _ZWUSE_AS_JNI
#include "jclmsCCB2014AlgCore.h"
#include "zwEcies529.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <time.h>

JCINPUT g_jcInputTest304;

time_t myNormalTime(const time_t inTime)
{
	const time_t tmFact=600;
	time_t tail=inTime % tmFact;
	return inTime-tail;
}

void myJclmsTest20150305()
{
	int handle = JcLockNew();
	JcLockSetString(handle, JCI_ATMNO, "atm10455761");
	JcLockSetString(handle, JCI_LOCKNO, "lock14771509");
	JcLockSetString(handle, JCI_PSK, "PSKDEMO728");
	//JcLockSetInt(handle,JCI_SEARCH_TIME_START,static_cast<int>(time(NULL)));
	int initCloseCode =38149728;
#ifdef _DEBUG_INITCOLSECODE306
	JcLockSetCmdType(handle, JCI_CMDTYPE, JCCMD_INIT_CLOSECODE);
	JcLockDebugPrint(handle);
	initCloseCode = JcLockGetDynaCode(handle);
	//检查初始闭锁码是否在正常范围内
	printf("initCloseCode=\t%d Expect 38149728\n", initCloseCode);
#endif // _DEBUG_INITCOLSECODE306
	//此处期待值已经改为固定依赖1400M秒的时间值，应该不会再变了。
	//20141113.1751根据前两天开会决定做的修改。周伟
	//这里是一个自检测试，如果失败，就说明有比较大的问题了，比如类似发生过的
	//ARM编译器优化级别问题导致的生成错误的二进制代码等等
	//dynaPass1
	//注意现在合法的时间值应该是1.4G以上了，注意位数。20140721.1709 

	JcLockSetInt(handle, JCI_DATETIME,static_cast < int >(myNormalTime(time(NULL))));
	JcLockSetCmdType(handle, JCI_CMDTYPE, JCCMD_CCB_DYPASS1);
	JcLockSetInt(handle, JCI_CLOSECODE, initCloseCode);
	JcLockDebugPrint(handle);
	int pass1DyCode = JcLockGetDynaCode(handle);
	printf("dynaPass1=\t%d\n", pass1DyCode);
	JCMATCH pass1Match =
		JcLockReverseVerifyDynaCode(handle, pass1DyCode);
	printf("current time=\t\t%d\n", time(NULL));
	printf("pass1Match Time =\t%d\tValidity=%d\n",
		pass1Match.s_datetime, pass1Match.s_validity);

	JcLockDelete(handle);
}

//生成第一开锁码
int embSrvGenDyCodePass1(const char *AtmNo,const char *LockNo,const char *PSK,
	const time_t CurUTCTime,const int CloseCode)
{
	int handle = JcLockNew();
	JcLockSetString(handle, JCI_ATMNO, AtmNo);
	JcLockSetString(handle, JCI_LOCKNO, LockNo);
	JcLockSetString(handle, JCI_PSK, PSK);
	//生成动态码时不必设置搜索起始时间参数，反推时才需要
	//JcLockSetInt(handle,JCI_SEARCH_TIME_START,static_cast<int>(SearchStartUTCTime));
	JcLockSetInt(handle, JCI_DATETIME,static_cast < int >(CurUTCTime));
	JcLockSetCmdType(handle, JCI_CMDTYPE, JCCMD_CCB_DYPASS1);
	JcLockSetInt(handle, JCI_CLOSECODE, CloseCode);
	//JcLockDebugPrint(handle);
	int pass1DyCode = JcLockGetDynaCode(handle);	
	JcLockDelete(handle);
	return pass1DyCode;
}

void myJclmsTest20150306()
{

	int pass1DyCode=embSrvGenDyCodePass1("atm10455761","lock14771509","PSKDEMO728",
		myNormalTime(time(NULL)),33334444);
	printf("dynaPass1=\t%d\n", pass1DyCode);
}

void myECIES_KeyGenTest123(void)
{
	//预先设置好的生成的一对非对称密钥，是Base64编码的二进制内容
	//pubkey= BFlfjkxoiRZFdjQKa/W1JWBwFx+FPyzcFGqXjnlVzMcvIAQyK3C1Ha+G2uGUM4nX5khPQP5AiPFiCyuH2WxZefg=
	//prikey= y+tgryY83ibv2RaQeb93a97+JX0/9cpWf4MrmUUtrzs=
	//加密结果实质上是用英文句点分隔的3个Base64编码的二进制内容，但是不必理解，原样透传即可
	//ECIES ENC Text= BMMXMJYun+G/Oz4i1LZYmNXR1UM9qTKVfNQPqdorUFCRLvBTE8+SMMoCC/OKAxflDafaosWqnLOA+nkcwvTV8iI=.4AF8XhKkuybS+BQ
	//On+M5BwgwScoCHmkr.0zqyC1eg+7HOhVcbadE7+FRlVZAscomIx9VIfXeHl64wdoDC0X3HJbjEQfIA+flD
	//ECIES DEC Text= C# Port Test plain text 20140722.1625

	//一般来说，生成密钥操作做一次就行了，以下的常量密钥对就是某一次生成的结果复制下来的
#ifdef _DEBUG126
	//首先用这一段程序生成公钥私钥对，然后保存在内存和FLASH里面，以后别处要用到的时候，就从内存或者FLASH里面取出来使用
	//注意一个ARM设备基本上运行一次这段程序就行了，一个公钥/私钥对存在于整个ARM设备的生命周期，除非重新初始化才会再次生成
	int hd = 0;
	hd = EciesGenKeyPair();
	const char *myPubKey=EciesGetPubKey(hd);
	const char *myPriKey=EciesGetPriKey(hd);
	//如果生成了密钥对，那么别忘了删除，释放内存
	EciesDelete(hd);
#endif // _DEBUG126
	//以下
	char pubkey[ZW_ECIES_PUBKEY_LEN];
	char prikey[ZW_ECIES_PRIKEY_LEN];
	memset(pubkey, 0, sizeof(pubkey));
	memset(prikey, 0, sizeof(prikey));
	//以下公钥/私钥对是前面EciesGenKeyPair生成，然后用EciesGetPubKey和EciesGetPriKey取得的，请保存在FLASH里面以备后用
	strcpy(pubkey,"BFlfjkxoiRZFdjQKa/W1JWBwFx+FPyzcFGqXjnlVzMcvIAQyK3C1Ha+G2uGUM4nX5khPQP5AiPFiCyuH2WxZefg=");
	strcpy(prikey,"y+tgryY83ibv2RaQeb93a97+JX0/9cpWf4MrmUUtrzs=");
	printf("pubkey=%s\nprikey=%s\n",pubkey,prikey);
	//////////////////////////////////////////////////////////////////////////
	//建行的两个用于生成PSK的输入因子，将其拼接在ccbInStr里面
	const char *ccbInput1="0123456789ABCDEF";
	const char *ccbInput2="01234ABCDEF56789";
	char ccbInStr[40];
	memset(ccbInStr,0,40);
	strcpy(ccbInStr,ccbInput1);
	strcat(ccbInStr,ccbInput2);
	//从ccbInStr生成PSK
	const char *ccbPSK=zwMergePsk(ccbInStr);
	printf("from CCB1 %s and CCB2 %s result \nccbPSK=\t%s\n",ccbInput1,ccbInput2,ccbPSK);
	char ccbActiveInfo[ZW_ECIES_CRYPT_TOTALLEN];
	memset(ccbActiveInfo, 0, sizeof(ccbActiveInfo));
	//从PSK和公钥生成激活信息ccbActiveInfo，然后激活信息就可以通过网络传输出去了
	strcpy(ccbActiveInfo, EciesEncrypt(pubkey, ccbPSK));
	printf("ccbActiveInfo= %s\n",ccbActiveInfo);
	//用私钥解开激活信息，获得PSK
	char dePSK[ZW_ECIES_CRYPT_TOTALLEN];
	memset(dePSK,0,ZW_ECIES_CRYPT_TOTALLEN);
	strcpy(dePSK,EciesDecrypt(prikey, ccbActiveInfo));	
	printf("ccbPSK=\t%s\n",dePSK);
}

//生成公钥私钥对,输入缓冲区必须有头文件里面宏定义值所指定的足够大小
void zwGenKeyPair(char *pubKey,char *priKey)
{
	if (NULL==pubKey || NULL==priKey)
	{
		return;
	}
	int hd=EciesGenKeyPair();
	strcpy(pubKey,EciesGetPubKey(hd));
	strcpy(priKey,EciesGetPriKey(hd));
	EciesDelete(hd);
}

//从公钥，建行的2个输入因子字符串，输出激活信息字符串，输出缓冲区必须有头文件里面指定的足够大小
void zwGenActiveInfo(const char *pubkey,const char *ccbFact1,const char *ccbFact2,char *ccbActiveInfo)
{
	if (NULL==ccbFact1 ||NULL==ccbFact2 || NULL==ccbActiveInfo
		||0==strlen(ccbFact1) || 0==strlen(ccbFact2))
	{
		return;
	}
	char ccbIn[ZW_ECIES_HASH_LEN];
	memset(ccbIn,0,ZW_ECIES_HASH_LEN);
	strcpy(ccbIn,ccbFact1);
	strcat(ccbIn,ccbFact2);
	//从ccbInStr生成PSK
	const char *ccbPSK=zwMergePsk(ccbIn);
#ifdef _DEBUG
	printf("ccbPSK=\t%s\n",ccbPSK);
#endif // _DEBUG
	//从PSK和公钥生成激活信息ccbActiveInfo，然后激活信息就可以通过网络传输出去了
	strcpy(ccbActiveInfo, EciesEncrypt(pubkey, ccbPSK));
}

//从私钥，激活信息，获取PSK，输出缓冲区必须有头文件里面指定的足够大小
void zwGetPSK(const char *priKey,const char *ccbActiveInfo,char *PSK)
{
	if (NULL==priKey || NULL==ccbActiveInfo || NULL==PSK
		||0==strlen(priKey) || 0==strlen(ccbActiveInfo))
	{
		return;
	}
	strcpy(PSK,EciesDecrypt(priKey,ccbActiveInfo));
}

void myECIESTest305();

int main(int argc, char * argv[])
{
	//myECIES_KeyGenTest123();
//////////////////////////////////////////////////////////////////////////
	//myECIESTest305();

	//////////////////////////////////////////////////////////////////////////
	//myJclmsTest20150305();
	myJclmsTest20150306();
	return 0;
}

void myECIESTest305()
{
	//生成公钥私钥对操作
	char pubKey[ZW_ECIES_PUBKEY_LEN];
	char priKey[ZW_ECIES_PRIKEY_LEN];
	memset(pubKey,0,ZW_ECIES_PUBKEY_LEN);
	memset(priKey,0,ZW_ECIES_PRIKEY_LEN);
	//生成操作只用一次，由于前面已经生成过了，所以此处改行注释掉，后面用生成的结果直接复制进来
	//正式使用时应该是先生成公钥私钥对之后保存到FLASH，用到时取出来使用
	//zwGenKeyPair(pubKey,priKey);
	strcpy(pubKey,"BFlfjkxoiRZFdjQKa/W1JWBwFx+FPyzcFGqXjnlVzMcvIAQyK3C1Ha+G2uGUM4nX5khPQP5AiPFiCyuH2WxZefg=");
	strcpy(priKey,"y+tgryY83ibv2RaQeb93a97+JX0/9cpWf4MrmUUtrzs=");
	printf("pubkey=%s\nprikey=%s\n",pubKey,priKey);
	/////////////////////////////生成激活信息/////////////////////////////////////////////
	char ccbActiveInfo[ZW_ECIES_CRYPT_TOTALLEN];
	//const char *ccbInput1="0123456789ABCDEF";
	//const char *ccbInput2="01234ABCDEF56789";
	const char *ccbInput1="1234567890abcdef";
	const char *ccbInput2="1234567890abcdef";

	memset(ccbActiveInfo,0,ZW_ECIES_CRYPT_TOTALLEN);
	zwGenActiveInfo(pubKey,ccbInput1,ccbInput2,ccbActiveInfo);
	printf("ccbActiveInfo=%s\n",ccbActiveInfo);
	/////////////////////////////解密激活信息/////////////////////////////////////////////
	char PSK[ZW_ECIES_HASH_LEN*2];
	memset(PSK,0,ZW_ECIES_HASH_LEN*2);
	zwGetPSK(priKey,ccbActiveInfo,PSK);
	printf("PSK=\t%s\n",PSK);
}

