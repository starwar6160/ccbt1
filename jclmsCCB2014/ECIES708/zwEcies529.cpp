#ifdef  __cplusplus
extern "C" {
#endif
#include "ecdh.h"
#ifdef  __cplusplus
}
#endif
#include "..\stdafx.h"
//#include <assert.h>
#include <string>
#include <vector>
using std::string;
using std::vector;

void zwRandSeedGen603(char *randBuf,const int randBufLen);

//方便在HEX和BASE64之间切换
enum zwOutputASCII_Format{
	ZWOUTFMT_HEX,
	ZWOUTFMT_BASE64
};
//控制ECIES算法的输入输出封装使用BASE64还是HEX抑或是其他可能的格式
const static int MYOUT_FORMAT=
	ZWOUTFMT_BASE64;
	//ZWOUTFMT_HEX;



#ifdef  __cplusplus
extern "C" {
#endif
#include "ecdh.h"
#include "octet.h"
#include "zwEcies529.h"

	//此处的初始化值控制着同样的明文，出来不同的对称密钥
	void myPrngInit( csprng *RNG );
	/* Convert an octet string to base64 string */
	//输入b的大小要是w->max的2倍+1个字节
	void ZWOCTET_FROM_ASCII(octet *dstOct,const char *srcAscii);
	int ZWOCTET_TO_ASCII(const octet *srcOct,char *dstAscii,const int dstLen);
	void myKDFHMACSeed( octet *KDF2Seed, octet *HmacSeed );
	int ZWOCTET_OUTPUT_STRING(const octet *srcOct,char *dstStr,const int dstLen);


#ifdef  __cplusplus
}
#endif


class jcOctex
{
public:
	jcOctex(int MaxSize){
		assert(MaxSize>0);
		m_oct.len=0;
		m_oct.max=MaxSize;
		m_oct.val=new char[MaxSize];
		memset(m_oct.val,0,MaxSize);
	};
	~jcOctex(){
		assert(m_oct.max>0);
		assert(m_oct.max>=m_oct.len);
		assert(m_oct.val!=NULL);
		memset(m_oct.val,0,m_oct.max);
		delete [] m_oct.val;
		m_oct.len=0;
		m_oct.max=0;
	};
	octet &Value(){return m_oct;};
private:
	octet m_oct;
};

//此处的初始化值控制着同样的明文，出来不同的对称密钥
void myPrngInit( csprng *RNG )
{
	assert(RNG!=NULL);
	int i;
	/* Crypto Strong RNG */
	jcOctex RAW(EFS*3);
	RAW.Value().len=EFS*3;				/* fake random seed source */
	zwRandSeedGen603(RAW.Value().val,RAW.Value().max);
	assert(RNG!=NULL);	
	//for (i=0;i<EFS*3;i++) RAW.Value().val[i]=i+1;
	//用前面的假的随机数种子来初始化“密码学强度伪随机数生成器”
	//该生成器函数本身很强，但是其种子是不是真随机数是弱点的根本
	//因为通常真随机数生成器的输出速率都极为有限，所以需要使用
	//真随机数来初始化“密码学强度伪随机数生成器”
	CREATE_CSPRNG(RNG,&RAW.Value());   /* initialise strong RNG */
}


/* Convert an octet string to HEX string */
//输入b的大小要是w->max的2倍+1个字节
int ZWOCTET_TO_ASCII( const octet *srcOct,char *dstAscii,const int dstLen )
{
	int i=0;
	assert(dstAscii!=NULL);
	assert(srcOct!=NULL);
	assert(srcOct->len>0);
	assert(srcOct->max>=srcOct->len);
	assert(dstAscii!=0);	
	
	if (ZWOUTFMT_BASE64==MYOUT_FORMAT)
	{		
		if (srcOct->len*4/3+1>dstLen)
		{
			return ECIES_OUTPUT_BUFFER_TOO_SHORT;
		}
		assert(srcOct->len*4/3+1<=dstLen);
		OCTET_TO_BASE64(srcOct,dstAscii);
	}
//////////////////////////////////////////////////////////////////////////
	if (ZWOUTFMT_HEX==MYOUT_FORMAT)
	{		
		if (srcOct->len*2+1>dstLen)
		{
			return ECIES_OUTPUT_BUFFER_TOO_SHORT;
		}
		assert(srcOct->len*2+1<=dstLen);
		//从BIN到HEX需要*2，末尾NULL需要+1
		int needBytes=srcOct->len*2+1;
		if (needBytes>dstLen)
		{		
			printf("Need %d bytes Real %d bytes\n",needBytes,dstLen);
			return ECIES_OUTPUT_BUFFER_TOO_SHORT;
		}

		for (i=0;i<srcOct->len;i++)
		{
			unsigned int ch=srcOct->val[i] & 0xFF;
			sprintf(dstAscii+i*2,"%02X",ch);
		}
		dstAscii[srcOct->len*2]=(char)NULL;
	}//ZWOUTFMT_HEX

	
	return ECIES_SUCCESS;
}

