using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using jclms;

namespace cstest702
{
    class Program
    {

        const String atmno = "atm10455761";
        const String lockno = "lock14771509";
        const String psk = "jclmsdemopsk201407071509aajclmsdemopsk201407071509";
        const Int32 validity = 5;

        static void Main(string[] args)
        {
            //安全初始化例子
            myECIEStest();
            //建行1.1版本动态码验证流程例子
            myV11DynaCodeTest();
            //建行1.1版本动态码NFC离线应急模式验证流程例子
            myV11DynaOfflineCodeTest();
        }

        //安全初始化例子
        private static void myECIEStest()
        {
            //生成ECIES公钥/私钥对，返回保存密钥对等等的内部数据结构句柄
            //1为模拟的上位机，2为模拟的下位机
            int hec = jclmsCCB2014.EciesGenKeyPair();
            //从句柄所指向的内部数据结构获取前面生成好的公钥和私钥，是Base64格式字符串，
            //不必理解其含义，原样透传即可，把公钥发给对方，私钥保存在断电后不丢失的
            //存储器中比如磁盘或者FLASH中
            String ecPub = jclmsCCB2014.EciesGetPubKey(hec);
            String ecPri = jclmsCCB2014.EciesGetPriKey(hec);
            Console.Out.WriteLine("CCB 1.1版本算法ECIES(椭圆曲线集成加密公钥算法)安全初始化演示开始");
            Console.Out.WriteLine("ECIES PubKey=\t{0},", ecPub);
            Console.Out.WriteLine("ECIES Prikey=\t{0}", ecPri);
            //删除保存密钥对等等的内部数据结构.实践中密钥对生成只用做一次，以后就是            
            //保存下来重复利用了；这里每次都生成新的公钥/私钥对，是因为测试程序的缘故
            jclmsCCB2014.EciesDelete(hec);

            String plainText = "myplaintext20140717.0918.012myplaintext20140717.0918.012end920;AAABBB";  //明文
            //用对方的公钥加密后发给对方
            String cryptText = jclmsCCB2014.EciesEncrypt(ecPub, plainText);
            //对方使用自己的私钥解密，还原出来明文
            String decryptText = jclmsCCB2014.EciesDecrypt(ecPri, cryptText);
            Console.Out.WriteLine("PlainText1:\t{0}", plainText);
            Console.Out.WriteLine("cryptText:\t{0}", cryptText);
            Console.Out.WriteLine("decryptText:\t{0}", decryptText);
            Console.Out.WriteLine("*************************");
            String mypsk = jclmsCCB2014.zwMergePsk("testpsk1");
            Console.Out.WriteLine("mypsk:\t{0}", mypsk);
        }

