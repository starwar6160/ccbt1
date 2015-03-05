//#include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>
#include "jclmsCCB2014.h"
#include "dCodeHdr.h"
#include "cJSON.h"
#include <string>
using std::string;

extern "C"
{
unsigned char crc8(const unsigned char crc8Input,const void *inputData, const int inputLen );
extern const int ZW_ONE_DAY;
};


//只是作为一个基本的块规整大小单位方便处理
const int ZW_SYNCALG_BLOCK_SIZE = (128 / 8);



//}     //end of namespace jclms


int JcLockGetVersion(void)
{
	//含义是是日期
	return 20140901;
}




void myCjsonTest1(void)
{
	cJSON *root,*fmt;   
	root=cJSON_CreateObject();     
	cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack Nimble"));   
	//又一层json对象，添加到根对象里面
	cJSON_AddItemToObject(root, "format", fmt=cJSON_CreateObject());   
	cJSON_AddStringToObject(fmt,"type",     "rect");   
	cJSON_AddNumberToObject(fmt,"width",        1920);   
	cJSON_AddNumberToObject(fmt,"height",       1080);   
	cJSON_AddFalseToObject (fmt,"interlace");   
	cJSON_AddNumberToObject(fmt,"frame rate",   24.7); 
	char *cjout=cJSON_Print(root);
	ZWDBG_INFO("%s\n",cjout);
}

const char * zwJcCmdToString(const JCCMD cmd)
{
	switch (cmd)
	{
	case JCCMD_CCB_DYPASS1:
		return "JCCMD_CCB_DYPASS1";
		break;
	case JCCMD_CCB_LOCK_VERCODE:
		return "JCCMD_CCB_LOCK_VERCODE";
		break;
	case JCCMD_CCB_DYPASS2:
		return "JCCMD_CCB_DYPASS2";
		break;
	case JCCMD_CCB_CLOSECODE:
		return "JCCMD_CCB_CLOSECODE";
		break;
	case JCCMD_INIT_CLOSECODE:
		return "JCCMD_INIT_CLOSECODE";
		break;
	default:
		return "JCCMD_INVALID_COMMAND";
	}
}

JCCMD zwJcCmdFromString(const char *cmdStr)
{
	if (0==strcmp("JCCMD_CCB_DYPASS1",cmdStr))
	{		
		return JCCMD_CCB_DYPASS1;
	}
	if (0==strcmp("JCCMD_CCB_LOCK_VERCODE",cmdStr))
	{		
		return JCCMD_CCB_LOCK_VERCODE;
	}
	if (0==strcmp("JCCMD_CCB_DYPASS2",cmdStr))
	{		
		return JCCMD_CCB_DYPASS2;
	}
	if (0==strcmp("JCCMD_CCB_CLOSECODE",cmdStr))
	{		
		return JCCMD_CCB_CLOSECODE;
	}
	if (0==strcmp("JCCMD_INIT_CLOSECODE",cmdStr))
	{		
		return JCCMD_INIT_CLOSECODE;
	}
	return JCCMD_START;	//什么都没找到就返回一个无效值
}

const char *jclmsRequestType_t[]={
	"JCLMS_CCB_CODEGEN",
	"JCLMS_CCB_CODEVERIFY"
};

const char * zwJclmsopToString(const JCLMSOP cmd)
{
	switch (cmd)
	{
	case JCLMS_CCB_CODEGEN:
		return "JCLMS_CCB_CODEGEN";
		break;
	case JCLMS_CCB_CODEVERIFY:
		return "JCLMS_CCB_CODEVERIFY";
		break;
	default:
		return "JCLMS_CCB_INVALID_COMMAND";
	}
}

JCLMSOP zwJclmsopFromString(const char *cmdStr)
{
	if (0==strcmp("JCLMS_CCB_CODEGEN",cmdStr))
	{		
		return JCLMS_CCB_CODEGEN;
	}
	if (0==strcmp("JCLMS_CCB_CODEVERIFY",cmdStr))
	{		
		return JCLMS_CCB_CODEVERIFY;
	}
	return JCLMS_CCB_INVALID;	//什么都没找到就返回一个无效值
}



