#pragma once
#include "../Filter.h"
class Kalman : public Filter
{
public:
	Kalman();
	~Kalman();
public:
	virtual cv::Point2d filterting(const cv::Point2d &pos, const cv::Point2f &u_k) override;
	virtual cv::Point2d re_init_filterting(const cv::Point2d& pos) override;
private:
	int stateNum = 2;
	int measureNum = 2;
	int controlNum = 2;

	// motion model
	// x(k) = A*x(k-1) + B*u(k) + w(k)
	// w(k) ~ N(0, Q)

	// observation model
	// z(k) = H*x(k) + v(k)
	// v(k) ~ N(0, R)

	// Where:
	// x = [x y]'
	// z = [x y]'
	// u = [dx dy]'
	// A = [1 0; 0 1]
	// B = [1 0; 0 1]
	// H = [1 0; 0 1]
	// Q = [1 0; 0 1] * 1e-5
	// R = [1 0; 0 1] * 1e-5
	cv::KalmanFilter KF;
	cv::Mat state; 
	cv::Mat processNoise;
	cv::Mat measurement;
};