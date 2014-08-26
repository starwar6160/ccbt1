#include "stdafx.h"
#include "zwEcies529.h"
#include "jclmsCCB2014.h"
void myJcLockInputTest1();

namespace CcbV11Test722Ecies{
const int ZWMEGA=1000*1000;
const int ZW_MATCHTIME_DIFF_START	=60*15;	//判断匹配结果时间往早允许的误差
const int ZW_MATCHTIME_DIFF_END		=60*5;	//判断匹配结果时间往晚允许的误差
const char *ts100="0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";
const char *g_invalid_testkey="myInvalidPubkey";
const char *g_invalid_prikey="myInvalidPrikey";

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
	//为了验证是否内存中同一个ECIES对象对于不同的明文确实加密出来不同的密文
	static char s_syncKey2[ZW_ECIES_ENCSYNCKEY_LEN];
	static char s_hash2[ZW_ECIES_HASH_LEN];
	static char s_crypt2[ZW_ECIES_MESSAGE_MAXLEN];

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
//为了验证是否内存中同一个ECIES对象对于不同的明文确实加密出来不同的密文
char ECIES_Test::s_syncKey2[ZW_ECIES_ENCSYNCKEY_LEN];
char ECIES_Test::s_hash2[ZW_ECIES_HASH_LEN];
char ECIES_Test::s_crypt2[ZW_ECIES_MESSAGE_MAXLEN];

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
//试试看同一个对象两次加密不同的明文，结果是否不同
string myPlainText2=s_PlainText;
myPlainText2=myPlainText2+"aaa";
zwEciesEncrypt(s_pubKey,myPlainText2.c_str(),s_syncKey2,sizeof(s_syncKey2),
	s_hash2,sizeof(s_hash2),s_crypt2,sizeof(s_crypt2));
EXPECT_NE(s_syncKey,s_syncKey2);
EXPECT_NE(s_hash,s_hash2);
EXPECT_NE(s_crypt,s_crypt2);
#ifdef _DEBUG
cout<<"syncKey=\t"<<s_syncKey<<endl;
cout<<"s_hash=\t"<<s_hash<<endl;
cout<<"s_crypt=\t"<<s_crypt<<endl;
cout<<"syncKey2=\t"<<s_syncKey2<<endl;
cout<<"s_hash2=\t"<<s_hash2<<endl;
cout<<"s_crypt2=\t"<<s_crypt2<<endl;

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

TEST_F(ECIES_Test,NormalKeyPairGen_BadInput)
{
	int keygenResult=ECIES_SUCCESS;
	keygenResult=zwEciesKeyPairGen("",
		s_priKey,ZW_ECIES_PRIKEY_LEN,s_pubKey,ZW_ECIES_PUBKEY_LEN);
	EXPECT_NE(keygenResult,ECIES_SUCCESS);
	keygenResult=zwEciesKeyPairGen("aaa",
		NULL,ZW_ECIES_PRIKEY_LEN,s_pubKey,ZW_ECIES_PUBKEY_LEN);
	EXPECT_NE(keygenResult,ECIES_SUCCESS);
	keygenResult=zwEciesKeyPairGen("aaa",
		s_priKey,0,s_pubKey,ZW_ECIES_PUBKEY_LEN);
	EXPECT_NE(keygenResult,ECIES_SUCCESS);
	keygenResult=zwEciesKeyPairGen("aaa",
		s_priKey,ZW_ECIES_PRIKEY_LEN,NULL,ZW_ECIES_PUBKEY_LEN);
	EXPECT_NE(keygenResult,ECIES_SUCCESS);
	keygenResult=zwEciesKeyPairGen("aaa",
		s_priKey,ZW_ECIES_PRIKEY_LEN,s_pubKey,0);
	EXPECT_NE(keygenResult,ECIES_SUCCESS);
}

TEST_F(ECIES_Test,NormalEnc_BadInput)
{
	memset(s_syncKey,0,sizeof(s_syncKey));
	memset(s_hash,0,sizeof(s_hash));
	memset(s_crypt,0,sizeof(s_crypt));
	int eciesEncRet=ECIES_SUCCESS;
	eciesEncRet=zwEciesEncrypt(NULL,s_PlainText,s_syncKey,sizeof(s_syncKey),
		s_hash,sizeof(s_hash),s_crypt,sizeof(s_crypt));
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
	eciesEncRet=zwEciesEncrypt(s_pubKey,NULL,s_syncKey,sizeof(s_syncKey),
		s_hash,sizeof(s_hash),s_crypt,sizeof(s_crypt));
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
	eciesEncRet=zwEciesEncrypt(s_pubKey,s_PlainText,NULL,sizeof(s_syncKey),
		s_hash,sizeof(s_hash),s_crypt,sizeof(s_crypt));
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
	eciesEncRet=zwEciesEncrypt(s_pubKey,s_PlainText,s_syncKey,0,
		s_hash,sizeof(s_hash),s_crypt,sizeof(s_crypt));
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
	eciesEncRet=zwEciesEncrypt(s_pubKey,s_PlainText,s_syncKey,sizeof(s_syncKey),
		NULL,sizeof(s_hash),s_crypt,sizeof(s_crypt));
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
	eciesEncRet=zwEciesEncrypt(s_pubKey,s_PlainText,s_syncKey,sizeof(s_syncKey),
		s_hash,0,s_crypt,sizeof(s_crypt));
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
	eciesEncRet=zwEciesEncrypt(s_pubKey,s_PlainText,s_syncKey,sizeof(s_syncKey),
		s_hash,sizeof(s_hash),NULL,sizeof(s_crypt));
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
	eciesEncRet=zwEciesEncrypt(s_pubKey,s_PlainText,s_syncKey,sizeof(s_syncKey),
		s_hash,sizeof(s_hash),s_crypt,0);
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
}

TEST_F(ECIES_Test,NormalEnc_TooLongInput)
{
	memset(s_syncKey,0,sizeof(s_syncKey));
	memset(s_hash,0,sizeof(s_hash));
	memset(s_crypt,0,sizeof(s_crypt));
	int eciesEncRet=ECIES_SUCCESS;
	//超长公钥
	eciesEncRet=zwEciesEncrypt(ts100,s_PlainText,s_syncKey,sizeof(s_syncKey),
		s_hash,sizeof(s_hash),s_crypt,sizeof(s_crypt));
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
	//不超长但是内容非法公钥
	eciesEncRet=zwEciesEncrypt(g_invalid_testkey,s_PlainText,s_syncKey,sizeof(s_syncKey),
		s_hash,sizeof(s_hash),s_crypt,sizeof(s_crypt));
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
	//超长明文
	eciesEncRet=zwEciesEncrypt(g_invalid_testkey,ts100,s_syncKey,sizeof(s_syncKey),
		s_hash,sizeof(s_hash),s_crypt,sizeof(s_crypt));
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
}


TEST_F(ECIES_Test,NormalDec_BadInput)
{
	char plainOut[ZW_ECIES_MESSAGE_MAXLEN];
	memset(plainOut,0,sizeof(plainOut));
	int eciesEncRet=ECIES_SUCCESS;
	eciesEncRet=zwEciesDecrypt(NULL,plainOut,sizeof(plainOut),
		s_syncKey,s_hash,s_crypt);
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
	eciesEncRet=zwEciesDecrypt(s_priKey,NULL,sizeof(plainOut),
		s_syncKey,s_hash,s_crypt);
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
	eciesEncRet=zwEciesDecrypt(s_priKey,plainOut,0,
		s_syncKey,s_hash,s_crypt);
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
	eciesEncRet=zwEciesDecrypt(s_priKey,plainOut,sizeof(plainOut),
		NULL,s_hash,s_crypt);
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
	eciesEncRet=zwEciesDecrypt(s_priKey,plainOut,sizeof(plainOut),
		s_syncKey,NULL,s_crypt);
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
	eciesEncRet=zwEciesDecrypt(s_priKey,plainOut,sizeof(plainOut),
		s_syncKey,s_hash,NULL);
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);

}

