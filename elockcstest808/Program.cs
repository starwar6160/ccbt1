using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;


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
            const int ZWPAUSE = 1500;
            //声明一个密盒对象；使用该对象的3个方法来认证，读取，写入，至于Open/Close由该对象内部自动完成；            

            //secTest2();
            myTest1();
        }


        private static void myTest1()
        {
            //string ty = P_function.TX1(TextBox6.Text);//生成XML 取得lock号
            string ty = "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><TransCode>0000</TransCode><TransName>CallForActInfo</TransName><TransDate>20140807</TransDate><TransTime>173828</TransTime><DevCode>12345698Z</DevCode><SpareString1>zwtest808.1623</SpareString1><SpareString2>NULL</SpareString2></root>";
            int t = SetRecvMsgRotine(dle1);
            Open(45);
            Notify(ty);
        }
    }
}
