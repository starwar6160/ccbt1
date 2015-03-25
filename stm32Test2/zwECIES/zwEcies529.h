#ifndef zwEcies529_h__
#define zwEcies529_h__
//#include <string>
//using std::string;

#ifndef ECDH_H
//#define EFS 32 /* ECCSI Field Size - 256 bits */
typedef int BOOL;
#endif 
const int ZWEFS=256/8;	
const int ZW_EXA=4;
//EFS=32字节=256bit
//经验值，程序实际输出90字节长度的BASE64字符串
//额外多留的保险字节数目
//#define ZW_EXA	(4)
//以下直接计算base64输出的字符串长度，所以*4/3
//////////////////////////////////////////////////////////////////////////
#define ZW_ECIES_PUBKEY_LEN	((ZWEFS*2+ZW_EXA)*4/3)	//公钥
#define ZW_ECIES_PRIKEY_LEN ((ZWEFS*1+ZW_EXA)*4/3)	//私钥
//////////////////////////////////////////////////////////////////////////
#define ZW_ECIES_ENCSYNCKEY_LEN ((ZWEFS*2+ZW_EXA)*4/3)	//加密结果PART1,加密过的对称密钥
#define ZW_ECIES_HASH_LEN		((ZWEFS*1+ZW_EXA)*4/3)	//加密结果PART2,HASH值
//ECIES的作用就是用来加密对称密钥，已知最大的BLOWFISH是448bit，
//SHA512的输出作为密钥的话是512bit，考虑可能加入其他一些开销，
//所以最多768bit(EFS*3)应该足够了
#define ZW_ECIES_MESSAGE_MAXLEN	((ZWEFS*2+6)*4/3)			//加密结果PART3，对称加密过的密文
//加密结果3部分长度之和
#define ZW_ECIES_CRYPT_TOTALLEN	(ZW_ECIES_ENCSYNCKEY_LEN+ZW_ECIES_HASH_LEN+ZW_ECIES_MESSAGE_MAXLEN)
//////////////////////////////////////////////////////////////////////////


#define ZWECIES_API

#ifdef  __cplusplus
extern "C" {
#endif

	enum ECIES_ERROR{
		ECIES_SUCCESS,
		ECIES_FAIL,
		ECIES_OUTPUT_BUFFER_TOO_SHORT,
		ECIES_PRIKEY_TOO_SHORT,
		ECIES_PUBKEY_TOO_SHORT,
		ECIES_HASH_TOO_SHORT,
		ECIES_CRYPT_TOO_SHORT,
		ECIES_ENCEDSYNCKEY_TOO_SHORT,
		ECIES_PLAIN_TOO_SHORT,
		ECIES_PUBKEY_GEN_FAIL,
		ECIES_INPUT_TOO_SHORT,
		ECIES_PUBKEY_INVALID,
		ECIES_PRIKEY_INVALID,
		ECIES_INPUT_NULL,
		ECIES_INIT_FLAG
	};

	enum ECIES_ITEMS{
		ECIES_PUBKEY,
		ECIES_PRIKEY,
		ECIES_ENCSYNCKEY,
		ECIES_ENCHASH,
		ECIES_ENCTEXT,
		ECIES_ENCALLOUT
	};

#ifdef _DEBUG_123
//以下所有涉及到结尾叫Str的参数，都是Base64或者HEX编码之一，默认一般是Base64以便减少参数长度
//其余char的参数是普通字符串；
//初始化ECIES，并生成私钥和公钥
//password:字符串形式的密码，是密钥对的随机种子，但解密时并不需要
//outPriKeyStr和priLen是私钥输出缓冲区指针以及缓冲区长度
//outPublicKeyStr和pubLen是公钥输出缓冲区指针以及缓冲区长度
//私钥和公钥两个输出缓冲区长度不够长的话会出错返回，返回值见ECIES_ERROR这个枚举
	ZWECIES_API int zwEciesKeyPairGen( const char *password,char *outPriKeyStr,const int priLen,
		char *outPublicKeyStr ,const int pubLen);

//ECIES加密：
//pubkeyStr：编码过的公钥	PlainText：明文
//接下来三组参数，是分为3个项目的加密结果以及长度
//三个输出缓冲区长度不够长的话会出错返回，返回值见ECIES_ERROR这个枚举
//outEncryptedSyncKeyStr：加密过的，编码过的对称会话密钥
//outMsgHashStr：编码过的HASH结果
//outCryptedTextStr：编码过的使用outEncryptedSyncKeyStr的对称加密结果
	ZWECIES_API int zwEciesEncrypt(const char *pubkeyStr,const char *PlainText, 
		char *outEncryptedSyncKeyStr,const int syncKeyLen, 
		char *outMsgHashStr,const int hashLen,
		char *outCryptedTextStr,const int cryptLen);
//ECIES解密：
//prikeyStr：编码过的私钥	outPlainText：输出明文缓冲区	plainLen：明文缓冲区长度
//EncryptedSyncKeyStr，MsgHashStr，CryptedTextStr：含义同加密输出的3个项目
	ZWECIES_API int zwEciesDecrypt(const char *prikeyStr,char *outPlainText,const int plainLen, const char *EncryptedSyncKeyStr,const char *MsgHashStr,const char *CryptedTextStr);
//int myTestEccMain();
#endif // _DEBUG_123
//////////////////////////////以下是适合包装给C#使用的接口////////////////////////////////////////////
//生成ECIES公钥/私钥对，返回保存密钥对等等的内部数据结构句柄
ZWECIES_API int		EciesGenKeyPair(void);
//删除保存密钥对等等的内部数据结构
ZWECIES_API void	EciesDelete(int eciesHandle);
//从句柄所指向的内部数据结构获取前面生成好的公钥，是Base64格式字符串，不必理解其含义，原样透传即可
ZWECIES_API const char * EciesGetPubKey(int eciesHandle);
//从句柄所指向的内部数据结构获取前面生成好的私钥，是Base64格式字符串，不必理解其含义，原样透传即可
ZWECIES_API const char * EciesGetPriKey(int eciesHandle);
//
//输入明文以及base64格式的公钥，返回值是base64编码的3个元素的组合，不必理解其意义，原样透传即可
//明文最长限制为ZW_ECIES_MESSAGE_MAXLEN-1个字符，必须为空结束字符串，-1是因为NULL字符占据位置
ZWECIES_API const char * EciesEncrypt(const char *pubKey,const char *plainText);
//输入加密函数输出的base64编码的3个元素的组合的密文，以及base64格式的私钥，返回明文
ZWECIES_API const char * EciesDecrypt(const char *priKey,const char *cryptText);
//要求eciesHandle已经被设置了公钥才能成功，返回值是3个元素的组合，不必理解其意义
//20150325.建行版本，明文增加了时间戳，是UTC秒数的字符串形式
ZWECIES_API const char *EciesEncryptCCB1503(const char *pubKey, const char *plainText,time_t nowTime);

ZWECIES_API const char * zwMergePsk(const char *pskInput);
#ifdef  __cplusplus
}
#endif




#endif // zwEcies529_h__
