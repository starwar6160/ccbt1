#include "stdafx.h"
#include "python.h"
//假设c库中有一个设置回调函数接口：
void set_callback_fun(void(*fun)(int, int , int));
//下面是c扩展库中要添加的代码：
//全局变量，保存Python中要回调的可调用对象。
static PyObject*gCallbackFun = NULL;
//调用上面函数设置的python脚本函数
//Python可调用对象的转换函数，转化为C的调用方式

static void callbacfun(int type,int chn,int dataType)
{
PyObject* pArgs = NULL;
PyObject* pRetVal = NULL;
int   nRetVal = 0;
pArgs = Py_BuildValue("(i, i,i)", type, chn, dataType);//将c的参数转化为Python的参数对象
pRetVal = PyEval_CallObject(gCallbackFun,pArgs);//调用Python的可调用对象。
Py_DECREF(pArgs);
Py_DECREF(pRetVal);
}

/// set_callback_fun函数的包装函数

static PyObject* wrap_set_callback_fun(PyObject *dummy, PyObject *args)
{
	PyObject *temp = NULL;
	if (PyArg_ParseTuple(args, "O:set_callback_fun",&temp)) {//获取Python对象
	if (!PyCallable_Check(temp)) {//检查对象是否可以调用
		PyErr_SetString(PyExc_TypeError,"parameter must be callable");
		Py_XINCREF(temp);         /* Add a reference to new callback */
		Py_XDECREF(gCallbackFun); /* Dispose ofprevious callback */
		gCallbackFun = temp;       /* 保存回调对象 */
		set_callback_fun(callbacfun);//注意，这里掉一下包，用一个C的函数注册到c库中。
		return Py_BuildValue("i",(gCallbackFun == NULL) ? 0 : 1);
	}
}
}