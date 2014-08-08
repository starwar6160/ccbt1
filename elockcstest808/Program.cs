using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;



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
            //string ty = P_function.TX1(TextBox6.Text);//生成XML 取得lock号
            string ty = "zwfakexmlzwfakexmlzwfakexmlzwfakexmlzwfakexmlzwfakexmlzwfakexmlzwfakexml";
            int t = SetRecvMsgRotine(dle1);
            Open(45);
            int y = 0;
            //for (Int64 i = 0; i <= 2000000000; i++)
            //{
            //    y = y + 1;
            //}

            Notify(ty);
        }
    }
}
