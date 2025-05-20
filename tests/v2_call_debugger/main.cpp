#include <Windows.h>
#include <cvAutoTrack.h>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <vector>
#ifdef _DEBUG
    #include <filesystem>
#endif // _DEBUG

int TEST_Version()
{
    char version_buff[256] = { 0 };
    if (GetCompileVersion(version_buff, 256))
        fmt::print("版本号       : {}\n", version_buff);
    else
        fmt::print("错误码       : {}\n", GetLastErr());

    char version_time_buff[256] = { 0 };
    if (GetCompileTime(version_time_buff, 256))
        fmt::print("编译时间     : {}\n", version_time_buff);
    else
        fmt::print("错误码       : {}\n", GetLastErr());

    std::cout << "测试完成\n";
    return 0;
}

int TEST()
{
    auto v2 = create_cvAutoTrack_context_v2();
    if (v2 == nullptr)
    {
        std::cout << "创建上下文失败\n";
        return 0;
    }
    bool ret = false;
    ret = v2->Call("debugger", "on", 2, nullptr, 0);
    if (ret == false)
    {
        std::cout << "debugger on 调用失败\n";
        return 0;
    }
    Sleep(300);
    ret = v2->Call("debugger", "off", 3, nullptr, 0);
    if (ret == false)
    {
        std::cout << "debugger off 调用失败\n";
        return 0;
    }
    Sleep(100);
    ret = v2->Call("debugger", "on", 2, nullptr, 0);
    if (ret == false)
    {
        std::cout << "debugger on 调用失败\n";
        return 0;
    }
    Sleep(100);
    ret = v2->Call("capture", "current", 7, nullptr, 0);
    if (ret == false)
    {
        std::cout << "capture current 调用失败\n";
        return 0;
    }

    ret = v2->Call("debugger", "wait", 4, nullptr, 0);
    if (ret == false)
    {
        std::cout << "debugger wait 调用失败\n";
        return 0;
    }
    destroy_cvAutoTrack_context_v2(v2);
    return 0;
}

int main(int argc, char* argv[])
{
    system("chcp 65001");
    TEST_Version();
    return TEST();
}
