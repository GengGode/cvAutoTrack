#pragma once
#include "../Capture.h"

class Video :public Capture
{
public:
	Video();
	~Video();

	bool init() override;
	bool uninit() override;
	bool capture(cv::Mat& frame) override;
	bool setHandle(HWND handle = 0) override;
private:
	
};