#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <iostream>
#include "frame/capture/capture.include.h"
#include "frame/capture/capture.bitblt.h"

int main()
{
    auto capture = std::make_shared<tianli::capture::capture_bitblt>();
    capture->set_handle(GetDesktopWindow());
    cv::Mat frame;
    capture->get_frame(frame);
    if (frame.empty())
        std::cout << "frame is empty" << std::endl;
    else
        std::cout << "frame is not empty" << std::endl;
    cv::imwrite("test.png", frame);
    return 0;
}