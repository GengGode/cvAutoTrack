#include <Windows.h>
#include <filesystem>
#include <fstream>
#include <iostream>

std::string cache_url = "https://gitee.com/WeiXiTianLi/cv-auto-track.cache-storge/releases/download/v20231205/cvAutoTrack_Cache.dat";

int main()
{
    std::string get_cache_cmd = std::string("curl -LJO ") + cache_url + " -o cvAutoTrack_Cache.dat";
    auto get_cache_res = std::system(get_cache_cmd.c_str());
    if (get_cache_res != 0)
    {
        std::cout << "get cache failed" << std::endl;
        return 0;
    }
    if (std::filesystem::exists("cvAutoTrack_Cache.dat") == false)
    {
        std::cout << "get cache failed" << std::endl;
        return 0;
    }

    std::cout << "cmd:" << get_cache_cmd << std::endl;
    std::cout << "get cache success" << std::endl;
}