TEST_F(ECIES_Test,NormalDec_TooLongInput)
{
	char plainOut[ZW_ECIES_MESSAGE_MAXLEN];
	memset(plainOut,0,sizeof(plainOut));
	int eciesEncRet=ECIES_SUCCESS;
	//超长私钥
	eciesEncRet=zwEciesDecrypt(ts100,plainOut,sizeof(plainOut),
		s_syncKey,s_hash,s_crypt);
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
	//不超长但是内容非法私钥
	eciesEncRet=zwEciesDecrypt(g_invalid_testkey,plainOut,sizeof(plainOut),
		s_syncKey,s_hash,s_crypt);
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);
	//不超长但是内容非法syncKey,hash,crypt
	eciesEncRet=zwEciesDecrypt(g_invalid_testkey,plainOut,sizeof(plainOut),
		g_invalid_testkey,g_invalid_testkey,g_invalid_testkey);
	EXPECT_NE(eciesEncRet,ECIES_SUCCESS);

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

//C#接口的非法输入测试
TEST_F(ECIES_Test,cs_BadInput)
{
	int hd=0;
	hd=EciesGenKeyPair();
	EXPECT_GT(hd,0);

	char *pubKey=NULL;	
	char *priKey=NULL;

	pubKey=(char *)EciesGetPubKey(NULL);
	priKey=(char *)EciesGetPriKey(NULL);
	EXPECT_EQ(NULL,pubKey);
	EXPECT_EQ(NULL,priKey);
	char *crypt=NULL;
	crypt=(char *)EciesEncrypt(NULL,"palintext");
	EXPECT_EQ(NULL,crypt);
	crypt=(char *)EciesEncrypt("pubkey",NULL);
	EXPECT_EQ(NULL,crypt);
	crypt=(char *)EciesDecrypt("pubkey",NULL);
	EXPECT_EQ(NULL,crypt);
	crypt=(char *)EciesDecrypt(NULL,"crypttext");
	EXPECT_EQ(NULL,crypt);

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
	char crypt2[ZW_ECIES_CRYPT_TOTALLEN];
	char outPlain[ZW_ECIES_CRYPT_TOTALLEN*2];
	memset(crypt,0,sizeof(crypt));
	memset(outPlain,0,sizeof(outPlain));
	strcpy(crypt, EciesEncrypt(pubkey,csPlainText));
	strcpy(crypt2, EciesEncrypt(pubkey,csPlainText));
	EXPECT_GT(strlen(crypt),0);
	EXPECT_NE(0,memcmp(crypt,crypt2,ZW_ECIES_CRYPT_TOTALLEN));
	string crStr=EciesDecrypt(prikey,crypt);
	strcpy(outPlain,crStr.c_str());
	EXPECT_GT(strlen(outPlain),0);
	EciesDelete(hd);
#ifdef _DEBUG
	cout<<"pubkey=\t"<<pubkey<<endl;
	cout<<"prikey=\t"<<prikey<<endl;
	cout<<"ecies crypt combie result is"<<endl<<crypt<<endl;
	cout<<"ecies crypt2 combie result is"<<endl<<crypt2<<endl;
#endif // _DEBUG

}

