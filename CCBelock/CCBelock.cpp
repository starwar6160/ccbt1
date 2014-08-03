// CCBelock.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
#include "zwwsClient.h"
using namespace std;

//�ص�����ָ�����Ͷ���
typedef void (cdecl *RecvMsgRotine)(const char *pszMsg);
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
	//���и��Ľӿڣ�û���������Ӳ����ĵط���Ҳ����˵����ȫ����д��IP�Ͷ˿ڣ��ֻ��Ǵ������ļ���ȡ
	zwWebSocket zwsc("localhost",1425);

}	//namespace zwCfg{




CCBELOCK_API long Open(long lTimeOut)
{
	boost:: mutex:: scoped_lock lock( zwCfg::io_mutex); 
	//�������0��С��JC_CCBDLL_TIMEOUT��������һ������Χ��
	assert(lTimeOut>0 && lTimeOut<zwCfg::JC_CCBDLL_TIMEOUT);
	if (lTimeOut<=0 || lTimeOut>=zwCfg::JC_CCBDLL_TIMEOUT)
	{
		return ELOCK_ERROR_PARAMINVALID;
	}	
try{
	zwccbthr::zwStartLockCommThread();
	zwCfg::zwsc.wsConnect();
}
catch (ConnectionRefusedException &exc)
{	//һ�������Ҳ���Ƿ����������ߣ��������Ӿܾ�(ConnectionRefusedException)�쳣
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
	assert(pszMsg!=NULL && strlen(pszMsg)>42);	//XML����42�ֽ�
	boost:: mutex:: scoped_lock lock( zwCfg::io_mutex); 
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
		string strJsonSend;
		string strRecv;
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
		//cout<<"MESSAGE FROM ATMC'S TYPE=\t"<<msgTypeStrSend<<endl;
		zwCfg::zwsc.SendString(strJsonSend);		
		zwCfg::zwsc.ReceiveString(strRecv);
		assert(strRecv.length()>9);	//json������ķ����������Ҫ9���ַ�����
		string outXML;
		int msgTypeRecv=jcAtmcConvertDLL::zwJCjson2CCBxml(strRecv,outXML);
		assert(outXML.length()>42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
		string msgTypeStrRecv;
		switch (msgTypeSend)
		{
		case JCMSG_LOCK_ACTIVE_REQUEST:
			msgTypeStrSend="JCMSG_LOCK_ACTIVE_REQUEST";
			break;
		}
		//cout<<"MESSAGE FROM LOCK'S TYPE=\t"<<msgTypeStrSend<<endl;

		//////////////////////////////////////////////////////////////////////////
		//���ӣ�����Notify����һ�»ص�����
		if (NULL!=zwCfg::g_WarnCallback)
		{
			zwCfg::g_WarnCallback(outXML.c_str());
		}
		return ELOCK_ERROR_SUCCESS;
	}
	catch (...)
	{//һ�������쳣��ֱ�ӷ��ش�����Ҫ��Ϊ�˲�׽δ����ʱ
		//WebSocket����Ϊ����ɷ���NULLָ��ĵ�SEH�쳣	
		//Ϊ��ʹ�õײ�Poco����cceblock�������ҵ�WS��
		//����WS������Ϊδ���Ӷ���NULLʱֱ��throwһ��ö��
		//Ȼ���ڴˣ�Ҳ�����ϲ㲶����ʱ��֪������ȷ����
		//����catch�����쳣��
		return ELOCK_ERROR_CONNECTLOST;
	}
}

void cdecl myATMCRecvMsgRotine(const char *pszMsg)
{
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


