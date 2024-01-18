#pragma once
#include <Windows.h>
namespace tianli::frame::capture::utils::window_scale
{
    namespace window_last_version
    {
        static double get_screen_scale(HWND current_handle = nullptr)
        {
            auto handle = current_handle;
            if (handle == nullptr)
                handle = GetDesktopWindow();
            HMONITOR monitor_handle = MonitorFromWindow(handle, MONITOR_DEFAULTTONEAREST);

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
    } // namespace window_last_version

    namespace window_current_version
    {
        static double get_screen_scale(HWND current_handle = nullptr)
        {
            auto handle = current_handle;
            if (handle == nullptr)
                handle = GetDesktopWindow();
            return GetDpiForWindow(handle) / 96.0;
        }
    } // namespace window_current_version

    namespace window_first_version
    {
        static double get_screen_scale(HWND current_handle = nullptr)
        {
            auto handle = current_handle;
            if (handle == nullptr)
                handle = GetDesktopWindow();
            HDC hdc = GetDC(handle);
            int x = GetDeviceCaps(hdc, DESKTOPHORZRES);
            int screen_x = GetSystemMetrics(SM_CXSCREEN);
            double scale = (double)x / (double)screen_x;
            return scale;
        }
    } // namespace window_first_version

#if defined(_WIN32) || defined(_WIN64)
    #if defined(_MSC_VER)
        #if _MSC_VER >= 1920
            #define WINDOW_VERSION window_last_version
        #elif _MSC_VER >= 1910
            #define WINDOW_VERSION window_current_version
        #else
            #define WINDOW_VERSION window_first_version
        #endif
    #else
        #define WINDOW_VERSION window_first_version
    #endif
#else
    #define WINDOW_VERSION window_first_version
#endif
    using namespace WINDOW_VERSION;
} // namespace tianli::frame::capture::utils::window_scale
