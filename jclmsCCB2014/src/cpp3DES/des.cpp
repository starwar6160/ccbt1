#include "des.h"
#include "des.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>

#ifdef WIN32
#include <assert.h>
#else
#define assert
#endif // WIN32



//#pragma GCC push_options
//#pragma GCC optimize ("unroll-loops")

#ifndef DES_KEY_H
#define DES_KEY_H

// Permuted Choice 1 Table [7*8]
static const char PC1[] =
{
	57, 49, 41, 33, 25, 17,  9,
	1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27,
	19, 11,  3, 60, 52, 44, 36,

	63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29,
	21, 13,  5, 28, 20, 12,  4
};

// Permuted Choice 2 Table [6*8]
static const char PC2[] =
{
	14, 17, 11, 24,  1,  5,
	3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8,
	16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};

// Iteration Shift Array
static const char ITERATION_SHIFT[] =
{
	//  1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16
	1,  1,  2,  2,  2,  2,  2,  2,  1,  2,  2,  2,  2,  2,  2,  1
};

#endif // DES_KEY_H


#ifndef DES_DATA_H
#define DES_DATA_H

#define LB32_MASK 0x00000001
#define LB64_MASK 0x0000000000000001
#define L64_MASK  0x00000000ffffffff

// Initial Permutation Table [8*8]
static const char IP[] =
{
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17,  9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7
};

// Inverse Initial Permutation Table [8*8]
static const char FP[] =
{
	40, 8, 48, 16, 56, 24, 64, 32,
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30,
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41,  9, 49, 17, 57, 25
};

// Expansion table [6*8]
static const char EXPANSION[] =
{
	32,  1,  2,  3,  4,  5,
	4,  5,  6,  7,  8,  9,
	8,  9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32,  1
};

// The S-Box tables [8*16*4]
static const char SBOX[8][64] =
{
	{
		// S1
		14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
			0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
			4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
			15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
	},
	{
		// S2
		15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
			3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
			0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
			13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
		},
		{
			// S3
			10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
				13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
				13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
				1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
		},
		{
			// S4
			7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
				13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
				10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
				3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
			},
			{
				// S5
				2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
					14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
					4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
					11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
			},
			{
				// S6
				12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
					10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
					9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
					4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
				},
				{
					// S7
					4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
						13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
						1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
						6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12
				},
				{
					// S8
					13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
						1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
						7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
						2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
					}
};

// Post S-Box permutation [4*8]
static const char PBOX[] =
{
	16,  7, 20, 21,
	29, 12, 28, 17,
	1, 15, 23, 26,
	5, 18, 31, 10,
	2,  8, 24, 14,
	32, 27,  3,  9,
	19, 13, 30,  6,
	22, 11,  4, 25
};

#endif // DES_DATA_H


DES::DES(ui64 key)
{
    keygen(key);
}

ui64 DES::encrypt(ui64 block)
{
    return des(block, false);
}

ui64 DES::decrypt(ui64 block)
{
    return des(block, true);
}

ui64 DES::encrypt(ui64 block, ui64 key)
{
    DES des(key);
    return des.des(block, false);
}

ui64 DES::decrypt(ui64 block, ui64 key)
{
    DES des(key);
    return des.des(block, true);
}

void DES::keygen(ui64 key)
{
    // initial key schedule calculation
    ui64 permuted_choice_1 = 0; // 56 bits
    for (ui8 i = 0; i < 56; i++)
    {
        permuted_choice_1 <<= 1;
        permuted_choice_1 |= (key >> (64-PC1[i])) & LB64_MASK;
    }

    // 28 bits
    ui32 C = (ui32) ((permuted_choice_1 >> 28) & 0x000000000fffffff);
    ui32 D = (ui32)  (permuted_choice_1 & 0x000000000fffffff);

    // Calculation of the 16 keys
    for (ui8 i = 0; i < 16; i++)
    {
        // key schedule, shifting Ci and Di
        for (ui8 j = 0; j < ITERATION_SHIFT[i]; j++)
        {
            C = (0x0fffffff & (C << 1)) | (0x00000001 & (C >> 27));
            D = (0x0fffffff & (D << 1)) | (0x00000001 & (D >> 27));
        }

        ui64 permuted_choice_2 = (((ui64) C) << 28) | (ui64) D;

        sub_key[i] = 0; // 48 bits (2*24)
        for (ui8 j = 0; j < 48; j++)
        {
            sub_key[i] <<= 1;
            sub_key[i] |= (permuted_choice_2 >> (56-PC2[j])) & LB64_MASK;
        }
    }
}

