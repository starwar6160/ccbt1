// stm32Test2.cpp : 定义控制台应用程序的入口点。
//
#define _ZWUSE_AS_JNI
#include "jclmsCCB2014AlgCore.h"
#include "zwEcies529.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include <string>
#include <assert.h>
#include "tests.h"


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

void myECIESTest305();

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

	JcLockSetInt(handle, JCI_DATETIME,static_cast < int >(time(NULL)));
	JcLockSetCmdType(handle, JCI_CMDTYPE, JCCMD_CCB_DYPASS1);
	JcLockSetInt(handle, JCI_CLOSECODE, initCloseCode);
	JcLockDebugPrint(handle);
	int pass1DyCode = JcLockGetDynaCode(handle);
	printf("dynaPass1=\t%d\n", pass1DyCode);
	JcLockDelete(handle);
	//////////////////////////////////////////////////////////////////////////

	handle = JcLockNew();
	JcLockSetString(handle, JCI_ATMNO, "atm10455761");
	JcLockSetString(handle, JCI_LOCKNO, "lock14771509");
	JcLockSetString(handle, JCI_PSK, "PSKDEMO728");
	JcLockSetInt(handle,JCI_SEARCH_TIME_START,static_cast<int>(time(NULL)));
	JcLockSetInt(handle, JCI_CLOSECODE, initCloseCode);
	JcLockSetCmdType(handle, JCI_CMDTYPE, JCCMD_CCB_DYPASS1);
	JCMATCH pass1Match =
		JcLockReverseVerifyDynaCode(handle, pass1DyCode);
	printf("current time=\t\t%d\n", time(NULL));
	printf("pass1Match Time =\t%d\tValidity=%d\n",
		pass1Match.s_datetime, pass1Match.s_validity);

	JcLockDelete(handle);
}

void myJclmsTest20150306STM32Demo()
{
	//基本条件
	const char *atmno="atm10455761";
	const char *lockno="lock14771509";
	const char *psk="PSKDEMO728";
	//此处是初始闭锁码,生成闭锁码和初始闭锁码的方式类似,初始闭锁码不需要时间和closecode输入，所以输入0
	int initCloseCode=embSrvGenDyCode(JCCMD_INIT_CLOSECODE,0,0,atmno,lockno,psk);

	//////////////////////////////////////////////////////////////////////////
	//从3个基本条件(ATM编号，锁具编号，PSK(也就是激活信息经过解密之后的内容)
	//和UTC时间秒数，初始闭锁码作为输入，密码服务器生成第一开锁码作为输出
	time_t curTime=time(NULL);
	curTime=1425711000;	//20150309调试临时修改固定时间值便于调试
	int pass1DyCode=embSrvGenDyCode(JCCMD_CCB_DYPASS1,curTime,initCloseCode,atmno,lockno,psk);
	printf("第一开锁码=\t%d\n", pass1DyCode);
	//锁具验证第一开锁码
	printf("验证第一开锁码开始\n");
	time_t pass1MatchTime=embSrvReverseDyCode(JCCMD_CCB_DYPASS1,pass1DyCode,initCloseCode, 
		//time(NULL),
		curTime,
		atmno,lockno,psk);
	printf("验证第一开锁码完毕,时间是%u\n",pass1MatchTime);

	//////////////////////////////////////////////////////////////////////////
	//锁具生成验证码,第一开锁码作为生成要素,
	int VerifyDyCode=embSrvGenDyCode(JCCMD_CCB_LOCK_VERCODE,curTime,pass1DyCode,atmno,
		lockno,psk);
	printf("验证码=\t%d\n", VerifyDyCode);
	//密码服务器验证验证码
	printf("验证验证码开始\n");
	time_t vercodeMatchTime=embSrvReverseDyCode(JCCMD_CCB_LOCK_VERCODE,VerifyDyCode,pass1DyCode, 
		//time(NULL),
		curTime,
		atmno,lockno,psk);
	printf("验证验证码结束,时间是%u\n",vercodeMatchTime);

	//////////////////////////////////////////////////////////////////////////
	//密码服务器生成第二开锁码，验证码作为生成要素
	int pass2DyCode=embSrvGenDyCode(JCCMD_CCB_DYPASS2,curTime,VerifyDyCode,"atm10455761",
		"lock14771509","PSKDEMO728");
	printf("第二开锁码=\t%d\n", pass2DyCode);
	//锁具验证第二开锁码
	printf("验证第二开锁码开始\n");
	time_t pass2MatchTime=embSrvReverseDyCode(JCCMD_CCB_DYPASS2,pass2DyCode,VerifyDyCode,
		//time(NULL),
		curTime,
		atmno,lockno,psk);

	printf("验证第二开锁码结束,时间是%u\n",pass2MatchTime);

	//闭锁码，由3个基本条件和当前时间以及第二开锁码作为条件生成
	int curCloseCode=embSrvGenDyCode(JCCMD_CCB_CLOSECODE,curTime,pass2DyCode,atmno,lockno,psk);
	printf("闭锁码=\t%d\n", curCloseCode);
}

using std::string;
string zw3desTest311(const char *ccbKey)
{
	//8位动态码转换为字符串，然后字符串8字节转换为HEX，以便满足3DES的
	//64bit输入要求，估计这样就满足建行的要求可以被正确解密了；
	int ccbKeyLen=strlen(ccbKey);
	assert(16==ccbKeyLen);

	const int BUFLEN = 48;
	char buf[BUFLEN];
	memset(buf, 0, BUFLEN);
	char hexbuf[BUFLEN];
	memset(hexbuf, 0, BUFLEN);
	for (int i = 0; i < ccbKeyLen; i++) {
		unsigned char ch = ccbKey[i];
		sprintf(hexbuf + i * 2, "%02X", ch);
	}
	string retHexStr = hexbuf;
	return retHexStr;
}


int main(int argc, char * argv[])
{
	//myECIES_KeyGenTest123();
	//////////////////////////////////////////////////////////////////////////
	//myECIESTest305();

	//////////////////////////////////////////////////////////////////////////
	//myJclmsTest20150305();
	//myJclmsTest20150306STM32Demo();
	//printf("%s\n",zw3desTest311("0123456789ABCDEF").c_str());
	test4CCB3DES_ECB_EDE2();
	return 0;
}


