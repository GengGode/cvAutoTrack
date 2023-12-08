#pragma once
#include "../global/global.include.h"
#include "../capture/capture.include.h"
#include "utils/utils.window.h"

namespace tianli::capture
{
    class capture_bitblt : public tianli::capture_source
    {
    public:
        capture_bitblt(std::shared_ptr<global::logger> logger = nullptr) : tianli::capture_source(logger)
        {
            this->type = source_type::bitblt;
        }
        ~capture_bitblt() = default;
    public:
        bool initialization() override
        {
            if (IsWindow(this->source_handle) == false)
                return false;
            return true;
        }
        bool uninitialized() override
        {
            return true;
        }
        bool set_handle(HWND handle = 0) override{
            if (handle == nullptr)
                return false;
            if (handle == this->source_handle)
                return true;
            if(uninitialized() == false)
                return false;
            this->source_handle = handle;
            if(initialization() == false)
                return false;
                this->is_callback = false;
            return true;
        }
        bool set_source_handle_callback(std::function<HWND()> callback) override
        {
            if (callback == nullptr)
                return false;
            if (this->source_handle_callback == callback)
                return true;
            if (uninitialized() == false)
                return false;
            this->source_handle_callback = callback;
            if (initialization() == false)
                return false;
            this->is_callback = true;
            return true;
        }
        bool get_frame(cv::Mat &frame) override
        {
            auto handle = this->source_handle;
            if (this->is_callback)
                handle = this->source_handle_callback();
            if (handle == nullptr)
                return false;
                if(IsWindow(handle) == false)
                    return false;
            RECT rect = {0, 0, 0, 0};
            if (GetWindowRect(handle, &rect) == false)
                return false;
                RECT client_rect = {0, 0, 0, 0};
            if (GetClientRect(handle, &client_rect) == false)
                return false;
            double screen_scale = utils::window::get_screen_scale(handle);
            cv::Size client_size = {0, 0};
            client_size.width = (int)(screen_scale * (client_rect.right - client_rect.left));
            client_size.height = (int)(screen_scale * (client_rect.bottom - client_rect.top));
            HDC hdc = GetDC(handle);
            HDC hmemdc = CreateCompatibleDC(hdc);
            HBITMAP hbitmap = CreateCompatibleBitmap(hdc, client_size.width, client_size.height);
            SelectObject(hmemdc, hbitmap);
            BitBlt(hmemdc, 0, 0, client_size.width, client_size.height, hdc, 0, 0, SRCCOPY);
            DeleteDC(hmemdc);
            ReleaseDC(handle, hdc);
            GetObject(hbitmap, sizeof(BITMAP), &this->source_bitmap);
            int nChannels = this->source_bitmap.bmBitsPixel == 1 ? 1 : this->source_bitmap.bmBitsPixel / 8;
            this->source_frame.create(cv::Size(this->source_bitmap.bmWidth, this->source_bitmap.bmHeight), CV_MAKETYPE(CV_8U, nChannels));
            GetBitmapBits(hbitmap, this->source_bitmap.bmHeight * this->source_bitmap.bmWidth * nChannels, this->source_frame.data);
            this->source_frame = this->source_frame(cv::Rect(client_rect.left, client_rect.top, client_size.width, client_size.height));
            if (this->source_frame.empty())
                return false;
            if (this->source_frame.cols < 480 || this->source_frame.rows < 360)
                return false;
            frame = this->source_frame;
            return true;
        }

    };

} // namespace tianli::capture
