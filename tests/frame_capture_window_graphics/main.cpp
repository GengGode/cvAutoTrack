#include <opencv2/core/utils/logger.hpp>
#include <opencv2/imgcodecs.hpp>

#include <frame/frame.include.h>
#include <global/record/record.stdlog.h>
#include <iostream>
#include <utils/convect.string.h>

#include <Windows.h>

std::vector<HWND> find_window_form_title(const std::wstring& title)
{
    std::vector<HWND> windows;
    EnumWindows(
        [](HWND hwnd, LPARAM lParam) -> BOOL {
            auto windows = reinterpret_cast<std::vector<HWND>*>(lParam);
            wchar_t buffer[1024];
            GetWindowTextW(hwnd, buffer, 1024);
            if (std::wstring(buffer) == L"云·原神")
                windows->push_back(hwnd);
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&windows));
    return windows;
}

HWND get_cloud_window()
{
    HWND cloud_window = NULL;
    EnumWindows(
        [](HWND hwnd, LPARAM lParam) -> BOOL {
            auto cloud_window = reinterpret_cast<HWND*>(lParam);
            wchar_t buffer[1024];
            auto style = GetWindowLongPtr(hwnd, GWL_STYLE);
            GetWindowTextW(hwnd, buffer, 1024);
            if (std::wstring(buffer) == L"云·原神" && (style & WS_EX_LAYERED))
            {
                *cloud_window = hwnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&cloud_window));
    return cloud_window;
}

cv::Mat get_frame_form_window(HWND handle)
{
    cv::Mat frame;
    auto logger = std::make_shared<tianli::global::record::std_logger>();
    auto capture = tianli::frame::frame_source::create(tianli::frame::frame_source::source_type::window_graphics, logger);

    capture->set_capture_handle(handle);
    std::this_thread::sleep_for(std::chrono::milliseconds(18));
    capture->get_frame(frame);
    if (frame.empty())
        std::cout << "frame is empty" << std::endl;
    else
    {
        std::cout << "frame is not empty" << std::endl;
        cv::imwrite("test.png", frame);
    }
    std::cout << frame.size() << std::endl;

    capture->set_capture_handle(GetDesktopWindow());
    std::this_thread::sleep_for(std::chrono::milliseconds(18));
    capture->get_frame(frame);
    if (frame.empty())
        std::cout << "frame is empty" << std::endl;
    else
    {
        std::cout << "frame is not empty" << std::endl;
        cv::imwrite("test.png", frame);
    }
    std::cout << frame.size() << std::endl;

    capture->set_source_handle_callback([]() { return FindWindowW(NULL, utils::to_wstring("原神").c_str()); });
    std::this_thread::sleep_for(std::chrono::milliseconds(18));
    capture->get_frame(frame);
    if (frame.empty())
        std::cout << "frame is empty" << std::endl;
    else
    {
        std::cout << "frame is not empty" << std::endl;
        cv::imwrite("test.png", frame);
        cv::imshow("gi", frame);
        cv::waitKey(10);
    }
    std::cout << frame.size() << std::endl;

    return frame;
}

int main()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    auto windows = find_window_form_title(L"云·原神");
    for (auto window : windows)
    {
        std::wcout << window << std::endl;
        // get rect
        RECT rect;
        GetWindowRect(window, &rect);
        std::wcout << rect.right - rect.left << " " << rect.bottom - rect.top << std::endl;

        auto style = GetWindowLongPtr(window, GWL_STYLE);
        std::wcout << std::hex << style << std::endl;

        auto long_style = GetWindowLongPtr(window, GWL_STYLE);
        std::wcout << std::hex << long_style << std::endl;

        auto frame = get_frame_form_window(window);
        if (!frame.empty())
            cv::imwrite(std::to_string((long long)window) + ".png", frame);
    }

    HWND max_handle = NULL;
    for (auto window : windows)
    {
        auto style = GetWindowLongPtr(window, GWL_STYLE);
        // WS_EX_LAYERED
        if (style & WS_EX_LAYERED)
        {
            max_handle = window;
            break;
        }
    }

    std::wcout << "-----------" << std::endl;
    std::wcout << max_handle << std::endl;

    auto frame = get_frame_form_window(max_handle);
    if (!frame.empty())
        cv::imwrite(std::to_string((long long)max_handle) + ".png", frame);

    std::wcout << "-----------" << std::endl;
    auto get_handle = get_cloud_window();
    std::wcout << get_handle << std::endl;

    frame = get_frame_form_window(get_handle);
    if (!frame.empty())
        cv::imwrite(std::to_string((long long)get_handle) + ".png", frame);

    return 0;
}