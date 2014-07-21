#include "stdafx.h"
#include "zwEcies529.h"

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

TEST(ECIES_Test,NormalKeyPairGen)
{
	EXPECT_GT(ZW_ECIES_PUBKEY_LEN,0);
	EXPECT_LT(ZW_ECIES_PUBKEY_LEN,100);
	//ZWECIES_API int zwEciesKeyPairGen( const char *password,char *outPriKeyStr,const int priLen,
	//	char *outPublicKeyStr ,const int pubLen);
	char priKey[ZW_ECIES_PRIKEY_LEN];
	char pubKey[ZW_ECIES_PUBKEY_LEN];
	memset(priKey,0,ZW_ECIES_PRIKEY_LEN);
	memset(pubKey,0,ZW_ECIES_PUBKEY_LEN);
	int keygenResult=zwEciesKeyPairGen("password",priKey,ZW_ECIES_PRIKEY_LEN,pubKey,ZW_ECIES_PUBKEY_LEN);
	EXPECT_EQ(keygenResult,ECIES_SUCCESS);
	EXPECT_GT(strlen(priKey),0)<<priKey;
	EXPECT_GT(strlen(pubKey),0)<<pubKey;
}