ui64 DES::des(ui64 block, bool mode)
{
    // applying initial permutation
    block = ip(block);

    // dividing T' into two 32-bit parts
    ui32 L = (ui32) (block >> 32) & L64_MASK;
    ui32 R = (ui32) (block & L64_MASK);

    // 16 rounds
    for (ui8 i = 0; i < 16; i++)
    {
        ui32 F = mode ? f(R, sub_key[15-i]) : f(R, sub_key[i]);
        feistel(L, R, F);
    }

    // swapping the two parts
    block = (((ui64) R) << 32) | (ui64) L;
    // applying final permutation
    return fp(block);
}

ui64 DES::ip(ui64 block)
{
    // initial permutation
    ui64 result = 0;
    for (ui8 i = 0; i < 64; i++)
    {
        result <<= 1;
        result |= (block >> (64-IP[i])) & LB64_MASK;
    }
    return result;
}

ui64 DES::fp(ui64 block)
{
    // inverse initial permutation
    ui64 result = 0;
    for (ui8 i = 0; i < 64; i++)
    {
        result <<= 1;
        result |= (block >> (64-FP[i])) & LB64_MASK;
    }
    return result;
}

void DES::feistel(ui32 &L, ui32 &R, ui32 F)
{
    ui32 temp = R;
    R = L ^ F;
    L = temp;
}

ui32 DES::f(ui32 R, ui64 k) // f(R,k) function
{
    // applying expansion permutation and returning 48-bit data
    ui64 s_input = 0;
    for (ui8 i = 0; i < 48; i++)
    {
        s_input <<= 1;
        s_input |= (ui64) ((R >> (32-EXPANSION[i])) & LB32_MASK);
    }

    // XORing expanded Ri with Ki, the round key
    s_input = s_input ^ k;

    // applying S-Boxes function and returning 32-bit data
    ui32 s_output = 0;
    for (ui8 i = 0; i < 8; i++)
    {
        // Outer bits
        char row = (char) ((s_input & (0x0000840000000000 >> 6*i)) >> (42-6*i));
        row = (row >> 4) | (row & 0x01);

        // Middle 4 bits of input
        char column = (char) ((s_input & (0x0000780000000000 >> 6*i)) >> (43-6*i));

        s_output <<= 4;
        s_output |= (ui32) (SBOX[i][16*row + column] & 0x0f);
    }

    // applying the round permutation
    ui32 f_result = 0;
    for (ui8 i = 0; i < 32; i++)
    {
        f_result <<= 1;
        f_result |= (s_output >> (32 - PBOX[i])) & LB32_MASK;
    }

    return f_result;
}

//#pragma GCC pop_options

/////////////////////////////////3DES/////////////////////////////////////////

DES3::DES3(ui64 k1, ui64 k2, ui64 k3) :
des1(k1),
	des2(k2),
	des3(k3)
{
}

ui64 DES3::encrypt(ui64 block)
{
	return des3.encrypt(des2.decrypt(des1.encrypt(block)));
}

ui64 DES3::decrypt(ui64 block)
{
	return des1.decrypt(des2.encrypt(des3.decrypt(block)));
}


///////////////////////////////DES-CBC///////////////////////////////////////////
DESCBC::DESCBC(ui64 key, ui64 iv) :
des(key),
	iv(iv),
	last_block(iv)
{
}

ui64 DESCBC::encrypt(ui64 block)
{
	last_block = des.encrypt(block ^ last_block);
	return last_block;
}

ui64 DESCBC::decrypt(ui64 block)
{
	ui64 result = des.decrypt(block) ^ last_block;
	last_block = block;
	return result;
}

void DESCBC::reset()
{
	last_block = iv;
}


//////////////////////////////Test for 3DES EDE2 ECB////////////////////////////////////////////

//输出缓冲区起码要17字节
void myui64sprintf(ui64 n64,char *outHex)
{
	ui32 n1=n64>>32;
	ui32 n2=n64 & 0xFFFFFFFF;
	memset(outHex,0,17);
	sprintf(outHex,"%08X%08X",n1,n2);
}

