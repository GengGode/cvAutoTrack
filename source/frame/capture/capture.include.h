#pragma once
#include "../global/global.include.h"

namespace tianli::frame
{
    class capture_source : public tianli::frame_source
    {
    public:
        capture_source(std::shared_ptr<global::logger> logger = nullptr) : tianli::frame_source(logger)
        {
            this->mode = source_mode::handle;
        }
        virtual ~capture_source() = default;
        virtual bool set_handle(HWND handle = 0) = 0;
    protected:
        HWND source_handle = nullptr;
        RECT source_rect = {0, 0, 0, 0};
        RECT source_client_rect = {0, 0, 0, 0};
        cv::Size source_client_size;
        cv::Mat source_frame;
    };

} // namespace tianli::frame
