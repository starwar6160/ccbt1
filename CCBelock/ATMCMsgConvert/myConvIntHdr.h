#include "stdafx.h"


#ifdef CCBELOCK_EXPORTS
#define CCBELOCK_API __declspec(dllexport) 
#else
#define CCBELOCK_API __declspec(dllimport)
#endif

//��ATMC DLL��XML��JSON��ת���������ڴˣ����ڵ�Ԫ���ԣ�
namespace jcAtmcConvertDLL {


//�·�������
	void zwconvLockActiveDown(const ptree & ptccb, ptree & ptjc);
	void zwconvLockInitDown(const ptree & ptccb, ptree & ptjc);
	void zwconvReadCloseCodeDown(const ptree & ptccb, ptree & ptjc);
	void zwconvQueryLockStatusDown(const ptree & ptccb, ptree & ptjc);
	void zwconvTimeSyncDown(const ptree & ptccb, ptree & ptjc);
	void zwconvGetLockLogDown(const ptree & ptccb, ptree & ptjc);
	void zwconvLockPushWarnDown(const ptree & ptccb, ptree & ptjc);
	void zwconvLockReqTimeSyncDown(const ptree & ptccb, ptree & ptjc);

//�ϴ�������
	void zwconvLockActiveUp(const ptree & ptjc, ptree & ptccb);
	void zwconvLockInitUp(const ptree & ptjc, ptree & ptccb);
	void zwconvReadCloseCodeUp(const ptree & ptjc, ptree & ptccb);
	void zwconvTimeSyncUp(const ptree & ptjc, ptree & ptccb);
	void zwconvCheckLockStatusUp(const ptree & ptjc, ptree & ptccb);
	void zwconvGetLockLogUp(const ptree & ptjc, ptree & ptccb);
//���������������͵ĳ�ʼ�����룬ֻ���ϴ�����
	void zwconvRecvInitCloseCodeDown(const ptree & ptccb, ptree & ptjc);
	void zwconvRecvInitCloseCodeUp(const ptree & ptjc, ptree & ptccb);
//���������������͵���֤�룬ֻ���ϴ�����
	void zwconvRecvVerifyCodeDown(const ptree & ptccb, ptree & ptjc);
	void zwconvRecvVerifyCodeUp(const ptree & ptjc, ptree & ptccb);
	void zwconvLockPushWarnUp(const ptree & ptjc, ptree & ptccb);
	void zwconvLockReqTimeSyncUp(const ptree & ptjc, ptree & ptccb);
//20141111.��������Ҫ�����ӵ������ڲ��Զ��屨�ģ��¶ȣ��񶯴�����
	void zwconvTemptureSenseDown(const ptree & ptccb, ptree & ptjc);
	void zwconvTemptureSenseUp(const ptree & ptjc, ptree & ptccb);
	void zwconvShockSenseDown(const ptree & ptccb, ptree & ptjc);
	void zwconvShockSenseUp(const ptree & ptjc, ptree & ptccb);
//20151125.����������3�������
	//ATM�������ϴ���Сѭ����������
	void zwconvTemptureSetInsideLoopTimesDown(const ptree & ptccb, ptree & ptjc);
	void zwconvTemptureSetInsideLoopTimesUp(const ptree & ptjc, ptree & ptccb);
	//ATM�������ϴ���Сѭ������(��λ��)����
	void zwconvTemptureSetInsideLoopPeriodDown(const ptree & ptccb, ptree & ptjc);
	void zwconvTemptureSetInsideLoopPeriodUp(const ptree & ptjc, ptree & ptccb) ;
	//ATM�������ϴ��Ĵ�ѭ������(��λ����)����
	void zwconvTemptureSetOutsideLoopPeriodDown(const ptree & ptccb, ptree & ptjc) ;
	void zwconvTemptureSetOutsideLoopPeriodUp(const ptree & ptjc, ptree & ptccb) ;


//����4���ֶΣ�Ϊ����������ת���ڼ䱣�潨�б���������ģ����ǻ������õ��ֱ��뷵�ظ����е��ֶ�
	extern string ns_ActReqName;
	extern string ns_LockInitName;
	extern string ns_ReadCloseCodeName;
	//extern string ns_ccbAtmno;    //ATM���

	//��ȡXML��������
	CCBELOCK_API string zwGetJcxmlMsgType(const char *jcXML);
	//��ȡJSON��������
	CCBELOCK_API string zwGetJcJsonMsgType(const char *jcJson);

	// zwtrim from start
	std::string &zwltrim(std::string &s);
	// zwtrim from end
	std::string &zwrtrim(std::string &s);
	// zwtrim from both ends
	std::string &zwtrim(std::string &s);

	//����һ���·����ļ��䷵�ر��ģ������߳�ID�������Ϣ��
	struct jcLockMsg1512_t 
	{
		DWORD CallerThreadID;	//�������߳�ID
		string NotifyMsg;		//�·��ı���
		string NotifyType;		//�·���������
		string UpMsg;			//���ر���	
		bool bSended;			//�Ѿ����ͱ�־
		RecvMsgRotine pRecvMsgFun;	//�ص�����ָ��
	};

}				//namespace jcAtmcConvertDLL{
