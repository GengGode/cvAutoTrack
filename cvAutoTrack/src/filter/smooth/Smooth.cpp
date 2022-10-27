#include "pch.h"
#include "Smooth.h"

Smooth::Smooth()
{
	type = FilterType::Smooth;
}

Smooth::~Smooth()
{
}

cv::Point2d Smooth::filterting(const cv::Point2d& pos)
{
	return pos;
}

cv::Point2d Smooth::re_init_filterting(const cv::Point2d& pos)
{
	return pos;
}
