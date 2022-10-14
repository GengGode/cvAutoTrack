#pragma once
#include "../Capture.h"
class Bitblt : public Capture
{
public:
    Bitblt();
	~Bitblt();
	
	bool init() override;
	bool uninit() override;
	bool capture(cv::Mat& frame) override;
	bool setHandle(HWND handle = 0) override;
private:
	HWND giHandle = nullptr;
	RECT giRect = { 0,0,0,0 };
	RECT giClientRect = { 0,0,0,0 };
	cv::Size giClientSize;
	cv::Mat giFrame;
};