void ZWOCTET_FROM_ASCII( octet *dstOct,const char *srcAscii )
{
	int inLen=strlen(srcAscii);
	int i=0;
	assert(srcAscii!=NULL && strlen(srcAscii)>0);
	assert(dstOct!=NULL && dstOct->max>0);
	if (ZWOUTFMT_BASE64==MYOUT_FORMAT)
	{
		OCTET_FROM_BASE64(srcAscii,dstOct);
	}
//////////////////////////////////////////////////////////////////////////
	if (ZWOUTFMT_HEX==MYOUT_FORMAT)
	{
	for (i=0;i<inLen/2;i++)
	{
		char *dst=dstOct->val+i;
		sscanf(srcAscii+2*i,"%02X",dst);
		dstOct->len++;
	}
	}	//ZWOUTFMT_HEX
}

void myKDFHMACSeed( octet *KDF2Seed, octet *HmacSeed )
{
	assert(KDF2Seed!=NULL && KDF2Seed->max>0);
	assert(HmacSeed!=NULL && HmacSeed->max>0);
	HmacSeed->val[3]=0x3; 
	//KDF2Seed是用在KDF2中  HmacSeed是计算MAC中的,都可以随意指定，只需要两端保持一致
	//所以，KDF2Seed的变更会引起CryptedText的变更，HmacSeed的变更会引起MsgHash的变更
	KDF2Seed->len=3; KDF2Seed->val[0]=0x90; KDF2Seed->val[1]=0x1; KDF2Seed->val[2]=0x2; 
	HmacSeed->len=4; HmacSeed->val[0]=0x41; HmacSeed->val[1]=0x1; HmacSeed->val[2]=0x2; 
}

int ZWOCTET_OUTPUT_STRING( const octet *srcOct,char *dstStr,const int dstLen )
{
	int i;
	unsigned char ch;
	if ((srcOct->len+1)>dstLen)
	{
		return ECIES_OUTPUT_BUFFER_TOO_SHORT;
	}
	
	for (i=0;i<srcOct->len;i++)
	{
		ch=srcOct->val[i];
		dstStr[i]=ch;
	}
	dstStr[srcOct->len]=NULL;
	return ECIES_SUCCESS;
}  


