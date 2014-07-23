#include "stdafx.h"
#include "zwEcies529.h"
#include "jclmsCCB2014.h"
void myJcLockInputTest1();

namespace CcbV11Test722Ecies{
const int ZWMEGA=1000*1000;
#ifdef _DEBUG722
int Foo(int a, int b)
{
	if (a == 0 || b == 0)
	{
		throw "don't do that";
	}
	int c = a % b;
	if (c == 0)
		return b;
	return Foo(b, c);
}

TEST(FooTest, HandleNoneZeroInput)
{
	EXPECT_EQ(2, Foo(4, 10));
	EXPECT_EQ(6, Foo(30, 18));
}
#endif // _DEBUG722


class ECIES_Test : public testing::Test {
	// Some expensive resource shared by all tests.
	//	static T* shared_resource_;
public:
	//注意要使得一个测试集合中的前后相关的各个测试共用变量
	//就需要设置变量为static才可以在多个测试中保留值
	//不然就会在每个测试新建一个新的类实例，变量被重新初始化，中间值不保留
	static char s_priKey[ZW_ECIES_PRIKEY_LEN];
	static char s_pubKey[ZW_ECIES_PUBKEY_LEN];
	static char s_syncKey[ZW_ECIES_ENCSYNCKEY_LEN];
	static char s_hash[ZW_ECIES_HASH_LEN];
	static char s_crypt[ZW_ECIES_MESSAGE_MAXLEN];
	static char *s_PlainText;

protected:
	static void SetUpTestCase() {
		//shared_resource_ = new ;		
	}
	static void TearDownTestCase() {
		//delete shared_resource_;
		//shared_resource_ = NULL;
	}
};
char ECIES_Test::s_priKey[ZW_ECIES_PRIKEY_LEN];
char ECIES_Test::s_pubKey[ZW_ECIES_PUBKEY_LEN];
char ECIES_Test::s_syncKey[ZW_ECIES_ENCSYNCKEY_LEN];
char ECIES_Test::s_hash[ZW_ECIES_HASH_LEN];
char ECIES_Test::s_crypt[ZW_ECIES_MESSAGE_MAXLEN];
char * ECIES_Test::s_PlainText;



TEST_F(ECIES_Test,NormalKeyPairGen)
{
	s_PlainText="zhouweiPlaintext20140722.1534Test";
	memset(s_priKey,0,sizeof(s_priKey));
	memset(s_pubKey,0,sizeof(s_pubKey));

	EXPECT_GT(ZW_ECIES_PUBKEY_LEN,0);
	EXPECT_LT(ZW_ECIES_PUBKEY_LEN,100);
	int keygenResult=zwEciesKeyPairGen("password",s_priKey,ZW_ECIES_PRIKEY_LEN,s_pubKey,ZW_ECIES_PUBKEY_LEN);
	EXPECT_EQ(keygenResult,ECIES_SUCCESS);
	EXPECT_GT(strlen(s_priKey),0);
	EXPECT_GT(strlen(s_pubKey),0);
#ifdef _DEBUG
cout<<"s_priKey=\t"<<s_priKey<<endl;
cout<<"s_pubKey=\t"<<s_pubKey<<endl;
#endif // _DEBUG
}

TEST_F(ECIES_Test,NormalEnc)
{
	memset(s_syncKey,0,sizeof(s_syncKey));
	memset(s_hash,0,sizeof(s_hash));
	memset(s_crypt,0,sizeof(s_crypt));
EXPECT_GT(strlen(s_priKey),0);
EXPECT_GT(strlen(s_pubKey),0);
int eciesEncRet=zwEciesEncrypt(s_pubKey,s_PlainText,s_syncKey,sizeof(s_syncKey),
	s_hash,sizeof(s_hash),s_crypt,sizeof(s_crypt));
EXPECT_EQ(eciesEncRet,ECIES_SUCCESS);
EXPECT_GT(strlen(s_syncKey),0);
EXPECT_GT(strlen(s_hash),0);
EXPECT_GT(strlen(s_crypt),0);
#ifdef _DEBUG
cout<<"syncKey=\t"<<s_syncKey<<endl;
cout<<"s_hash=\t"<<s_hash<<endl;
cout<<"s_crypt=\t"<<s_crypt<<endl;
#endif // _DEBUG
}

TEST_F(ECIES_Test,NormalDec)
{
	EXPECT_GT(strlen(s_syncKey),0);
	EXPECT_GT(strlen(s_hash),0);
	EXPECT_GT(strlen(s_crypt),0);
	char plainOut[ZW_ECIES_MESSAGE_MAXLEN];
	memset(plainOut,0,sizeof(plainOut));
	int eciesEncRet=zwEciesDecrypt(s_priKey,plainOut,sizeof(plainOut),
		s_syncKey,s_hash,s_crypt);
	EXPECT_EQ(eciesEncRet,ECIES_SUCCESS);
	EXPECT_GT(strlen(plainOut),0);
#ifdef _DEBUG
	cout<<"plainOut=\t"<<plainOut<<endl;
#endif // _DEBUG
}

TEST_F(ECIES_Test,csGenKeyPair)
{
	int hd=0;
	hd=EciesGenKeyPair();
	EXPECT_NE(hd,0);
	char pubkey[ZW_ECIES_PUBKEY_LEN];
	char prikey[ZW_ECIES_PRIKEY_LEN];
	memset(pubkey,0,sizeof(pubkey));
	memset(prikey,0,sizeof(prikey));
	strcpy(pubkey,EciesGetPubKey(hd));
	strcpy(prikey,EciesGetPriKey(hd));
	EXPECT_GT(strlen(pubkey),0);
	EXPECT_GT(strlen(prikey),0);
#ifdef _DEBUG
	cout<<"pubkey=\t"<<pubkey<<endl;
	cout<<"prikey=\t"<<prikey<<endl;
#endif // _DEBUG
	EciesDelete(hd);
}

TEST_F(ECIES_Test,csEncDec)
{
	const char *csPlainText="C# Port Test plain text 20140722.1625";
	int hd=0;
	hd=EciesGenKeyPair();
	EXPECT_NE(hd,0);
	char pubkey[ZW_ECIES_PUBKEY_LEN];
	char prikey[ZW_ECIES_PRIKEY_LEN];
	memset(pubkey,0,sizeof(pubkey));
	memset(prikey,0,sizeof(prikey));
	strcpy(pubkey,EciesGetPubKey(hd));
	strcpy(prikey,EciesGetPriKey(hd));
	EXPECT_GT(strlen(pubkey),0);
	EXPECT_GT(strlen(prikey),0);
	char crypt[ZW_ECIES_CRYPT_TOTALLEN];
	char outPlain[ZW_ECIES_CRYPT_TOTALLEN];
	memset(crypt,0,sizeof(crypt));
	memset(outPlain,0,sizeof(outPlain));
	strcpy(crypt, EciesEncrypt(pubkey,csPlainText));
	EXPECT_GT(strlen(crypt),0);
	strcpy(outPlain,EciesDecrypt(prikey,crypt));
	EXPECT_GT(strlen(outPlain),0);
	EciesDelete(hd);
#ifdef _DEBUG
	cout<<"pubkey=\t"<<pubkey<<endl;
	cout<<"prikey=\t"<<prikey<<endl;
	cout<<"ecies crypt combie result is"<<endl<<crypt<<endl;
#endif // _DEBUG

}



class jclmsCCBV11_Test:public testing::Test {
	// Some expensive resource shared by all tests.
	//	static T* shared_resource_;
public:
	static JCINPUT jc;
	static const int MY_DATETIME_TEST_VALUE=1400077751;
protected:
	static void SetUpTestCase() {
		//shared_resource_ = new ;
		//memset(s_priKey,0,sizeof(s_priKey));
		
	}
	static void TearDownTestCase() {
		//delete shared_resource_;
		//shared_resource_ = NULL;
	}
};

JCINPUT jclmsCCBV11_Test::jc;

/////////////////////////////////JCLMS算法测试/////////////////////////////////////////
TEST_F(jclmsCCBV11_Test,inputNew)
{		
	JcLockNew(&jc);
	//简单检查几个值，基本就可以判断是否初始化成功了
	EXPECT_EQ(strlen(jc.m_atmno),0);
	EXPECT_EQ(jc.m_datetime,JC_INVALID_VALUE);
}

TEST_F(jclmsCCBV11_Test,inputCheck)
{
	strncpy(jc.m_atmno,"ATMNO723",JC_ATMNO_MAXLEN);
	strncpy(jc.m_lockno,"LOCKNO1430",JC_LOCKNO_MAXLEN);
	strncpy(jc.m_psk,"PSKTESTJINCHU",JC_PSK_LEN);
	//注意现在合法的时间值应该是1.4G以上了，注意位数。20140721.1709
	jc.m_datetime=time(NULL);
	jc.m_validity=5;
	jc.m_closecode=87654325;
	jc.m_cmdtype=JCCMD_INIT_CLOSECODE;
	//检查输入是否合法
	EXPECT_EQ(EJC_SUSSESS,JcLockCheckInput(&jc));
}

//第一开锁码测试
TEST_F(jclmsCCBV11_Test,getDynaCodePass1)
{
	jc.m_cmdtype=JCCMD_INIT_CLOSECODE;
	JcLockDebugPrint(&jc);
	int initCloseCode=JcLockGetDynaCode(&jc);
	//检查初始闭锁码是否在正常范围内
	EXPECT_GT(initCloseCode,0);
	EXPECT_LT(initCloseCode,100000000);
	printf("initCloseCode=\t%d\n",initCloseCode);
	//dynaPass1
	jc.m_cmdtype=JCCMD_CCB_DYPASS1;
	jc.m_closecode=initCloseCode;
	int dynaPass1=JcLockGetDynaCode(&jc);
	EXPECT_GT(dynaPass1,10*ZWMEGA);
	EXPECT_LT(dynaPass1,100*ZWMEGA);
	printf("dynaPass1=\t%d\n",dynaPass1);
	JCMATCH pass1Match= JcLockReverseVerifyDynaCode(&jc,dynaPass1);
	EXPECT_GT(pass1Match.s_datetime,time(NULL)-60);
	EXPECT_LT(pass1Match.s_datetime,time(NULL)+15);
	printf("current time=\t%d\n",time(NULL));
	printf("pass1Match Time =\t%d\tValidity=%d\n",pass1Match.s_datetime,pass1Match.s_validity);
}

//下位机校验码测试
TEST_F(jclmsCCBV11_Test,getDynaCodeVerifyCode)
{
	jc.m_cmdtype=JCCMD_CCB_LOCK_VERCODE;
	int verCode=JcLockGetDynaCode(&jc);
	EXPECT_GT(verCode,10*ZWMEGA);
	EXPECT_LT(verCode,100*ZWMEGA);
	printf("verCode=\t%d\n",verCode);
	JCMATCH verCodeMatch=JcLockReverseVerifyDynaCode(&jc,verCode);
	EXPECT_GT(verCodeMatch.s_datetime,time(NULL)-60);
	EXPECT_LT(verCodeMatch.s_datetime,time(NULL)+15);
	printf("current time=\t%d\n",time(NULL));
	printf("verCodeMatch Time =\t%d\tValidity=%d\n",verCodeMatch.s_datetime,verCodeMatch.s_validity);

}

//////////////////////////////////////////////////////////////////////////
}	//namespace ccbtest722{
