// atmcpy.cpp : ���� DLL Ӧ�ó���ĵ���������
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

#endif // _DEBUG1221


class jclock_data_input_ccb11 {
public:
	//�̶����ز���
	std::string AtmNo;	//ATM��
	std::string LockNo;	//����
	std::string PSK;	//PSK������λ����ͬ���е�Ψһ��������
	//�ɱ����ز���
	int CodeGenDateTime;		//����ʱ��
	int Validity;		//��Ч��
	int CloseCode;	//������             	
	JCCMD CmdType;	//ģʽ���룬���翪��ģʽ��Զ������ģʽ�����е�����Ҫ��ĸ���ģʽ�ȵ�
	int SearchTimeStart;	//����ʱ����ʼ��UTC������Ĭ��ֵӦ���趨Ϊ��ǰʱ��
	/////////////////////����Ϊ�����㷨����ģʽ�����ݣ���Ĭ��ֵ��һ�㲻�øĶ�///////////////////	
	//����ʱ�䲽��������Ĭ��Ϊ����ģʽ������6�룬����ģʽ���Լ�����Ϊ3600�����������ֵ
	int SearchTimeStep;
	//��ǰ���Ƶ�ʱ�䳤��������Ĭ��Ϊ����ģʽ��9���ӣ�ֵΪ540������ֵ��������24Сʱ���Լ�����
	int SearchTimeLength;
	int getValItemNum(void);	//��ȡ��Ч������Ԫ�ظ���N,������0��N-1
	int setValItem(const int index,const int validity);
private:
	//��Ч�ڣ�����NUM_VALIDITY��,Ĭ��ֵ�Ǵ�5���ӵ�24Сʱ��һϵ�У���λ�Ƿ��ӣ������Լ��趨
	//���԰���õ���Ч�������ڸ�������ʼ���ӿ�ƥ���ٶ�
	int ValidityArray[NUM_VALIDITY];
};

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

	struct test1222{
		int code;
		std::string name;
	};

	class_<JcSecBox>("JcSecBox", init<>())
		//.def_readonly("name", &Var::name)
		.def("SecboxAuth",&JcSecBox::SecboxAuth)
		.def("SecboxWriteData",&JcSecBox::SecboxWriteData)
		.def("SecboxReadData",&JcSecBox::SecboxReadData);

	//2014/12/22 15:59:10 [����һ] �Ҹշ��֣�boost.python���޷��ڽӿ��е�
	//�ṹ��/������ʹ�ÿɶ���д���������ͳ�Ա������ͻ������󣬽������
	//��δ�ҵ�������趨Ϊֻ����Ҳ���ԡ�������������Ӧ�ÿ�����get/set����
	//��д�ڲ������������֮�����ַ����������std::string���֮��
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
