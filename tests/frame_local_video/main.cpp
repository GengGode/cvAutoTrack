
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/opencv.hpp>

#include <frame/frame.include.h>
#include <frame/local/local.include.h>
#include <frame/local/local.video.h>
#include <global/record/record.stdlog.h>
#include <iostream>
#include <utils/convect.string.h>

int main()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    // 随机生成一个MP4视频
    auto writer = cv::VideoWriter("test.mp4", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, cv::Size(1920, 1080), true);
    cv::Mat test = cv::Mat::zeros(1080, 1920, CV_8UC3);
    for (int i = 0; i < 30; ++i)
    {
        cv::randu(test, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
        writer.write(test);
    }
    writer.release();

    auto logger = std::make_shared<tianli::global::record::std_logger>();
    auto local = std::make_shared<tianli::frame::local::local_video>(logger);
    local->set_local_file("test.mp4");
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