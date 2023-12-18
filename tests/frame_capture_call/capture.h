#pragma once
#include <Windows.h>
#include <shellscalingapi.h>
#pragma comment(lib, "Shcore.lib")
#include <iostream>
#include <opencv2/opencv.hpp>

class LockCycleTime
{
    int _frame_rate = 30;
    long long _now_time = 0;
    long long _last_time = 0;

public:
    LockCycleTime(int frame_rate)
    {
        _frame_rate = frame_rate;
        _last_time = GetTickCount64();
    }
    void wait_time()
    {
        _now_time = GetTickCount64();
        auto wait_time = 1000 / _frame_rate - (_now_time - _last_time);
        if (wait_time > 0)
            Sleep(static_cast<DWORD>(wait_time));
        _last_time = _now_time;
    }
};

cv::Mat gi_frame()
{
    static HWND gi_handle = nullptr;
    static RECT gi_window_rect = {0, 0, 0, 0};
    static RECT gi_client_rect = {0, 0, 0, 0};
    static double gi_frame_scale = 1.0;
    static cv::Size re_size_frame = {1920,1080};
    static cv::Mat gi_frame;

    if (gi_handle == nullptr)
    {
        gi_handle = FindWindowW(L"UnityWndClass", L"\u539F\u795E");
        if (gi_handle == nullptr)
        {
            std::cout << "FindWindowW failed" << std::endl;
            return cv::Mat();
        }
    }
    {
        GetWindowRect(gi_handle, &gi_window_rect);
        GetClientRect(gi_handle, &gi_client_rect);
    }
    {
        HMONITOR monitor_handle = MonitorFromWindow(gi_handle, MONITOR_DEFAULTTONEAREST);
        UINT dpi_x, dpi_y;
        GetDpiForMonitor(monitor_handle, MDT_EFFECTIVE_DPI, &dpi_x, &dpi_y);
        gi_frame_scale = dpi_x / 96.0;
    }
    {
        double x = gi_client_rect.right - gi_client_rect.left;
        double y = gi_client_rect.bottom - gi_client_rect.top;

        if (x / y == 16.0 / 9.0)
        {
            re_size_frame = cv::Size(1920, 1080);
        }
        else if (x / y > 16.0 / 9.0)
        {
            re_size_frame = cv::Size(static_cast<int>(x / (y / 1080.0)), 1080);
        }
        else if (x / y < 16.0 / 9.0)
        {
            re_size_frame = cv::Size(1920, static_cast<int>(y / (x / 1920.0)));
        }
    }
    {
        int w = static_cast<int>(std::round((gi_client_rect.right - gi_client_rect.left) / gi_frame_scale));
        int h = static_cast<int>(std::round((gi_client_rect.bottom - gi_client_rect.top) / gi_frame_scale));

        HDC dc_handle = GetDC(gi_handle);
        HDC dc_comp_handle = CreateCompatibleDC(dc_handle);
        HBITMAP bmp_handle;
        bmp_handle = CreateCompatibleBitmap(dc_handle, w, h);
        SelectObject(dc_comp_handle, bmp_handle);
        BitBlt(dc_comp_handle, 0, 0, w, h, dc_handle, 0, 0, SRCCOPY);
        DeleteDC(dc_handle);
        DeleteDC(dc_comp_handle);
        BITMAP bmp;
        GetObject(bmp_handle, sizeof(BITMAP), &bmp);
        int frame_channels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
        cv::Mat tmp_frame(cv::Size(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8U, frame_channels));
        GetBitmapBits(bmp_handle, bmp.bmHeight * bmp.bmWidth * frame_channels, tmp_frame.data);
        if (tmp_frame.empty())
        {
            std::cout << "GetBitmapBits failed" << std::endl;
            return cv::Mat();
        }
        if (frame_channels == 3)
        {
            cvtColor(tmp_frame, tmp_frame, cv::COLOR_RGB2RGBA);
        }
        cv::resize(tmp_frame, gi_frame, re_size_frame);
    }

    return gi_frame;
}

void show_frame(cv::Mat frame)
{
    if (frame.empty())
    {
        std::cout << "frame is empty" << std::endl;
        return;
    }
    cv::imshow("frame", frame);
    cv::waitKey(1);
}

void test(std::function<bool(const cv::Mat &)> func_test, int frame_rate = 30)
{
    auto lct = LockCycleTime(frame_rate);
    while (1)
    {
        auto frame = gi_frame();
        if (frame.empty() || frame.channels()==1)
        {
            std::cout << "frame is empty" << std::endl;
        }
        else
        {
            auto res = func_test(frame);
        }
        // show_frame(frame);
        lct.wait_time();
    }
}

void test_local(std::function<bool(const cv::Mat &)> func_test, std::string path = "img.png", int frame_rate = 30)
{
    auto lct = LockCycleTime(frame_rate);
    while (1)
    {
        auto frame = cv::imread(path, -1);
        if (frame.empty())
        {
            std::cout << "frame is empty" << std::endl;
        }
        else
        {
            auto res = func_test(frame);
        }
        // show_frame(frame);
        lct.wait_time();
    }
}
