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

void JCLMSCCB2014_API zwJclmsRsp( void * inLmsReq,const int inLmsReqLen,JCRESULT *lmsResult );

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

void myHexDump( const void * hidSendBuf,const int outLen )
{
	const char *pt=static_cast<const char *>(hidSendBuf);
	for (int i=0;i<outLen;i++)
	{
		if (i>0)
		{
			if(i%8==0 && i%16!=0)printf("\t");
			if(i%16==0)printf("\n");
		}
		unsigned char c=pt[i] & 0xFF;
		printf("%02X ",c);
	}
	printf("\n");
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
	JCRESULT lmsRsp;
	//printf("zwJclmsReqGenDyCode initCloseCode\n");
	int initCloseCode=0;
	initCloseCode=JcLockGetDynaCode(handle);
	//这里是一个自检测试，如果失败，就说明有比较大的问题了，比如类似发生过的
	//ARM编译器优化级别问题导致的生成错误的二进制代码等等
	if(38149728!=initCloseCode)
	{
		printf("initCloseCode Gen Error! JCLMS Algorithm GenDynaCode Self Check Fail! 20141203\n");
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
		printf("JcLockReverseVerifyDynaCode Error! JCLMS Algorithm Reverse DynaCode Self Check Fail! 20141203\n");
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
	printf("%s jsonLen=%d\n%s\n",__FUNCTION__,strlen(tmpjson),tmpjson);
}
#endif // _DEBUG_1205