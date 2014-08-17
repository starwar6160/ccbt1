using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Security.Cryptography;

namespace cs3DESex1
{
    class Program3DES0816zw
    {
        //这两个byte和HEX字符串转换函数是我从网上找来的
        public static byte[] strToToHexByte(string hexString)
        {
            hexString = hexString.Replace(" ", "");
            if ((hexString.Length % 2) != 0)
                hexString += " ";
            byte[] returnBytes = new byte[hexString.Length / 2];
            for (int i = 0; i < returnBytes.Length; i++)
                returnBytes[i] = Convert.ToByte(hexString.Substring(i * 2, 2), 16);
            return returnBytes;
        }

        public static string byteToHexStr(byte[] bytes)
        {
            string returnStr = "";
            if (bytes != null)
            {
                for (int i = 0; i < bytes.Length; i++)
                {
                    returnStr += bytes[i].ToString("X2");
                }
            }
            return returnStr;
        }  

        //注意，秘钥和数据，密文都是HEX格式
        public static string DES3Encrypt(string data, string key)
        {
            //建行的扩充秘钥的方法，截取16字节字符串的前8字节作为最后一组8字节秘钥
            //合计成为24字节也就是192bit秘钥
            String exKey = key + key.Substring(0, 8);
            TripleDESCryptoServiceProvider DES = 
                new TripleDESCryptoServiceProvider();
            //此处建行因为历史原因将错就错的生成Key的方式：要求输入16个字符
            // (其实不限于HEX，任何可打印字符都可以)，然后把前8个字符再次添加
            // 到输入的末尾从而16个字符扩展到24个字符也就是192bit，作为3DES
            // 要求的192bit秘钥；此处的要诀与建行“将错就错”在于：3DES要求的是
            // 192 bit的二进制内容作为Key，但建行给出的是24个HEX字符，实际
            // 信息量只有24*4=96bit，而通过把24个HEX字符的二进制形式共192bit
            // 作为Key，将错就错的满足了3DES对于Key长度的要求；
            DES.Key = ASCIIEncoding.ASCII.GetBytes(exKey);
            //此处模式一定要是ECB，一是因为建行本来就指定的ECB模式，二是其他
            //模式都有一个叫做初始化向量的东西，会使得每次加密结果都不同；
            DES.Mode = CipherMode.ECB;
            //注意建行指定的填充模式是“无”，不这么做，加密结果就会不同导致建行无法解密；
            DES.Padding = PaddingMode.None;

            ICryptoTransform DESEncrypt = DES.CreateEncryptor();
            //注意此处，数据确实是HEX格式字符串输入，但是要化为二进制格式
            //去给3DES算法做加密；原来使用ASCIIEncoding.ASCII.GetBytes的做法
            // 使得加密的明文就是错误，是HEX字符串而非他们所代表的本体二进制
            // 数据被加密的，自然建行解密肯定失败；
            byte[] Buffer = strToToHexByte(data);
            //加密结果这次转化为HEX字符串返回；
            return byteToHexStr(DESEncrypt.TransformFinalBlock(Buffer, 0, Buffer.Length));
        }

        //注意，秘钥，密文，解密出来的明文，都是HEX格式；
        public static string DES3Decrypt(string data, string key)
        {
            //建行的扩充秘钥的方法，截取16字节字符串的前8字节作为最后一组8字节秘钥
            //合计成为24字节也就是192bit秘钥
            String exKey = key + key.Substring(0, 8);
            TripleDESCryptoServiceProvider DES =
                new TripleDESCryptoServiceProvider();
            DES.Key = ASCIIEncoding.ASCII.GetBytes(exKey);
            //此处模式一定要是ECB，一是因为建行本来就指定的ECB模式，二是其他
            //模式都有一个叫做初始化向量的东西，会使得每次加密结果都不同；
            DES.Mode = CipherMode.ECB;
            //注意建行指定的填充模式是“无”，不这么做，加密结果就会不同导致建行无法解密；
            DES.Padding = PaddingMode.None;

            ICryptoTransform DESDecrypt = DES.CreateDecryptor();
            //注意此处，数据确实是HEX格式字符串输入，但是要化为二进制格式
            //去给3DES算法做解密；
            byte[] Buffer = strToToHexByte(data);
            //解密结果这次转化为HEX字符串返回；
            return byteToHexStr(DESDecrypt.TransformFinalBlock(Buffer, 0, Buffer.Length));
        }

        static void Main(string[] args)
        {
            String data = "F856272510DC7307";
            String key = "0123456789ABCDEF";
            String crypt;
            crypt=DES3Encrypt(data,key);
            Console.WriteLine("Data={0},Key={1}\nCrypt={2}", data, key, crypt);
            String dataDecoded = DES3Decrypt(crypt, key);
            Console.WriteLine("Decrypted is {0}", dataDecoded);
        }
    }
}
