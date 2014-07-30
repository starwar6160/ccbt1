// CCBelock.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "CCBelock.h"
#include "zwwsClient.h"
using namespace std;

//回调函数指针类型定义
typedef void (cdecl *RecvMsgRotine)(const char *pszMsg);
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
	//建行给的接口，没有设置连接参数的地方，也就是说，完全可以写死IP和端口，抑或是从配置文件读取
	zwWebSocket zwsc("localhost",1425);

}	//namespace zwCfg{




CCBELOCK_API long Open(long lTimeOut)
{
	boost:: mutex:: scoped_lock lock( zwCfg::io_mutex); 
	//必须大于0，小于JC_CCBDLL_TIMEOUT，限制在一个合理范围内
	assert(lTimeOut>0 && lTimeOut<zwCfg::JC_CCBDLL_TIMEOUT);
	if (lTimeOut<=0 || lTimeOut>=zwCfg::JC_CCBDLL_TIMEOUT)
	{
		return ELOCK_ERROR_PARAMINVALID;
	}	
try{
	zwCfg::zwsc.wsConnect();
}
catch (ConnectionRefusedException &exc)
{	//一般最常见的也就是服务器不在线，所以连接拒绝(ConnectionRefusedException)异常
cout<<__FUNCTION__<<" \t"<<exc.displayText()<<endl;
	return ELOCK_ERROR_CONNECTLOST;
}

	return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long Close()
{
	boost:: mutex:: scoped_lock lock( zwCfg::io_mutex); 
	zwCfg::g_WarnCallback=NULL;
	zwCfg::zwsc.wsClose();	
	return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long Notify(const char *pszMsg)
{
	boost:: mutex:: scoped_lock lock( zwCfg::io_mutex); 
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
		string strJsonSend;
		string strRecv;
		int msgType=zwXML2Json(strXMLSend,strJsonSend);
		cout<<"MESSAGE TYPE 730=\t"<<msgType<<endl;
		zwCfg::zwsc.SendString(strJsonSend);		
		zwCfg::zwsc.ReceiveString(strRecv);
		//////////////////////////////////////////////////////////////////////////
		//例子，利用Notify测试一下回调函数
		if (NULL!=zwCfg::g_WarnCallback)
		{
			zwCfg::g_WarnCallback(strRecv.c_str());
		}
		return ELOCK_ERROR_SUCCESS;
	}
	catch (...)
	{//一切网络异常都直接返回错误。主要是为了捕捉未连接时
		//WebSocket对象为空造成访问NULL指针的的SEH异常	
		//为了使得底层Poco库与cceblock类解耦，从我的WS类
		//发现WS对象因为未连接而是NULL时直接throw一个枚举
		//然后在此，也就是上层捕获。暂时不知道捕获精确类型
		//所以catch所有异常了
		return ELOCK_ERROR_CONNECTLOST;
	}
}

void cdecl myATMCRecvMsgRotine(const char *pszMsg)
{
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


CCBELOCK_API int SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun)
{
	boost:: mutex:: scoped_lock lock( zwCfg::io_mutex); 
	assert(NULL!=pRecvMsgFun);
	if (NULL==pRecvMsgFun)
	{
		return ELOCK_ERROR_PARAMINVALID;
	}
	zwCfg::g_WarnCallback=pRecvMsgFun;
	return ELOCK_ERROR_SUCCESS;
}