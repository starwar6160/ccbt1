#include "stdafx.h"
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include "LuaBridge.h"
#include "CCBelock.h"

using namespace luabridge;
class zwNumTest1
{
public:
	zwNumTest1();
	~zwNumTest1();
	int zwAdd(int a,int b);
private:
	int sum;
};

int zwNumTest1::zwAdd(int a,int b)
{
	return a+b;
}

namespace zwluat1{
	int s_count=0;
	int zwAdd(int a,int b)
	{
		return 100+a+b;
	}
	std::string zwMergeStr(const string &str1,const string &str2)
	{
		return "zwMergeStr_"+str1+str2;
	}
}

void myLuaBridgeTest1(void)
{
	printf("##################Start#################\n");
	lua_State* L = luaL_newstate();  //也可以用luaL_newState()函数  
	luaL_openlibs(L);   //注意将lua默认库打开，要不会出现N多错误的，比如print函数都没有  
	//将c++中的函数暴露给lua  
	getGlobalNamespace (L)
		.beginNamespace ("test1226")  
		.addFunction ("zwAdd", zwluat1::zwAdd)  
		.addFunction ("zwMergeStr", zwluat1::zwMergeStr)  		
		.addVariable ("s_count", &zwluat1::s_count)  
		.endNamespace ();  
	printf("zwluat1::s_count=%d\n",zwluat1::s_count);
	luaL_dofile(L, "E:\\zwWorkSrc\\zwBaseLib\\zwTest201407\\test1226.lua");  	
	//luaL_dofile(L, "E:\\zwWorkSrc\\zwBaseLib\\zwTest201407\\test107.lua");

	printf("zwluat1::s_count=%d\n",zwluat1::s_count);
	printf("##################End#################\n");
}

void myLuaBridgeSendJsonAPI(const char *luaScriptFile)
{
	printf("#################Start##################\n");
	lua_State* L = luaL_newstate();  //也可以用luaL_newState()函数  
	luaL_openlibs(L);   //注意将lua默认库打开，要不会出现N多错误的，比如print函数都没有  
	//将c++中的函数暴露给lua  
	getGlobalNamespace (L)
		.beginNamespace ("jclock")  
		.addFunction ("Open", jcLockJsonCmd_t2015a::OpenJson)
		.addFunction ("Close", jcLockJsonCmd_t2015a::CloseJson)
		.addFunction ("Send", jcLockJsonCmd_t2015a::SendToLockJson)
		.addFunction ("Recv", jcLockJsonCmd_t2015a::RecvFromLockJson)
		.endNamespace ();  	
	luaL_dofile(L, luaScriptFile);  	
	//lua_close(L);
	printf("#################End##################\n");
}