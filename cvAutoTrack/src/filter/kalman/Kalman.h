#pragma once
#include "../Filter.h"
class Kalman : public Filter
{
public:
	Kalman();
	~Kalman();
public:
	virtual cv::Point2d filterting(const cv::Point2d& pos) override;
	virtual cv::Point2d re_init_filterting(const cv::Point2d& pos) override;
private:
	int stateNum = 4;
	int measureNum = 2;

	cv::KalmanFilter KF;
	cv::Mat state; /* (phi, delta_phi) */
	cv::Mat processNoise;
	cv::Mat measurement;
};