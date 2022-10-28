#pragma once
#include "ErrorCode.h"
class Capture
{
public:
	enum CaptureMode
	{
		Bitblt,
		DirectX,
		Video,
		Unknown
	};
	CaptureMode mode = CaptureMode::Unknown;
	ErrorCode& err = ErrorCode::getInstance();
	
	Capture();
	virtual ~Capture();
	
	virtual bool init() = 0;
	virtual bool uninit() = 0;
	virtual bool capture(cv::Mat& frame) = 0;
	virtual bool setHandle(HWND handle = 0) = 0;
};

