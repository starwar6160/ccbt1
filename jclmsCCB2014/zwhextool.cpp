#include "stdafx.h"
using namespace std;
#include "jclmsCCB2014.h"

namespace zwTools{
/////////////////////////////////十六进制操作/////////////////////////////////////////
zwHexTool::zwHexTool(const char *HexInput)
{
	assert(HexInput!=NULL);
	int len=strlen(HexInput);
	assert(len % 2 ==0);
	assert(len>0);
	if (HexInput==NULL || (len %2 !=0) || (len==0))
	{
		return;
	}

	//计算分配整块内存所需的大小，为加密运算做准备
	int blockNum=len /ZW_AES_BLOCK_SIZE;
	int blockTail=len % ZW_AES_BLOCK_SIZE;
	if (blockTail>0)
	{
		blockNum=blockNum+1;
	}
	//分配足够的Pad空间以备加密运算使用，但是只使用实际长度所需要的空间
	m_padLen=blockNum*ZW_AES_BLOCK_SIZE;
	m_binLen=len/2;
	m_bin=new char [m_padLen];
	assert(m_bin!=NULL);
	memset(m_bin,0,m_padLen);
	//////////////////////////////////////////////////////////////////////////
	for (int i=0;i<m_binLen;i++)
	{
		unsigned int ch=0;	//注意此处不能用char,否则sscanf影响4个字节导致堆栈损坏
		char buf[3];
		memcpy(buf,HexInput+i*2,2);
		buf[2]=NULL;
		sscanf(buf,"%02X",&ch);
		assert(ch<0x100);
		m_bin[i]=ch;
	}
}

zwHexTool::zwHexTool(const void *msg,const int msgLen)
{
	assert(msg!=NULL);
	assert(msgLen>0);
	if (msg==NULL || msgLen<=0)
	{
		return;
	}
	int len=msgLen;

	//计算分配整块内存所需的大小，为加密运算做准备
	int blockNum=len /ZW_AES_BLOCK_SIZE;
	int blockTail=len % ZW_AES_BLOCK_SIZE;
	if (blockTail>0)
	{
		blockNum=blockNum+1;
	}
	//分配足够的Pad空间以备加密运算使用，但是只使用实际长度所需要的空间
	m_padLen=blockNum*ZW_AES_BLOCK_SIZE;
	m_binLen=len/1;	//HEX时除以2，现在就等长了
	m_bin=new char [m_padLen];
	assert(m_bin!=NULL);
	memset(m_bin,0,m_padLen);
	//////////////////////////////////////////////////////////////////////////
	memcpy(m_bin,msg,msgLen);
}


char * zwHexTool::getBin( void )
{
	return m_bin;	
}

int zwHexTool::getBinLen( void )
{
	assert(m_binLen>0);
	return m_binLen;	
}

int zwHexTool::getPadedLen( void )
{
	assert(m_padLen>0);
	return m_padLen;	
}

int zwHexTool::getXXTEABlockNum( void )
{
	assert(m_padLen>0);
	return m_padLen/sizeof(uint32_t);	
}

void zwHexTool::PrintBin()
{
	assert(m_binLen>0);

	for (int i=0;i<m_binLen;i++)
	{
		unsigned char ch=m_bin[i];
		printf("%02X", ch);
	}
	printf("\n");
}

const char * zwHexTool::getCArrayStr( void )
{
	assert(m_binLen>0);
	m_CArrayStr="unsigned char myArray[]={";
	for (int i=0;i<m_binLen;i++)
	{
		unsigned char ch=m_bin[i];
		char buf[5];
		memset(buf,0,5);
		sprintf(buf,"0x%02X",ch);
		//result.append(bstr);
		m_CArrayStr.append(buf);
		//用于去掉最后一个元素后面多余的逗号
		if (i<(m_binLen-1))
		{
			m_CArrayStr+=",";
		}
	}
	//result.append("};");
	m_CArrayStr+="};";
	return m_CArrayStr.c_str();
}

zwHexTool::~zwHexTool()
{
	delete [] m_bin;
}



///////////////////////////////XXTEA start///////////////////////////////////////////
#include <stdint.h>
#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[(p&3)^e] ^ z)))

void btea(uint32_t *v, int n, uint32_t const key[4]) {
	uint32_t y, z, sum;
	unsigned p, rounds, e;
	if (n > 1) {          /* Coding Part */
		rounds = 6 + 52/n;
		sum = 0;
		z = v[n-1];
		do {
			sum += DELTA;
			e = (sum >> 2) & 3;
			for (p=0; p<n-1; p++) {
				y = v[p+1]; 
				z = v[p] += MX;
			}
			y = v[0];
			z = v[n-1] += MX;
		} while (--rounds);
	} else if (n < -1) {  /* Decoding Part */
		n = -n;
		rounds = 6 + 52/n;
		sum = rounds*DELTA;
		y = v[0];
		do {
			e = (sum >> 2) & 3;
			for (p=n-1; p>0; p--) {
				z = v[p-1];
				y = v[p] -= MX;
			}
			z = v[n-1];
			y = v[0] -= MX;
		} while ((sum -= DELTA) != 0);
	}
}
//////////////////////////////////XXTEA end ////////////////////////////////////////

}	//namespace zwTools{