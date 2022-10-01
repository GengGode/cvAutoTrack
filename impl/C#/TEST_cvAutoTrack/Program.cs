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

            int count = 0;
            while (true)
            {
                if (count==1)
                {
                    cvAutoTrack.cvAutoTrack.SetUseDx11CaptureMode();
                    Console.WriteLine("启用Dx模式");
                }
                if (count == 15)
                {
                    cvAutoTrack.cvAutoTrack.SetUseBitbltCaptureMode();
                    Console.WriteLine("启用Bitblt模式");
                }

                if (count>=30)
                {
                    count = 0;
                }

                count = count + 1;

                float x = 0;
                float y = 0;
                float a = 0;
                int mapid = 0;
                cvAutoTrack.cvAutoTrack.GetTransformOfMap(ref x,ref y,ref a,ref mapid);
                Console.WriteLine("x = " + x + " y = " + y + " a = " + a + " mapid = " + mapid);

                // 获取错误码及错误信息
                byte[] errorBuff = new byte[1024];
                for (int i = 0; i < 1024; i++)
                {
                    errorBuff[i] = 0;
                }
                int res=cvAutoTrack.cvAutoTrack.GetLastErrMsg(errorBuff,1024);
                string error = Encoding.UTF8.GetString(errorBuff);
                Console.WriteLine("error: " + error);

            }

            Console.ReadKey();
            
        }
    }
}