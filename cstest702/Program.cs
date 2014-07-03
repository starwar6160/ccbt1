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
            JcLockInput aa=new JcLockInput();
            aa.m_atmno = "atmno1";
            aa.m_lockno = "lockno1";
            aa.m_psk = "mypskexample1";
            aa.m_datetime = 1400887765;
            aa.m_validity = 241;
            aa.m_closecode = 87654322;
            aa.m_cmdtype = JCCMD.JCCMD_INIT_CLOSECODE;
            aa.DebugPrint();
            int dyCode=jclmsCCB2014.zwGetDynaCode(aa);
            Console.Out.WriteLine(aa);
            Console.Out.WriteLine("DynaCode={0}", dyCode);
            JCERROR err = jclmsCCB2014.zwVerifyDynaCode(aa, dyCode);
            if (err == JCERROR.EJC_SUSSESS)
            {
                Console.Out.WriteLine("verify Success");
            }
            else
            {
                Console.Out.WriteLine("verify Fail");
            }

        }
    }
}
