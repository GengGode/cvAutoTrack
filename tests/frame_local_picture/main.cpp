#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <Windows.h>
#include <iostream>
#include <frame/frame.include.h>
#include <frame/local/local.include.h>
#include <frame/local/local.picture.h>
#include <utils/convect.string.h>
#include <global/record/record.stdlog.h>

int main()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    // 随机生成图片
    cv::Mat test = cv::Mat::zeros(1080, 1920, CV_8UC3);
    cv::randu(test, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
    cv::imwrite("test.png", test);
    
    auto logger = std::make_shared<tianli::global::record::std_logger>();
    auto local = std::make_shared<tianli::frame::local::local_picture>(logger);
    local->set_current_file("test.png");
    cv::Mat frame;
    local->get_frame(frame);
    if (frame.empty())
        std::cout << "frame is empty" << std::endl;
    else
    {
        std::cout << "frame is not empty" << std::endl;
        cv::imwrite("test_out.png", frame);
    }

    return 0;
}