TEST_F(ECIES_Test,cs_TooLongInput)
{
	char *pubKey=NULL;	
	char *priKey=NULL;

	pubKey=(char *)EciesGetPubKey(NULL);
	priKey=(char *)EciesGetPriKey(NULL);
	EXPECT_EQ(NULL,pubKey);
	EXPECT_EQ(NULL,priKey);
	char *crypt=NULL;
	//超长公钥输入
	crypt=(char *)EciesEncrypt(ts100,"palintext");
	EXPECT_EQ(NULL,crypt);
	//内容长度合法但是根本不是公钥的输入
	crypt=(char *)EciesEncrypt("pubkey","plaintext");
	EXPECT_EQ(NULL,crypt);
	//超长明文输入
	crypt=(char *)EciesEncrypt("pubkey",ts100);
	EXPECT_EQ(NULL,crypt);

	//超长私钥输入
	crypt=(char *)EciesDecrypt(ts100,"crypttext");
	EXPECT_EQ(NULL,crypt);
	//内容长度合法但是根本不是私钥的输入
	crypt=(char *)EciesDecrypt("myFakePrivateKey","crypt.text.333");
	EXPECT_EQ(NULL,crypt);
	//超长密文输入
	crypt=(char *)EciesDecrypt("pubkey",ts100);
	EXPECT_EQ(NULL,crypt);

}



