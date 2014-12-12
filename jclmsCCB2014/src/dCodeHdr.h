#ifndef dCodeHdr_h__
#define dCodeHdr_h__
#include "sm3.h"
#include "cJSON.h"
//#include "jclmsCCB2014.h"

#ifdef  __cplusplus
extern "C" {
#endif



//获得规格化的时间，也就是按照某个值取整的时间
int myGetNormalTime(int gmtTime, const int TIMEMOD);
//获取初始闭锁码的3个可变条件的“固定值”
void myGetInitCloseCodeVarItem(int *mdatetime, int *mvalidity, int *mclosecode);
//void zwJclmsReq2Json(const JCINPUT *p,char *outJson,const int outBufLen);
cJSON * zwJcInputConv2Json( cJSON ** root, const JCINPUT * p );
void zwJclmsGenReq2Json(const JCINPUT *p,char *outJson,const int outBufLen);
void zwJclmsVerReq2Json(const JCINPUT *p,const int dstCode,char *outJson,const int outBufLen);
void zwJclmsReqDecode(const char *inJclmsReqJson,JCLMSREQ *outReq);
void zwJclmsRersult2Json(const JCRESULT *p,const JCLMSOP op,char *outJson,const int outBufLen);
void zwJclmsResultFromJson(const char *inJson,JCRESULT *p);
//ARM编译去掉assert，避免链接找不到符号
#ifndef _WIN32
#define assert
#endif // _WIN32

void myJcInputHton(JCINPUT *p);
void myJcInputNtoh(JCINPUT *p);
void myLmsReqZHton(JCLMSREQ *req);
void myLmsReqZNtoh(JCLMSREQ *req);
//只能用于计算单段CRC8
unsigned char crc8Short( const void *inputData,const int inputLen );
//可以用于多段CRC8计算，第一次使用时,crc8参数输入必须为0
unsigned char crc8(const unsigned char crc8Input,const void *inputData, const int inputLen );

//生成各种类型的动态码
int zwJcLockGetDynaCode(const int handle);

#ifdef  __cplusplus
}	//extern "C" {
#endif

#endif // dCodeHdr_h__
