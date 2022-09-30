#pragma once
#include "ErrorCode.h"
class Mat;
class Capture
{
public:
	enum Mode
	{
		Mode_Bitblt,
		Mode_DirectX,
		Mode_Unknown
	};
	ErrorCode& err = ErrorCode::getInstance();
	Mode mode = Mode_Unknown;
	Capture();
	virtual ~Capture();
	
	virtual bool init() = 0;
	virtual bool uninit() = 0;
	virtual bool capture(cv::Mat& frame) = 0;
	virtual bool setHandle(HWND handle = 0) = 0;
};