class jclmsCCBV11_Test:public testing::Test {
	// Some expensive resource shared by all tests.
	//	static T* shared_resource_;
public:
	static int handle;
	static int pass1DyCode;
	static int verifyCode;
	static int pass2DyCode;
protected:
	static void SetUpTestCase() {
		//shared_resource_ = new ;
		//memset(s_priKey,0,sizeof(s_priKey));
		handle=JcLockNew();
		JcLockSetString(handle,JCI_ATMNO,"atm10455761");
		JcLockSetString(handle,JCI_LOCKNO,"lock14771509");
		JcLockSetString(handle,JCI_PSK,"PSKDEMO728");

		
	}
	static void TearDownTestCase() {
		//delete shared_resource_;
		//shared_resource_ = NULL;
		JcLockDelete(handle);
		handle=0;
	}
};

int jclmsCCBV11_Test::handle;
int jclmsCCBV11_Test::pass1DyCode;
int jclmsCCBV11_Test::verifyCode;
int jclmsCCBV11_Test::pass2DyCode;


/////////////////////////////////JCLMS算法测试/////////////////////////////////////////
TEST_F(jclmsCCBV11_Test,CloseCode)
{
	JcLockSetCmdType(handle,JCI_CMDTYPE,JCCMD_CCB_CLOSECODE);
	int CloseCode=JcLockGetDynaCode(handle);
	cout<<"CloseCode729=\t"<<CloseCode<<endl;
	//检查闭锁码是否在正常范围内
	EXPECT_GT(CloseCode,10*ZWMEGA);
	EXPECT_LT(CloseCode,100*ZWMEGA);
	JCMATCH ccodeMatch= JcLockReverseVerifyDynaCode(handle,CloseCode);
	EXPECT_GT(ccodeMatch.s_datetime,1400*ZWMEGA);
}


TEST_F(jclmsCCBV11_Test,inputNew)
{		
	//简单检查几个值，基本就可以判断是否初始化成功了
	EXPECT_GT(handle,0);
}

TEST_F(jclmsCCBV11_Test,inputCheck)
{
	//生成初始闭锁码的时候，有效期和闭锁码字段都无效，随便填写，是正整数就可以
	JcLockSetInt(handle,JCI_VALIDITY,5);
	JcLockSetInt(handle,JCI_CLOSECODE,0);
	JcLockSetCmdType(handle,JCI_CMDTYPE,JCCMD_INIT_CLOSECODE);
	//JcLockDebugPrint(handle);
	//检查输入是否合法
	EXPECT_EQ(EJC_SUSSESS,JcLockCheckInput(handle));
}