//把进来的64比特信息转换为64比特无符号整型
ui64 myChar2Ui64(const char *inStr)
{
	ui64 res=0;
	memcpy(&res,inStr,sizeof(ui64));
	return res;
}

void test4CCB3DES_ECB_EDE2()
{
	//2014/8/16 21:26:47 建行的3DES的JAVA例子代码运行结果：
	//可以作为我们使用C#来写相应代码的参考测试向量：
	//主密钥:[0123456789ABCDEF] 数据:[F856272510DC7307]
	//加密结果为:[CF8ACCB9945FE89D] 解密结果为: 16-[F856272510DC7307] 
	//建行测试主密钥的HEX形式为3031323334353637 3839414243444546
	//假设以上主密钥的HEX形式分为A,B两份，则采用扩展到ABA形式的密钥
	//作为EDE方式ECB加密的3DES，就是建行的“历史原因"采用的3DES
	//此处输出已经符合建行3DES的规范了

	DES3 des(0x3031323334353637, 0x3839414243444546, 0x3031323334353637);
	ui64 input = 0xF856272510DC7307;
	ui64 ccbExpectEncResult=0xCF8ACCB9945FE89D;
	ui64 result = des.encrypt(input);
	char hexBuf[16+1];
	printf("CCB 3DES Test 20150311\n");
	myui64sprintf(input,hexBuf);
	printf("CCB PlainText:\t\t%s\n", hexBuf);    
	myui64sprintf(result,hexBuf);
	printf("CCB Encrypt REAL Result:\t%s\n", hexBuf);
	myui64sprintf(ccbExpectEncResult,hexBuf);
	printf("CCB Encrypt EXPECT Result:\t%s\n", hexBuf);
	result = des.decrypt(result);
	myui64sprintf(result,hexBuf);
	printf("CCB Decrypt Result:\t%s (shuld same as PlainText)\n", hexBuf);   
}


//判断输入字符串是否是HEX字符串，如果是，返回HEX字符串长度，如果不是，返回0
int myHexStringLength(const char *hexStr)
{
	assert(NULL!=hexStr);
	assert(0!=strlen(hexStr));
	if (NULL==hexStr)
	{
		return 0;
	}
	int inLen=strlen(hexStr);
	if (0==inLen)
	{
		return 0;
	}
	int hLen=0;	//HEX长度
	for (int i=0;i<inLen;i++)
	{
		char t=hexStr[i];
		if(	(t>='0' && t<='9')	||
			(t>='a' && t<='f')	||
			(t>='A' && t<='F')	)
		{
			hLen++;
		}
		else
		{
			return 0;	//发现一个非HEX字符就错误返回
		}
	}

	return hLen;
}

//检测常见DES弱密钥
JC3DES_ERROR myIsDESWeakKey(const char *desKey)
{
	assert(NULL!=desKey && strlen(desKey)>=16);
	if (NULL==desKey || strlen(desKey)<16)
	{
		return JC3DES_KEY_INVALID_LENGTH;
	}
	if(	
		strcmp("0101010101010101",desKey)==0 ||
		strcmp("FEFEFEFEFEFEFEFE",desKey)==0 ||
		strcmp("E0E0E0E0F1F1F1F1",desKey)==0 ||
		strcmp("1F1F1F1F0E0E0E0E",desKey)==0 ||
		strcmp("0000000000000000",desKey)==0 ||
		strcmp("FFFFFFFFFFFFFFFF",desKey)==0 ||
		strcmp("E1E1E1E1F0F0F0F0",desKey)==0 ||
		strcmp("1E1E1E1E0F0F0F0F",desKey)==0 ||
		strcmp("011F011F010E010E",desKey)==0 ||
		strcmp("1F011F010E010E01",desKey)==0 ||
		strcmp("01E001E001F101F1",desKey)==0 ||
		strcmp("E001E001F101F101",desKey)==0 ||
		strcmp("01FE01FE01FE01FE",desKey)==0 ||
		strcmp("FE01FE01FE01FE01",desKey)==0 ||
		strcmp("1FE01FE00EF10EF1",desKey)==0 ||
		strcmp("E01FE01FF10EF10E",desKey)==0 ||
		strcmp("1FFE1FFE0EFE0EFE",desKey)==0 ||
		strcmp("FE1FFE1FFE0EFE0E",desKey)==0 ||
		strcmp("E0FEE0FEF1FEF1FE",desKey)==0 ||
		strcmp("FEE0FEE0FEF1FEF1",desKey)==0
		)
	{
		return JC3DES_KEY_WEAKKEY;
	}



	return JC3DES_OK;
}


