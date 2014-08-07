// CCBelock.cpp : ���� DLL Ӧ�ó���ĵ���������
//
//20140804.1346.Ŀǰ���ڵ����⣺
//1.�������߼���������������û��"State","get"���ֶ��ˣ�û�����õ�"Public_Key","123456"�ֶ��ˣ�
//2.�������߼�����Ϣ��JC��Ӧ������LOCKNO�ֶΣ���Ҫ�����
//JC��Ӧ���ĵ�"������Ϣ"�����Ƿ����Ѿ����ܵ�PSK?
//3.����ʱ���ֶΣ�����Ӧ����������ԭ����������ʱ�仹�Ǹ���ʵ��ʱ��������ʱ���ֶΣ�
//4.��ʼ����ʱ�򣬽����·��������������������Կ�����ǵı�������û�У������ڲ��Դ˽��д�������
//5.�°��ĵ�����"���߳�ʼ��"�������ǡ�Atm_Serial����Ӧ���ǡ�Lock_Serial�������ĸ�Ϊ׼��


#include "stdafx.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
#include "zwwsClient.h"
using namespace std;
using boost::property_tree::ptree_error;
using boost::property_tree::ptree_bad_data;
using boost::property_tree::ptree_bad_path;

//�ص�����ָ�����Ͷ���
using Poco::Net::ConnectionRefusedException;

namespace zwCfg{
//#ifdef _DEBUG
	const long	JC_CCBDLL_TIMEOUT=30;	//���ʱʱ��Ϊ30��,���ڲ���Ŀ�ľ���ﵽ���Ʊ�¶����
	const int	JC_MSG_MAXLEN=4*1024;	//�Ϊ128�ֽ�,���ڲ���Ŀ�ľ���ﵽ���Ʊ�¶����
//#else
//	const long	JC_CCBDLL_TIMEOUT=3600;	//���ʱʱ��Ϊ1��Сʱ������Ҳû��������
//	const int	JC_MSG_MAXLEN=128*1024;	//�Ϊ��λ��RAM�Ĵ�С������Ҳû��������
//#endif // _DEBUG
	//����һ���ص�����ָ��
	RecvMsgRotine g_WarnCallback=NULL;
	boost:: mutex io_mutex; 	
	//zwWebSocket zwsc("localhost",1425);

}	//namespace zwCfg{




CCBELOCK_API long JCAPISTD Open(long lTimeOut)
{
	ZWFUNCTRACE
	boost:: mutex:: scoped_lock lock( zwCfg::io_mutex); 
	//�������0��С��JC_CCBDLL_TIMEOUT��������һ������Χ��
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
	assert(pszMsg!=NULL && strlen(pszMsg)>42);	//XML����42�ֽ�
	boost:: mutex:: scoped_lock lock( zwCfg::io_mutex); 
	string strJsonSend;
	try{
		//������������ݣ�������󲻵ó�����λ���ڴ��С������������
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
		assert(strXMLSend.length()>42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
		int msgTypeSend=jcAtmcConvertDLL::zwCCBxml2JCjson(strXMLSend,strJsonSend);
		assert(strJsonSend.length()>9);	//json������ķ����������Ҫ9���ַ�����
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
	{//һ�������쳣��ֱ�ӷ��ش�����Ҫ��Ϊ�˲�׽δ����ʱ
		//WebSocket����Ϊ����ɷ���NULLָ��ĵ�SEH�쳣	
		//Ϊ��ʹ�õײ�Poco����cceblock�������ҵ�WS��
		//����WS������Ϊδ���Ӷ���NULLʱֱ��throwһ��ö��
		//Ȼ���ڴˣ�Ҳ�����ϲ㲶����ʱ��֪������ȷ����
		//����catch�����쳣��
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
	//������������ݣ�������󲻵ó�����λ���ڴ��С������������
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


