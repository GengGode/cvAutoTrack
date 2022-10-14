#pragma once
#include "../Filter.h"
class Untouched : public Filter
{
public:
	Untouched();
	~Untouched();
public:
	virtual cv::Point2d filterting(const cv::Point2d& pos) override;
	virtual cv::Point2d re_init_filterting(const cv::Point2d& pos) override;
};