//初始化ECIES，并生成私钥和公钥
ZWECIES_API int zwEciesKeyPairGen( const char *password,char *outPriKeyStr,const int priLen, char *outPublicKeyStr ,const int pubLen )
{	
	if (password==NULL || strlen(password)==0)
	{
		return ECIES_INPUT_TOO_SHORT;
	}
	if (priLen==0 || pubLen==0)
	{
		return ECIES_INPUT_TOO_SHORT;
	}
	if (outPriKeyStr==NULL || outPublicKeyStr==NULL)
	{
		return ECIES_INPUT_NULL;
	}
	
	assert(password!=NULL && strlen(password)>0);
	assert(outPriKeyStr!=NULL);
	assert(outPublicKeyStr!=NULL);
	assert(priLen>0 && pubLen>0);

	//从固定存储读取ECC曲线参数
	ecp_domain eciesCtx;
	ECP_DOMAIN_INIT(&eciesCtx,ecrom);	

	//此处SALT控制着同样的password出来不同的公钥和私钥
	//可以作为一个写死的秘密值
	jcOctex PW(EFS),SALT(EFS);
	//注意此处，SALT的长度必须要正确初始化，否则SALT就会不起作用导致
	//同一个password生成的密钥对始终是同一个
	SALT.Value().len=EFS;
	zwRandSeedGen603(SALT.Value().val,EFS);
	//把pp的字符串密码放到PW的OCTET里面
	OCTET_JOIN_STRING(password,&PW.Value());   // set Password from string
	/* private key S0 of size EGS bytes derived from Password and Salt */
	//从用户密码和SALT通过PBKDF2派生出来256bit(EGS决定)的ECC私钥S1
	//也就是说私钥可以是任意的，再由私钥去生成公钥
	jcOctex oct_private_key(ZW_ECIES_PRIKEY_LEN);
	PBKDF2(&PW.Value(),&SALT.Value(),1000,EGS,&oct_private_key.Value());
	if (ECIES_SUCCESS!=
		ZWOCTET_TO_ASCII(&oct_private_key.Value(),outPriKeyStr, priLen))
	{
		printf("privateKey Buffer Too Short\n");
		return (ECIES_PRIKEY_TOO_SHORT);
	}
	//printf("Server private key= %s\n",outPriKeyHex);

	//从私钥S1生成公钥W1
	jcOctex oct_public_key(ZW_ECIES_PUBKEY_LEN);
	ECP_KEY_PAIR_GENERATE(&eciesCtx,NULL,&oct_private_key.Value(),&oct_public_key.Value());
	//检验生成的公钥正确性
	int res=ECP_PUBLIC_KEY_VALIDATE(&eciesCtx,TRUE,&oct_public_key.Value());
	if (res!=0)
	{
		printf("ECP Public Key is invalid!\n");
		return ECIES_PUBKEY_GEN_FAIL;
	}
	if (ECIES_SUCCESS!=
	ZWOCTET_TO_ASCII(&oct_public_key.Value(),outPublicKeyStr, pubLen))
	{
		printf("publicKey Buffer Too Short\n");
		return (ECIES_PUBKEY_TOO_SHORT);
	}
	assert(strlen(outPublicKeyStr)>0);
	//printf("Servers public key= %s\n",outPublicKeyHex); 	
	return ECIES_SUCCESS;
}

ZWECIES_API int zwEciesEncrypt( const char *pubkeyStr,const char *PlainText, char *outEncryptedSyncKeyStr,const int syncKeyLen, char *outMsgHashStr,const int hashLen, char *outCryptedTextStr,const int cryptLen )
{		      
	if (pubkeyStr==NULL || strlen(pubkeyStr)==0)
	{
		return ECIES_INPUT_NULL;
	}
	if (PlainText==NULL || strlen(PlainText)==0)
	{
		return ECIES_INPUT_NULL;
	}
	if (outEncryptedSyncKeyStr==NULL || syncKeyLen==0)
	{
		return ECIES_INPUT_NULL;
	}	
	if (outCryptedTextStr==NULL || cryptLen==0)
	{
		return ECIES_INPUT_NULL;
	}	
	if (outMsgHashStr==NULL || hashLen==0)
	{
		return ECIES_INPUT_NULL;
	}	
	//公钥从HEX字符串化为octet的内部格式的数据区			
	assert(pubkeyStr!=NULL && strlen(pubkeyStr)>0);
	assert(PlainText!=NULL && strlen(PlainText)>0);
	assert(outEncryptedSyncKeyStr!=NULL && syncKeyLen>0);
	assert(outCryptedTextStr!=NULL && cryptLen>0);
	assert(outMsgHashStr!=NULL && hashLen>0);

	//公钥从HEX字符串化为octet的内部格式
	jcOctex oct_pubkey(ZW_ECIES_PUBKEY_LEN);
	jcOctex oct_PlainText(ZW_ECIES_MESSAGE_MAXLEN);
	ZWOCTET_FROM_ASCII(&oct_pubkey.Value(),pubkeyStr);
	OCTET_JOIN_STRING(PlainText,&oct_PlainText.Value());

	//从固定存储读取ECC曲线参数
	ecp_domain ecies_ctx;
	ECP_DOMAIN_INIT(&ecies_ctx,ecrom);	

	//加密函数内部初始化KDF和HMAC的种子，作为我们的算法的特有秘密因素
	jcOctex KDF2Seed(EFS),HmacSeed(EFS);
	myKDFHMACSeed(&KDF2Seed.Value(),&HmacSeed.Value());
	csprng RNG; 
	myPrngInit(&RNG);
	//此处加密时MSGHASH的长度最大值，也就是MsgHash->max指定多长，出来就有多长
	//而并不影响解密结果的正确性，不知道开始的MIRACL例子代码中直接指定魔术数字12
	//是什么意思？	20140522.1045.周伟
	jcOctex oct_syncKey(ZW_ECIES_ENCSYNCKEY_LEN);
	jcOctex oct_hash(ZW_ECIES_HASH_LEN);
	jcOctex oct_crypt(ZW_ECIES_MESSAGE_MAXLEN);
	ECP_ECIES_ENCRYPT(&ecies_ctx,&KDF2Seed.Value(),&HmacSeed.Value(),&RNG,&oct_pubkey.Value(),
		&oct_PlainText.Value(),oct_hash.Value().max/2,&oct_syncKey.Value(),&oct_crypt.Value(),&oct_hash.Value());
	if (ECIES_SUCCESS!=ZWOCTET_TO_ASCII(&oct_syncKey.Value(),outEncryptedSyncKeyStr, syncKeyLen))
	{
		printf("EncryptedSyncKey Buffer Too Short\n");
		return (ECIES_ENCEDSYNCKEY_TOO_SHORT);
	}
	if (ECIES_SUCCESS!=ZWOCTET_TO_ASCII(&oct_hash.Value(),outMsgHashStr, hashLen))
	{
		printf("MsgHash Buffer Too Short\n");
		return (ECIES_HASH_TOO_SHORT);
	}
	if (ECIES_SUCCESS!=ZWOCTET_TO_ASCII(&oct_crypt.Value(),outCryptedTextStr, cryptLen))
	{
		printf("CryptedText Buffer Too Short\n");
		return (ECIES_CRYPT_TOO_SHORT);
	}
	KILL_CSPRNG(&RNG);
	return ECIES_SUCCESS;
}


