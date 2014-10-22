﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;
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
            //const int ZWPAUSE = 1500;
            //声明一个密盒对象；使用该对象的3个方法来认证，读取，写入，至于Open/Close由该对象内部自动完成；            
            
            for (int i = 0; i < 5; i++)
            //while(true)
            {
                jclms.JcSecBox secBox = new JcSecBox();
                Console.Out.WriteLine("Secret Box Open###########################################################");
                //打开密盒
                //System.Threading.Thread.Sleep(ZWPAUSE*2);
                int status =
                    secBox.SecboxAuth();

                if (0==status)
                {
                    Console.Out.WriteLine("Good Secret Box");
                }
                if (1==status)
                {
                    Console.Out.WriteLine("Fake Secret Box");
                    continue;
                }
                //////////////////////////////////////////////////////////
                //随便用一段比较长的文字经过base64编码形成的下面这段有待写入的base64数据
                //实践中，可以用二进制数据编码之后成为base64字符串写入；
                //第二个参数是索引号，大致上是0到10左右，具体还得和赵工确认
                //第三个参数，也就是数据，大体上可以达到最大400多个字节，具体多少还得和赵工确认
                const String myLongB64Str1 = "emhvdXdlaXRlc3RPdXRwdXREZWJ1Z1N0cmluZ0FuZEppbkNodUVMb2NraW5kZXg9MFRvdGFsQmxvY2s9MkN1ckJsb2NrTGVuPTU4U2VkaW5nIERhdGEgQmxvY2sgIzBSZWNldmVkIERhdGEgRnJvbSBKQ0VMb2NrIGlzOg==";
                //通过句柄，索引号，读取密盒数据，返回的也是Base64编码过的字符串，解码后可能是文本，也可能是二进制数据
                //Console.Out.WriteLine("Secret Box WriteData");
                secBox.SecboxWriteData(1, myLongB64Str1);
                //Console.Out.WriteLine("Secret Box ReadData");
                String recvFromSecBox = secBox.SecboxReadData(1);

                //secBox.jcClose();
            }
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
