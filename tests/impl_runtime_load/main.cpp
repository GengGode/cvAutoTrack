#include <Windows.h>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <vector>
#ifdef _DEBUG
    #include <filesystem>
#endif // _DEBUG
#define explicit_link
#include <cvAutoTrack.h>

int TEST(cvAutoTrack& at)
{
    char version_buff[256] = { 0 };
    if (at.GetCompileVersion(version_buff, 256))
        fmt::print("版本号       : {}\n", version_buff);
    else
        fmt::print("错误码       : {}\n", at.GetLastErr());

    char version_time_buff[256] = { 0 };
    if (at.GetCompileTime(version_time_buff, 256))
        fmt::print("编译时间     : {}\n", version_time_buff);
    else
        fmt::print("错误码       : {}\n", at.GetLastErr());

    std::cout << "测试完成\n";
    return 0;
}

void Run_GetAll(cvAutoTrack& at)
{
    double x, y, a, r;
    int mapId, uid;
    std::string mapType;
    double time_beg = GetTickCount();
    if (at.GetAllInfo(x, y, mapId, a, r, uid))
    {
        double time_end = GetTickCount();
        switch (mapId)
        {
            case 0: mapType = "提瓦特大陆"; break;
            case 1: mapType = "渊下宫"; break;
            case 2: mapType = "地下矿区"; break;
        }
        std::cout << fmt::format(R"(
全部信息：
地区:{}
坐标:x = {:6.2f}; y = {:6.2f}
朝向:角色 = {:4.2f}; 相机 = {:4.2f}
UID:{:d}
UseTime:{:4.2f}ms
----------------
)",
                                 mapType, x, y, a, r, uid, time_end - time_beg);
    }
    else
    {
        fmt::print("错误码       : {}\n", at.GetLastErr());
    }
}

int Run(cvAutoTrack& at)
{
    std::ios::sync_with_stdio(false);
    // 调用循环
    while (1)
    {
        // 显示菜单
        std::cout <<
            R"(
10.可视化调试【Debug模式】
11. 初始化
=====================
-1. 获取版本号
0. 退出
请输入选项: 
)";
        int option = 0;
        std::cin >> option;
        std::cout << "\n";
        switch (option)
        {
            case 11:
                at.InitResource();
                system("pause");
                break;
            case 10:
                while (1)
                {
                    // 30ms 内检测到ECS键就退出
                    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
                    {
                        break;
                    }
                    Sleep(100);
                    Run_GetAll(at);
                }
                break;
            case 0: return 0; break;
            default: break;
        }
        // 推送io流缓存
        std::cout << std::flush;

        Sleep(30);
        system("cls");
    }
    return 0;
}
int main(int argc, char* argv[])
{
    cvAutoTrack at;

    std::vector<std::string> args;
    for (int i = 0; i < argc; i++)
    {
        args.push_back(argv[i]);
    }

    // 如果输入参数 -test 就执行测试
    if (argc > 1 && strcmp(argv[1], "-test") == 0)
    {
        return TEST(at);
    }
    else
    {
        // 否则执行正常的程序
        return Run(at);
    }
}
