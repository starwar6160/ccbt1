// stm32Test2.cpp : 定义控制台应用程序的入口点。
//

#include "zwEcies529.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>


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

int main(int argc, char * argv[])
{
	myECIES_KeyGenTest123();
	return 0;
}

