#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sm3.h"
#include "cJSON.h"
#include "jclmsCCB2014.h"

#include "dCodeHdr.h"
#include "zwhidComm.h"
#include "zwHidSplitMsg.h"
#include "zwSecretBoxAuth.h"

#define _DEBUG_USE_LMS_FUNC_CALL_20141202

void JCLMSCCB2014_API zwJclmsRsp( void * inLmsReq,const int inLmsReqLen,char *outJson,const int outJsonLen );

#ifdef _DEBUG_1205
void myJcInputHton(JCINPUT *p)
{
	p->CodeGenDateTime=HtoNl(p->CodeGenDateTime);
	p->Validity=HtoNl(p->Validity);
	p->CloseCode=HtoNl(p->CloseCode);
	p->CmdType=static_cast<JCCMD>(HtoNl(p->CmdType));

	p->SearchTimeStart=HtoNl(p->SearchTimeStart);
	p->SearchTimeStep=HtoNl(p->SearchTimeStep);
	p->SearchTimeLength=HtoNl(p->SearchTimeLength);
	for (int i=0;i<NUM_VALIDITY;i++)
	{
		p->ValidityArray[i]=HtoNl(p->ValidityArray[i]);
	}
}

void myJcInputNtoh(JCINPUT *p)
{
	p->CodeGenDateTime=NtoHl(p->CodeGenDateTime);
	p->Validity=NtoHl(p->Validity);
	p->CloseCode=NtoHl(p->CloseCode);
	p->CmdType=static_cast<JCCMD>(NtoHl(p->CmdType));

	p->SearchTimeStart=NtoHl(p->SearchTimeStart);
	p->SearchTimeStep=NtoHl(p->SearchTimeStep);
	p->SearchTimeLength=NtoHl(p->SearchTimeLength);
	for (int i=0;i<NUM_VALIDITY;i++)
	{
		p->ValidityArray[i]=NtoHl(p->ValidityArray[i]);
	}
}

void myLmsReqZHton(JCLMSREQ *req)
{
	myJcInputHton(&req->inputData);
	req->Type=static_cast<JCLMSOP>(HtoNl(req->Type));
	req->dstCode=HtoNl(req->dstCode);
}

void myLmsReqZNtoh(JCLMSREQ *req)
{
	myJcInputNtoh(&req->inputData);
	req->Type=static_cast<JCLMSOP>(NtoHl(req->Type));
	req->dstCode=NtoHl(req->dstCode);
}
#endif // _DEBUG_1205

void myHexDump( const void * hidSendBuf,const int outLen )
{
#ifdef _WIN32
#pragma warning( disable : 4390)
#endif // _WIN32

	const char *pt=static_cast<const char *>(hidSendBuf);
	int Last=outLen-1;
	while (NULL==pt[Last])
	{
		Last--;
	}
	Last++;
	for (int i=0;i<Last;i++)
	{
		if (i>0)
		{
			if(i%8==0 && i%16!=0)
				ZWDBG_INFO("\t");

			if(i%16==0)
				ZWDBG_INFO("\n");
		}
		unsigned char c=pt[i] & 0xFF;
		ZWDBG_INFO("%02X ",c);
	}
	ZWDBG_INFO("\n");

#ifdef _WIN32
#pragma warning( default : 4390)
#endif // _WIN32
}

void myPrintBinAsString(const void *binData,const int binLen)
{
	const char *b=reinterpret_cast<const char *>(binData);
	int Last=binLen-1;
	while (NULL==b[Last])
	{
		Last--;
	}
	Last++;
	for (int i=0;i<Last;i++)
	{		
		char c=b[i];
		ZWDBG_INFO("%c",c);
	}
	ZWDBG_INFO("\n");
}

