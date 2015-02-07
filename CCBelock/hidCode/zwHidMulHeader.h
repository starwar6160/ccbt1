#ifndef zwHidMulHeader_h__
#define zwHidMulHeader_h__

#ifdef __cplusplus
extern "C" {
#endif

	////////////////////////调试用Json命令接口//////////////////////////////////////////////////
	//一、设备列表接口 1、回调函数类型
	typedef void (ZJY1501STD *ReturnDrives)(const char* DrivesTypePID,const char * DrivesIDListJson);

	CCBELOCK_API void ZJY1501STD SetReturnDrives(ReturnDrives _DrivesListFun);
	//3、获取设备列表的指令
	//说明：
	//	（1）DrivesType表示设备类型，具体内容是：锁具=Lock，密盒=Encryption
	//	（2）所有int类型函数的返回值中0表示成功，非零表示错误（下同）
	//	（3）DrivesIDList为json格式字符，其中列出了所有符合要求设备的DrivesID（HID设备序列号）
	//	（4）当调用ListDrives时，接口函数应返回当前所有此类设备的列表
	//	（5）当有新设备插入或拔出时，接口函数应该返回插入设备所属类型的最新列表（重复内容由上位机过滤去重）
	CCBELOCK_API int ZJY1501STD ListDrives(const char * DrivesTypePID);
	//1、打开设备
	CCBELOCK_API int ZJY1501STD OpenDrives(const char* DrivesTypePID,const char * DrivesIdSN);
	//	2、关闭设备
	CCBELOCK_API int ZJY1501STD CloseDrives(const char* DrivesTypePID,const char * DrivesIdSN);
	
	//extern ReturnDrives G_JCHID_ENUM_DEV2015A;

	//三、设备消息接口 说明：（1）DrivesMessage和AnyMessage均为json格式字符
	//1、回调函数类型
	typedef void (ZJY1501STD *ReturnMessage)(const char* DrivesIdSN,char* DrivesMessageJson);
	//2、设置设备消息返回的回调函数
	CCBELOCK_API void ZJY1501STD SetReturnMessage(ReturnMessage _MessageHandleFun);
	//3、向设备发送指令的函数
	CCBELOCK_API int ZJY1501STD InputMessage(const char * DrivesTypePID,const char * DrivesIdSN,const char * AnyMessageJson);
	CCBELOCK_API void myHidSerialTest126(void);
#ifdef __cplusplus
}
#endif

/////////////////////////////Google Test In DLL/////////////////////////////////////////////
CCBELOCK_API int zwStartGtestInDLL(void);

namespace jcLockJsonCmd_t2015a21{
	CCBELOCK_API long OpenJson(long lTimeOut);
	CCBELOCK_API long SendToLockJson(const char *pszJson);
	CCBELOCK_API const char * RecvFromLockJson(const int timeoutMs);
	CCBELOCK_API long CloseJson();	
	CCBELOCK_API void jcMulHidEnum( const int hidPid ,string &jcDevListJson);
	extern ReturnDrives G_JCHID_ENUM_DEV2015A;
	extern ReturnMessage G_JCHID_RECVMSG_CB;
	extern const char * G_DEV_LOCK;
	extern const char * G_DEV_SECBOX;
	extern const int G_SUSSESS;
	extern const int G_FAIL;
	class zwJcHidDbg15A;
	uint32_t myHidSerialToInt(char *hidSerial);
	void zwCleanJchidVec(void);
}	//end of namespace jcLockJsonCmd_t2015a21{
namespace jch=jcLockJsonCmd_t2015a21;

namespace jcLockJsonCmd_t2015a27{
	CCBELOCK_API long OpenJson(long lTimeOut);
	CCBELOCK_API long JCAPISTD CloseJson();
	CCBELOCK_API long SendToLockJson(const char *pszJson);
	CCBELOCK_API const char * RecvFromLockJson( const int timeoutMs );
}	//namespace jcLockJsonCmd_t2015a27{
namespace jlua=jcLockJsonCmd_t2015a27;


#endif // zwHidMulHeader_h__