cJSON * zwJcInputConv2Json( cJSON ** root, const JCINPUT * p )
{
	cJSON *jcInput,*validityArray;   
	//root:整个json的root,第一级别
	//jcInput:JCINPUT结构体的root，第二级别
	//validityArray:有效期数组的root，第三级别
	*root=cJSON_CreateObject();
	//jciRoot=cJSON_CreateObject();     
	cJSON_AddItemToObject(*root, "JCINPUT", jcInput=cJSON_CreateObject());   
	cJSON_AddItemToObject(jcInput, "ATMNO", cJSON_CreateString(p->AtmNo));   
	cJSON_AddItemToObject(jcInput, "LOCKNO", cJSON_CreateString(p->LockNo));   
	cJSON_AddItemToObject(jcInput, "PSK", cJSON_CreateString(p->PSK));   
	cJSON_AddNumberToObject(jcInput,"CodeGenDateTime",        p->CodeGenDateTime);   
	cJSON_AddNumberToObject(jcInput,"Validity",        p->Validity);  
	cJSON_AddNumberToObject(jcInput,"CloseCode",        p->CloseCode);  
	cJSON_AddStringToObject(jcInput,"CmdType",        zwJcCmdToString(p->CmdType));  

	cJSON_AddNumberToObject(jcInput,"SearchTimeStart",        p->SearchTimeStart);  
	cJSON_AddNumberToObject(jcInput,"SearchTimeStep",        p->SearchTimeStep);  
	cJSON_AddNumberToObject(jcInput,"SearchTimeLength",        p->SearchTimeLength);  

	cJSON_AddItemToObject(jcInput, "ValidityArray", validityArray=cJSON_CreateObject());   
	for(int i=0;i<NUM_VALIDITY;i++)
	{
		cJSON_AddNumberToObject(validityArray,"Min",        p->ValidityArray[i]);  
	}
	return *root;
}

void zwJclmsGenReq2Json(const JCINPUT *p,char *outJson,const int outBufLen)
{
	cJSON *root;     ;
	zwJcInputConv2Json(&root, p);
	cJSON *ztNode1;
	cJSON_AddItemToObject(root, "jcLmsRequest", ztNode1=cJSON_CreateObject());   
	cJSON_AddStringToObject(ztNode1,"Type",     zwJclmsopToString(JCLMS_CCB_CODEGEN));   
	//又一层json对象，添加到根对象里面
	//cJSON_AddItemToObject(root, "format", fmt=cJSON_CreateObject());   
	//cJSON_AddStringToObject(fmt,"type",     "rect");   
	//cJSON_AddNumberToObject(fmt,"width",        1920);   
	//cJSON_AddNumberToObject(fmt,"height",       1080);   
	//cJSON_AddFalseToObject (fmt,"interlace");   
	//cJSON_AddNumberToObject(fmt,"frame rate",   24.7); 
	char *cjout=cJSON_Print(root);
	int cjLen=strlen(cjout);
	if (cjLen>outBufLen)
	{
		cjLen=outBufLen;
	}
	strncpy(outJson,cjout,cjLen);
	free(cjout);
	ZWDBG_INFO("%s\n",outJson);
	cJSON_Delete(root);	
}