        //建行1.1版本动态码验证流程例子
        private static void myV11DynaCodeTest()
        {
            Console.Out.WriteLine("******************在线模式计算开始******************");
            //以后每次算法如果有了不兼容的修改，或者出一个正式版本，
            //都会有一个版本号，就是一个整数，前8位是日期，最后一位是次版本号
            //一般为0，除非一天之内出了超过1个版本；有问题请先给我版本号；
            int lmsver = jclmsCCB2014.getVersion();
            Console.Out.WriteLine("Jclms DLL Version is {0}", lmsver);

            //锁具的模拟对象
            JcLockInput jcLock = new JcLockInput();
            //上位机的模拟对象
            JcLockInput jcSrv = new JcLockInput();

            //在此我特地用了普通的字符串，用意在于，这些字符串的字段内容是什么都可以，
            //长度多长都可以,因为内部使用的C++的String，对于长度没有限制，只受内存大小限制；
            //从几个字节到几百字节乃至于更长都可以，只要内存足够
            //当然实践中建议限制在100字节以内
            //传入当前时间的GMT(格林尼治时间)
            DateTime jcdt = DateTime.Now.ToUniversalTime();
            Console.Out.WriteLine("当前的格林尼治时间(GMT)是{0},建行1.1版本算法上下位机都统一采用GMT来计算减少混乱"
                , jcdt.ToString("yyyy MMdd HHmm ss"));
            //计算当前时间距离GMT的秒数
            DateTime dt = new DateTime(1970, 1, 1);
            TimeSpan dp = jcdt - dt;
            int seconddiff = (int)(dp.Ticks / 10000000);
            Console.Out.WriteLine("当前的GMT秒数是\t{0}", seconddiff);

            //锁具和上位机填入相同的初始条件，暂时替代初始化过程
            //固定条件部分
            jcLock.m_atmno = atmno;
            jcSrv.m_atmno = atmno;
            jcLock.m_lockno = lockno;
            jcSrv.m_lockno = lockno;
            jcLock.m_psk = psk;
            jcSrv.m_psk = psk;
            //可变条件部分
            jcLock.m_datetime = seconddiff;
            jcSrv.m_datetime = seconddiff;
            jcLock.m_validity = validity;
            jcSrv.m_validity = validity;
            jcLock.m_closecode = 0;
            jcSrv.m_closecode = 0;
            //此处不同的命令码指示生成不同的动态码
            jcLock.m_cmdtype = JCCMD.JCCMD_INIT_CLOSECODE;
            jcSrv.m_cmdtype = JCCMD.JCCMD_INIT_CLOSECODE;
            //有问题请给我这个字符串
            Console.Out.WriteLine("动态码输入条件调试信息字符串是");
            jcLock.DebugPrint();
            //锁具产生初始闭锁码
            int firstCloseCode = jclmsCCB2014.zwGetDynaCode(jcLock);
            Console.Out.WriteLine("锁具产生的初始闭锁码是 {0}", firstCloseCode);
            //初始闭锁码输入到上位机DLL，其他条件已经准备好
            jcSrv.m_closecode = firstCloseCode;
            //获取第一开锁密码.注意，获得每一类动态码的方式都是这个调用，区别在于m_cmdtype
            //jcSrv.DebugPrint();
            jcSrv.m_cmdtype = JCCMD.JCCMD_CCB_DYPASS1;
            int dyCode1 = jclmsCCB2014.zwGetDynaCode(jcSrv);
            Console.Out.WriteLine("上位机产生的第一开锁动态码是 {0}", dyCode1);

            //锁具验证第一开锁动态码，实质上是在下位机把该动态码再次计算一次
            jcLock.m_closecode = firstCloseCode;
            jcLock.m_cmdtype = JCCMD.JCCMD_CCB_DYPASS1;
            int dyCode1Verify = jclmsCCB2014.zwGetDynaCode(jcLock);
            if (dyCode1 == dyCode1Verify)
            {
                Console.Out.WriteLine("锁具对于第一开锁密码验证成功，证实了上位机的身份");
            }
            else
            {
                Console.Out.WriteLine("锁具对于第一开锁密码验证失败，上位机的身份是非法的");
                Environment.Exit(-1654);
            }
            //锁具生成验证码
            jcLock.m_cmdtype = JCCMD.JCCMD_CCB_LOCK_VERCODE;
            //用第一开锁密码作为验证码的元素，以便适应建行的3个码环环相扣的要求
            jcLock.m_closecode = dyCode1;   
            int lockVerifyCode = jclmsCCB2014.zwGetDynaCode(jcLock);
            Console.Out.WriteLine("锁具产生的验证码是 {0}", lockVerifyCode);
            jcSrv.m_cmdtype = JCCMD.JCCMD_CCB_LOCK_VERCODE;
            //上位机也计算锁具应该返回的验证码的值，予以比对
            jcSrv.m_closecode = dyCode1;
            int srvLockVerCode = jclmsCCB2014.zwGetDynaCode(jcSrv);
            if (lockVerifyCode == srvLockVerCode)
            {
                Console.Out.WriteLine("上位机对于锁具应该返回的验证码验证成功，证实了锁具的身份");
            }
            else
            {
                Console.Out.WriteLine("上位机对于锁具应该返回的验证码验证失败，锁具的身份是非法的");
                Environment.Exit(-1739);
            }
            //上位机计算第二开锁码
            jcSrv.m_cmdtype = JCCMD.JCCMD_CCB_DYPASS2;
            jcSrv.m_closecode = lockVerifyCode; //锁具验证码作为第二开锁码的计算要素
            int dyCode2 = jclmsCCB2014.zwGetDynaCode(jcSrv);
            Console.Out.WriteLine("上位机计算的第二开锁码是 {0}", dyCode2);
            jcLock.m_cmdtype = JCCMD.JCCMD_CCB_DYPASS2;
            //锁具计算第二开锁码，以便于上位机传来的第二开锁码比对
            jcLock.m_closecode = lockVerifyCode;//锁具验证码作为第二开锁码的计算要素
            int dyCode2Verify = jclmsCCB2014.zwGetDynaCode(jcLock);
            if (dyCode2 == dyCode2Verify)
            {
                Console.Out.WriteLine("锁具验证第二开锁码成功，开锁成功");
            }
            else
            {
                Console.Out.WriteLine("锁具验证第二开锁码失败，开锁失败");
            }
            Console.Out.WriteLine("******************在线模式计算结束******************");
        }

