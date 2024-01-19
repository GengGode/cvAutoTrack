#pragma once
#include "../frame.include.h"
#include "global/global.include.h"

namespace tianli::frame::local
{
    class local_source : public frame_source
    {
    public:
        local_source(std::shared_ptr<global::logger> logger = nullptr) : frame_source(logger) { this->mode = source_mode::frame; }
        ~local_source() override = default;
        bool set_source_handle_callback(std::function<HWND()> callback) override { return false; };

        bool set_capture_handle(HWND handle = 0) override { return false; }
        // TODO: frame_source::set_source_frame_callback
        bool set_frame_rect_callback(std::function<cv::Rect(cv::Rect)> callback) override { return false; }

    protected:
        std::function<cv::Mat()> source_frame_callback;
        std::string source_local;
        cv::Mat source_frame;
    };

} // namespace tianli::frame::local
