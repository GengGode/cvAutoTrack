using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Threading;
using System.Threading.Tasks;
using System.Text;
using cvAutoTrackCSharp;
using cvAutoTrackCSharpImpl;

namespace TEST_cvAutoTrack_CSharp
{
    class Program
    {
        static int Main(string[] args)
        {
            // 设置控制台编码为65001
            Console.OutputEncoding = Encoding.UTF8;

            // 如果输入参数 -test 就执行测试
            if (args.Length > 0 && args[0] == "-test")
            {
                {
                    // 测试版本号
                    byte[] version_buff = new byte[256];
                    if (cvAutoTrackCSharp.cvAutoTrack.GetCompileVersion(version_buff, 256))
                    {
                        string version_str = Encoding.UTF8.GetString(version_buff);
                        Console.WriteLine("版本号       : " + version_str);
                    }
                    else
                    {
                        Console.WriteLine("错误码       : " + cvAutoTrackCSharp.cvAutoTrack.GetLastErr().ToString());
                    }
                    byte[] version_time_buff = new byte[256];
                    if (cvAutoTrackCSharp.cvAutoTrack.GetCompileTime(version_time_buff, 256))
                    {
                        string version_time_str = Encoding.UTF8.GetString(version_time_buff);
                        Console.WriteLine("编译时间     : " + version_time_str);
                    }
                    else
                    {
                        Console.WriteLine("错误码       : " + cvAutoTrackCSharp.cvAutoTrack.GetLastErr().ToString());
                    }

                    Console.WriteLine("隐式加载 测试完成");
                }
                {
                    // 测试版本号
                    cvAutoTrackCSharpImpl.cvAutoTrack cvAutoTrack = new cvAutoTrackCSharpImpl.cvAutoTrack(true);
                    byte[] version_buff = new byte[256];
                    if (cvAutoTrack.GetCompileVersion(version_buff, 256))
                    {
                        string version_str = Encoding.UTF8.GetString(version_buff);
                        Console.WriteLine("版本号       : " + version_str);
                    }
                    else
                    {
                        Console.WriteLine("错误码       : " + cvAutoTrack.GetLastErr().ToString());
                    }
                    byte[] version_time_buff = new byte[256];
                    if (cvAutoTrack.GetCompileTime(version_time_buff, 256))
                    {
                        string version_time_str = Encoding.UTF8.GetString(version_time_buff);
                        Console.WriteLine("编译时间     : " + version_time_str);
                    }
                    else
                    {
                        Console.WriteLine("错误码       : " + cvAutoTrack.GetLastErr().ToString());
                    }

                    Console.WriteLine("显式加载 测试完成");
                }
                return 0;
            }
            else
            {
                // 获取版本号
                byte[] versionBuff = new byte[1024];

                cvAutoTrackCSharp.cvAutoTrack.GetCompileVersion(versionBuff,256);
                string version = Encoding.UTF8.GetString(versionBuff);
                Console.WriteLine("version: " + version);

                cvAutoTrackCSharpImpl.cvAutoTrack cvAutoTrack = new cvAutoTrackCSharpImpl.cvAutoTrack();
                cvAutoTrack.GetCompileVersion(versionBuff, 256);
                version = Encoding.UTF8.GetString(versionBuff);
                
                Console.WriteLine("version: " + version);
                
                // 申请内存
                byte[] jsonBuffer = new byte[1024];

                // 调用 cvAutoTrack.dll
                bool ret = cvAutoTrackCSharp.cvAutoTrack.GetStarJson(jsonBuffer);

                // 输出结果
                Console.WriteLine("ret = " + ret);
                Console.WriteLine("jsonBuffer = " + Encoding.UTF8.GetString(jsonBuffer));

                int count = 0;
                while (true)
                {
                    if (count == 1)
                    {
                        cvAutoTrackCSharp.cvAutoTrack.SetUseDx11CaptureMode();
                        Console.WriteLine("启用Dx模式");
                    }
                    if (count == 15)
                    {
                        cvAutoTrackCSharp.cvAutoTrack.SetUseBitbltCaptureMode();
                        Console.WriteLine("启用Bitblt模式");
                    }

                    if (count >= 30)
                    {
                        count = 0;
                    }

                    count = count + 1;

                    double x = 0;
                    double y = 0;
                    double a = 0;
                    int mapid = 0;
                    cvAutoTrackCSharp.cvAutoTrack.GetTransformOfMap(ref x, ref y, ref a, ref mapid);
                    Console.WriteLine("x = " + x + " y = " + y + " a = " + a + " mapid = " + mapid);

                    // 获取错误码及错误信息
                    byte[] errorBuff = new byte[1024];
                    for (int i = 0; i < 1024; i++)
                    {
                        errorBuff[i] = 0;
                    }
                    int res = cvAutoTrackCSharp.cvAutoTrack.GetLastErrMsg(errorBuff, 1024);
                    string error = Encoding.UTF8.GetString(errorBuff);
                    Console.WriteLine("error: " + error);

                }

                return 0;
            }
        }
    }
}