#pragma once
#include <string>
#include <Windows.h>
namespace utils
{
    static std::string utf8_to_gbk(const std::string &utf8_string)
    {
        std::string ret_string;
        int len = MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, NULL, 0);
        wchar_t *gbk_wstring = new wchar_t[len + 1];
        memset(gbk_wstring, 0, len * 2 + 2);
        MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, gbk_wstring, len);
        len = WideCharToMultiByte(CP_ACP, 0, gbk_wstring, -1, NULL, 0, NULL, NULL);
        char *gbk_string = new char[len + 1];
        memset(gbk_string, 0, len + 1);
        WideCharToMultiByte(CP_ACP, 0, gbk_wstring, -1, gbk_string, len, NULL, NULL);
        ret_string = gbk_string;
        delete[] gbk_string;
        delete[] gbk_wstring;
        return ret_string;
    }

    static std::string gbk_to_utf8(const std::string &gbk_string)
    {
        std::string ret_string;
        int len = MultiByteToWideChar(CP_ACP, 0, gbk_string.c_str(), -1, NULL, 0);
        wchar_t *utf8_wstring = new wchar_t[len + 1];
        memset(utf8_wstring, 0, len * 2 + 2);
        MultiByteToWideChar(CP_ACP, 0, gbk_string.c_str(), -1, utf8_wstring, len);
        len = WideCharToMultiByte(CP_UTF8, 0, utf8_wstring, -1, NULL, 0, NULL, NULL);
        char *utf8_string = new char[len + 1];
        memset(utf8_string, 0, len + 1);
        WideCharToMultiByte(CP_UTF8, 0, utf8_wstring, -1, utf8_string, len, NULL, NULL);
        ret_string = utf8_string;
        delete[] utf8_string;
        delete[] utf8_wstring;
        return ret_string;
    }

    static std::string to_string(const std::wstring &wstring)
    {
        std::string ret_string;
        int len = WideCharToMultiByte(CP_ACP, 0, wstring.c_str(), -1, NULL, 0, NULL, NULL);
        char *utf8_string = new char[len + 1];
        memset(utf8_string, 0, len + 1);
        WideCharToMultiByte(CP_ACP, 0, wstring.c_str(), -1, utf8_string, len, NULL, NULL);
        ret_string = utf8_string;
        delete[] utf8_string;
        return ret_string;
    }

    static std::wstring to_wstring(const std::string &string)
    {
        std::wstring ret_string;
        int len = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, NULL, 0);
        wchar_t *utf8_wstring = new wchar_t[len + 1];
        memset(utf8_wstring, 0, len * 2 + 2);
        MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, utf8_wstring, len);
        ret_string = utf8_wstring;
        delete[] utf8_wstring;
        return ret_string;
    }

    static bool is_utf8(const std::string &string)
    {
        int len = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, NULL, 0);
        wchar_t *utf8_wstring = new wchar_t[len + 1];
        memset(utf8_wstring, 0, len * 2 + 2);
        MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, utf8_wstring, len);
        len = WideCharToMultiByte(CP_ACP, 0, utf8_wstring, -1, NULL, 0, NULL, NULL);
        char *gbk_string = new char[len + 1];
        memset(gbk_string, 0, len + 1);
        WideCharToMultiByte(CP_ACP, 0, utf8_wstring, -1, gbk_string, len, NULL, NULL);
        std::string ret_string = gbk_string;
        delete[] gbk_string;
        delete[] utf8_wstring;
        return ret_string == string;
    }

    static std::string to_utf8(const std::string &string)
    {
        if (is_utf8(string))
            return string;
        else
            return gbk_to_utf8(string);
    }

    static std::string to_gbk(const std::string &string)
    {
        if (is_utf8(string))
            return utf8_to_gbk(string);
        else
            return string;
    }
}