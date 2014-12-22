// atmcpy.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "atmcpy.h"
#include "zwSecretBoxCCBcsWrap.h"

#ifdef _DEBUG1221
typedef struct _zwstu20_t{
	int age;
	std::string name;
}EXPERSON;



char const* greet()                //定义模块中的函数
{
	return "hello, world";
}

int zwAdd(int a,int b)
{
	return 100+a+b;
}

std::string zwStrAdd(const std::string stra,const std::string strb)
{
	return stra+strb;
}

int dumpPerson(EXPERSON *p)
{
	printf("age=%d\tname=%s\n",p->age,p->name.c_str());
	p->age++;
	p->name+="TAIL";
	return p->age;
}

CCBELOCK_API class JcSecBox {
public:
	CCBELOCK_API JcSecBox();
	CCBELOCK_API ~ JcSecBox();
	CCBELOCK_API int SecboxAuth(void);
	CCBELOCK_API int SecboxWriteData(const int index,
		const char *dataB64);
	CCBELOCK_API const char *SecboxReadData(const int index);
private:
};
#endif // _DEBUG1221

//同一个模块下，内部可以导出多个类/结构体，函数等等；
BOOST_PYTHON_MODULE(atmcpy)      //定义导出的模块名atmcpy
{
	using namespace boost::python;
#ifdef _DEBUG1221
	//注意此处，导出给Python用的函数名可以不同于原始函数名
	def("greet", greet);
	def("zwAdd",zwAdd);
	def("zwStrAdd",zwStrAdd);
	def("dumpPerson",dumpPerson);
	//注意此处，init<>尖括号里面可以有初始化参数的类型列表
	//此外即便本来是可读可写的成员，也可以导出给Python只读的成员
	class_<EXPERSON>("EXPERSON", init<>())
		//.def_readonly("name", &Var::name)
		.def_readwrite("age", &EXPERSON::age)
		.def_readwrite("name", &EXPERSON::name);
#endif // _DEBUG1221

	class_<JcSecBox>("JcSecBox", init<>())
		//.def_readonly("name", &Var::name)
		.def("SecboxAuth",&JcSecBox::SecboxAuth)
		.def("SecboxWriteData",&JcSecBox::SecboxWriteData)
		.def("SecboxReadData",&JcSecBox::SecboxReadData)
		;
}
