#include "pch.h"
#include "module.frame.h"
#include "frame/capture/capture.bitblt.h"
#ifdef BUILD_CAPTURE_DXGI
    #include "frame/capture/capture.window_graphics.h"
#endif // BUILD_CAPTURE_DXGI
// #include "frame/capture/capture.dwm.h"
#include "frame/local/local.picture.h"
#include "frame/local/local.video.h"

bool create_capture_bitblt(std::shared_ptr<tianli::frame::frame_source>& source)
{
    if (source == nullptr)
    {
        source = std::make_shared<tianli::frame::capture::capture_bitblt>();
        return true;
    }
    if (source->type == tianli::frame::frame_source::source_type::bitblt)
    {
        return true;
    }
    source.reset();
    source = std::make_shared<tianli::frame::capture::capture_bitblt>();
    return true;
}
bool create_capture_graphics(std::shared_ptr<tianli::frame::frame_source>& source)
{
#ifdef BUILD_CAPTURE_DXGI
    if (source == nullptr)
    {
        source = std::make_shared<tianli::frame::capture::capture_window_graphics>();
        return true;
    }
    if (source->type == tianli::frame::frame_source::source_type::window_graphics)
    {
        return true;
    }
    source.reset();
    source = std::make_shared<tianli::frame::capture::capture_window_graphics>();
    return true;
#else
    return false;
#endif // BUILD_CAPTURE_DXGI
}
bool create_capture_dwm(std::shared_ptr<tianli::frame::frame_source>& source)
{
    return false;
}
bool create_local_picture(std::shared_ptr<tianli::frame::frame_source>& source)
{
    source = std::make_shared<tianli::frame::local::local_picture>();
    return true;
}
bool create_local_video(std::shared_ptr<tianli::frame::frame_source>& source)
{
    source = std::make_shared<tianli::frame::local::local_video>();
    return true;
}

/// @brief 设置源的客户端区域计算回调
/// @param source 帧画面源
/// @param callback 计算裁切后的客户端区域回调
/// @return 是否设置成功
bool set_source_client_rect_callback(std::shared_ptr<tianli::frame::frame_source>& source, void (*callback)(int& x, int& y, int& width, int& height))
{
    return source->set_frame_rect_callback([callback](cv::Rect rect) {
        int x = rect.x;
        int y = rect.y;
        int width = rect.width;
        int height = rect.height;
        callback(x, y, width, height);
        return cv::Rect(x, y, width, height);
    });
}

/// @brief
/// @param source
/// @param handle
/// @return
bool set_capture_handle(std::shared_ptr<tianli::frame::frame_source>& source, HWND handle)
{
    return source->set_capture_handle(handle);
}
bool set_capture_handle_callback(std::shared_ptr<tianli::frame::frame_source>& source, HWND (*callback)())
{
    return source->set_source_handle_callback(callback);
}
bool set_local_file(std::shared_ptr<tianli::frame::frame_source>& source, std::string file)
{
    return source->set_local_file(file);
}
bool set_local_frame(std::shared_ptr<tianli::frame::frame_source>& source, const char* image_encode_data, int image_data_size)
{
    return source->set_local_frame(cv::imdecode(cv::Mat(1, image_data_size, CV_8UC1, (void*)image_encode_data), cv::IMREAD_UNCHANGED));
}
bool set_local_frame(std::shared_ptr<tianli::frame::frame_source>& source, const char* image_data, int image_width, int image_height, int image_channels)
{
    return source->set_local_frame(cv::Mat(image_height, image_width, CV_8UC(image_channels), (void*)image_data));
}
bool set_source_frame_callback(std::shared_ptr<tianli::frame::frame_source>& source, std::function<void(const char* image_encode_data, int& image_data_size)> callback)
{
    auto _callback = [callback]() {
        const char* image_encode_data = nullptr;
        int image_data_size = 0;
        callback(image_encode_data, image_data_size);
        return cv::imdecode(cv::Mat(1, image_data_size, CV_8UC1, (void*)image_encode_data), cv::IMREAD_UNCHANGED);
    };
    return source->set_source_frame_callback(_callback);
}
bool set_source_frame_callback(std::shared_ptr<tianli::frame::frame_source>& source, std::function<void(const char* image_data, int& image_width, int& image_height, int& image_channels)> callback)
{
    auto _callback = [callback]() {
        const char* image_data = nullptr;
        int image_width = 0;
        int image_height = 0;
        int image_channels = 0;
        callback(image_data, image_width, image_height, image_channels);
        return cv::Mat(image_height, image_width, CV_8UC(image_channels), (void*)image_data);
    };
    return source->set_source_frame_callback(_callback);
}

// bool set_frame_rect_callback(std::shared_ptr<tianli::frame::frame_source> &source, std::function<void(int &x, int &y,
// int &width, int &height)> callback)
// {
//     return source->set_frame_rect_callback([callback](cv::Rect rect)
//                                            {
//                                                int x = rect.x;
//                                                int y = rect.y;
//                                                int width = rect.width;
//                                                int height = rect.height;
//                                                callback(x, y, width, height);
//                                                return cv::Rect(x, y, width, height);
//                                            });
// }