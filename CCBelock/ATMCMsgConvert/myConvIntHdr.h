#include "stdafx.h"


#ifdef CCBELOCK_EXPORTS
#define CCBELOCK_API __declspec(dllexport) 
#else
#define CCBELOCK_API __declspec(dllimport)
#endif

//把ATMC DLL的XML和JSON互转函数集中于此，便于单元测试；
namespace jcAtmcConvertDLL {


//下发方向处理
	void zwconvLockActiveDown(const ptree & ptccb, ptree & ptjc);
	void zwconvLockInitDown(const ptree & ptccb, ptree & ptjc);
	void zwconvReadCloseCodeDown(const ptree & ptccb, ptree & ptjc);
	void zwconvQueryLockStatusDown(const ptree & ptccb, ptree & ptjc);
	void zwconvTimeSyncDown(const ptree & ptccb, ptree & ptjc);
	void zwconvGetLockLogDown(const ptree & ptccb, ptree & ptjc);
	void zwconvLockPushWarnDown(const ptree & ptccb, ptree & ptjc);
	void zwconvLockReqTimeSyncDown(const ptree & ptccb, ptree & ptjc);

//上传方向处理
	void zwconvLockActiveUp(const ptree & ptjc, ptree & ptccb);
	void zwconvLockInitUp(const ptree & ptjc, ptree & ptccb);
	void zwconvReadCloseCodeUp(const ptree & ptjc, ptree & ptccb);
	void zwconvTimeSyncUp(const ptree & ptjc, ptree & ptccb);
	void zwconvCheckLockStatusUp(const ptree & ptjc, ptree & ptccb);
	void zwconvGetLockLogUp(const ptree & ptjc, ptree & ptccb);
//接收锁具主动发送的初始闭锁码，只有上传方向
	void zwconvRecvInitCloseCodeDown(const ptree & ptccb, ptree & ptjc);
	void zwconvRecvInitCloseCodeUp(const ptree & ptjc, ptree & ptccb);
//接收锁具主动发送的验证码，只有上传方向
	void zwconvRecvVerifyCodeDown(const ptree & ptccb, ptree & ptjc);
	void zwconvRecvVerifyCodeUp(const ptree & ptjc, ptree & ptccb);
	void zwconvLockPushWarnUp(const ptree & ptjc, ptree & ptccb);
	void zwconvLockReqTimeSyncUp(const ptree & ptjc, ptree & ptccb);
//20141111.万敏昨天要求增加的我们内部自定义报文，温度，振动传感器
	void zwconvTemptureSenseDown(const ptree & ptccb, ptree & ptjc);
	void zwconvTemptureSenseUp(const ptree & ptjc, ptree & ptccb);
	void zwconvShockSenseDown(const ptree & ptccb, ptree & ptjc);
	void zwconvShockSenseUp(const ptree & ptjc, ptree & ptccb);
//20151125.万敏新增的3个命令报文
	//ATM机设置上传的小循环次数命令
	void zwconvTemptureSetInsideLoopTimesDown(const ptree & ptccb, ptree & ptjc);
	void zwconvTemptureSetInsideLoopTimesUp(const ptree & ptjc, ptree & ptccb);
	//ATM机设置上传的小循环周期(单位秒)命令
	void zwconvTemptureSetInsideLoopPeriodDown(const ptree & ptccb, ptree & ptjc);
	void zwconvTemptureSetInsideLoopPeriodUp(const ptree & ptjc, ptree & ptccb) ;
	//ATM机设置上传的大循环周期(单位分钟)命令
	void zwconvTemptureSetOutsideLoopPeriodDown(const ptree & ptccb, ptree & ptjc) ;
	void zwconvTemptureSetOutsideLoopPeriodUp(const ptree & ptjc, ptree & ptccb) ;


//以下4个字段，为的是在上下转换期间保存建行报文中冗余的，我们基本不用但又必须返回给建行的字段
	extern string ns_ActReqName;
	extern string ns_LockInitName;
	extern string ns_ReadCloseCodeName;
	//extern string ns_ccbAtmno;    //ATM编号

	//获取XML报文类型
	CCBELOCK_API string zwGetJcxmlMsgType(const char *jcXML);
	//获取JSON报文类型
	CCBELOCK_API string zwGetJcJsonMsgType(const char *jcJson);

	// zwtrim from start
	std::string &zwltrim(std::string &s);
	// zwtrim from end
	std::string &zwrtrim(std::string &s);
	// zwtrim from both ends
	std::string &zwtrim(std::string &s);

	//保存一条下发报文及其返回报文，还有线程ID等相关信息；
	struct jcLockMsg1512_t 
	{
		DWORD CallerThreadID;	//主程序线程ID
		string NotifyMsg;		//下发的报文
		string NotifyType;		//下发报文类型
		string UpMsg;			//返回报文	
		bool bSended;			//已经发送标志
		RecvMsgRotine pRecvMsgFun;	//回调函数指针
	};

}				//namespace jcAtmcConvertDLL{
