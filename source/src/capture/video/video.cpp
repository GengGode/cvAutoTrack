#include "pch.h"
#include "video.h"

Video::Video()
{
	mode = Capture::Video;
}

Video::~Video()
{
}

bool Video::init()
{
	return false;
}

bool Video::uninit()
{
	return false;
}

bool Video::capture(cv::Mat& frame)
{
	UNREFERENCED_PARAMETER(frame);
	return false;
}

bool Video::setHandle(HWND handle)
{
	UNREFERENCED_PARAMETER(handle);
	return false;
}
