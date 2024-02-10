#include <opencv2/core/utils/logger.hpp>
#include <opencv2/opencv.hpp>

#include <frame/capture/capture.include.h>
#include <frame/capture/capture.window_graphics.h>
#include <frame/frame.include.h>
#include <global/record/record.stdlog.h>
#include <iostream>
#include <utils/convect.string.h>

#include <Windows.h>


int main()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    cv::Mat frame;
    auto logger = std::make_shared<tianli::global::record::std_logger>();
    auto capture = std::make_shared<tianli::frame::capture::capture_window_graphics>(logger);
    capture->set_capture_handle(GetForegroundWindow());
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
        cv::waitKey(0);
    }
    std::cout << frame.size() << std::endl;
    return 0;
}