// atmcpy.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "atmcpy.h"


//// 这是导出变量的一个示例
//ATMCPY_API int natmcpy=0;
//
//// 这是导出函数的一个示例。
//ATMCPY_API int fnatmcpy(void)
//{
//	return 42;
//}
//
//// 这是已导出类的构造函数。
//// 有关类定义的信息，请参阅 atmcpy.h
//Catmcpy::Catmcpy()
//{
//	return;
//}


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

//同一个模块下，内部可以导出多个类/结构体，函数等等；
BOOST_PYTHON_MODULE(atmcpy)      //定义导出的模块名atmcpy
{
	//注意此处，导出给Python用的函数名可以不同于原始函数名
	using namespace boost::python;
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
}
