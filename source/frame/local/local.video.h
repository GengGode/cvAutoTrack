#pragma once
#include "local.include.h"
#include "global/global.include.h"
#include <filesystem>

namespace tianli::frame::local
{
    class local_video : public local_source
    {
    public:
        local_video(std::shared_ptr<global::logger> logger = nullptr) : local_source(logger) { this->type = source_type::video; }
        ~local_video() override = default;

    public:
        bool initialization() override
        {
            if (this->is_callback == true && this->source_frame_callback == nullptr)
                return false;
            if (std::filesystem::exists(this->source_local) == false)
                return false;
            this->cache_frame_current = 0;
            return true;
        }
        bool uninitialized() override { return true; }
        bool set_source_frame_callback(std::function<cv::Mat()> callback) override
        {
            if (callback == nullptr)
                return false;
            this->source_frame_callback = callback;
            this->is_callback = true;
            this->cache_frame_current = 0;
            return true;
        }
        bool set_local_frame(cv::Mat frame) override
        {
            if (frame.empty())
                return false;
            this->source_frame = frame;
            this->is_callback = false;
            this->cache_frame_current = 0;
            return true;
        }
        bool set_local_file(std::string file) override
        {
            if (std::filesystem::exists(file) == false)
                return false;
            this->source_local = file;
            this->cache_frame_current = 0;
            return true;
        }
        bool get_frame(cv::Mat& frame) override
        {
            if (this->is_callback)
            {
                frame = this->source_frame_callback();
                return true;
            }
            if (cache_local_file == this->source_local)
            {
                if (this->cache_source_video.isOpened() == false)
                    this->cache_source_video.open(this->cache_local_file);
                if (this->cache_source_video.isOpened() == false)
                    return false;
                this->cache_source_video >> frame;
                if (frame.empty())
                    return false;
                return true;
            }
            this->cache_local_file = this->source_local;
            this->cache_source_video.open(this->cache_local_file);
            if (this->cache_source_video.isOpened() == false)
                return false;
            this->cache_source_video >> frame;
            if (frame.empty())
                return false;
            return true;
        }

    protected:
        cv::VideoCapture cache_source_video;
        std::string cache_local_file;
        int cache_frame_current;
    };

} // namespace tianli::frame::local