//一个纯算法层面的标准测试，测试了动态码生成和验证两个环节，用于ARM校验自己是否有编译器优化问题等等；
//20141203.1001.周伟
int zwLmsAlgStandTest20141203(void)
{
	int handle=0;
	int pass1DyCode=0;
	handle = JcLockNew();
	JcLockSetString(handle, JCI_ATMNO, "atm10455761");
	JcLockSetString(handle, JCI_LOCKNO, "lock14771509");
	JcLockSetString(handle, JCI_PSK, "PSKDEMO728");
	//////////////////////////////////////////////////////////////////////////
	//固定开锁时间,应该出来固定的结果
	const int ZWFIX_STARTTIME=1416*ZWMEGA;
	JcLockSetInt(handle,JCI_TIMESTEP,6);
	JcLockSetInt(handle,JCI_SEARCH_TIME_START,1416*ZWMEGA+127);
	JcLockSetCmdType(handle, JCI_CMDTYPE, JCCMD_INIT_CLOSECODE);
	//////////////////////////////////////////////////////////////////////////
	//ZWPRINTF("zwJclmsReqGenDyCode initCloseCode\n");
	int initCloseCode=0;
	initCloseCode=JcLockGetDynaCode(handle);
	//这里是一个自检测试，如果失败，就说明有比较大的问题了，比如类似发生过的
	//ARM编译器优化级别问题导致的生成错误的二进制代码等等
	if(38149728!=initCloseCode)
	{
		ZWDBG_ERROR("initCloseCode Gen Error! JCLMS Algorithm GenDynaCode Self Check Fail! 20141203\n");
		return -1;
	}
	JcLockSetInt(handle, JCI_CLOSECODE, initCloseCode);
	JcLockSetInt(handle,JCI_DATETIME,1416*ZWMEGA);
	JcLockSetInt(handle,JCI_SEARCH_TIME_START,1416*ZWMEGA+127);
	JcLockSetCmdType(handle, JCI_CMDTYPE, JCCMD_CCB_DYPASS1);
	pass1DyCode=JcLockGetDynaCode(handle);
	assert(57174184==pass1DyCode);
	JCMATCH pass1Match =
		JcLockReverseVerifyDynaCode(handle,pass1DyCode);
	if(ZWFIX_STARTTIME!=pass1Match.s_datetime)
	{
		ZWDBG_ERROR("JcLockReverseVerifyDynaCode Error! JCLMS Algorithm Reverse DynaCode Self Check Fail! 20141203\n");
		return -2;
	}	
	//////////////////////////////////////////////////////////////////////////
	JcLockDelete(handle);
	return 0;
}

#ifdef _DEBUG_1205
void myLmsReq2Json( int lmsHandle, char * tmpjson )
{
	JCINPUT *jcp=reinterpret_cast<JCINPUT *>(lmsHandle);
	//zwJclmsReq2Json(jcp,tmpjson,ZWBUFLEN);
	cJSON *json,*jsReq;
	//把主要的JCINPUT结构体转换为JSON内部格式
	zwJcInputConv2Json(&json,jcp);
	cJSON_AddItemToObject(json, "LMSRequest", jsReq=cJSON_CreateObject()); 
	cJSON_AddStringToObject(jsReq,"Type",     "JCLMS_CCB_CODEGEN");   
	char *cjout=cJSON_Print(json);
	strcpy(tmpjson,cjout);
	free(cjout);
	ZWDBG_NOTICE("%s jsonLen=%d\n%s\n",__FUNCTION__,strlen(tmpjson),tmpjson);
}
#endif // _DEBUG_1205

const int ZW_JSONBUF_LEN=640;

