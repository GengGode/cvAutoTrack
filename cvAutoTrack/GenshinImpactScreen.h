#pragma once
#include <time.h>
#include <Windows.h>
#include <opencv2/opencv.hpp>

#include "ErrorCode.h"

using namespace cv;

#ifndef _CLOCK_T_DEFINED
typedef long clock_t;
#define _CLOCK_T_DEFINED   
#endif 

class GenshinImpactScreen
{
	clock_t LastTimeMs = 0;
	clock_t NowTimeMs = 0;

	Mat Screen;
	
	ErrorCode& err = ErrorCode::getInstance();

	GenshinImpactScreen();
public:
	GenshinImpactScreen* getInstance();
	Mat getGenshinImpactScreen();
private:
	void getNowTimeMs();
	void updateTime();

	Mat getGenshinImpactScreen_GetAfter();
	Mat getGenshinImpactScreen_NewGet();
};

