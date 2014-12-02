#ifndef dCodeHdr_h__
#define dCodeHdr_h__
#include "sm3.h"
//#include "jclmsCCB2014.h"

#ifdef  __cplusplus
extern "C" {
#endif



//获得规格化的时间，也就是按照某个值取整的时间
int myGetNormalTime(int gmtTime, const int TIMEMOD);
//获取初始闭锁码的3个可变条件的“固定值”
void myGetInitCloseCodeVarItem(int *mdatetime, int *mvalidity, int *mclosecode);

#ifdef  __cplusplus
}	//extern "C" {
#endif

#endif // dCodeHdr_h__
