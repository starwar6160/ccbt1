using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

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
            aa.m_datetime = 1400887766;
            aa.m_validity = 240;
            aa.m_closecode = 87654321;
            aa.m_cmdtype = 0;
            aa.DebugPrint();
        }
    }
}
