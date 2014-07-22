#include "stdafx.h"
#include "zwEcies529.h"
void myJcLockInputTest1();

namespace CcbV11Test722Ecies{

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



char s_priKey[ZW_ECIES_PRIKEY_LEN];
char s_pubKey[ZW_ECIES_PUBKEY_LEN];
char *s_PlainText="zhouweiPlaintext20140722.1534Test";
char s_syncKey[ZW_ECIES_ENCSYNCKEY_LEN];
char s_hash[ZW_ECIES_HASH_LEN];
char s_crypt[ZW_ECIES_MESSAGE_MAXLEN];

class ECIES_Test : public testing::Test {
	// Some expensive resource shared by all tests.
	//	static T* shared_resource_;

protected:
	static void SetUpTestCase() {
		//shared_resource_ = new ;
		memset(s_priKey,0,sizeof(s_priKey));
		memset(s_pubKey,0,sizeof(s_pubKey));
		memset(s_syncKey,0,sizeof(s_syncKey));
		memset(s_hash,0,sizeof(s_hash));
		memset(s_crypt,0,sizeof(s_crypt));
		
	}
	static void TearDownTestCase() {
		//delete shared_resource_;
		//shared_resource_ = NULL;
	}
};

TEST_F(ECIES_Test,NormalKeyPairGen)
{
	//myJcLockInputTest1();
	EXPECT_GT(ZW_ECIES_PUBKEY_LEN,0);
	EXPECT_LT(ZW_ECIES_PUBKEY_LEN,100);
	//ZWECIES_API int zwEciesKeyPairGen( const char *password,char *outPriKeyStr,const int priLen,
	//	char *outPublicKeyStr ,const int pubLen);
	int keygenResult=zwEciesKeyPairGen("password",s_priKey,ZW_ECIES_PRIKEY_LEN,s_pubKey,ZW_ECIES_PUBKEY_LEN);
	EXPECT_EQ(keygenResult,ECIES_SUCCESS);
	EXPECT_GT(strlen(s_priKey),0);
	EXPECT_GT(strlen(s_pubKey),0);
#ifdef _DEBUG
cout<<"s_priKey=\t"<<s_priKey<<endl;
cout<<"s_pubKey=\t"<<s_pubKey<<endl;
#endif // _DEBUG
}

//ZWECIES_API int zwEciesEncrypt(const char *pubkeyStr,const char *PlainText, 
//	char *outEncryptedSyncKeyStr,const int syncKeyLen, 
//	char *outMsgHashStr,const int hashLen,
//	char *outCryptedTextStr,const int cryptLen);

TEST_F(ECIES_Test,NormalEnc)
{
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

//////////////////////////////////////////////////////////////////////////
}	//namespace ccbtest722{