//使用建行的通讯加密密钥ccbComm3DESKeyHex把8位动态码dyCode加密，返回在出参outEncDyCodeHex中
//其中通讯加密密钥，以及加密结果都是HEX字符串，动态码是整数
JC3DES_ERROR zwCCB3DESEncryptDyCode( const char *ccbComm3DESKeyHex,const int dyCode,char *outEncDyCodeHex )
{
	//8位动态码转换为字符串，然后字符串8字节转换为HEX，以便满足3DES的
	//64bit输入要求，估计这样就满足建行的要求可以被正确解密了；	
	//检查输入参数
	const int DESLEN=sizeof(ui64);	//一个3DES算法基本的64bit块大小的字节数
	assert(NULL!=ccbComm3DESKeyHex);
	assert(dyCode>=10000000 && dyCode<=99999999);
	int ccbKeyLen=myHexStringLength(ccbComm3DESKeyHex);

	assert(16==ccbKeyLen);	
	if (16!=ccbKeyLen)
	{
		printf("invalid ccbComm3DESKeyHex\n");
		return JC3DES_KEY_INVALID_LENGTH;
	}
	if (dyCode<10000000 && dyCode>99999999)
	{
		printf("invalid dyCode\n");
		return JC3DES_DYCODE_OUTOFRANGE;
	}
	if (NULL==outEncDyCodeHex)
	{
		printf("NULL outEncDyCodeHex\n");
		return JC3DES_OUTBUF_NULL;
	}
	/////////////////////////////////动态码转换为字符串/////////////////////////////////////////
	char dyCodeStr[DESLEN*2];
	memset(dyCodeStr,0,DESLEN*2);
	sprintf(dyCodeStr,"%d",dyCode);
	memset(outEncDyCodeHex,0,DESLEN*2+1);
#ifdef _DEBUG
	printf("ccbComm3DESKeyHex:%s\n",ccbComm3DESKeyHex);
	printf("dyCode=%d\tdyCodeStr=%s\n",dyCode,dyCodeStr);
#endif // _DEBUG
	assert(strlen(dyCodeStr)<=8);
	ui64 dyCodePlain=myChar2Ui64(dyCodeStr);
	////////////////////////////////3DES加密//////////////////////////////////////////
	char ccbKeyTmp[DESLEN*2+1];
	memset(ccbKeyTmp,0,DESLEN*2+1);
	memcpy(ccbKeyTmp,ccbComm3DESKeyHex,DESLEN*2);
	//分别把A,B,A当作3DES EDE2的3个Key
	//检查CCB原始64bit密钥，以及分解出来的2个64bit密钥是否弱密钥
	ui64 key1=myChar2Ui64(ccbKeyTmp);
	ui64 key2=myChar2Ui64(ccbKeyTmp+DESLEN);
	if (	JC3DES_OK!=myIsDESWeakKey(ccbComm3DESKeyHex)
		||	JC3DES_OK!=myIsDESWeakKey(ccbKeyTmp) 
		||	JC3DES_OK!=myIsDESWeakKey(ccbKeyTmp+DESLEN)	)
	{
		printf("ERROR:WEAK 3DES KEY!\n");
		return JC3DES_KEY_WEAKKEY;
	}
	DES3 des3(key1,key2,key1);	

	ui64 encDyCode=des3.encrypt(dyCodePlain);
	myui64sprintf(encDyCode,outEncDyCodeHex);
#ifdef _DEBUG
	printf("encKey:%016I64X\n",encDyCode);
	printf("dyCodePlain:%016I64X\n",dyCodePlain);
	printf("K1:%016I64X K2:%016I64X K3:%016I64X \n",key1,key2,key1);
	printf("3DES Encrypted dyCode is %s\n",outEncDyCodeHex);
#endif // _DEBUG
	return JC3DES_OK;
}