void zwJclmsVerReq2Json(const JCINPUT *p,const int dstCode,char *outJson,const int outBufLen)
{
	cJSON *root;     ;
	zwJcInputConv2Json(&root, p);
	cJSON *ztNode1;
	cJSON_AddItemToObject(root, "jcLmsRequest", ztNode1=cJSON_CreateObject());   
	cJSON_AddStringToObject(ztNode1,"Type",     zwJclmsopToString(JCLMS_CCB_CODEVERIFY));   
	cJSON_AddNumberToObject(ztNode1,"dstCode",dstCode);
	//又一层json对象，添加到根对象里面
	//cJSON_AddItemToObject(root, "format", fmt=cJSON_CreateObject());   
	//cJSON_AddStringToObject(fmt,"type",     "rect");   
	//cJSON_AddNumberToObject(fmt,"width",        1920);   
	//cJSON_AddNumberToObject(fmt,"height",       1080);   
	//cJSON_AddFalseToObject (fmt,"interlace");   
	//cJSON_AddNumberToObject(fmt,"frame rate",   24.7); 
	char *cjout=cJSON_Print(root);
	int cjLen=strlen(cjout);
	if (cjLen>outBufLen)
	{
		cjLen=outBufLen;
	}
	strncpy(outJson,cjout,cjLen);
	free(cjout);
	ZWDBG_INFO("%s\n",outJson);
	cJSON_Delete(root);	
}
void zwJclmsReqDecode(const char *inJclmsReqJson,JCLMSREQ *outReq)
{
	assert(NULL!=inJclmsReqJson && strlen(inJclmsReqJson)>0 && NULL!=outReq);
	if (NULL==inJclmsReqJson || strlen(inJclmsReqJson)==0 ||NULL==outReq)
	{
		ZWDBG_ERROR("ERROR:%s:Input jclms Json Request is NULL!Return.\n",__FUNCTION__);
		return;
	}
ZWDBG_INFO("%s:inJclmsReqJson:\n%s\n",__FUNCTION__,inJclmsReqJson);
	cJSON *root=cJSON_Parse(inJclmsReqJson); 
	if (NULL==root)
	{
		ZWDBG_ERROR("ERROR:JCLMS REQUEST JSON Pares Fail.Return");
		return;
	}
	cJSON *req = cJSON_GetObjectItem(root,"jcLmsRequest");   	
	if (NULL==req)
	{
		ZWDBG_ERROR("ERROR:jcLmsRequest not found!Return\n");
		return;
	}
	cJSON *jsType=cJSON_GetObjectItem(req,"Type");
	if (NULL==jsType)
	{
		ZWDBG_ERROR("ERROR:jcLmsRequest Operate Type Item not found!Return\n");
		return;
	}
	outReq->Type=zwJclmsopFromString(jsType->valuestring);
	cJSON *dstCodeJson=cJSON_GetObjectItem(req,"dstCode");
	//给dstCode一个默认值0，然后如果有该项目，用实际值体代之
	outReq->dstCode=0;
	if (NULL!=dstCodeJson)
	{		
		outReq->dstCode=dstCodeJson->valueint;
		ZWDBG_NOTICE("dstCode=%d\n",outReq->dstCode);
	}
	else
	{
		ZWDBG_INFO("dstCode Not Found\n");
	}
	
	//JCINPUT
	cJSON *jci = cJSON_GetObjectItem(root,"JCINPUT");   
	if (NULL==jci)
	{
		ZWDBG_ERROR("ERROR:JCINPUT not found!Return\n");
		return;
	}

	//注意此处，所有最终参与动态码计算的字符串输入因素字段都需要先清零，
	//否则就可能有垃圾数据干扰，导致动态码计算出错误值	
	//20141209.1007.周伟
	memset(outReq->inputData.AtmNo,0,JC_ATMNO_MAXLEN+1);
	memset(outReq->inputData.LockNo,0,JC_LOCKNO_MAXLEN+1);
	memset(outReq->inputData.PSK,0,JC_PSK_LEN+1);
	strncpy(outReq->inputData.AtmNo,cJSON_GetObjectItem(jci,"ATMNO")->valuestring,JC_ATMNO_MAXLEN);
	strncpy(outReq->inputData.LockNo,cJSON_GetObjectItem(jci,"LOCKNO")->valuestring,JC_LOCKNO_MAXLEN);
	strncpy(outReq->inputData.PSK,cJSON_GetObjectItem(jci,"PSK")->valuestring,JC_PSK_LEN);
	outReq->inputData.CodeGenDateTime=cJSON_GetObjectItem(jci,"CodeGenDateTime")->valueint;
	outReq->inputData.Validity=cJSON_GetObjectItem(jci,"Validity")->valueint;
	outReq->inputData.CloseCode=cJSON_GetObjectItem(jci,"CloseCode")->valueint;
	outReq->inputData.CmdType=zwJcCmdFromString(cJSON_GetObjectItem(jci,"CmdType")->valuestring);
	outReq->inputData.SearchTimeStart=cJSON_GetObjectItem(jci,"SearchTimeStart")->valueint;
	outReq->inputData.SearchTimeStep=cJSON_GetObjectItem(jci,"SearchTimeStep")->valueint;
	outReq->inputData.SearchTimeLength=cJSON_GetObjectItem(jci,"SearchTimeLength")->valueint;
	ZWDBG_INFO("jclms Json Main Item Parsed\n");
	//有效期数组
	cJSON *valArr=cJSON_GetObjectItem(jci,"ValidityArray");   
	if (NULL==valArr)
	{
		ZWDBG_ERROR("ERROR:ValidityArray not found!Return\n");
		return;
	}
	for (int i=0;i<NUM_VALIDITY;i++)
	{
		outReq->inputData.ValidityArray[i]=
		cJSON_GetArrayItem(valArr,i)->valueint;
	}
	ZWDBG_INFO("jclms Json Parse Result is:\n");
	zwJcLockDumpJCINPUT(reinterpret_cast<int>(&outReq->inputData));
	cJSON_Delete(root);	
}


