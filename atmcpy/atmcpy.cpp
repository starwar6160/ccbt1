// atmcpy.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "atmcpy.h"
#include "zwSecretBoxCCBcsWrap.h"

#ifdef _DEBUG1221
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

//ͬһ��ģ���£��ڲ����Ե��������/�ṹ�壬�����ȵȣ�
BOOST_PYTHON_MODULE(atmcpy)      //���嵼����ģ����atmcpy
{
	using namespace boost::python;
#ifdef _DEBUG1221
	//ע��˴���������Python�õĺ��������Բ�ͬ��ԭʼ������
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
#endif // _DEBUG1221

	class_<JcSecBox>("JcSecBox", init<>())
		//.def_readonly("name", &Var::name)
		.def("SecboxAuth",&JcSecBox::SecboxAuth)
		.def("SecboxWriteData",&JcSecBox::SecboxWriteData)
		.def("SecboxReadData",&JcSecBox::SecboxReadData)
		;
}
