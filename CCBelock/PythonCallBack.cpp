#include "stdafx.h"
#include "python.h"
//����c������һ�����ûص������ӿڣ�
void set_callback_fun(void(*fun)(int, int , int));
//������c��չ����Ҫ��ӵĴ��룺
//ȫ�ֱ���������Python��Ҫ�ص��Ŀɵ��ö���
static PyObject*gCallbackFun = NULL;
//�������溯�����õ�python�ű�����
//Python�ɵ��ö����ת��������ת��ΪC�ĵ��÷�ʽ

static void callbacfun(int type,int chn,int dataType)
{
PyObject* pArgs = NULL;
PyObject* pRetVal = NULL;
int   nRetVal = 0;
pArgs = Py_BuildValue("(i, i,i)", type, chn, dataType);//��c�Ĳ���ת��ΪPython�Ĳ�������
pRetVal = PyEval_CallObject(gCallbackFun,pArgs);//����Python�Ŀɵ��ö���
Py_DECREF(pArgs);
Py_DECREF(pRetVal);
}

/// set_callback_fun�����İ�װ����

static PyObject* wrap_set_callback_fun(PyObject *dummy, PyObject *args)
{
	PyObject *temp = NULL;
	if (PyArg_ParseTuple(args, "O:set_callback_fun",&temp)) {//��ȡPython����
	if (!PyCallable_Check(temp)) {//�������Ƿ���Ե���
		PyErr_SetString(PyExc_TypeError,"parameter must be callable");
		Py_XINCREF(temp);         /* Add a reference to new callback */
		Py_XDECREF(gCallbackFun); /* Dispose ofprevious callback */
		gCallbackFun = temp;       /* ����ص����� */
		set_callback_fun(callbacfun);//ע�⣬�����һ�°�����һ��C�ĺ���ע�ᵽc���С�
		return Py_BuildValue("i",(gCallbackFun == NULL) ? 0 : 1);
	}
}
}