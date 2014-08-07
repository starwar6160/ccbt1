// CCBelock.cpp : 定义 DLL 应用程序的导出函数。
//
//20140804.1346.目前存在的问题：
//1.发送锁具激活请求：请求报文中没有"State","get"的字段了？没有无用的"Public_Key","123456"字段了？
//2.发送锁具激活信息：JC的应答报文无LOCKNO字段，需要解决；
//JC的应答报文的"激活信息"内容是否是已经解密的PSK?
//3.日期时间字段，我们应答报文里面是原样复制日期时间还是根据实际时间再生成时间字段？
//4.初始化的时候，建行下发报文中有密码服务器公钥，我们的报文里面没有，锁具内部对此进行处理了吗？
//5.新版文档里面"锁具初始化"，请求是“Atm_Serial”，应答是“Lock_Serial”，以哪个为准？


#include "stdafx.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
#include "zwwsClient.h"
using namespace std;
using boost::property_tree::ptree_error;
using boost::property_tree::ptree_bad_data;
using boost::property_tree::ptree_bad_path;

//回调函数指针类型定义
using Poco::Net::ConnectionRefusedException;

namespace zwCfg{
//#ifdef _DEBUG
	const long	JC_CCBDLL_TIMEOUT=30;	//最长超时时间为30秒,用于测试目的尽快达到限制暴露问题
	const int	JC_MSG_MAXLEN=4*1024;	//最长为128字节,用于测试目的尽快达到限制暴露问题
//#else
//	const long	JC_CCBDLL_TIMEOUT=3600;	//最长超时时间为1个小时，更长也没有意义了
//	const int	JC_MSG_MAXLEN=128*1024;	//最长为下位机RAM的大小，更大也没有意义了
//#endif // _DEBUG
	//定义一个回调函数指针
	RecvMsgRotine g_WarnCallback=NULL;
	boost:: mutex io_mutex; 	
	//zwWebSocket zwsc("localhost",1425);

}	//namespace zwCfg{




CCBELOCK_API long JCAPISTD Open(long lTimeOut)
{
	ZWFUNCTRACE
	boost:: mutex:: scoped_lock lock( zwCfg::io_mutex); 
	//必须大于0，小于JC_CCBDLL_TIMEOUT，限制在一个合理范围内
	assert(lTimeOut>0 && lTimeOut<zwCfg::JC_CCBDLL_TIMEOUT);
	if (lTimeOut<=0 || lTimeOut>=zwCfg::JC_CCBDLL_TIMEOUT)
	{
		return ELOCK_ERROR_PARAMINVALID;
	}	
	return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long JCAPISTD Close()
{
	ZWFUNCTRACE
	boost:: mutex:: scoped_lock lock( zwCfg::io_mutex); 
	//zwccbthr::zwStopLockCommThread();
	zwCfg::g_WarnCallback=NULL;
	//zwCfg::zwsc.wsClose();	
	return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long JCAPISTD Notify(const char *pszMsg)
{
	ZWFUNCTRACE
	assert(pszMsg!=NULL && strlen(pszMsg)>42);	//XML至少42字节
	boost:: mutex:: scoped_lock lock( zwCfg::io_mutex); 
	string strJsonSend;
	try{
		//输入必须有内容，但是最大不得长于下位机内存大小，做合理限制
		assert(NULL!=pszMsg);
		if (NULL==pszMsg)
		{
			return ELOCK_ERROR_PARAMINVALID;
		}
		int inlen=strlen(pszMsg);
		assert(inlen>0 && inlen<zwCfg::JC_MSG_MAXLEN);
		if (inlen==0 || inlen>=zwCfg::JC_MSG_MAXLEN )
		{
			return ELOCK_ERROR_PARAMINVALID;
		}
		//////////////////////////////////////////////////////////////////////////
		string strXMLSend=pszMsg;		
		//string strRecv;
		assert(strXMLSend.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
		int msgTypeSend=jcAtmcConvertDLL::zwCCBxml2JCjson(strXMLSend,strJsonSend);
		assert(strJsonSend.length()>9);	//json最基本的符号起码好像要9个字符左右
		string msgTypeStrSend;
		switch (msgTypeSend)
		{
		case JCMSG_LOCK_ACTIVE_REQUEST:
			msgTypeStrSend="JCMSG_LOCK_ACTIVE_REQUEST";
			break;
		}
		zwPushString(strJsonSend);
		return ELOCK_ERROR_SUCCESS;
	}
	catch(ptree_bad_data &e)
	{
		ZWTRACE("XML2JSON BAD DATA:");
		ZWTRACE(e.what());
		return ELOCK_ERROR_CONNECTLOST;
	}
	catch(ptree_bad_path &e)
	{
		ZWTRACE("XML2JSON BAD DATA:");
		ZWTRACE(e.what());
		return ELOCK_ERROR_CONNECTLOST;
	}
	catch(ptree_error &e)
	{
		
		ZWTRACE("XML2JSON ERROR PERROR");
		ZWTRACE(e.what());
		return ELOCK_ERROR_CONNECTLOST;
	}
	catch (...)
	{//一切网络异常都直接返回错误。主要是为了捕捉未连接时
		//WebSocket对象为空造成访问NULL指针的的SEH异常	
		//为了使得底层Poco库与cceblock类解耦，从我的WS类
		//发现WS对象因为未连接而是NULL时直接throw一个枚举
		//然后在此，也就是上层捕获。暂时不知道捕获精确类型
		//所以catch所有异常了
		ZWTRACE("CPPEXECPTION804A OTHER ERROR");
		ZWTRACE(__FUNCTION__);
		ZWTRACE(pszMsg);
		ZWTRACE(strJsonSend.c_str());
		return ELOCK_ERROR_CONNECTLOST;
	}
}

void cdecl myATMCRecvMsgRotine(const char *pszMsg)
{
	ZWFUNCTRACE
	assert(pszMsg!=NULL && strlen(pszMsg)>42);
	boost:: mutex:: scoped_lock lock( zwCfg::io_mutex); 
	//输入必须有内容，但是最大不得长于下位机内存大小，做合理限制
	assert(NULL!=pszMsg);
	int inlen=strlen(pszMsg);
	assert(inlen>0 && inlen<zwCfg::JC_MSG_MAXLEN);
	if (NULL==pszMsg || inlen==0 || inlen>=zwCfg::JC_MSG_MAXLEN )
	{
		return;
	}
}


CCBELOCK_API int JCAPISTD SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun)
{
	ZWFUNCTRACE
	boost:: mutex:: scoped_lock lock( zwCfg::io_mutex); 
	assert(NULL!=pRecvMsgFun);
	if (NULL==pRecvMsgFun)
	{
		return ELOCK_ERROR_PARAMINVALID;
	}
	zwCfg::g_WarnCallback=pRecvMsgFun;
	return ELOCK_ERROR_SUCCESS;
}