//出参PlainText需要自己保证足够长，不过鉴于一般都是加密对称密钥，所以某个不长的定长肯定就可以了
ZWECIES_API BOOL zwEciesDecrypt( const char *prikeyStr,char *outPlainText,const int plainLen, const char *EncryptedSyncKeyStr,const char *MsgHashStr,const char *CryptedTextStr )
{
	if (prikeyStr==NULL || strlen(prikeyStr)==0)
	{
		return ECIES_INPUT_NULL;
	}
	if (EncryptedSyncKeyStr==NULL || strlen(EncryptedSyncKeyStr)==0)
	{
		return ECIES_INPUT_NULL;
	}
	if (MsgHashStr==NULL || strlen(MsgHashStr)==0)
	{
		return ECIES_INPUT_NULL;
	}
	if (CryptedTextStr==NULL || strlen(CryptedTextStr)==0)
	{
		return ECIES_INPUT_NULL;
	}
	if (outPlainText==NULL || plainLen==0)
	{
		return ECIES_INPUT_NULL;
	}
	//入口参数简单校验
	assert(prikeyStr!=NULL && strlen(prikeyStr)>0);	
	assert(EncryptedSyncKeyStr!=NULL &&strlen(EncryptedSyncKeyStr)>0);
	assert(MsgHashStr!=NULL && strlen(MsgHashStr)>0);
	assert(CryptedTextStr!=NULL && strlen(CryptedTextStr)>0);	
	assert(outPlainText!=NULL && plainLen>0);

	//私钥从base64字符串化为octet的内部格式
	jcOctex oct_prikey(ZW_ECIES_PRIKEY_LEN);
	jcOctex oct_sykey(ZW_ECIES_ENCSYNCKEY_LEN);
	jcOctex oct_hash(ZW_ECIES_HASH_LEN);
	jcOctex oct_crypted_bin(ZW_ECIES_MESSAGE_MAXLEN);
	jcOctex oct_plain(ZW_ECIES_MESSAGE_MAXLEN);
	ZWOCTET_FROM_ASCII(&oct_prikey.Value(),prikeyStr);
	ZWOCTET_FROM_ASCII(&oct_sykey.Value(),EncryptedSyncKeyStr);
	ZWOCTET_FROM_ASCII(&oct_hash.Value(),MsgHashStr);
	ZWOCTET_FROM_ASCII(&oct_crypted_bin.Value(),CryptedTextStr);	
	

	//从固定存储读取ECC曲线参数
	ecp_domain ecies_ctx;
	ECP_DOMAIN_INIT(&ecies_ctx,ecrom);	

	//解密函数内部初始化KDF和HMAC的种子，作为我们的算法的特有秘密因素，
	//该种子需要和加密函数内部初始化的种子值完全一致才能正确解密；
	jcOctex KDF2Seed(EFS),HmacSeed(EFS);
	myKDFHMACSeed(&KDF2Seed.Value(),&HmacSeed.Value());
	BOOL res=ECP_ECIES_DECRYPT(&ecies_ctx,&KDF2Seed.Value(),&HmacSeed.Value(),
		&oct_sykey.Value(),&oct_crypted_bin.Value(),
		&oct_hash.Value(),&oct_prikey.Value(),&oct_plain.Value());

	if (ECIES_SUCCESS!=ZWOCTET_OUTPUT_STRING(&oct_plain.Value(),outPlainText, plainLen))
	{
		printf("PlainText Buffer Too Short\n");
		return ECIES_PLAIN_TOO_SHORT;
	}
	if (TRUE==res)
	{
		return ECIES_SUCCESS;
	}
	else
	{
		return ECIES_FAIL;
	}
	
	return res;
}