//第一开锁码测试
TEST_F(jclmsCCBV11_Test,getDynaCodePass1)
{
	JcLockSetCmdType(handle,JCI_CMDTYPE,JCCMD_INIT_CLOSECODE);
	JcLockDebugPrint(handle);
	int initCloseCode=JcLockGetDynaCode(handle);
	//检查初始闭锁码是否在正常范围内
	EXPECT_GT(initCloseCode,0);
	EXPECT_LT(initCloseCode,100000000);
	printf("initCloseCode=\t%d\n",initCloseCode);
	//此处期待值由于时间秒数按照一百万秒取整的原因，每隔12天需要更新一次
	//这里是一个自检测试，如果失败，就说明有比较大的问题了，比如类似发生过的
	//ARM编译器优化级别问题导致的生成错误的二进制代码等等
	EXPECT_EQ(48731379,initCloseCode);
	//dynaPass1
	//注意现在合法的时间值应该是1.4G以上了，注意位数。20140721.1709	
	JcLockSetInt(handle,JCI_DATETIME,static_cast<int>(time(NULL)));
	JcLockSetCmdType(handle,JCI_CMDTYPE,JCCMD_CCB_DYPASS1);
	JcLockSetInt(handle,JCI_CLOSECODE,initCloseCode);	
	JcLockDebugPrint(handle);
	pass1DyCode=JcLockGetDynaCode(handle);	
	EXPECT_GT(pass1DyCode,10*ZWMEGA);
	EXPECT_LT(pass1DyCode,100*ZWMEGA);
	printf("dynaPass1=\t%d\n",pass1DyCode);
	JCMATCH pass1Match= JcLockReverseVerifyDynaCode(handle,pass1DyCode);
	EXPECT_GT(pass1Match.s_datetime,time(NULL)-ZW_MATCHTIME_DIFF_START);
	EXPECT_LT(pass1Match.s_datetime,time(NULL)+ZW_MATCHTIME_DIFF_END);
	printf("current time=\t\t%d\n",time(NULL));
	printf("pass1Match Time =\t%d\tValidity=%d\n",pass1Match.s_datetime,pass1Match.s_validity);
}


//下位机校验码测试
TEST_F(jclmsCCBV11_Test,getDynaCodeVerifyCode)
{
	JcLockSetCmdType(handle,JCI_CMDTYPE,JCCMD_CCB_LOCK_VERCODE);
	//第一开锁码作为要素参与生成校验码
	JcLockSetInt(handle,JCI_CLOSECODE,pass1DyCode);
	JcLockDebugPrint(handle);
	verifyCode=JcLockGetDynaCode(handle);
	EXPECT_GT(verifyCode,10*ZWMEGA);
	EXPECT_LT(verifyCode,100*ZWMEGA);
	printf("verCode=\t%d\n",verifyCode);
	JCMATCH verCodeMatch=JcLockReverseVerifyDynaCode(handle,verifyCode);
	EXPECT_GT(verCodeMatch.s_datetime,time(NULL)-ZW_MATCHTIME_DIFF_START);
	EXPECT_LT(verCodeMatch.s_datetime,time(NULL)+ZW_MATCHTIME_DIFF_END);
	printf("current time=\t\t%d\n",time(NULL));
	printf("verCodeMatch Time =\t%d\tValidity=%d\n",verCodeMatch.s_datetime,verCodeMatch.s_validity);
}

//第二开锁码测试
TEST_F(jclmsCCBV11_Test,getDynaCodePass2)
{
	JcLockSetCmdType(handle,JCI_CMDTYPE,JCCMD_CCB_DYPASS2);
	//校验码作为要素参与生成第二开锁码
	JcLockSetInt(handle,JCI_CLOSECODE,verifyCode);
	JcLockDebugPrint(handle);
	pass2DyCode=JcLockGetDynaCode(handle);
	EXPECT_GT(pass2DyCode,10*ZWMEGA);
	EXPECT_LT(pass2DyCode,100*ZWMEGA);
	printf("pass2DyCode=\t%d\n",pass2DyCode);
	JCMATCH pass2Match=JcLockReverseVerifyDynaCode(handle,pass2DyCode);
	EXPECT_GT(pass2Match.s_datetime,time(NULL)-ZW_MATCHTIME_DIFF_START);
	EXPECT_LT(pass2Match.s_datetime,time(NULL)+ZW_MATCHTIME_DIFF_END);
	printf("current time=\t\t%d\n",time(NULL));
	printf("pass2Match Time =\t%d\tValidity=%d\n",pass2Match.s_datetime,pass2Match.s_validity);
}


//////////////////////////////////////////////////////////////////////////
}	//namespace ccbtest722{
