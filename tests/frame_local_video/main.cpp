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
    return 0;
}