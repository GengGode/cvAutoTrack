using System.Text;

namespace TEST_cvAutoTrack
{
    internal class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Hello, World!");

            // 获取版本号
            byte[] versionBuff = new byte[1024];
            //string version = "       ";
            
            cvAutoTrack.cvAutoTrack.verison(versionBuff);
            string version = Encoding.UTF8.GetString(versionBuff);
            //cvAutoTrack.cvAutoTrack.verison( version);
            
            Console.WriteLine("version: " + version);
            


            // 申请内存
            byte[] jsonBuffer = new byte[1024];

            // 调用 cvAutoTrack.dll
            bool ret = cvAutoTrack.cvAutoTrack.GetStarJson(jsonBuffer);

            // 输出结果
            Console.WriteLine("ret = " + ret);
            Console.WriteLine("jsonBuffer = " + Encoding.UTF8.GetString(jsonBuffer));

            Console.ReadKey();
            
        }
    }
}