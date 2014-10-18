using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using jclms;

namespace elockcstest808
{
    class Program
    {
        [DllImport("CCBelock.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern Int32 Open(Int32 szVersion);
        [DllImport("CCBelock.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern Int32 Close();
        [DllImport("CCBelock.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern Int32 Notify(string tuuuu);
        [DllImport("CCBelock.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern Int32 SetRecvMsgRotine(getxml inttr);
        public delegate string getxml(ref string xmls);

        public static string dle1(ref string xmls)
        {
            String sxml = xmls;
            return sxml;
        }


        static void Main(string[] args)
        {
            //myTest1();
            int hnd = jclms.ccbElock.SecboxOpen();
            jclms.ccbElock.SecboxSendAuthReq(hnd);
            int authResult= jclms.ccbElock.SecboxVerifyAuthRsp(hnd);
            if (authResult == 0)
            {
                Console.Out.WriteLine("Good Secret Box");
            }
            else
            {
                Console.Out.WriteLine("Fake Secret Box");
            }
            const String myLongB64Str1 = "emhvdXdlaXRlc3RPdXRwdXREZWJ1Z1N0cmluZ0FuZEppbkNodUVMb2NraW5kZXg9MFRvdGFsQmxvY2s9MkN1ckJsb2NrTGVuPTU4U2VkaW5nIERhdGEgQmxvY2sgIzBSZWNldmVkIERhdGEgRnJvbSBKQ0VMb2NrIGlzOg==";
            jclms.ccbElock.SecboxWriteData(hnd, 2, myLongB64Str1);
            String recvFromSecBox=jclms.ccbElock.SecboxReadData(hnd, 2);
            jclms.ccbElock.SecboxClose(hnd);

        }

        private static void myTest1()
        {
            //string ty = P_function.TX1(TextBox6.Text);//生成XML 取得lock号
            string ty = "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><TransCode>0000</TransCode><TransName>CallForActInfo</TransName><TransDate>20140807</TransDate><TransTime>173828</TransTime><DevCode>12345698Z</DevCode><SpareString1>zwtest808.1623</SpareString1><SpareString2>NULL</SpareString2></root>";
            int t = SetRecvMsgRotine(dle1);
            Open(45);
            int y = 0;
            Notify(ty);
        }
    }
}
