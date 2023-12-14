#pragma once
#include <opencv2/opencv.hpp>
#include "../global/global.include.h"

namespace tianli::algorithms::filter
{
    class filter
    {
    public:
        enum class filter_type
        {
            unknown,
            kalman
        };

    public:
        filter_type type = filter_type::unknown;
        std::shared_ptr<global::logger> logger;

    public:
        filter(std::shared_ptr<global::logger> logger = nullptr) : logger(logger)
        {
            if (logger == nullptr)
            {
                this->logger = std::make_shared<global::logger>();
            }
        }
        virtual ~filter() = default;

        virtual cv::Point2d filterting(const cv::Point2d &pos, const cv::Point2f &u_k) = 0;
        virtual cv::Point2d re_init_filterting(const cv::Point2d &pos) = 0;
    };

} // namespace tianli::algorithms::filter
