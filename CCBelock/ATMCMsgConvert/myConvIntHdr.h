#include "stdafx.h"

//��ATMC DLL��XML��JSON��ת���������ڴˣ����ڵ�Ԫ���ԣ�
namespace jcAtmcConvertDLL{

//�·�������
void zwconvLockActiveDown(const ptree &ptccb, ptree &ptjc );
void zwconvLockInitDown( const ptree &ptccb, ptree &ptjc );
void zwconvReadCloseCodeDown( const ptree &ptccb, ptree &ptjc );
void zwconvQueryLockStatusDown( const ptree &ptccb, ptree &ptjc );
void zwconvTimeSyncDown( const ptree &ptccb, ptree &ptjc );
void zwconvGetLockLogDown( const ptree &ptccb, ptree &ptjc );
void zwconvLockPushWarnDown(const ptree &ptccb, ptree &ptjc );
void zwconvLockReqTimeSyncDown(const ptree &ptccb, ptree &ptjc );

//�ϴ�������
void zwconvLockActiveUp(const ptree &ptjc, ptree &ptccb );
void zwconvLockInitUp( const ptree &ptjc, ptree &ptccb );
void zwconvReadCloseCodeUp( const ptree &ptjc, ptree &ptccb );
void zwconvTimeSyncUp( const ptree &ptjc, ptree &ptccb );
void zwconvCheckLockStatusUp( const ptree &ptjc, ptree &ptccb );
void zwconvGetLockLogUp( const ptree &ptjc, ptree &ptccb );
//���������������͵ĳ�ʼ�����룬ֻ���ϴ�����
void zwconvRecvInitCloseCodeDown(const ptree &ptccb, ptree &ptjc );
void zwconvRecvInitCloseCodeUp(const ptree &ptjc, ptree &ptccb);
//���������������͵���֤�룬ֻ���ϴ�����
void zwconvRecvVerifyCodeDown(const ptree &ptccb, ptree &ptjc );
void zwconvRecvVerifyCodeUp(const ptree &ptjc, ptree &ptccb);
void zwconvLockPushWarnUp(const ptree &ptjc, ptree &ptccb);
void zwconvLockReqTimeSyncUp(const ptree &ptjc, ptree &ptccb);
//����4���ֶΣ�Ϊ����������ת���ڼ䱣�潨�б���������ģ����ǻ������õ��ֱ��뷵�ظ����е��ֶ�
extern string ns_ActReqName;
extern string ns_LockInitName;
extern string ns_ReadCloseCodeName;
extern string ns_ccbAtmno;		//ATM���

}	//namespace jcAtmcConvertDLL{