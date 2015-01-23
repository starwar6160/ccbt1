using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;

namespace ConsoleApplication1
{
    class Program
    {
        public delegate void ReturnMessage(string DrivesID, string DrivesMessage);

        [DllImport("HidProtocol.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern int ListDrives(string MyDrivesType);
        //设置回调的接口
        [DllImport("HidProtocol.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern int SetReturnMessage(ReturnMessage MyReturnMessage);
        //打开设备
        [DllImport("HidProtocol.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern int OpenDrives(string DrivesType, string DrivesID);
        //关闭设备
        [DllImport("HidProtocol.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern int CloseDrives(string DrivesType, string DrivesID);
        //发送获取设备列表的接口
        [DllImport("HidProtocol.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern int InputMessage(string DrivesType, string MyDrivesID, string AnyMessage);

        [DllImport("HidProtocol.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern void zjyTest116();
        

        public static void MyReturnMessage(string DrivesID, string DrivesMessage)
        {
            Console.WriteLine("回调内容-{0}", DrivesMessage);
        }

        //public ReturnMessage _ReturnMessage = new ReturnMessage(MyReturnMessage);

        static void Main(string[] args)
        {
            //ReturnMessage _ReturnMessage = new ReturnMessage(MyReturnMessage);
            //SetReturnMessage(_ReturnMessage);
            
            //ListDrives("Lock");
            string abcd = "";
            string _Send = "";

            OpenDrives("Lock", abcd);
            Console.WriteLine("打开设备");

                //_Send = @"{""command"":""Test_Motor_Open"",""cmd_id"":""1234567890"",""State"":""test""}";
                //InputMessage("Lock", abcd, _Send);
                //Console.WriteLine("发送内容-{0}", _Send);
                //Thread.Sleep(5000);

                //_Send = @"{""command"":""Test_Motor_Close"",""cmd_id"":""1234567890"",""State"":""test""}";
                //InputMessage("Lock", abcd, _Send);
                //Console.WriteLine("发送内容-{0}", _Send);
                //Thread.Sleep(5000);

                //_Send = @"{""command"":""Test_Motor_Open"",""cmd_id"":""1234567890"",""State"":""test""}";
                //InputMessage("Lock", abcd, _Send);
                //Console.WriteLine("发送内容-{0}", _Send);
                //Thread.Sleep(5000);

                //_Send = @"{""command"":""Test_Motor_Close"",""cmd_id"":""1234567890"",""State"":""test""}";
                //InputMessage("Lock", abcd, _Send);
                //Console.WriteLine("发送内容-{0}", _Send);
                //Thread.Sleep(5000);


            for(int i=0;i<50;++i )
            {
                string AnyString = "1234567890";
                _Send = @"{""command"":""Test_USB_HID"",""cmd_id"":""1234567890"",""input"":""" + AnyString + @""",""output"":""""}";
                InputMessage("Lock", abcd, _Send);
                Console.WriteLine(i.ToString() + "--发送内容-{0}", _Send);
                Thread.Sleep(1200);
            }


            CloseDrives("Lock", abcd);
            Console.WriteLine("关闭设备");
            
            Console.WriteLine("测试1完毕{0}", DateTime.Now.ToString());
            Console.ReadKey();

            //OpenDrives("Lock", abcd);

            //InputMessage("Lock", abcd, @"{""command"":""Test_Motor_Open"",""cmd_id"":""1234567890"",""State"":""test""}");

            //Thread.Sleep(5000);

            //InputMessage("Lock", abcd, @"{""command"":""Test_Motor_Close"",""cmd_id"":""1234567890"",""State"":""test""}");

            //Thread.Sleep(5000);

            //InputMessage("Lock", abcd, @"{""command"":""Test_Motor_Open"",""cmd_id"":""1234567890"",""State"":""test""}");

            //Thread.Sleep(5000);

            //InputMessage("Lock", abcd, @"{""command"":""Test_Motor_Close"",""cmd_id"":""1234567890"",""State"":""test""}");

            //Thread.Sleep(5000);

            //CloseDrives("Lock", abcd);

            //zjyTest116();

            //Console.WriteLine("测试2完毕{0}", DateTime.Now.ToString());
            //Console.ReadKey();

        }

        //获取指定时间对应的格林威治时间（距离1970年1月1日的秒数）
        static int GreenwichTime(DateTime AnyDateTime)
        {
            //传入当前时间的GMT(格林尼治时间)
            //DateTime jcdt = DateTime.Now.ToUniversalTime();
            DateTime jcdt = AnyDateTime.ToUniversalTime();
            //Console.Out.WriteLine("当前的格林尼治时间(GMT)是{0},建行1.1版本算法上下位机都统一采用GMT来计算减少混乱"
            //    , jcdt.ToString("yyyy MMdd HHmm ss"));
            //计算当前时间距离GMT的秒数
            DateTime dt = new DateTime(1970, 1, 1);
            TimeSpan dp = jcdt - dt;
            int seconddiff = (int)(dp.Ticks / 10000000);
            Console.Out.WriteLine("当前的GMT秒数是\t{0}", seconddiff);
            return seconddiff;
        }
    }
}
