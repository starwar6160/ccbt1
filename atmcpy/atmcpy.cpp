// atmcpy.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "atmcpy.h"


//// ���ǵ���������һ��ʾ��
//ATMCPY_API int natmcpy=0;
//
//// ���ǵ���������һ��ʾ����
//ATMCPY_API int fnatmcpy(void)
//{
//	return 42;
//}
//
//// �����ѵ�����Ĺ��캯����
//// �й��ඨ�����Ϣ������� atmcpy.h
//Catmcpy::Catmcpy()
//{
//	return;
//}


typedef struct _zwstu20_t{
	int age;
	std::string name;
}EXPERSON;



char const* greet()                //����ģ���еĺ���
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

//ͬһ��ģ���£��ڲ����Ե��������/�ṹ�壬�����ȵȣ�
BOOST_PYTHON_MODULE(atmcpy)      //���嵼����ģ����atmcpy
{
	//ע��˴���������Python�õĺ��������Բ�ͬ��ԭʼ������
	using namespace boost::python;
	def("greet", greet);
	def("zwAdd",zwAdd);
	def("zwStrAdd",zwStrAdd);
	def("dumpPerson",dumpPerson);
	//ע��˴���init<>��������������г�ʼ�������������б�
	//���⼴�㱾���ǿɶ���д�ĳ�Ա��Ҳ���Ե�����Pythonֻ���ĳ�Ա
	class_<EXPERSON>("EXPERSON", init<>())
		//.def_readonly("name", &Var::name)
		.def_readwrite("age", &EXPERSON::age)
		.def_readwrite("name", &EXPERSON::name);
}
