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
        const String psk = "PSKDEMO728";
        const Int32 validity = 5;
        const int ZW_FAKE_PASS1 = 11111111;

        static void Main(string[] args)
        {
            //安全初始化例子
            //myECIEStest();
            //String aa = Console.ReadLine();
            //myECIEStest2();
            //建行1.1版本动态码验证流程例子
            //myV11DynaCodeTest();
            //myV11DynaCodeTestKeyBoardInput();
            jclms.JcSecBox secBox = new JcSecBox();
            secBox.CloseHid();
            int handle = jclmsCCB2014.JcLockNew();
            jclmsCCB2014.JcLockDebugPrint(handle);
            int myDyCodePass1 = jclmsCCB2014.csJclmsReqGenDyCode(handle);
            return;

            mySecBoxTest1221();                        
            tmyLmsReq2SecBoxEx20141221GenPass1DyCode();
            return;

            //初始闭锁码生成
            myLmsReq2SecBoxEx20141212GenInitCloseCode();
            //第一开锁码生成
            myLmsReq2SecBoxEx20141212GenPass1DyCode();
            return;
            //第一开锁码验证
            myLmsReq2SecBoxEx20141212VerifyPass1DyCode();
            //验证码生成
            myLmsReq2SecBoxEx20141218GenVerifyCode();
            //验证码验证
            myLmsReq2SecBoxEx20141218VerifyVerifyCode();
            //第二开锁码生成
            myLmsReq2SecBoxEx20141218GenPass2DyCode();
            //第二开锁码验证
            myLmsReq2SecBoxEx20141218VerifyPass2DyCode();
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
            //String ecPub = jclmsCCB2014.EciesGetPubKey(hec);
            //String ecPri = jclmsCCB2014.EciesGetPriKey(hec);
            String ecPub = "BGN5aG7J5MLBFCiMQhaHJUI54SOVEO+Amti+cYmh17wgiJm+dnUq/C2p5daHrCmc3XxbVeVQWNEOGXDoHajwcNU=";
                //"BNtNCJWl769SUMXlAA9zgO0G2OgqOscwU15rJ29GsUpJWdFw+OISxJz5s2+Xe9mXnzHxrvkdAxLOcTRjT9LWm8U=,";
            
            String ecPri = "vKikXLMXWZPK831V021NiMVSC4YPGlYT/j2BFHhtpYE=";

            Console.Out.WriteLine("CCB 1.1版本算法ECIES(椭圆曲线集成加密公钥算法)安全初始化演示开始");
            Console.Out.WriteLine("ECIES PubKey=\t{0},", ecPub);
            Console.Out.WriteLine("ECIES Prikey=\t{0}", ecPri);
            String mypsk = jclmsCCB2014.zwMergePsk("testpsk1");
            //注意明文长度不能超出一定限度，目前是62字节左右，否则加解密运算结果将是错误的；
            String plainText =
                //mypsk;
                //"myplaintext20140717.0918.012myplaintext20140717.0918.012end920";  //明文
                //"77498EB7D7CE8B92D871791C99B85AB337FF73235A89E7A20764EFE6EA41E4CE";
                "77498EB7D7CE8B92D871791C99B85AB337FF73235A89E7A20764EFE6EA41E4CE";
            //"emhvdXdlaXRlc3RPdXRwdXREZWJ1Z1N0cmluZ0";
            //"FuZEppbkNodUVMb2NraW5kZXg9MFRvdGFsQmxvY2s9MkN1ckJsb2NrTGVuPTU4U2VkaW5nIERhdGEgQmxvY2sgIzBSZWNldmVkIERhdGEgRnJvbSBKQ0VMb2NrIGlzOg==";
            //用对方的公钥加密后发给对方
            String cryptText = jclmsCCB2014.EciesEncrypt(ecPub, plainText);
            //对方使用自己的私钥解密，还原出来明文
            String decryptText = jclmsCCB2014.EciesDecrypt(ecPri, cryptText);
            Console.Out.WriteLine("PSK:\n{0}", plainText);
            Console.Out.WriteLine("cryptText:\t{0}", cryptText);
            Console.Out.WriteLine("decryptPSK:\n{0}", decryptText);
            Console.Out.WriteLine("*************************");
            //String aa = Console.ReadLine();
            String cryptText2 = jclmsCCB2014.EciesEncrypt(ecPub, plainText+"test806");
            Console.Out.WriteLine("cryptText2:\t{0}", cryptText2);
            if (cryptText == cryptText2)
            {
                Console.WriteLine("同一个ECIES对象两次加密不同的内容输出是一样的！,必须改正\n");
            }
            //删除保存密钥对等等的内部数据结构.实践中密钥对生成只用做一次，以后就是            
            //保存下来重复利用了；这里每次都生成新的公钥/私钥对，是因为测试程序的缘故
            //删除句柄必须放到程序末尾，否则解密运算就无法进行了
            jclmsCCB2014.EciesDelete(hec);

        }

        private static void myECIEStest2()
        {
            //生成ECIES公钥/私钥对，返回保存密钥对等等的内部数据结构句柄
            //1为模拟的上位机，2为模拟的下位机
            int hec = jclmsCCB2014.EciesGenKeyPair();
            String ecPub = "BNtNCJWl769SUMXlAA9zgO0G2OgqOscwU15rJ29GsUpJWdFw+OISxJz5s2+Xe9mXnzHxrvkdAxLOcTRjT9LWm8U=,";
            String ecPri = "vKikXLMXWZPK831V021NiMVSC4YPGlYT/j2BFHhtpYE=";

            Console.Out.WriteLine("CCB 1.1版本算法ECIES(椭圆曲线集成加密公钥算法)安全初始化演示开始");
            Console.Out.WriteLine("ECIES PubKey=\t{0},", ecPub);
            Console.Out.WriteLine("ECIES Prikey=\t{0}", ecPri);
            String mypsk = jclmsCCB2014.zwMergePsk("testpsk1");
            //注意明文长度不能超出一定限度，目前是62字节左右，否则加解密运算结果将是错误的；
            String plainText =
                //mypsk;
                //"myplaintext20140717.0918.012myplaintext20140717.0918.012end920";  //明文
            "77498EB7D7CE8B92D871791C99B85AB337FF73235A89E7A20764EFE6EA41E4CE";
            //用对方的公钥加密后发给对方
            String cryptText = jclmsCCB2014.EciesEncrypt(ecPub, plainText);
            //对方使用自己的私钥解密，还原出来明文
            String decryptText = jclmsCCB2014.EciesDecrypt(ecPri, cryptText);
            Console.Out.WriteLine("PSK:\n{0}", plainText);
            Console.Out.WriteLine("cryptText:\t{0}", cryptText);
            Console.Out.WriteLine("decryptPSK:\n{0}", decryptText);
            Console.Out.WriteLine("*************************");
            String cryptText2 = jclmsCCB2014.EciesEncrypt(ecPub, plainText+"806");
            Console.Out.WriteLine("cryptText2:\t{0}", cryptText2);
            if (cryptText == cryptText2)
            {
                Console.WriteLine("同一个ECIES对象两次加密同样的内容输出是一样的！,必须改正\n");
            }
            //删除保存密钥对等等的内部数据结构.实践中密钥对生成只用做一次，以后就是            
            //保存下来重复利用了；这里每次都生成新的公钥/私钥对，是因为测试程序的缘故
            //删除句柄必须放到程序末尾，否则解密运算就无法进行了
            jclmsCCB2014.EciesDelete(hec);

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
            int curUTCSeconds = zwGetUTCSeconds();

            //锁具和上位机填入相同的初始条件，暂时替代初始化过程
            //固定条件部分
            jclmsCCB2014.JcLockSetString(jcLock, JCITYPE.JCI_ATMNO, atmno);
            jclmsCCB2014.JcLockSetString(jcLock, JCITYPE.JCI_LOCKNO, lockno);
            jclmsCCB2014.JcLockSetString(jcLock, JCITYPE.JCI_PSK, psk);
            jclmsCCB2014.JcLockSetString(jcSrv, JCITYPE.JCI_ATMNO, atmno);
            jclmsCCB2014.JcLockSetString(jcSrv, JCITYPE.JCI_LOCKNO, lockno);
            jclmsCCB2014.JcLockSetString(jcSrv, JCITYPE.JCI_PSK, psk);

            //此处不同的命令码指示生成不同的动态码
            jclmsCCB2014.JcLockSetCmdType(jcLock, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_INIT_CLOSECODE);
            jclmsCCB2014.JcLockSetCmdType(jcSrv, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_INIT_CLOSECODE);
            //锁具产生初始闭锁码
            int firstCloseCode = jclmsCCB2014.JcLockGetDynaCode(jcLock);
            jclmsCCB2014.JcLockDebugPrint(jcLock);
            Console.Out.WriteLine("锁具产生的初始闭锁码是 {0}", firstCloseCode);

            //可变条件部分.
            jclmsCCB2014.JcLockSetInt(jcLock, JCITYPE.JCI_DATETIME, curUTCSeconds);
            jclmsCCB2014.JcLockSetInt(jcLock, JCITYPE.JCI_VALIDITY, validity);
            jclmsCCB2014.JcLockSetInt(jcSrv, JCITYPE.JCI_DATETIME, curUTCSeconds);
            jclmsCCB2014.JcLockSetInt(jcSrv, JCITYPE.JCI_VALIDITY, validity);

            //初始闭锁码输入到上位机DLL，其他条件已经准备好
            jclmsCCB2014.JcLockSetInt(jcSrv, JCITYPE.JCI_CLOSECODE, firstCloseCode);
            //获取第一开锁密码.注意，获得每一类动态码的方式都是这个调用，区别在于m_cmdtype
            //jcSrv.DebugPrint();
            jclmsCCB2014.JcLockSetCmdType(jcSrv, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_CCB_DYPASS1);
            int dyCode1 = jclmsCCB2014.JcLockGetDynaCode(jcSrv);
            jclmsCCB2014.JcLockDebugPrint(jcSrv);
            Console.Out.WriteLine("上位机产生的第一开锁动态码是 {0}", dyCode1);
            //有问题请给我这个字符串
            //Console.Out.WriteLine("动态码输入条件调试信息字符串是");
            //jclmsCCB2014.JcLockDebugPrint(jcSrv);

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
            jclmsCCB2014.JcLockSetInt(jcLock, JCITYPE.JCI_CLOSECODE, ZW_FAKE_PASS1);
            int lockVerifyCode = jclmsCCB2014.JcLockGetDynaCode(jcLock);
            jclmsCCB2014.JcLockDebugPrint(jcLock);
            Console.Out.WriteLine("锁具产生的验证码是 {0}", lockVerifyCode);
            jclmsCCB2014.JcLockSetCmdType(jcSrv, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_CCB_LOCK_VERCODE);
            //上位机也计算锁具应该返回的验证码的值，予以比对
            jclmsCCB2014.JcLockSetInt(jcSrv, JCITYPE.JCI_CLOSECODE, ZW_FAKE_PASS1);
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
            jclmsCCB2014.JcLockDebugPrint(jcSrv);
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

        private static int zwGetUTCSeconds()
        {
            int curUTCSeconds = 0;
            DateTime jcdt = DateTime.Now.ToUniversalTime();
            Console.Out.WriteLine("当前的格林尼治时间(GMT)是{0},建行1.1版本算法上下位机都统一采用GMT来计算减少混乱"
                , jcdt.ToString("yyyy MMdd HHmm ss"));
            //计算当前时间距离GMT的秒数
            DateTime dt = new DateTime(1970, 1, 1);
            TimeSpan dp = jcdt - dt;
            curUTCSeconds = (int)(dp.Ticks / 10000000);
            Console.Out.WriteLine("当前的GMT秒数是\t{0}", curUTCSeconds);
            return curUTCSeconds;
        }

        //建行1.1版本动态码验证流程例子 WITH INPUT
        private static void myV11DynaCodeTestKeyBoardInput()
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
            jcSrv = jclmsCCB2014.JcLockNew();
            jcLock = jclmsCCB2014.JcLockNew();

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

            //此处不同的命令码指示生成不同的动态码
            jclmsCCB2014.JcLockSetCmdType(jcLock, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_INIT_CLOSECODE);
            jclmsCCB2014.JcLockSetCmdType(jcSrv, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_INIT_CLOSECODE);
            //锁具产生初始闭锁码
            int firstCloseCode = jclmsCCB2014.JcLockGetDynaCode(jcLock);
            jclmsCCB2014.JcLockDebugPrint(jcLock);
            Console.Out.WriteLine("锁具产生的初始闭锁码是 {0}", firstCloseCode);
            Console.WriteLine("请输入你的闭锁码");
            firstCloseCode = int.Parse(Console.ReadLine());


            //可变条件部分.
            jclmsCCB2014.JcLockSetInt(jcLock, JCITYPE.JCI_DATETIME, seconddiff);
            jclmsCCB2014.JcLockSetInt(jcLock, JCITYPE.JCI_VALIDITY, validity);
            jclmsCCB2014.JcLockSetInt(jcSrv, JCITYPE.JCI_DATETIME, seconddiff);
            jclmsCCB2014.JcLockSetInt(jcSrv, JCITYPE.JCI_VALIDITY, validity);

            //初始闭锁码输入到上位机DLL，其他条件已经准备好
            jclmsCCB2014.JcLockSetInt(jcSrv, JCITYPE.JCI_CLOSECODE, firstCloseCode);
            //获取第一开锁密码.注意，获得每一类动态码的方式都是这个调用，区别在于m_cmdtype
            //jcSrv.DebugPrint();
            jclmsCCB2014.JcLockSetCmdType(jcSrv, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_CCB_DYPASS1);
            int dyCode1 = jclmsCCB2014.JcLockGetDynaCode(jcSrv);
            jclmsCCB2014.JcLockDebugPrint(jcSrv);
            Console.Out.WriteLine("上位机产生的第一开锁动态码是 {0}", dyCode1);
            //有问题请给我这个字符串
            //Console.Out.WriteLine("动态码输入条件调试信息字符串是");
            //jclmsCCB2014.JcLockDebugPrint(jcSrv);

            //锁具反推验证第一开锁动态码，
            jclmsCCB2014.JcLockSetInt(jcLock, JCITYPE.JCI_CLOSECODE, firstCloseCode);
            jclmsCCB2014.JcLockSetCmdType(jcLock, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_CCB_DYPASS1);
            JCMATCH pass1Match = jclmsCCB2014.JcLockReverseVerifyDynaCode(jcLock, dyCode1);
            if (pass1Match.s_datetime > 0)
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
            jclmsCCB2014.JcLockSetInt(jcLock, JCITYPE.JCI_CLOSECODE, ZW_FAKE_PASS1);
            int lockVerifyCode = jclmsCCB2014.JcLockGetDynaCode(jcLock);
            jclmsCCB2014.JcLockDebugPrint(jcLock);
            Console.Out.WriteLine("锁具产生的验证码是 {0}", lockVerifyCode);
            Console.WriteLine("请输入你的验证码");
            lockVerifyCode = int.Parse(Console.ReadLine());
            jclmsCCB2014.JcLockSetCmdType(jcSrv, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_CCB_LOCK_VERCODE);
            //上位机也计算锁具应该返回的验证码的值，予以比对
            jclmsCCB2014.JcLockSetInt(jcSrv, JCITYPE.JCI_CLOSECODE, ZW_FAKE_PASS1);
            JCMATCH vercodeMatch = jclmsCCB2014.JcLockReverseVerifyDynaCode(jcSrv, lockVerifyCode);
            if (vercodeMatch.s_datetime > 0)
            {
                Console.Out.WriteLine("上位机对于锁具应该返回的验证码验证成功，证实了锁具的身份,匹配结果时间为{0}", vercodeMatch.s_datetime);
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
            jclmsCCB2014.JcLockDebugPrint(jcSrv);
            Console.Out.WriteLine("上位机计算的第二开锁码是 {0}", dyCode2);
            jclmsCCB2014.JcLockSetCmdType(jcLock, JCITYPE.JCI_CMDTYPE, JCCMD.JCCMD_CCB_DYPASS2);
            //锁具计算第二开锁码，以便于上位机传来的第二开锁码比对
            //锁具验证码作为第二开锁码的计算要素
            jclmsCCB2014.JcLockSetInt(jcLock, JCITYPE.JCI_CLOSECODE, lockVerifyCode);
            JCMATCH pass2Match = jclmsCCB2014.JcLockReverseVerifyDynaCode(jcLock, dyCode2);
            if (pass2Match.s_datetime > 0)
            {
                Console.Out.WriteLine("锁具验证第二开锁码成功，开锁成功,匹配结果时间为{0}", pass2Match.s_datetime);
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

/// <summary>
///     20141212,JCLMS通过密盒计算的测试代码
/// </summary>
        const int ZWMEGA = 1000 * 1000;
        //固定开锁时间,应该出来固定的结果
        const int ZWFIX_STARTTIME = 1416 * ZWMEGA;
        const String MYT_ATMNO = "atm10455761";
        const String MYT_LOCKNO = "lock14771509";
        const String MYT_PSK = "PSKDEMO728";
        const int MYT_INITCLOSECODE = 38149728;
        const int MYT_DYPASS1 = 57174184;
        const int MYT_VERCODE = 58387712;
        const int MYT_DYPASS2 = 52451262;

        //设置几个测试共同的ATM编号，锁具编号，PSK3项输入值
        private static void mySetPubInput1218(int handle)
        {
            jclmsCCB2014.JcLockSetString(handle, jclms.JCITYPE.JCI_ATMNO, MYT_ATMNO);
            jclmsCCB2014.JcLockSetString(handle, jclms.JCITYPE.JCI_LOCKNO, MYT_LOCKNO);
            jclmsCCB2014.JcLockSetString(handle, jclms.JCITYPE.JCI_PSK, MYT_PSK);
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_TIMESTEP, 6);
        }

        //初始闭锁码生成
        private static void myLmsReq2SecBoxEx20141212GenInitCloseCode()
        {
            int handle = jclmsCCB2014.JcLockNew();
            mySetPubInput1218(handle);
            //指明要生成初始闭锁码；由于初始闭锁码的日期时间，有效期等可变因素是预先定死的，所以不用设置了
            jclmsCCB2014.JcLockSetCmdType(handle, jclms.JCITYPE.JCI_CMDTYPE, jclms.JCCMD.JCCMD_INIT_CLOSECODE);       

            int myInitCloseCode = jclmsCCB2014.csJclmsReqGenDyCode(handle);
            if (MYT_INITCLOSECODE != myInitCloseCode)
            {
                Console.Out.WriteLine("密盒返回的初始闭锁码结果{0}是错误的，正确值是{1}", myInitCloseCode, MYT_INITCLOSECODE);
            }
            else
            {
                Console.Out.WriteLine("密盒返回的初始闭锁码结果{0}是正确的", myInitCloseCode);
            }
            Console.Out.WriteLine("########################################################################");
        }

        //第一开锁码生成
        private static void myLmsReq2SecBoxEx20141212GenPass1DyCode()
        {
            int handle = jclmsCCB2014.JcLockNew();
            mySetPubInput1218(handle);
            //第一开锁码的生成,必须由初始闭锁码作为输入要素填写在JCI_CLOSECODE里面
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_CLOSECODE, MYT_INITCLOSECODE);
            //指定要为什么时间生成第一开锁码，可以提前为将来某个时刻生成开锁码
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_DATETIME, ZWFIX_STARTTIME);
            //生成第一开锁码,必须填写正确的类型
            jclmsCCB2014.JcLockSetCmdType(handle, jclms.JCITYPE.JCI_CMDTYPE, jclms.JCCMD.JCCMD_CCB_DYPASS1);            

            int myDyCodePass1 = jclmsCCB2014.csJclmsReqGenDyCode(handle);

            if (MYT_DYPASS1 != myDyCodePass1)
            {
                Console.Out.WriteLine("密盒返回的第一开锁码结果{0}是错误的，正确值是{1}", myDyCodePass1, MYT_DYPASS1);
            }
            else
            {
                Console.Out.WriteLine("密盒返回的第一开锁码结果{0}是正确的", myDyCodePass1);
            }
            Console.Out.WriteLine("########################################################################");
        }

        //第一开锁码验证
        private static void myLmsReq2SecBoxEx20141212VerifyPass1DyCode()
        {
            int handle = jclmsCCB2014.JcLockNew();
            mySetPubInput1218(handle);
            //第一开锁码的验证,必须要有初始闭锁码填写在JCI_CLOSECODE里面
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_CLOSECODE, MYT_INITCLOSECODE);
            //由于是预设好的距离现在有很多日子的一个时间值1416000000秒,所以需要特地设置搜索起始时间
            //为该时间之后5分钟以内的某个时间点,此处设置2分钟多一点,应该具有一定的典型性;
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_SEARCH_TIME_START, 1416 * ZWMEGA + 123);
            //既然是验证第一开锁码,必须填写正确的类型,无论是生成还是验证
            jclmsCCB2014.JcLockSetCmdType(handle, jclms.JCITYPE.JCI_CMDTYPE, jclms.JCCMD.JCCMD_CCB_DYPASS1);
            JCMATCH match=new JCMATCH();
            jclmsCCB2014.zwJclmsReqVerifyDyCode(handle, MYT_DYPASS1, match);
            

            if (0!=match.s_datetime)
            {
                Console.Out.WriteLine("密盒返回的第一开锁码验证结果是正确的，时间是{0}", match.s_datetime);
            }
            else
            {
                Console.Out.WriteLine("密盒返回的第一开锁码验证结果是错误的，时间是{0}", match.s_datetime);
            }
            Console.Out.WriteLine("########################################################################");
        }

        //验证码生成
        private static void myLmsReq2SecBoxEx20141218GenVerifyCode()
        {
            int handle = jclmsCCB2014.JcLockNew();
            mySetPubInput1218(handle);
            //验证码的生成,应该把第一开锁码作为输入要素填写在JCI_CLOSECODE里面
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_CLOSECODE, MYT_DYPASS1);
            //指定要为什么时间生成验证码,考虑到模拟实际情况，双人输入完毕自己的密码，获得第一开锁码之后
            //再在锁具上输入，25秒应该是最小延迟了，所以加上25秒
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_DATETIME, ZWFIX_STARTTIME+25);
            //生成验证码,必须填写正确的类型
            jclmsCCB2014.JcLockSetCmdType(handle, jclms.JCITYPE.JCI_CMDTYPE, jclms.JCCMD.JCCMD_CCB_LOCK_VERCODE);

            int myVerifyCode = jclmsCCB2014.csJclmsReqGenDyCode(handle);

            if (MYT_VERCODE != myVerifyCode)
            {
                Console.Out.WriteLine("密盒返回的验证码结果{0}是错误的，正确值是{1}", myVerifyCode, MYT_VERCODE);
            }
            else
            {
                Console.Out.WriteLine("密盒返回的验证码结果{0}是正确的", myVerifyCode);
            }
            Console.Out.WriteLine("########################################################################");
        }

        //验证码验证
        private static void myLmsReq2SecBoxEx20141218VerifyVerifyCode()
        {
            int handle = jclmsCCB2014.JcLockNew();
            mySetPubInput1218(handle);
            //验证码的验证,应该把第一开锁码作为输入要素填写在JCI_CLOSECODE里面
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_CLOSECODE, MYT_DYPASS1);
            //由于是预设好的距离现在有很多日子的一个时间值1416000000秒,所以需要特地设置搜索起始时间
            //为该时间之后5分钟以内的某个时间点,此处设置2分钟多一点,应该具有一定的典型性;
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_SEARCH_TIME_START, 1416 * ZWMEGA + 123);
            //既然是验证验证,必须填写正确的类型,无论是生成还是验证
            jclmsCCB2014.JcLockSetCmdType(handle, jclms.JCITYPE.JCI_CMDTYPE, jclms.JCCMD.JCCMD_CCB_LOCK_VERCODE);
            JCMATCH match = new JCMATCH();
            jclmsCCB2014.zwJclmsReqVerifyDyCode(handle, MYT_VERCODE, match);


            if (0 != match.s_datetime)
            {
                Console.Out.WriteLine("密盒返回的验证码验证结果是正确的，时间是{0}", match.s_datetime);
            }
            else
            {
                Console.Out.WriteLine("密盒返回的验证码验证结果是错误的，时间是{0}", match.s_datetime);
            }
            Console.Out.WriteLine("########################################################################");
        }

        //第二开锁码生成
        private static void myLmsReq2SecBoxEx20141218GenPass2DyCode()
        {
            int handle = jclmsCCB2014.JcLockNew();
            mySetPubInput1218(handle);
            //第二开锁码的生成,必须由验证码作为输入要素填写在JCI_CLOSECODE里面
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_CLOSECODE, MYT_VERCODE);
            //指定要为什么时间生成第二开锁码，现在假设是第一开锁码之后55秒
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_DATETIME, ZWFIX_STARTTIME+55);
            //生成第二开锁码,必须填写正确的类型
            jclmsCCB2014.JcLockSetCmdType(handle, jclms.JCITYPE.JCI_CMDTYPE, jclms.JCCMD.JCCMD_CCB_DYPASS2);

            int myDyCodePass1 = jclmsCCB2014.csJclmsReqGenDyCode(handle);

            if (MYT_DYPASS2 != myDyCodePass1)
            {
                Console.Out.WriteLine("密盒返回的第二开锁码结果{0}是错误的，正确值是{1}", myDyCodePass1, MYT_DYPASS2);
            }
            else
            {
                Console.Out.WriteLine("密盒返回的第二开锁码结果{0}是正确的", myDyCodePass1);
            }
            Console.Out.WriteLine("########################################################################");
        }

        //第二开锁码验证
        private static void myLmsReq2SecBoxEx20141218VerifyPass2DyCode()
        {
            int handle = jclmsCCB2014.JcLockNew();
            mySetPubInput1218(handle);
            //第二开锁码的验证,必须由验证码作为输入要素填写在JCI_CLOSECODE里面
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_CLOSECODE, MYT_VERCODE);
            //由于是预设好的距离现在有很多日子的一个时间值1416000000秒,所以需要特地设置搜索起始时间
            //为该时间之后5分钟以内的某个时间点,此处设置2分钟多一点,应该具有一定的典型性;
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_SEARCH_TIME_START, 1416 * ZWMEGA + 123);
            //既然是验证第二开锁码,必须填写正确的类型,无论是生成还是验证
            jclmsCCB2014.JcLockSetCmdType(handle, jclms.JCITYPE.JCI_CMDTYPE, jclms.JCCMD.JCCMD_CCB_DYPASS2);
            JCMATCH match = new JCMATCH();
            jclmsCCB2014.zwJclmsReqVerifyDyCode(handle, MYT_DYPASS2, match);


            if (0 != match.s_datetime)
            {
                Console.Out.WriteLine("密盒返回的第二开锁码验证结果是正确的，时间是{0}", match.s_datetime);
            }
            else
            {
                Console.Out.WriteLine("密盒返回的第二开锁码验证结果是错误的，时间是{0}", match.s_datetime);
            }
            Console.Out.WriteLine("########################################################################");
        }

        private static void tmyLmsReq2SecBoxEx20141221GenPass1DyCode()
        {
            int handle = jclmsCCB2014.JcLockNew();
                //"ATMNO":        "123456789012",
                //"LOCKNO":       "1111222233334444",
                //"PSK":  "A2E61F74FFB44E7F78282381732B0E280DD14882AAA91A1FD0511BF230DB23C3",
                //"CodeGenDateTime":      1419150780,
                //"Validity":     5,
                //"CloseCode":    12345678,
                //"CmdType":      "JCCMD_CCB_DYPASS1",

            jclmsCCB2014.JcLockSetString(handle, jclms.JCITYPE.JCI_ATMNO, "123456789012");
            jclmsCCB2014.JcLockSetString(handle, jclms.JCITYPE.JCI_LOCKNO, "1111222233334444");
            jclmsCCB2014.JcLockSetString(handle, jclms.JCITYPE.JCI_PSK, "A2E61F74FFB44E7F78282381732B0E280DD14882AAA91A1FD0511BF230DB23C3");
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_TIMESTEP, 6);

            //第一开锁码的生成,必须由初始闭锁码作为输入要素填写在JCI_CLOSECODE里面
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_CLOSECODE, MYT_INITCLOSECODE);
            //指定要为什么时间生成第一开锁码，可以提前为将来某个时刻生成开锁码
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_DATETIME, 1419150780);
            //生成第一开锁码,必须填写正确的类型
            jclmsCCB2014.JcLockSetCmdType(handle, jclms.JCITYPE.JCI_CMDTYPE, jclms.JCCMD.JCCMD_CCB_DYPASS1);

            int myDyCodePass1 = jclmsCCB2014.csJclmsReqGenDyCode(handle);

            if (MYT_DYPASS1 != myDyCodePass1)
            {
                Console.Out.WriteLine("密盒返回的第一开锁码结果{0}是错误的，正确值是{1}", myDyCodePass1, MYT_DYPASS1);
            }
            else
            {
                Console.Out.WriteLine("密盒返回的第一开锁码结果{0}是正确的", myDyCodePass1);
            }
            Console.Out.WriteLine("########################################################################");
        }

        private static void mySecBoxTest1221()
        {
            //声明一个密盒对象；使用该对象的3个方法来认证，读取，写入，至于Open/Close由该对象内部自动完成；            
            
            for (int i = 0; i < 1; i++)
            //while(true)
            {
                jclms.JcSecBox secBox = new JcSecBox();
                Console.Out.WriteLine("Secret Box Open###########################################################");
                //打开密盒                
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
                const String myLongB64Str1 = "12345678";
                    //"emhvdXdlaXRlc3RPdXRwdXREZWJ1Z1N0cmluZ0FuZEppbkNodUVMb2NraW5kZXg9MFRvdGFsQmxvY2s9MkN1ckJsb2NrTGVuPTU4U2VkaW5nIERhdGEgQmxvY2sgIzBSZWNldmVkIERhdGEgRnJvbSBKQ0VMb2NrIGlzOg==";
                //通过句柄，索引号，读取密盒数据，返回的也是Base64编码过的字符串，解码后可能是文本，也可能是二进制数据


                String recvFromSecBox = secBox.SecboxReadData(2);
                Console.Out.WriteLine("Secret Box ReadData is {0}", recvFromSecBox);
                secBox.SecboxWriteData(2, myLongB64Str1);
                Console.Out.WriteLine("Secret Box WriteData is {0}",myLongB64Str1);
                secBox.SecboxReadData(2);
                Console.Out.WriteLine("Secret Box ReadData2 is {0}", recvFromSecBox);
                //Console.Out.WriteLine("Secret Box ReadData");
                
                Console.Out.WriteLine("WAIT 4 SECONDS FOR PLUG OUT/IN SECRET BOX");
                //System.Threading.Thread.Sleep(ZWPAUSE*5);
            }

        }

        //验证码验证.test1222
        private static void tmyLmsReq2SecBoxEx20141222VerifyVerifyCode()
        {
            int handle = jclmsCCB2014.JcLockNew();
            jclmsCCB2014.JcLockSetString(handle, jclms.JCITYPE.JCI_ATMNO, MYT_ATMNO);
            jclmsCCB2014.JcLockSetString(handle, jclms.JCITYPE.JCI_LOCKNO, MYT_LOCKNO);
            jclmsCCB2014.JcLockSetString(handle, jclms.JCITYPE.JCI_PSK, MYT_PSK);

            //验证码的验证,应该把闭锁码作为输入要素填写在JCI_CLOSECODE里面
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_CLOSECODE, MYT_DYPASS1);
            //由于是预设好的距离现在有很多日子的一个时间值1416000000秒,所以需要特地设置搜索起始时间
            //为该时间之后5分钟以内的某个时间点,此处设置2分钟多一点,应该具有一定的典型性;
            jclmsCCB2014.JcLockSetInt(handle, jclms.JCITYPE.JCI_SEARCH_TIME_START, zwGetUTCSeconds());
            //既然是验证验证,必须填写正确的类型,无论是生成还是验证
            jclmsCCB2014.JcLockSetCmdType(handle, jclms.JCITYPE.JCI_CMDTYPE, jclms.JCCMD.JCCMD_CCB_LOCK_VERCODE);
            JCMATCH match = new JCMATCH();
            jclmsCCB2014.zwJclmsReqVerifyDyCode(handle, MYT_VERCODE, match);


            if (0 != match.s_datetime)
            {
                Console.Out.WriteLine("密盒返回的验证码验证结果是正确的，时间是{0}", match.s_datetime);
            }
            else
            {
                Console.Out.WriteLine("密盒返回的验证码验证结果是错误的，时间是{0}", match.s_datetime);
            }
            Console.Out.WriteLine("########################################################################");
        }

        private static void secTest1()
        {
            for (int i = 0; i < 1; i++)
            //while(true)
            {
                jclms.JcSecBox secBox = new JcSecBox();
                Console.Out.WriteLine("Secret Box Open###########################################################");
                //打开密盒                
                int status =
                    secBox.SecboxAuth();

                if (0 == status)
                {
                    Console.Out.WriteLine("Good Secret Box");
                }
                if (1 == status)
                {
                    Console.Out.WriteLine("Fake Secret Box");
                    continue;
                }
                //////////////////////////////////////////////////////////
                //随便用一段比较长的文字经过base64编码形成的下面这段有待写入的base64数据
                //实践中，可以用二进制数据编码之后成为base64字符串写入；
                //第二个参数是索引号，大致上是0到10左右，具体还得和赵工确认
                //第三个参数，也就是数据，大体上可以达到最大400多个字节，具体多少还得和赵工确认
                const String myLongB64Str1 = "12345678";
                //"emhvdXdlaXRlc3RPdXRwdXREZWJ1Z1N0cmluZ0FuZEppbkNodUVMb2NraW5kZXg9MFRvdGFsQmxvY2s9MkN1ckJsb2NrTGVuPTU4U2VkaW5nIERhdGEgQmxvY2sgIzBSZWNldmVkIERhdGEgRnJvbSBKQ0VMb2NrIGlzOg==";
                //通过句柄，索引号，读取密盒数据，返回的也是Base64编码过的字符串，解码后可能是文本，也可能是二进制数据


                String recvFromSecBox = secBox.SecboxReadData(2);
                Console.Out.WriteLine("Secret Box ReadData is {0}", recvFromSecBox);
                secBox.SecboxWriteData(2, myLongB64Str1);
                Console.Out.WriteLine("Secret Box WriteData is {0}", myLongB64Str1);
                secBox.SecboxReadData(2);
                Console.Out.WriteLine("Secret Box ReadData2 is {0}", recvFromSecBox);
                //Console.Out.WriteLine("Secret Box ReadData");

                Console.Out.WriteLine("WAIT 4 SECONDS FOR PLUG OUT/IN SECRET BOX");
                //System.Threading.Thread.Sleep(ZWPAUSE*5);
            }
        }

        private static void secTest2()
        {
            Console.Out.WriteLine("Secret Box Authentic Test");
            for (int i = 0; i < 64 * 3; i++)
            {
                jclms.JcSecBox secBox = new JcSecBox();
                //打开密盒                
                int status =
                    secBox.SecboxAuth();

                if (0 == status)
                {
                    Console.Out.Write(".");
                }
                if (1 == status)
                {
                    Console.Out.WriteLine("FAKE ");
                    continue;
                }
                if (i > 0 && i % 32 == 0)
                {
                    Console.Out.Write("{0}\t", i);
                }
                System.Threading.Thread.Sleep(50);
            }
        }

///////////////////////////////////////////////////////////////////////////////////////////
    }   //class Program
}   //namespace cstest702