void zwJclmsRersult2Json(const JCRESULT *p,const JCLMSOP op,char *outJson,const int outBufLen)
{
	cJSON *root=cJSON_CreateObject();;     ;	
	cJSON *ztNode1;
	cJSON_AddItemToObject(root, "JCRESULT", ztNode1=cJSON_CreateObject());  
	cJSON_AddStringToObject(ztNode1,"Type",     zwJclmsopToString(op));   
	if (JCLMS_CCB_CODEGEN==op)
	{
		cJSON_AddNumberToObject(ztNode1,"dynaCode",p->dynaCode);   
	}
	if (JCLMS_CCB_CODEVERIFY==op)
	{
		cJSON *vMatch;
		cJSON_AddItemToObject(ztNode1, "verCodeMatch", vMatch=cJSON_CreateObject());
		cJSON_AddNumberToObject(vMatch,"s_datetime",p->verCodeMatch.s_datetime);
		cJSON_AddNumberToObject(vMatch,"s_validity",p->verCodeMatch.s_validity);
	}
	char *cjout=cJSON_Print(root);
	int cjLen=strlen(cjout);
	if (cjLen>outBufLen)
	{
		cjLen=outBufLen;
	}
	strncpy(outJson,cjout,cjLen);
	free(cjout);
	ZWDBG_INFO("%s\n",outJson);
	cJSON_Delete(root);	
}

void zwJclmsResultFromJson(const char *inJson,JCRESULT *p)
{	
	cJSON *root=cJSON_Parse(inJson); 
	assert(NULL!=root);
	cJSON *result = cJSON_GetObjectItem(root,"JCRESULT");   	
	assert(NULL!=result);
	cJSON *type=cJSON_GetObjectItem(result,"Type");   	
	assert(NULL!=type);
	if (NULL==type)
	{
		p->dynaCode=-1209;
		return;
	}
	JCLMSOP jcType=zwJclmsopFromString(type->valuestring);
	memset(p,0,sizeof(JCRESULT));
	switch(jcType)
	{
	case JCLMS_CCB_CODEGEN:
		p->dynaCode=cJSON_GetObjectItem(result,"dynaCode")->valueint;
		break;
	case JCLMS_CCB_CODEVERIFY:
		cJSON *vMatch=cJSON_GetObjectItem(result,"verCodeMatch");
		p->verCodeMatch.s_datetime=cJSON_GetObjectItem(vMatch,"s_datetime")->valueint;
		p->verCodeMatch.s_validity=cJSON_GetObjectItem(vMatch,"s_validity")->valueint;
		break;
	}
	cJSON_Delete(root);
}