//该函数是下位机专用
//输入：inLmsReq，指向一个HID接收到的，拼装完毕的整条jclms请求消息。该消息只有一个网络字节序
//short int头部，余下的部分就是JSON的数据包了，JSON长度在HID头部的长度字段中
//注意HID头部长度字段是网络字节序的
//输入：inLmsReqLen，是整条JCLMS请求消息的长度，包含HID头部在内
//输出：JCRESULT联合体，取决于是生成请求还是验证请求，相应的哪一个字段有效；
void JCLMSCCB2014_API zwJclmsRsp( void * inLmsReq,const int inLmsReqLen,char *outJson,const int outJsonLen )
{	
	assert(NULL!=inLmsReq && inLmsReqLen>0);
	assert(NULL!=outJson && outJsonLen>0);
	if (NULL==inLmsReq || inLmsReqLen<=0)
	{
		ZWDBG_ERROR("ERROR:%s:input LMS Request is NULL! Return",__FUNCTION__);
		return;
	}
	if (NULL==outJson || outJsonLen<=0)
	{
		ZWDBG_ERROR("ERROR:%s:output LMS Respon JSON Buffer is NULL! Return",__FUNCTION__);
		return;
	}
	ZWDBG_INFO("INFO:%s:input LMS Request Data is:\n",__FUNCTION__);
	myHexDump(inLmsReq,inLmsReqLen);
	//从外部接收数据
	JCLMSREQ lmsReq;
	JCRESULT lmsResult;
	char inJson[ZW_JSONBUF_LEN];
	memset(inJson,0,ZW_JSONBUF_LEN);

	//PC调试时输入大小必须是HID有效载荷头部+JCLMSREQ JSON的大小
	assert(inLmsReqLen+sizeof(SECBOX_DATA_INFO)<=ZW_JSONBUF_LEN);
	if (inLmsReqLen+sizeof(SECBOX_DATA_INFO)>ZW_JSONBUF_LEN)
	{
		ZWDBG_ERROR("ERROR:%s:INTERNAL JSON Input Buffer Tool Small.",__FUNCTION__);
	}
	//跳过HID有效载荷头部
	//memcpy((void *)&lmsReq,(char *)inLmsReq+sizeof(SECBOX_DATA_INFO),inLmsReqLen-sizeof(SECBOX_DATA_INFO));
	strncpy(inJson,(char *)inLmsReq+sizeof(SECBOX_DATA_INFO),ZW_JSONBUF_LEN);
	int inJsonLen=strlen(inJson);
	int inJsonCRC8=crc8Short(inJson,inJsonLen);
	SECBOX_DATA_INFO *inHdr=(SECBOX_DATA_INFO *)inLmsReq;
	//备注：在JCLMS HID的数据包中，data_index无意义，被挪用作为CRC8校验码的地方
	if(inHdr->data_index!=inJsonCRC8)
	{
		ZWDBG_ERROR("Input JCLMS Hid Request Json CRC8 Sum Check Fail!\nGood CRC8=%d\tReal CRC8=%d\n",
			inHdr->data_index,inJsonCRC8);
	}
	zwJclmsReqDecode(inJson,&lmsReq);

	zwJcLockDumpJCINPUT((int)(&lmsReq));
	ZWDBG_NOTICE("%s dstCode=%d\n",__FUNCTION__,lmsReq.dstCode);
	//既不是0，又不是8位数字，那么就是错误值了
	if (0!=lmsReq.dstCode &&(lmsReq.dstCode<10*ZWMEGA || lmsReq.dstCode>100*ZWMEGA) )
	{
		lmsResult.dynaCode=-1208;
		ZWDBG_ERROR("ERROR:%s:dstCode Invalid!\n",__FUNCTION__);
		return ;
	}

	memset(outJson,0,outJsonLen);

	//通过出参结构体返回计算结果给外部
	int dyCode=0;
	if (JCLMS_CCB_CODEGEN==lmsReq.Type)
	{
		dyCode=zwJcLockGetDynaCode((int)(&lmsReq.inputData));
		assert(dyCode>=10*ZWMEGA && dyCode<100*ZWMEGA);
		if (dyCode<10*ZWMEGA || dyCode>=100*ZWMEGA)
		{
			ZWDBG_ERROR("ERROR:%s:dyCode result Out of Range Invalid!\n",__FUNCTION__);
		}
		lmsResult.dynaCode=dyCode;
		zwJclmsRersult2Json(&lmsResult,JCLMS_CCB_CODEGEN,outJson,outJsonLen);
	}
	if (JCLMS_CCB_CODEVERIFY==lmsReq.Type)
	{
		JCMATCH jm=JcLockReverseVerifyDynaCode((int)(&lmsReq.inputData),lmsReq.dstCode);
		assert(jm.s_datetime>1400*ZWMEGA);
		assert(jm.s_validity>0 && jm.s_validity<=1440);
		if (jm.s_datetime<=1400*ZWMEGA)
		{
			ZWDBG_WARN("WARN:%s:Match s_datetime too old!\n",__FUNCTION__);
		}
		if (jm.s_validity<=0 || jm.s_validity>1440)
		{
			ZWDBG_WARN("WARN:%s:Match Validity out of Range!\n",__FUNCTION__);
		}
		memcpy((void *)&(lmsResult.verCodeMatch),(void *)&jm,sizeof(JCMATCH));
		zwJclmsRersult2Json(&lmsResult,JCLMS_CCB_CODEVERIFY,outJson,outJsonLen);
	}	
	ZWDBG_INFO("INFO:%s:jclms Result JSON is:\n%s\n",__FUNCTION__,outJson);
	
}

