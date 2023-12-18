#pragma once
#include "frame/frame.include.h"

bool create_capture_bitblt(std::shared_ptr<tianli::frame::frame_source> &source);
bool create_capture_graphics(std::shared_ptr<tianli::frame::frame_source> &source);
bool create_capture_dwm(std::shared_ptr<tianli::frame::frame_source> &source);
bool create_local_picture(std::shared_ptr<tianli::frame::frame_source> &source);
bool create_local_video(std::shared_ptr<tianli::frame::frame_source> &source);


/// @brief 设置源的客户端区域计算回调
/// @param source 帧画面源
/// @param callback 计算裁切后的客户端区域回调
/// @return 是否设置成功
bool set_source_client_rect_callback(std::shared_ptr<tianli::frame::frame_source> &source, void(*callback)(int &x, int &y, int &width, int &height));

/// @brief 
/// @param source 
/// @param handle 
/// @return 
bool set_capture_handle(std::shared_ptr<tianli::frame::frame_source> &source, HWND handle);
bool set_capture_handle_callback(std::shared_ptr<tianli::frame::frame_source> &source, HWND (*callback)());

bool set_local_file(std::shared_ptr<tianli::frame::frame_source> &source, std::string file);
bool set_local_frame(std::shared_ptr<tianli::frame::frame_source> &source, const char *image_encode_data, int image_data_size);
bool set_local_frame(std::shared_ptr<tianli::frame::frame_source> &source, const char *image_data, int image_width, int image_height, int image_channels);
bool set_source_frame_callback(std::shared_ptr<tianli::frame::frame_source> &source, std::function<void(const char *image_encode_data, int &image_data_size)> callback);
bool set_source_frame_callback(std::shared_ptr<tianli::frame::frame_source> &source, std::function<void(const char *image_data, int &image_width, int& image_height, int &image_channels)> callback);

// bool set_frame_rect_callback(std::shared_ptr<tianli::frame::frame_source> &source, std::function<void(int &x, int &y, int &width, int &height)> callback);
