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
	mean_pos =  pos * 0.1 + mean_pos * 0.9;
	return mean_pos;
}

cv::Point2d Smooth::re_init_filterting(const cv::Point2d& pos)
{
	mean_pos = pos;
	return filterting(pos);
}
