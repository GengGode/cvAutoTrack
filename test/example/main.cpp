#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <iostream>
double get_window_screen_scale()
{
    HWND desktop_hwnd = GetDesktopWindow();
    HMONITOR monitor_handle = MonitorFromWindow(desktop_hwnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFOEX miex;
    miex.cbSize = sizeof(miex);
    GetMonitorInfo(monitor_handle, &miex);
    int screen_x_logical = (miex.rcMonitor.right - miex.rcMonitor.left);

    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm);
    int screen_x_physical = dm.dmPelsWidth;

    double screen_scale = ((double)screen_x_physical / (double)screen_x_logical);
    return screen_scale;
}

double get_window_screen_scale_2()
{
    return GetDpiForWindow(GetDesktopWindow()) / 96.0;
}
double get_window_screen_scale_3()
{
    HDC hdc = GetDC(GetDesktopWindow());
    int width = GetDeviceCaps(hdc, DESKTOPHORZRES);
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    double scale = (double)width / (double)screenW;
    return scale;
}
TEST(Example, Example)
{
    // std::cout << "screen scale: " << get_window_screen_scale() << std::endl;
    // std::cout << "screen scale 2: " << get_window_screen_scale_2() << std::endl;
    // std::cout << "screen scale 3: " << get_window_screen_scale_3() << std::endl;
    ASSERT_TRUE(true);
}