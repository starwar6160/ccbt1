#include "stdafx.h"
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include "LuaBridge.h"
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
	lua_State* L = luaL_newstate();  //Ҳ������luaL_newState()����  
	luaL_openlibs(L);   //ע�⽫luaĬ�Ͽ�򿪣�Ҫ�������N�����ģ�����print������û��  
	//��c++�еĺ�����¶��lua  
	getGlobalNamespace (L)
		.beginNamespace ("test1226")  
		.addFunction ("zwAdd", zwluat1::zwAdd)  
		.addFunction ("zwMergeStr", zwluat1::zwMergeStr)  		
		.addVariable ("s_count", &zwluat1::s_count)  
		.endNamespace ();  
	printf("zwluat1::s_count=%d\n",zwluat1::s_count);
	luaL_dofile(L, "test1226.lua");  
	printf("zwluat1::s_count=%d\n",zwluat1::s_count);
}