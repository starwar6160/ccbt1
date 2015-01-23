// stm32Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "zwEcies529.h"

void myECIES_KeyGenTest123(void)
{

		int hd = 0;
		hd = EciesGenKeyPair();
		//EXPECT_NE(hd, 0);
		char pubkey[ZW_ECIES_PUBKEY_LEN];
		char prikey[ZW_ECIES_PRIKEY_LEN];
		memset(pubkey, 0, sizeof(pubkey));
		memset(prikey, 0, sizeof(prikey));
		strcpy(pubkey, EciesGetPubKey(hd));
		strcpy(prikey, EciesGetPriKey(hd));
		//EXPECT_GT(strlen(pubkey), 0);
		//EXPECT_GT(strlen(prikey), 0);
		cout << "pubkey=\t" << pubkey << endl;
		cout << "prikey=\t" << prikey << endl;
		EciesDelete(hd);
}

int _tmain(int argc, _TCHAR* argv[])
{
	myECIES_KeyGenTest123();
	return 0;
}