int JCLMSCCB2014_API csJclmsReqGenDyCode( int lmsHandle )
{
	int dyCode;
	zwJclmsReqGenDyCode(lmsHandle,&dyCode);
	return dyCode;
}

#ifdef _WIN32
void myHexDump(const void * hidSendBuf,const int outLen );
const int ZWHIDBUFLEN=640;
char g_dbg_hid_common1202[ZWHIDBUFLEN];

void myLmsReq2Json( int lmsHandle, char * tmpjson );

//两个zwJclmsReq函数是上位机专用

//填写完毕handle里面的数据结构以后，调用该函数生成动态码，该函数在底层将请求
//做JSON序列化以后通过HID等通信线路发送到密盒，然后阻塞接收密盒返回结果，通过出参返回；
int JCLMSCCB2014_API zwJclmsReqGenDyCode( int lmsHandle,int *dyCode )
{
	JCLMSREQ req;
	memset(&req,0,sizeof(JCLMSREQ));
	req.Type=JCLMS_CCB_CODEGEN;
	memcpy((void *)&req.inputData,(void *)lmsHandle,sizeof(JCINPUT));
	////////////////////////////JSON序列化开始//////////////////////////////////////////////	
	char tmpjson[ZW_JSONBUF_LEN];
	int tmpJsonLen=0;
	memset(tmpjson,0,ZW_JSONBUF_LEN);
	//////////////////////////////////////////////////////////////////////////
	zwJclmsGenReq2Json(reinterpret_cast<JCINPUT *>(lmsHandle),tmpjson,ZW_JSONBUF_LEN);
	tmpJsonLen=strlen(tmpjson);
	//JCLMSREQ req2t;
	//zwJclmsReqDecode(tmpjson,&req2t);
	////////////////////////////JSON序列化结束//////////////////////////////////////////////
	//////////////////////////////////模拟发送数据////////////////////////////////////////
	//此处由于是模拟，时序不好控制，为了便于调试，在此直接调用密盒端的函数zwJclmsRsp来做处理
	//ZWPRINTF("%s Send Data to Secbox for Gen DynaCode:\n",__FUNCTION__);
	//zwJcLockDumpJCINPUT(lmsHandle);	
	//////////////////////////////////////////////////////////////////////////
	//构建整个HID发送数据包，给下层HID函数去切分和发送
	char hidSendBuf[ZWHIDBUFLEN];
	memset(hidSendBuf,0,ZWHIDBUFLEN);
	//HID有效载荷的头部
	SECBOX_DATA_INFO hidPayloadHeader;
	memset(&hidPayloadHeader,0,sizeof(hidPayloadHeader));
	hidPayloadHeader.msg_type=HtoNs(JC_SECBOX_LMS_GENDYCODE);
	hidPayloadHeader.data_index=crc8Short(tmpjson,tmpJsonLen);
	hidPayloadHeader.data_len=tmpJsonLen;
	const int outLen=sizeof(hidPayloadHeader)+tmpJsonLen;
	
	//先加入头部
	memcpy(hidSendBuf,&hidPayloadHeader,sizeof(hidPayloadHeader));
	//然后加入实际的请求部分
	//myLmsReqZHton(&req);
	//memcpy(hidSendBuf+sizeof(hidPayloadHeader),&req,sizeof(req));
	assert(outLen<ZWHIDBUFLEN);
	strncpy(hidSendBuf+sizeof(hidPayloadHeader),tmpjson,ZWHIDBUFLEN-sizeof(hidPayloadHeader));
	ZWDBG_INFO("HidSend Data is(Net ByteOrder)\n");
	myHexDump(hidSendBuf, outLen);
	//////////////////////////////////////////////////////////////////////////	
	int rspRealLen=0;
	JCRESULT rsp;
	memset(&rsp,0,sizeof(rsp));
	char resJson[ZW_JSONBUF_LEN];
	memset(resJson,0,ZW_JSONBUF_LEN);
	ZWDBG_INFO("%s:jclms Request Json is:\n%s\n",__FUNCTION__,hidSendBuf+sizeof(short int));
#ifdef _DEBUG_USE_LMS_FUNC_CALL_20141202
	//调试状态，直接调用下位机函数即可
	zwJclmsRsp(hidSendBuf,outLen,resJson,ZW_JSONBUF_LEN);	
	ZWDBG_INFO("HidRecv Data ASCII is\n");
	myPrintBinAsString(resJson,ZW_JSONBUF_LEN);
	zwJclmsResultFromJson(resJson,&rsp);
#else
	JCHID hidHandle;
	memset(&hidHandle,0,sizeof(JCHID));
	hidHandle.vid=0x0483;
	hidHandle.pid=0x5710;

	if (JCHID_STATUS_OK != jcHidOpen(&hidHandle)) {
		//return -1118;
	}
	jcHidSendData(&hidHandle,hidSendBuf,outLen);


	ZWDBG_INFO("GenWait To SecBox Return Result now..\n");
	jcHidRecvData(&hidHandle,resJson,ZW_JSONBUF_LEN,&rspRealLen);
	ZWDBG_INFO("HidRecv Data HEX is\n");
	myHexDump(resJson, rspRealLen);
	//ZWPRINTF("%s:jclms Respone Json is:\n%s\n",__FUNCTION__,resJson);	
	jcHidClose(&hidHandle);
	ZWDBG_INFO("HidRecv Data ASCII is\n");
	myPrintBinAsString(resJson,rspRealLen);
	zwJclmsResultFromJson(resJson,&rsp);
#endif // _DEBUG_USE_LMS_FUNC_CALL_20141202
	ZWDBG_NOTICE("%s:jclms Respone Json is:\n%s\n",__FUNCTION__,resJson);
	ZWDBG_INFO("Received lms Respon is:\n");
	myHexDump(resJson,ZW_JSONBUF_LEN);
	assert(0!=rsp.dynaCode);
	*dyCode=rsp.dynaCode;
	ZWDBG_WARN("%s Return dynaCode=%d\n",__FUNCTION__,rsp.dynaCode);

	return 0;
}


