#include "pch.h"
#include "Kalman.h"

Kalman::Kalman()
{
	type = FilterType::Kalman;
	
	KF = cv::KalmanFilter(stateNum, measureNum, controlNum);
	state = cv::Mat(stateNum, 1, CV_32F); 
	processNoise = cv::Mat(stateNum, 1, CV_32F);
	measurement = cv::Mat::zeros(measureNum, 1, CV_32F);	//measurement(x,y)

	randn(state, cv::Scalar::all(0), cv::Scalar::all(0.1)); //随机生成一个矩阵，期望是0，标准差为0.1;
	// set A
	KF.transitionMatrix = (cv::Mat_<float>(stateNum, stateNum) <<
		1, 0,
		0, 1);
	// set B
	KF.controlMatrix = (cv::Mat_<float>(stateNum, controlNum) <<
		1, 0,
		0, 1);
	// set Q
	setIdentity(KF.processNoiseCov, cv::Scalar::all(1e-5));
	// set H
	KF.measurementMatrix = (cv::Mat_<float>(measureNum, stateNum) <<
		1, 0,
		0, 1);
	// set R
	setIdentity(KF.measurementNoiseCov, cv::Scalar::all(1e-3));

	randn(KF.statePost, cv::Scalar::all(0), cv::Scalar::all(0.1));
}

Kalman::~Kalman()
{
}

cv::Point2d Kalman::filterting(const cv::Point2d& pos, const cv::Point2f& u_k)
{
	// KF中弃用！
	// 若要调用，相当于调用了一次predict和一次update

	// use u_k to predict
	// make u_k to cv::Mat
	cv::Mat u_k_mat = cv::Mat::zeros(controlNum, 1, CV_32F);
	u_k_mat.at<float>(0, 0) = u_k.x;
	u_k_mat.at<float>(1, 0) = u_k.y;
	cv::Mat prediction = KF.predict(u_k_mat);
	cv::Point2d predictPt = cv::Point2d(prediction.at<float>(0), prediction.at<float>(1));

	//3.update measurement
	measurement.at<float>(0, 0) = static_cast<float>(pos.x);
	measurement.at<float>(1, 0) = static_cast<float>(pos.y);

	//4.update
	KF.correct(measurement);

	cv::Point2d resP = cv::Point2d(KF.statePost.at<float>(0), KF.statePost.at<float>(1));
	return resP;
}


cv::Point2d Kalman::re_init_filterting(const cv::Point2d& pos)
{
	// set state
	KF.statePost.at<float>(0) = static_cast<float>(pos.x);
	KF.statePost.at<float>(1) = static_cast<float>(pos.y);
	// predict without u_k
	cv::Mat prediction = KF.predict();
	cv::Point2d predictPt = cv::Point2d(prediction.at<float>(0), prediction.at<float>(1));

	//3.update measurement
	measurement.at<float>(0, 0) = static_cast<float>(pos.x);
	measurement.at<float>(1, 0) = static_cast<float>(pos.y);
	KF.correct(measurement);

	cv::Point2d resP = cv::Point2d(KF.statePost.at<float>(0), KF.statePost.at<float>(1));
	return resP;
}

cv::Point2d Kalman::predict(const cv::Point2f& u_k)
{
	// use u_k to predict
	// make u_k to cv::Mat
	cv::Mat u_k_mat = cv::Mat::zeros(controlNum, 1, CV_32F);
	u_k_mat.at<float>(0, 0) = u_k.x;
	u_k_mat.at<float>(1, 0) = u_k.y;
	cv::Mat prediction = KF.predict(u_k_mat);
	cv::Point2d predictPt = cv::Point2d(prediction.at<float>(0), prediction.at<float>(1));
	return predictPt;
}

cv::Point2d Kalman::update(const cv::Point2d& pos)
{
	// update measurement
	measurement.at<float>(0, 0) = static_cast<float>(pos.x);
	measurement.at<float>(1, 0) = static_cast<float>(pos.y);
	// update
	KF.correct(measurement);
	cv::Point2d resP = cv::Point2d(KF.statePost.at<float>(0), KF.statePost.at<float>(1));
	return resP;
}