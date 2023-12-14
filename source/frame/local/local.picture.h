#pragma once
#include "global/global.include.h"
#include "local.include.h"
#include <filesystem>

namespace tianli::frame::local
{
    class local_picture : public local_source
    {
    public:
        local_picture(std::shared_ptr<global::logger> logger = nullptr) : local_source(logger)
        {
            this->type = source_type::picture;
        }
        ~local_picture() override = default;

    public:
        bool initialization() override
        {
            if (this->is_callback == true && this->source_frame_callback == nullptr)
                return false;
            if (std::filesystem::exists(this->source_local) == false)
                return false;
            return true;
        }
        bool uninitialized() override
        {
            return true;
        }
        bool set_source_frame_callback(std::function<cv::Mat()> callback) override
        {
            if (callback == nullptr)
                return false;
            this->source_frame_callback = callback;
            this->is_callback = true;
            return true;
        }
        bool set_current_file(std::string file) override
        {
            if (std::filesystem::exists(file) == false)
                return false;
            this->source_local = file;
            return true;
        }
        bool get_frame(cv::Mat &frame) override
        {
            if (this->is_callback)
            {
                frame = this->source_frame_callback();
                return true;
            }
            if (cache_local_file == this->source_local)
            {
                frame = this->cache_source_frame;
                return true;
            }
            this->cache_local_file = this->source_local;
            frame = cv::imread(this->cache_local_file);
            this->cache_source_frame = frame;
            return true;
        }

    protected:
        cv::Mat cache_source_frame;
        std::string cache_local_file;
    };

} // namespace tianli::frame::local
