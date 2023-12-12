#pragma once
#include "global/global.include.h"
#include "../frame.include.h"

namespace tianli::frame::capture
{
    class capture_source : public frame_source
    {
    public:
        capture_source(std::shared_ptr<global::logger> logger = nullptr) : frame_source(logger)
        {
            this->mode = source_mode::handle;
        }
        ~capture_source() override = default;
        bool set_source_frame_callback(std::function<cv::Mat()> callback) override { return false; };

        virtual bool set_handle(HWND handle = 0) = 0;

    protected:
        std::function<HWND()> source_handle_callback;
        HWND source_handle = nullptr;
        cv::Mat source_frame;
    };

} // namespace tianli::frame
