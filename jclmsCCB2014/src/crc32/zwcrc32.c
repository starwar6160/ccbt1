///////////////////////////////CRC32 START///////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
unsigned int crc32(char *input,const int len);

#define CRCPOLYNOMIAL 0xEDB88320 

typedef unsigned int UINT;
typedef unsigned long DWORD;

UINT *create_table(UINT *t)
{
	UINT crctable[256];
	UINT crcreg;
	int i, j;
	for (i = 0; i < 256; ++i)
	{
		crcreg = i;
		for (j = 0; j < 8; ++j)
		{
			if ((crcreg & 1) != 0)
			{
				crcreg = CRCPOLYNOMIAL ^ (crcreg >> 1);
			}

			else
			{
				crcreg >>= 1;
			}
		}
		crctable[i] = crcreg;
	}

	t = crctable;
	return t;
}

unsigned int crc32(char *input,const int len)
{
	int i;
	UINT crcstart= 0xFFFFFFFF;
	UINT temp[256];
	UINT *table = (UINT*)create_table(temp);
	for (i = 0; i < len; ++i)
	{
		UINT index = (crcstart ^ input[i]) & 0xFF;
		crcstart = (crcstart >> 8) ^ table[index];
	}
	return crcstart;
}

int crc32testmain1127()
{
	char data[] = "Enter something here";
	UINT a = crc32(data,strlen(data));

	DWORD chksum = a;
	printf("%s %lu\n", __FUNCTION__,chksum);

	return 0;
}
///////////////////////////////CRC32 END///////////////////////////////////////////
