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

        }

        private static int myInitCloseCodeTest1(JcLockInput myLock, int dyCode)
        {
            myLock.m_atmno = "atmno1";
            myLock.m_lockno = "lockno1";
            myLock.m_psk = "mypskexample1";
            myLock.m_datetime = 1400887765;
            myLock.m_validity = 241;
            myLock.m_closecode = 87654322;
            myLock.m_cmdtype = JCCMD.JCCMD_INIT_CLOSECODE;
            myLock.DebugPrint();
            dyCode = jclmsCCB2014.zwGetDynaCode(myLock);
            Console.Out.WriteLine(myLock);
            Console.Out.WriteLine("DynaCode={0}", dyCode);
            JCERROR err = jclmsCCB2014.zwVerifyDynaCode(myLock, dyCode);
            if (err == JCERROR.EJC_SUSSESS)
            {
                Console.Out.WriteLine("verify Success");
            }
            else
            {
                Console.Out.WriteLine("verify Fail");
            }
            return dyCode;
        }
    }
}