        //建行1.1版本动态码验证流程例子
        private static void myV11DynaOfflineCodeTest()
        {
            Console.Out.WriteLine("******************离线应急模式计算开始******************");
            //以后每次算法如果有了不兼容的修改，或者出一个正式版本，
            //都会有一个版本号，就是一个整数，前8位是日期，最后一位是次版本号
            //一般为0，除非一天之内出了超过1个版本；有问题请先给我版本号；
            int lmsver = jclmsCCB2014.getVersion();
            Console.Out.WriteLine("Jclms DLL Version is {0}", lmsver);

            //锁具的模拟对象
            JcLockInput jcLock = new JcLockInput();
            //上位机的模拟对象
            JcLockInput jcSrv = new JcLockInput();

            //在此我特地用了普通的字符串，用意在于，这些字符串的字段内容是什么都可以，长度多长都可以
            //因为内部使用的C++的String，对于长度没有限制，只受内存大小限制；从几个字节
            //到几百字节乃至于更长都可以，只要内存足够，当然实践中建议限制在100字节以内
            //传入当前时间的GMT(格林尼治时间)
            DateTime jcdt = DateTime.Now.ToUniversalTime();
            Console.Out.WriteLine("当前的格林尼治时间(GMT)是{0},建行1.1版本算法上下位机都统一采用GMT来计算减少混乱"
                , jcdt.ToString("yyyy MMdd HHmm ss"));
            //计算当前时间距离GMT的秒数
            DateTime dt = new DateTime(1970, 1, 1);
            TimeSpan dp = jcdt - dt;
            int seconddiff = (int)(dp.Ticks / 10000000);
            Console.Out.WriteLine("当前的GMT秒数是\t{0}", seconddiff);
            int tail = seconddiff % 60;
            seconddiff -= tail;
            Console.Out.WriteLine("当前的GMT秒数规格化到整点是\t{0}", seconddiff);
            //锁具和上位机填入相同的初始条件，暂时替代初始化过程
            //固定条件部分
            jcLock.m_atmno = atmno;
            jcSrv.m_atmno = atmno;
            jcLock.m_lockno = lockno;
            jcSrv.m_lockno = lockno;
            jcLock.m_psk = psk;
            jcSrv.m_psk = psk;
            //可变条件部分
            jcLock.m_datetime = seconddiff;
            jcSrv.m_datetime = seconddiff;
            jcLock.m_validity = validity;
            jcSrv.m_validity = validity;
            jcLock.m_closecode = 0;
            jcSrv.m_closecode = 0;
            //jcLock.m_reverse_time_length=
            //此处不同的命令码指示生成不同的动态码
            jcLock.m_cmdtype = JCCMD.JCCMD_INIT_CLOSECODE;
            jcSrv.m_cmdtype = JCCMD.JCCMD_INIT_CLOSECODE;
            //有问题请给我这个字符串
            Console.Out.WriteLine("动态码输入条件调试信息字符串是");
            jcLock.DebugPrint();
            //锁具产生初始闭锁码
            int firstCloseCode = jclmsCCB2014.zwGetDynaCode(jcLock);
            Console.Out.WriteLine("锁具产生的初始闭锁码是 {0}", firstCloseCode);
            //初始闭锁码输入到上位机DLL，其他条件已经准备好
            jcSrv.m_closecode = firstCloseCode;
            //获取第一开锁密码.注意，获得每一类动态码的方式都是这个调用，区别在于m_cmdtype
            //jcSrv.DebugPrint();
            jcSrv.m_cmdtype = JCCMD.JCCMD_CCB_DYPASS1;
            int dyCode1 = jclmsCCB2014.zwGetDynaCode(jcSrv);
            Console.Out.WriteLine("上位机产生的第一开锁动态码是 {0}", dyCode1);

            //jcSrv.SetValidity(2, 17);
            JCMATCH jcoret= jclmsCCB2014.zwReverseVerifyDynaCode(jcSrv, dyCode1);
            Console.Out.WriteLine("离线匹配的时间(GMT)和有效期(分钟)是 {0},\t{1}",
                jcoret.s_datetime, jcoret.s_validity);
            if (jcoret.s_datetime == 0)
            {
                Console.Out.WriteLine("离线匹配失败，无法找到前24小时以内的，4小时到24小时之间有效期的匹配");
            }
            //请接下来把jcoret.s_datetime转化为北京时间给人看
            //因为是离线应急模式，所以没有在线模式的来回3趟反复互相验证，就此一趟就计算结束了
            //计算当前时间距离GMT的秒数
            DateTime dr = new DateTime(1970, 1, 1);            
            ;
            Console.Out.WriteLine("当前的北京时间匹配结果\t{0}", 
                dr.AddSeconds(jcoret.s_datetime).ToLocalTime().ToString());
            Console.Out.WriteLine("因为是离线应急模式，所以没有在线模式的来回3趟反复互相验证，就此一趟就计算结束了");
            Console.Out.WriteLine("******************离线应急模式计算结束******************");
        }


        private static int myInitCloseCodeTest1(JcLockInput myLock, int dyCode)
        {
            //填写完整JcLockInput结构体中的各个项目
            myLock.m_atmno = "atmno1";
            myLock.m_lockno = "lockno1";
            myLock.m_psk = "mypskexample1";
            myLock.m_datetime = 1400887765;
            myLock.m_validity = 241;
            myLock.m_closecode = 87654321;
            //要生成哪一类动态码，请看JCCMD的定义
            myLock.m_cmdtype = JCCMD.JCCMD_INIT_CLOSECODE;
            myLock.DebugPrint();
            dyCode = jclmsCCB2014.zwGetDynaCode(myLock);
            Console.Out.WriteLine("InitCloseCode={0}", dyCode);
            //验证动态码，同样填写完毕各项输入要素，然后把结构体连同动态码传入
            //返回值只有成功或者失败；请不要依赖于具体值，而是要用枚举量，
            //因为具体值随着枚举量的变化可能变化，而某个枚举符号的含义是不会变化的
            JCERROR err = JCERROR.EJC_FAIL;
                //jclmsCCB2014.zwVerifyDynaCode(myLock, dyCode);
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
