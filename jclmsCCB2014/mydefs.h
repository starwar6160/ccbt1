#ifndef mydefs_h__
#define mydefs_h__
#include "stdafx.h"
//实际上不限于AES,只是作为一个基本的块规整大小单位方便处理
#define ZW_AES_BLOCK_SIZE	(128/8)	
class zwHexTool
{
	char *m_bin;
	int m_binLen;
	int m_padLen;
	string m_CArrayStr;
public:
	zwHexTool(const char *HexInput);
	zwHexTool(const void *msg,const int msgLen);
	~zwHexTool();
	//出参给出内部bin数据区地址,以及长度
	char * getBin(void);
	int getBinLen(void);
	int getPadedLen(void);
	int getXXTEABlockNum(void);
	void PrintBin(void);
	const char * getCArrayStr(void);
protected:

private:
};


//XXTEA算法代码
void btea(uint32_t *v, int n, uint32_t const key[4]) ;
#endif // mydefs_h__
