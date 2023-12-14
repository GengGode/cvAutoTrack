#pragma once
#include "global/global.include.h"
#include "../frame.include.h"

namespace tianli::frame::local
{
    class local_source : public frame_source
    {
    public:
        local_source(std::shared_ptr<global::logger> logger = nullptr) : frame_source(logger)
        {
            this->mode = source_mode::frame;
        }
        ~local_source() override = default;
        bool set_source_handle_callback(std::function<HWND()> callback) override { return false; };

        virtual bool set_current_file(std::string file) = 0;

    protected:
        std::function<cv::Mat()> source_frame_callback;
        std::string source_local;
        cv::Mat source_frame;
    };

} // namespace tianli::frame::local