int myTestEccMain()
{
	/* These octets are automatically protected against buffer overflow attacks */
	/* Note salt must be big enough to include an appended word */
	/* Note ECIES ciphertext C must be big enough to include at least 1 appended block */
	/* Recall EFS is field size in bytes. So EFS=32 for 256-bit curve */
	//c必须足够大以便容纳起码一个附加的块(对于17字节的明文起码要有2个128bit块也就是32字节长度)
	//因为内部使用的AES的缘故，加上Padding。所以从明文大小17字节按照128bit块大小向上取整密文大小到32字节)
	//ecp_domain eciesCtx;	
	char PriKey[ZW_ECIES_PRIKEY_LEN*2];
	char PubKey[ZW_ECIES_PUBKEY_LEN*2];
	char encSyncKey[ZW_ECIES_ENCSYNCKEY_LEN*2],msgHashBuf[EFS*2+ZW_EXA];
	char cryptText[ZW_ECIES_MESSAGE_MAXLEN*2];
	if (ECIES_SUCCESS!=
		zwEciesKeyPairGen("privatekeypassword",PriKey, sizeof(PriKey), PubKey, sizeof(PubKey)))
	{
		printf("ECIES KeyPair Gen Fail!\n");
		return ECIES_PUBKEY_GEN_FAIL;
	}
		
	//////////////////////////////////////////////////////////////////////////
	{   
		const char *myPlainText="PlainText 20140523.1405";
		printf("Testing ECIES\n");

		//epdom:ECC加密环境，也就是各种内部参数所在的结构体
		//P1,P2:随便指定，P1用于KDF2，P2用于HMAC
		//W1:对方公钥	M:明文	12：是T(tag)出参的长度
		//V:对称密钥的密文	C:待加密消息的密文	T:HASH值	
		zwEciesEncrypt(PubKey,
			myPlainText,encSyncKey, sizeof(encSyncKey),
			msgHashBuf, sizeof(msgHashBuf),
			cryptText, sizeof(cryptText));

		assert(strlen(encSyncKey)>0);
		printf("V(Encrypted Sync Key)=\t%s\n",encSyncKey);	
		assert(strlen(cryptText)>0);
		printf("C(Encrypted Message)=\t%s\n",cryptText);
		printf("T(HASH of message)=\t%s\n",msgHashBuf);
		{				
			char plainTextBuf[ZW_ECIES_MESSAGE_MAXLEN];

			//解密
			//各项参数和加密差不多，除了PubKey的公钥替换为PriKey的私钥
			memset(plainTextBuf,0,sizeof(plainTextBuf));

			if (!zwEciesDecrypt(PriKey,plainTextBuf, sizeof(plainTextBuf),
				encSyncKey,msgHashBuf,cryptText))
			{
				printf("*** ECIES Decryption Failed\n");
				return 0;
			}
			else printf("Decryption succeeded\n");

			printf("Message is \n%s\n",plainTextBuf); 
		}

		//ECP_DOMAIN_KILL(&eciesCtx);
	}
	return 0;
}

//ZWECIES_API const string & zwtestString(const string &inStr,const string &outStr)
//{
//	return "";
//}

