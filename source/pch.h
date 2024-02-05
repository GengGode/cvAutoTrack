// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// Opencv
// 如果使用了c++14以上版本，关闭以下警告
#ifdef _HAS_CXX17
    #pragma warning(disable : 4828)
    #pragma warning(disable : 5054)
    #pragma warning(disable : 6201)
    #pragma warning(disable : 6294)
    #pragma warning(disable : 26451)
    #pragma warning(disable : 26495)
#endif

#ifdef SUPPORT_WINRT
    #define WIN32_LEAN_AND_MEAN // 从 Windows 头文件中排除极少使用的内容
    // Windows 头文件
    #include <Unknwn.h>
    #include <inspectable.h>
    #include <windows.h>

    // WinRT
    #include <winrt/Windows.Foundation.h>
    #include <winrt/Windows.Graphics.DirectX.Direct3d11.h>
    #include <winrt/Windows.Graphics.DirectX.h>
    #include <winrt/windows.foundation.metadata.h>

    #include <dwmapi.h>
    #pragma comment(lib, "dwmapi.lib")
#endif // SUPPORT_WINRT

// STL
#include <algorithm>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <numeric>
#include <random>
#include <string>

// opencv
// #include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>

#if (_MSC_VER && _MSVC_LANG <= 201703L) || (!_MSC_VER && __cplusplus <= 201703L)
    #include <fmt/format.h>
#else
    #include <format>
#endif

namespace global
{
#if (_MSC_VER && _MSVC_LANG <= 201703L) || (!_MSC_VER && __cplusplus <= 201703L)
    #define fmt_namespace_ fmt
#else
    #define fmt_namespace_ std
#endif

    template <typename... Args> std::string format(fmt_namespace_::format_string<Args...> fmt, Args&&... args)
    {
        return fmt_namespace_::format(fmt, std::forward<Args>(args)...);
    }
} // namespace global

#endif // PCH_H