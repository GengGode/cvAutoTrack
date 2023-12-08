#pragma once
#include "../global/global.include.h"

class capture   
{
public:
	enum capture_type
	{
        bitblt,
        window_graphics,
        local_source,
		unknown
	};
    enum capture_mode
    {
        handle,
        frame,
		unknown
    };
	capture_type type = capture_type::unknown;
    capture_mode mode = capture_mode::unknown;
    std::shared_ptr<error_logger> log;

	capture(std::shared_ptr<error_logger> log) = default;
	virtual ~capture() = default;
	
	virtual bool init() = 0;
	virtual bool uninit() = 0;
	virtual bool get_frame(cv::Mat& frame) = 0;
	virtual bool set_handle(HWND handle = 0) = 0;
    virtual bool set_capture_handle_callback(HWND (*callback)());
    virtual bool set_frame_source_callback(void(*callback)(cv::Mat&));
};

