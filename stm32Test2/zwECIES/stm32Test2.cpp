// stm32Test2.cpp : 定义控制台应用程序的入口点。
//

#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;


#include "zwEcies529.h"

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
	//int hd = 0;
	//hd = EciesGenKeyPair();
	//EXPECT_NE(hd, 0);
	char pubkey[ZW_ECIES_PUBKEY_LEN];
	char prikey[ZW_ECIES_PRIKEY_LEN];
	memset(pubkey, 0, sizeof(pubkey));
	memset(prikey, 0, sizeof(prikey));
	//strcpy(pubkey, EciesGetPubKey(hd));
	//strcpy(prikey, EciesGetPriKey(hd));
	strcpy(pubkey,"BFlfjkxoiRZFdjQKa/W1JWBwFx+FPyzcFGqXjnlVzMcvIAQyK3C1Ha+G2uGUM4nX5khPQP5AiPFiCyuH2WxZefg=");
	strcpy(prikey,"y+tgryY83ibv2RaQeb93a97+JX0/9cpWf4MrmUUtrzs=");
	//EXPECT_GT(strlen(pubkey), 0);
	//EXPECT_GT(strlen(prikey), 0);
	cout << "pubkey=\t" << pubkey << endl;
	cout << "prikey=\t" << prikey << endl;
	//////////////////////////////////////////////////////////////////////////
	const char *csPlainText =
		"C# Port Test plain text 20140722.1625";
	char crypt[ZW_ECIES_CRYPT_TOTALLEN];
	memset(crypt, 0, sizeof(crypt));
	strcpy(crypt, EciesEncrypt(pubkey, csPlainText));
	cout<<"ECIES ENC Text= "<<crypt<<endl;
	//strcpy(crypt2, EciesEncrypt(pubkey, csPlainText));
	//EXPECT_GT(strlen(crypt), 0);
	//EXPECT_NE(0, memcmp(crypt, crypt2, ZW_ECIES_CRYPT_TOTALLEN));
	string crStr = EciesDecrypt(prikey, crypt);
	cout<<"ECIES DEC Text= "<<crStr<<endl;
	//EXPECT_GT(strlen(outPlain), 0);
	//如果生成了密钥对，那么别忘了删除，释放内存
	//EciesDelete(hd);
}

int main(int argc, char * argv[])
{
	myECIES_KeyGenTest123();
	return 0;
}

