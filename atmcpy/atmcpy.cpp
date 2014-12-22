// atmcpy.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "atmcpy.h"
#include "zwSecretBoxCCBcsWrap.h"
#include "jclmsCCB2014.h"

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

#endif // _DEBUG1221


class jclock_data_input_ccb11 {
public:
	//固定因素部分
	std::string AtmNo;	//ATM号
	std::string LockNo;	//锁号
	std::string PSK;	//PSK，上下位机共同持有的唯一机密因素
	//可变因素部分
	int CodeGenDateTime;		//日期时间
	int Validity;		//有效期
	int CloseCode;	//闭锁码             	
	JCCMD CmdType;	//模式代码，比如开锁模式，远程重置模式，建行的流程要求的各种模式等等
	int SearchTimeStart;	//搜索时间起始点UTC秒数，默认值应该设定为当前时间
	/////////////////////以下为配置算法运作模式的数据，有默认值，一般不用改动///////////////////	
	//反推时间步长秒数，默认为在线模式，精度6秒，离线模式请自己设置为3600秒或者其他数值
	int SearchTimeStep;
	//往前反推的时间长度秒数，默认为在线模式，9分钟，值为540，其他值比如离线24小时请自己设置
	int SearchTimeLength;
	int getValItemNum(void);	//获取有效期数组元素个数N,索引从0到N-1
	int setValItem(const int index,const int validity);
private:
	//有效期，共有NUM_VALIDITY个,默认值是从5分钟到24小时那一系列，单位是分钟；可以自己设定
	//可以把最常用的有效期设置在更靠近开始处加快匹配速度
	int ValidityArray[NUM_VALIDITY];
};

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

	struct test1222{
		int code;
		std::string name;
	};

	class_<JcSecBox>("JcSecBox", init<>())
		//.def_readonly("name", &Var::name)
		.def("SecboxAuth",&JcSecBox::SecboxAuth)
		.def("SecboxWriteData",&JcSecBox::SecboxWriteData)
		.def("SecboxReadData",&JcSecBox::SecboxReadData);

	//2014/12/22 15:59:10 [星期一] 我刚发现，boost.python，无法在接口中的
	//结构体/类里面使用可读可写的数组类型成员，否则就会编译错误，解决方法
	//暂未找到；如果设定为只读，也可以。估计数字数组应该可以用get/set函数
	//读写内部的数组来替代之；而字符数组可以用std::string替代之；
	class_<JCINPUT>("JCINPUT", init<>())
	.def_readonly("AtmNo", &JCINPUT::AtmNo)
	.def_readonly("LockNo", &JCINPUT::LockNo)
	.def_readonly("PSK", &JCINPUT::PSK)
	.def_readwrite("CodeGenDateTime", &JCINPUT::CodeGenDateTime)
	.def_readwrite("Validity", &JCINPUT::Validity)
	.def_readwrite("CloseCode", &JCINPUT::CloseCode)
	.def_readwrite("CmdType", &JCINPUT::CmdType)
	.def_readwrite("SearchTimeStart", &JCINPUT::SearchTimeStart)
	.def_readwrite("SearchTimeStep", &JCINPUT::SearchTimeStep)
	.def_readwrite("SearchTimeLength", &JCINPUT::SearchTimeLength)
	.def_readonly("ValidityArray", &JCINPUT::ValidityArray)
	;

	class_<JCMATCH>("JCMATCH", init<>())
		//.def_readonly("name", &Var::name)
		.def_readwrite("s_datetime", &JCMATCH::s_datetime)
		.def_readwrite("s_validity", &JCMATCH::s_validity);

	class_<test1222>("test1222", init<>())
		//.def_readonly("name", &Var::name)
		.def_readwrite("s_datetime", &test1222::code)
		.def_readwrite("s_validity", &test1222::name);


}
