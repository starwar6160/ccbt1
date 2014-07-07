﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using jclms;

namespace cstest702
{
    class Program
    {
        static void Main(string[] args)
        {
            int lmsver = jclmsCCB2014.getVersion();
            Console.Out.WriteLine("Jclms DLL Version is {0}", lmsver);

            //锁具的模拟对象
            JcLockInput jcLock=new JcLockInput();
            //上位机的模拟对象
            JcLockInput jcSrv = new JcLockInput();

            const String atmno = "atm1045576";
            const String lockno = "lock14771509";
            const String psk = "jclmsdemopsk201407071509aajclmsdemopsk201407071509";
            const Int32 validity=240;
            //传入当前时间的GMT(格林尼治时间)
            DateTime jcdt = DateTime.Now.ToUniversalTime();
            Console.Out.WriteLine("当前的格林尼治时间(GMT)是{0},建行1.1版本算法上下位机都统一采用GMT来计算减少混乱"
                , jcdt.ToString("yyyy MMdd HHmm ss"));
            //计算当前时间距离GMT的秒数
            DateTime dt = new DateTime(1970, 1, 1);
            TimeSpan dp = jcdt - dt;
            int seconddiff = (int)(dp.Ticks / 10000000);
            Console.Out.WriteLine("当前的GMT秒数是 is {0}", seconddiff);

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
            jcLock.m_cmdtype = JCCMD.JCCMD_INIT_CLOSECODE;
            jcSrv.m_cmdtype = JCCMD.JCCMD_INIT_CLOSECODE;
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
            int lockVerifyCode = jclmsCCB2014.zwGetDynaCode(jcLock);
            Console.Out.WriteLine("锁具产生的验证码是 {0}", lockVerifyCode);
            jcSrv.m_cmdtype = JCCMD.JCCMD_CCB_LOCK_VERCODE;
            //上位机也计算锁具应该返回的验证码的值，予以比对
            int srvLockVerCode = jclmsCCB2014.zwGetDynaCode(jcSrv);
            if (dyCode1 == dyCode1Verify)
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
            int dyCode2 = jclmsCCB2014.zwGetDynaCode(jcSrv);
            Console.Out.WriteLine("上位机计算的第二开锁码是 {0}", dyCode2);
            jcLock.m_cmdtype = JCCMD.JCCMD_CCB_DYPASS2;
            //锁具计算第二开锁码，以便于上位机传来的第二开锁码比对
            int dyCode2Verify = jclmsCCB2014.zwGetDynaCode(jcLock);
            if (dyCode2 == dyCode2Verify)
            {
                Console.Out.WriteLine("锁具验证第二开锁码成功，开锁成功");
            }
            else
            {
                Console.Out.WriteLine("锁具验证第二开锁码失败，开锁失败");
            }
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
            JCERROR err = jclmsCCB2014.zwVerifyDynaCode(myLock, dyCode);
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
