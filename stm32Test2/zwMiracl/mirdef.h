/* 
 *   MIRACL compiler/hardware definitions - mirdef.h
 *   This version suitable for use with most 32-bit computers
 *   e.g. 80386+ PC, VAX, ARM etc. Assembly language versions of muldiv,
 *   muldvm, muldvd and muldvd2 will be necessary. See mrmuldv.any 
 *
 *   Also suitable for DJGPP GNU C Compiler
 *   ... but change __int64 to long long
 */

#define MR_LITTLE_ENDIAN
#define MIRACL 32
#define mr_utype int
#define MR_IBITS 32
#define MR_LBITS 32
#define mr_unsign32 unsigned int
#define mr_dltype long long
#define mr_unsign64 unsigned long long
#define MR_ALWAYS_BINARY
//速度关键函数没有ARM汇编优化版本，所以这么定义
#ifndef WIN32
#define MR_NOASM
#endif	//#ifndef WIN32

//#define MR_STRIPPED_DOWN
#define MAXBASE ((mr_small)1<<(MIRACL-1))
#define MR_BITSINCHAR 8
//#define MR_NOKOBLITZ
//#define MR_NO_SS

//#define MR_SIMPLE_BASE
//#define MR_SIMPLE_IO
//#define MR_GENERIC_MT
//#define MR_STATIC 6

