#include "pch.h"
#include "Untouched.h"

Untouched::Untouched()
{
	type = Filter::Untouched;
}

Untouched::~Untouched()
{
}

cv::Point2d Untouched::filterting(const cv::Point2d& pos)
{
	return pos;
}

cv::Point2d Untouched::re_init_filterting(const cv::Point2d& pos)
{
	return pos;
}
