using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using jclms;

namespace cstest702
{
    class Program
    {
        static void Main(string[] args)
        {
            JcLockInput myLock=new JcLockInput();
            int dyCode = 0;
            dyCode = myInitCloseCodeTest1(myLock, dyCode);
            int lmsver = jclmsCCB2014.getVersion();
            Console.Out.WriteLine("Jclms DLL Version is {0}", lmsver);

        }

        private static int myInitCloseCodeTest1(JcLockInput myLock, int dyCode)
        {
            //填写完整JcLockInput结构体中的各个项目
            myLock.m_atmno = "atmno1";
            myLock.m_lockno = "lockno1";
            myLock.m_psk = "mypskexample1";
            myLock.m_datetime = 1400887765;
            myLock.m_validity = 241;
            myLock.m_closecode = 87654322;
            //要生成哪一类动态码，请看JCCMD的定义
            myLock.m_cmdtype = JCCMD.JCCMD_INIT_CLOSECODE;
            myLock.DebugPrint();
            dyCode = jclmsCCB2014.zwGetDynaCode(myLock);
            Console.Out.WriteLine("InitCloseCode={0}", dyCode);
            //验证动态码，同样填写完毕各项输入要素，然后把结构体连同动态码传入
            //返回值只有成功或者失败；请不要依赖于具体值，而是要用枚举量，
            //因为具体值随着枚举量的变化可能变化，而某个枚举符号的含义是不会变化的
            JCERROR err = jclmsCCB2014.zwVerifyDynaCode(myLock, dyCode);
            if (err == JCERROR.EJC_SUSSESS)
            {
                Console.Out.WriteLine("InitCloseCode verify Success");
            }
            else
            {
                Console.Out.WriteLine("InitCloseCode verify Fail");
            }
            return dyCode;
        }
    }
}
