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
	return false;
}

bool Video::setHandle(HWND handle)
{
	return false;
}