//填写完毕handle里面的数据结构以后，调用该函数验证动态码（第一和第二动态码中间，锁具生成的校验码
//也是使用其他两个动态码的同样算法生成的，所以也算一种动态码，该函数在底层将验证请求做JSON序列化
//以后通过HID等通信线路发送到密盒，然后阻塞接收密盒返回结果，通过出参返回；
int JCLMSCCB2014_API zwJclmsReqVerifyDyCode( int lmsHandle,int dstCode,JCMATCH *match )
{
	////////////////////////////JSON序列化开始//////////////////////////////////////////////	
	char tmpjson[ZW_JSONBUF_LEN];
	int tmpJsonLen=0;
	memset(tmpjson,0,ZW_JSONBUF_LEN);
	//////////////////////////////////////////////////////////////////////////
	zwJclmsVerReq2Json(reinterpret_cast<JCINPUT *>(lmsHandle),dstCode,tmpjson,ZW_JSONBUF_LEN);
	tmpJsonLen=strlen(tmpjson);
	//////////////////////////////////模拟发送数据////////////////////////////////////////
	//此处由于是模拟，时序不好控制，为了便于调试，在此直接调用密盒端的函数zwJclmsRsp来做处理
	ZWDBG_NOTICE("%s Send Data to Secbox with Wait To Verify DestCode %d\n",__FUNCTION__,dstCode);
	zwJcLockDumpJCINPUT(lmsHandle);
	//////////////////////////////////////////////////////////////////////////
	//构建整个HID发送数据包，给下层HID函数去切分和发送
	char hidSendBuf[ZWHIDBUFLEN];
	memset(hidSendBuf,0,ZWHIDBUFLEN);
	//HID有效载荷的头部
	SECBOX_DATA_INFO hidPayloadHeader;
	memset(&hidPayloadHeader,0,sizeof(hidPayloadHeader));
	hidPayloadHeader.msg_type=HtoNs(JC_SECBOX_LMS_GENDYCODE);
	hidPayloadHeader.data_index=crc8Short(tmpjson,tmpJsonLen);
	hidPayloadHeader.data_len=tmpJsonLen;

	const int outLen=sizeof(hidPayloadHeader)+tmpJsonLen;

	//先加入头部
	memcpy(hidSendBuf,&hidPayloadHeader,sizeof(hidPayloadHeader));
	//然后加入实际的请求部分
	//myLmsReqZHton(&req);
	//memcpy(hidSendBuf+sizeof(hidPayloadHeader),&req,sizeof(req));
	assert(sizeof(hidPayloadHeader)+tmpJsonLen<ZWHIDBUFLEN);
	strncpy(hidSendBuf+sizeof(hidPayloadHeader),tmpjson,ZWHIDBUFLEN-sizeof(hidPayloadHeader));
	ZWDBG_INFO("HidSend Data is(Net ByteOrder)\n");
	myHexDump(hidSendBuf, outLen);

	//////////////////////////////////////////////////////////////////////////
	JCRESULT rsp;
	memset(&rsp,0,sizeof(rsp));
	char resJson[ZW_JSONBUF_LEN];
	memset(resJson,0,ZW_JSONBUF_LEN);
	ZWDBG_INFO("%s:jclms Request Json is:\n%s\n",__FUNCTION__,hidSendBuf+sizeof(short int));
#ifdef _DEBUG_USE_LMS_FUNC_CALL_20141202
	//调试状态，直接调用下位机函数即可
	zwJclmsRsp(hidSendBuf,outLen,resJson,ZW_JSONBUF_LEN);
	zwJclmsResultFromJson(resJson,&rsp);
#else
	JCHID hidHandle;
	hidHandle.vid=0x0483;
	hidHandle.pid=0x5710;
	if (JCHID_STATUS_OK != jcHidOpen(&hidHandle)) {
		return -1118;
	}
	jcHidSendData(&hidHandle,hidSendBuf,outLen);
	ZWDBG_INFO("VerWait To SecBox Return Result now..\n");
	int rspRealLen=0;
	jcHidRecvData(&hidHandle,resJson,ZW_JSONBUF_LEN,&rspRealLen);
	//ZWPRINTF("HidRecv Data is\n");
	//myHexDump(&rsp, rspRealLen);
	//assert(rsp.verCodeMatch.s_datetime>1400*ZWMEGA);
	ZWDBG_INFO("HidRecv Data HEX is\n");
	myHexDump(resJson, rspRealLen);
	//ZWPRINTF("%s:jclms Respone Json is:\n%s\n",__FUNCTION__,resJson);	
	jcHidClose(&hidHandle);
	ZWDBG_INFO("HidRecv Data ASCII is\n");
	myPrintBinAsString(resJson,rspRealLen);
	zwJclmsResultFromJson(resJson,&rsp);
#endif // _DEBUG_USE_LMS_FUNC_CALL_20141202
	ZWDBG_NOTICE("%s:jclms Respone Json is:\n%s\n",__FUNCTION__,resJson);
	ZWDBG_INFO("Received lms Respon is:\n");
	myHexDump(resJson,ZW_JSONBUF_LEN);
	memcpy((void *)match,(void *)&rsp.verCodeMatch,sizeof(JCMATCH));
	ZWDBG_WARN("%s Match DateTime=%d\tValidity=%d\n",__FUNCTION__,match->s_datetime,match->s_validity);	
	return 0;
}
#endif // _WIN32