struct zwEcies_t{
	int status;
	string pubKey;
	string priKey;
	string encoutSyncKey;
	string encoutHash;
	string encoutCryptText;
};
ZWECIES_API int EciesGenKeyPair(void)
{
#define ZWPRILEN	(ZW_ECIES_PRIKEY_LEN*4/3)
#define ZWPUBLEN	(ZW_ECIES_PUBKEY_LEN*4/3)
	zwEcies_t *stu=new zwEcies_t;
	stu->status=ECIES_INIT_FLAG;	//标志该结构体已经被初始化
	char PriKey[ZWPRILEN];
	char PubKey[ZWPUBLEN];
	memset(PriKey,0,ZWPRILEN);
	memset(PubKey,0,ZWPUBLEN);
	zwEciesKeyPairGen("randpassword",PriKey,ZWPRILEN,PubKey,ZWPUBLEN);
	stu->pubKey=PubKey;
	stu->priKey=PriKey;
	return (int)stu;
}

ZWECIES_API void EciesDelete(int eciesHandle)
{
	zwEcies_t *stu=(zwEcies_t *)eciesHandle;
	if (NULL!=stu && ECIES_INIT_FLAG==stu->status)
	{
		delete stu;
	}
}

ZWECIES_API string EciesGetPubKey(int eciesHandle)
{
	zwEcies_t *stu=(zwEcies_t *)eciesHandle;
	if (NULL==stu || ECIES_INIT_FLAG!=stu->status)
	{
		return "BADHANDLE20140604";
	}
	return stu->pubKey;
}

ZWECIES_API string EciesGetPriKey(int eciesHandle)
{
	zwEcies_t *stu=(zwEcies_t *)eciesHandle;
	if (NULL==stu || ECIES_INIT_FLAG!=stu->status)
	{
		return "BADHANDLE20140604";
	}
	return stu->priKey;
}

//自定义实现split函数
void zwsplit(const std::string& s, std::string& delim,std::vector< std::string > *ret)
{
	size_t last = 0;
	size_t index=s.find_first_of(delim,last);
	while (index!=std::string::npos)
	{
		string tt = s.substr(last,index-last);		
		ret->push_back(tt);
		last=index+1;
		index=s.find_first_of(delim,last);
	}
	if (index-last>0)
	{
		ret->push_back(s.substr(last,index-last));
	}
}



//把3项加密输出合并为一项字符串返回
static string myMergeEncOutItems(int eciesHandle)
{
	zwEcies_t *stu=(zwEcies_t *)eciesHandle;
	if (NULL==stu || ECIES_INIT_FLAG!=stu->status)
	{
		return "BADHANDLE20140604";
	}
	if (stu->encoutSyncKey.length()==0 ||
		stu->encoutHash.length()==0 ||
		stu->encoutCryptText.length()==0)
	{
		return "NULL ENCRYPT OUT 20140604";
	}
	return	stu->encoutSyncKey+"."+
			stu->encoutHash+"."+
			stu->encoutCryptText;
}


//要求eciesHandle已经被设置了公钥才能成功，返回值是3个元素的组合，不必理解其意义
ZWECIES_API string EciesEncrypt( const string &pubKey,const string &plainText )
{
#define SKELEN	(ZW_ECIES_ENCSYNCKEY_LEN*2)
#define HASHLEN	(EFS*2+ZW_EXA)
#define CRLEN	(ZW_ECIES_MESSAGE_MAXLEN*2)
	char encSyncKey[SKELEN],msgHashBuf[HASHLEN];
	char cryptText[CRLEN];
	memset(encSyncKey,0,SKELEN);
	memset(msgHashBuf,0,HASHLEN);
	memset(cryptText,0,CRLEN);
	int res=zwEciesEncrypt(pubKey.c_str(),plainText.c_str(),
		encSyncKey,SKELEN,msgHashBuf,HASHLEN,cryptText,CRLEN);
	string dot=".";
	return encSyncKey+dot+msgHashBuf+dot+cryptText;
}

//要求eciesHandle已经被设置了私钥才能成功，输入密文是3个元素的组合，不必理解其意义
ZWECIES_API string EciesDecrypt(const string &priKey,const string &cryptText)
{
	if (priKey.length()==0)
	{
		return "";
	}

	//首先把组合的3个项目切分开来
	vector<string> encout;
	string delm=".";
	zwsplit(cryptText,delm,&encout);
	char plainTextBuf[ZW_ECIES_MESSAGE_MAXLEN];
	memset(plainTextBuf,0,ZW_ECIES_MESSAGE_MAXLEN);

	zwEciesDecrypt(priKey.c_str(),plainTextBuf,ZW_ECIES_MESSAGE_MAXLEN,
		encout[0].c_str(),encout[1].c_str(),encout[2].c_str());
	return plainTextBuf;
}

