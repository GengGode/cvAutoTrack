#pragma once
#include "../frame.include.h"
#include "global/global.include.h"

namespace tianli::frame::capture
{
    class capture_source : public frame_source
    {
    public:
        capture_source(std::shared_ptr<global::logger> logger = nullptr) : frame_source(logger) { this->mode = source_mode::handle; }
        ~capture_source() override = default;
        bool set_source_frame_callback(std::function<cv::Mat()> callback) override { return false; }

        bool set_local_frame(cv::Mat frame) override { return false; }
        bool set_local_file(std::string file) override { return false; }
        // TODO: frame_source::set_source_frame_callback
        bool set_frame_rect_callback(std::function<cv::Rect(cv::Rect)> callback) override { return false; }

    protected:
        std::function<HWND()> source_handle_callback;
        HWND source_handle = nullptr;
        cv::Mat source_frame;
    };

} // namespace tianli::frame::capture
