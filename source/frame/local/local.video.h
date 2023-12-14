#pragma once
#include "global/global.include.h"
#include "local.include.h"
#include <filesystem>

namespace tianli::frame::local
{
    class local_video : public local_source
    {
    public:
        local_video(std::shared_ptr<global::logger> logger = nullptr) : local_source(logger)
        {         
            this->type = source_type::video;
        }
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
            this->cache_frame_current = 0;
            return true;
        }
        bool set_current_file(std::string file) override
        {
            if (std::filesystem::exists(file) == false)
                return false;
            this->source_local = file;
            this->cache_frame_current = 0;
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
                if (this->cache_frame_current >= this->cache_source_frame.size())
                    this->cache_frame_current = 0;
                frame = this->cache_source_frame[this->cache_frame_current];
                return true;
            }
            this->cache_local_file = this->source_local;
            auto video = cv::VideoCapture(this->cache_local_file);
            if (video.isOpened() == false)
                return false;
            this->cache_source_frame.clear();
            while (true)
            {
                cv::Mat frame;
                video >> frame;
                if (frame.empty())
                    break;
                this->cache_source_frame.push_back(frame);
            }
            video.release();
            if (this->cache_source_frame.empty())
                return false;
            this->cache_frame_current = 0;
            frame = this->cache_source_frame[this->cache_frame_current];
            return true;
        }

    protected:
        std::vector<cv::Mat> cache_source_frame;
        std::string cache_local_file;
        int cache_frame_current;
    };

} // namespace tianli::frame::local
