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
            int lmsver = jclmsCCB2014.JcLockGetVersion();
            Console.Out.WriteLine("Jclms DLL Version is {0}", lmsver);

            //锁具的模拟对象
            int jcLock = 0;
            //上位机的模拟对象
            int jcSrv = 0;
            jcSrv= jclmsCCB2014.JcLockNew();
            jcLock= jclmsCCB2014.JcLockNew();

            //离线模式只不过是把时间追溯长度和步长相应调整；
            //const int ZWHOUR=3600;
            //jcSrv.m_stepoftime = ZWHOUR;
            //jcSrv.m_reverse_time_length = ZWHOUR * 25;
            //jcLock.m_stepoftime = ZWHOUR;
            //jcLock.m_reverse_time_length = ZWHOUR * 25;

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
            jclmsCCB2014.JcLockSetString(jcLock, JCITYPE.JCI_ATMNO, atmno);
            jclmsCCB2014.JcLockSetString(jcLock, JCITYPE.JCI_LOCKNO, lockno);
            jclmsCCB2014.JcLockSetString(jcLock, JCITYPE.JCI_PSK, psk);
            jclmsCCB2014.JcLockSetString(jcSrv, JCITYPE.JCI_ATMNO, atmno);
            jclmsCCB2014.JcLockSetString(jcSrv, JCITYPE.JCI_LOCKNO, lockno);
            jclmsCCB2014.JcLockSetString(jcSrv, JCITYPE.JCI_PSK, psk);

            //可变条件部分.
            jclmsCCB2014.JcLockSetInt(jcLock, JCITYPE.JCI_DATETIME, seconddiff);
            jclmsCCB2014.JcLockSetInt(jcLock, JCITYPE.JCI_VALIDITY, validity);
            jclmsCCB2014.JcLockSetInt(jcSrv, JCITYPE.JCI_DATETIME, seconddiff);
            jclmsCCB2014.JcLockSetInt(jcSrv, JCITYPE.JCI_VALIDITY, validity);

            //此处不同的命令码指示生成不同的动态码
            jclmsCCB2014.JcLockSetCmdType(jcLock, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_INIT_CLOSECODE);
            jclmsCCB2014.JcLockSetCmdType(jcSrv, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_INIT_CLOSECODE);
            //锁具产生初始闭锁码
            int firstCloseCode = jclmsCCB2014.JcLockGetDynaCode(jcLock);
            Console.Out.WriteLine("锁具产生的初始闭锁码是 {0}", firstCloseCode);
            //初始闭锁码输入到上位机DLL，其他条件已经准备好
            jclmsCCB2014.JcLockSetInt(jcSrv, JCITYPE.JCI_CLOSECODE, firstCloseCode);
            //获取第一开锁密码.注意，获得每一类动态码的方式都是这个调用，区别在于m_cmdtype
            //jcSrv.DebugPrint();
            jclmsCCB2014.JcLockSetCmdType(jcSrv, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_CCB_DYPASS1);
            int dyCode1 = jclmsCCB2014.JcLockGetDynaCode(jcSrv);
            Console.Out.WriteLine("上位机产生的第一开锁动态码是 {0}", dyCode1);
            //有问题请给我这个字符串
            Console.Out.WriteLine("动态码输入条件调试信息字符串是");
            jclmsCCB2014.JcLockDebugPrint(jcSrv);

            //锁具反推验证第一开锁动态码，
            jclmsCCB2014.JcLockSetInt(jcLock, JCITYPE.JCI_CLOSECODE, firstCloseCode);
            jclmsCCB2014.JcLockSetCmdType(jcLock, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_CCB_DYPASS1);
            JCMATCH pass1Match = jclmsCCB2014.JcLockReverseVerifyDynaCode(jcLock, dyCode1);
            if (pass1Match.s_datetime>0)
            {
                Console.Out.WriteLine("锁具对于第一开锁密码验证成功，证实了上位机的身份,匹配结果时间为{0}", pass1Match.s_datetime);
            }
            else
            {
                Console.Out.WriteLine("锁具对于第一开锁密码验证失败，上位机的身份是非法的");
                Environment.Exit(-1654);
            }
            //锁具生成验证码
            jclmsCCB2014.JcLockSetCmdType(jcLock, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_CCB_LOCK_VERCODE);
            //用第一开锁密码作为验证码的元素，以便适应建行的3个码环环相扣的要求
            jclmsCCB2014.JcLockSetInt(jcLock, JCITYPE.JCI_CLOSECODE, dyCode1);
            int lockVerifyCode = jclmsCCB2014.JcLockGetDynaCode(jcLock);
            Console.Out.WriteLine("锁具产生的验证码是 {0}", lockVerifyCode);
            jclmsCCB2014.JcLockSetCmdType(jcSrv, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_CCB_LOCK_VERCODE);
            //上位机也计算锁具应该返回的验证码的值，予以比对
            jclmsCCB2014.JcLockSetInt(jcSrv, JCITYPE.JCI_CLOSECODE, dyCode1);
            JCMATCH vercodeMatch = jclmsCCB2014.JcLockReverseVerifyDynaCode(jcSrv, lockVerifyCode);
            if (vercodeMatch.s_datetime>0)
            {
                Console.Out.WriteLine("上位机对于锁具应该返回的验证码验证成功，证实了锁具的身份,匹配结果时间为{0}",vercodeMatch.s_datetime);
            }
            else
            {
                Console.Out.WriteLine("上位机对于锁具应该返回的验证码验证失败，锁具的身份是非法的");
                Environment.Exit(-1739);
            }
            //上位机计算第二开锁码
            jclmsCCB2014.JcLockSetCmdType(jcSrv, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_CCB_DYPASS2);
            //锁具验证码作为第二开锁码的计算要素
            jclmsCCB2014.JcLockSetInt(jcSrv, JCITYPE.JCI_CLOSECODE, lockVerifyCode);
            int dyCode2 = jclmsCCB2014.JcLockGetDynaCode(jcSrv);
            Console.Out.WriteLine("上位机计算的第二开锁码是 {0}", dyCode2);
            jclmsCCB2014.JcLockSetCmdType(jcLock, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_CCB_DYPASS2);
            //锁具计算第二开锁码，以便于上位机传来的第二开锁码比对
            //锁具验证码作为第二开锁码的计算要素
            jclmsCCB2014.JcLockSetInt(jcLock, JCITYPE.JCI_CLOSECODE, lockVerifyCode);
            JCMATCH pass2Match = jclmsCCB2014.JcLockReverseVerifyDynaCode(jcLock, dyCode2);
            if (pass2Match.s_datetime>0)
            {
                Console.Out.WriteLine("锁具验证第二开锁码成功，开锁成功,匹配结果时间为{0}",pass2Match.s_datetime);
            }
            else
            {
                Console.Out.WriteLine("锁具验证第二开锁码失败，开锁失败");
            }
            DateTime dr = new DateTime(1970, 1, 1);              
            Console.Out.WriteLine("当前的北京时间匹配结果\t{0}",
                dr.AddSeconds(pass2Match.s_datetime).ToLocalTime().ToString());

            Console.Out.WriteLine("******************在线模式计算结束******************");
            jclmsCCB2014.JcLockDelete(jcLock);
            jclmsCCB2014.JcLockDelete(jcSrv);
        }


///////////////////////////////////////////////////////////////////////////////////////////
    }   //class Program
}   //namespace cstest702
