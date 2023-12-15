#pragma once
#include "../global/global.include.h"

namespace tianli::frame
{
    class frame_source
    {
    public:
        enum class source_type
        {
            unknown,
            bitblt,
            window_graphics,
            video,
            picture
        };
        enum class source_mode
        {
            handle,
            frame
        };

    public:
        source_type type = source_type::unknown;
        source_mode mode = source_mode::handle;
        std::shared_ptr<global::logger> logger;
        bool is_initialized = false;
        bool is_callback = false;

    public:
        frame_source(std::shared_ptr<global::logger> logger = nullptr) : logger(logger)
        {
            if (logger == nullptr)
            {
                this->logger = std::make_shared<global::logger>();
            }
        }
        virtual ~frame_source() = default;
        virtual bool initialization() { return false; }
        virtual bool uninitialized() { return false; }
        virtual bool get_frame(cv::Mat &frame) = 0;
        virtual bool set_handle(HWND handle = 0) = 0;
        virtual bool set_current_file(std::string file) = 0;
        virtual bool set_source_handle_callback(std::function<HWND()> callback) = 0;
        virtual bool set_source_frame_callback(std::function<cv::Mat()> callback) = 0;
    };

} // namespace tianli::frame
