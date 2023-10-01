#pragma once
#include "../Filter.h"
class Smooth : public Filter
{
public:
	Smooth();
	~Smooth();
public:
	virtual cv::Point2d filterting(const cv::Point2d& pos) override;
	virtual cv::Point2d re_init_filterting(const cv::Point2d& pos) override;
private:
	cv::Point2d mean_pos;
};
