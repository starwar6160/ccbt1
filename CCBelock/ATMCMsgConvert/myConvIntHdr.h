#include "stdafx.h"

//把ATMC DLL的XML和JSON互转函数集中于此，便于单元测试；
namespace jcAtmcConvertDLL{

//下发方向处理
void zwconvLockActiveDown(const ptree &ptccb, ptree &ptjc );
void zwconvLockInitDown( const ptree &ptccb, ptree &ptjc );
void zwconvReadCloseCodeDown( const ptree &ptccb, ptree &ptjc );
void zwconvQueryLockStatusDown( const ptree &ptccb, ptree &ptjc );
void zwconvTimeSyncDown( const ptree &ptccb, ptree &ptjc );
void zwconvGetLockLogDown( const ptree &ptccb, ptree &ptjc );
void zwconvLockPushWarnDown(const ptree &ptccb, ptree &ptjc );
void zwconvLockReqTimeSyncDown(const ptree &ptccb, ptree &ptjc );

//上传方向处理
void zwconvLockActiveUp(const ptree &ptjc, ptree &ptccb );
void zwconvLockInitUp( const ptree &ptjc, ptree &ptccb );
void zwconvReadCloseCodeUp( const ptree &ptjc, ptree &ptccb );
void zwconvTimeSyncUp( const ptree &ptjc, ptree &ptccb );
void zwconvCheckLockStatusUp( const ptree &ptjc, ptree &ptccb );
void zwconvGetLockLogUp( const ptree &ptjc, ptree &ptccb );
//接收锁具主动发送的初始闭锁码，只有上传方向
void zwconvRecvInitCloseCodeDown(const ptree &ptccb, ptree &ptjc );
void zwconvRecvInitCloseCodeUp(const ptree &ptjc, ptree &ptccb);
//接收锁具主动发送的验证码，只有上传方向
void zwconvRecvVerifyCodeDown(const ptree &ptccb, ptree &ptjc );
void zwconvRecvVerifyCodeUp(const ptree &ptjc, ptree &ptccb);
void zwconvLockPushWarnUp(const ptree &ptjc, ptree &ptccb);
void zwconvLockReqTimeSyncUp(const ptree &ptjc, ptree &ptccb);
//以下4个字段，为的是在上下转换期间保存建行报文中冗余的，我们基本不用但又必须返回给建行的字段
extern string ns_ActReqName;
extern string ns_LockInitName;
extern string ns_ReadCloseCodeName;
extern string ns_ccbAtmno;		//ATM编号

}	//namespace jcAtmcConvertDLL{