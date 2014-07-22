#include "stdafx.h"
#include "zwEcies529.h"
void myJcLockInputTest1();

namespace ccbtest722{

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



		static char g_priKey[ZW_ECIES_PRIKEY_LEN];
		static char g_pubKey[ZW_ECIES_PUBKEY_LEN];

class ECIES_Test : public testing::Test {
	// Some expensive resource shared by all tests.
	//	static T* shared_resource_;

protected:
	static void SetUpTestCase() {
		//shared_resource_ = new ;
		memset(g_priKey,0,ZW_ECIES_PRIKEY_LEN);
		memset(g_pubKey,0,ZW_ECIES_PUBKEY_LEN);
	}
	static void TearDownTestCase() {
		//delete shared_resource_;
		//shared_resource_ = NULL;
	}
};

TEST_F(ECIES_Test,NormalKeyPairGen)
{
	myJcLockInputTest1();
	EXPECT_GT(ZW_ECIES_PUBKEY_LEN,0);
	EXPECT_LT(ZW_ECIES_PUBKEY_LEN,100);
	//ZWECIES_API int zwEciesKeyPairGen( const char *password,char *outPriKeyStr,const int priLen,
	//	char *outPublicKeyStr ,const int pubLen);
	int keygenResult=zwEciesKeyPairGen("password",g_priKey,ZW_ECIES_PRIKEY_LEN,g_pubKey,ZW_ECIES_PUBKEY_LEN);
	EXPECT_EQ(keygenResult,ECIES_SUCCESS);
	EXPECT_GT(strlen(g_priKey),0)<<g_priKey;
	EXPECT_GT(strlen(g_pubKey),0)<<g_pubKey;
}


TEST_F(ECIES_Test,NormalEncDec)
{
	//ZWECIES_API int zwEciesEncrypt(const char *pubkeyStr,const char *PlainText, 
	//	char *outEncryptedSyncKeyStr,const int syncKeyLen, 
	//	char *outMsgHashStr,const int hashLen,
	//	char *outCryptedTextStr,const int cryptLen);


}


}	//namespace ccbtest722{
