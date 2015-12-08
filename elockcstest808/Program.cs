using System;
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


        static void Main(string[] args)
        {
            //sexBoxTestMain1();
            ZJELOCKLib.ZJELock zjOcx=new ZJELOCKLib.ZJELock();
            zjOcx.Open(22);
            zjOcx.Close();

        }

        private static void sexBoxTestMain1()
        {
            const int ZWPAUSE = 1500;
            //声明一个密盒对象；使用该对象的3个方法来认证，读取，写入，至于Open/Close由该对象内部自动完成；            

            //secTest2();
            //myTest1();
            myJcLockJsonTest20150107();
        }
        public static void myJcLockJsonTest20150107()
        {
            int hidhnd=HidProtocol.OpenJson(25);
            HidProtocol.SendToLockJson("{ \"command\": \"Lock_System_Journal\",\"State\": \"get\"}");            
            String outJson = HidProtocol.RecvFromLockJson(2500);
            Console.Out.WriteLine(outJson);
            HidProtocol.CloseJson();
        }


    }
}
