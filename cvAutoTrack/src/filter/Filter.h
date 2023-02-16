#pragma once
#include "ErrorCode.h"
class Point2d;
class Filter
{
public:
	Filter();
	virtual ~Filter();
public:
	virtual cv::Point2d filterting(const cv::Point2d& pos) = 0;
	virtual cv::Point2d re_init_filterting(const cv::Point2d& pos) = 0;
public:
	enum FilterType
	{
		Kalman,
		Smooth,
		Untouched,
		Unknown
	};
public:
	FilterType type = FilterType::Unknown;
};