#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <fstream>

std::string cache_name_url = "https://gitee.com/WeiXiTianLi/cv-auto-track.cache-storge/raw/v20231205/cache_name";
std::string cache_url = "https://gitee.com/WeiXiTianLi/cv-auto-track.cache-storge/raw/v20231205/";

int main()
{
    std::string get_cache_name_cmd = "curl -s " + cache_name_url + " -o cache_name";
    auto get_cache_name_res = std::system(get_cache_name_cmd.c_str());
    if(get_cache_name_res != 0)
    {
        std::cout << "get cache name failed" << std::endl;
        return 0;
    }

    std::string cache_name;
    std::ifstream cache_name_file("cache_name", std::ios::in);
    cache_name_file >> cache_name;
    cache_name_file.close();
    std::cout << "cache name:" << cache_name << std::endl;

    std::string get_cache_cmd = std::string("curl -s ") + cache_url + cache_name + " -o cvAutoTrack_Cache.dat" ;
    auto get_cache_res = std::system(get_cache_cmd.c_str());
    if(get_cache_res != 0)
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