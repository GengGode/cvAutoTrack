#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

namespace TianLi::Utils
{
	double dis(cv::Point2d p);
	cv::Point2d SPC(std::vector<double> lisx, double sumx, std::vector<double> lisy, double sumy);
	double var(std::vector<double> lisx, double sumx, std::vector<double> lisy, double sumy);
	
	int getMaxID(double lis[], int len);
	int getMinID(double lis[], int len);

	std::vector<cv::Point2f> Vector2UnitVector(std::vector<cv::Point2f> pLis);
	double Line2Angle(cv::Point2f p);
	cv::Point2d TransferTianLiAxes(cv::Point2d pos, cv::Point2d origin, double scale);
	cv::Point2d TransferUserAxes(cv::Point2d pos, double x, double y, double scale);
	cv::Point2d TransferTianLiAxes_Tr(cv::Point2d pos, cv::Point2d origin, double scale);
	cv::Point2d TransferUserAxes_Tr(cv::Point2d pos, double x, double y, double scale);


	void draw_good_matches(cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<cv::DMatch>& good_matches);

	void calc_good_matches(cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<std::vector<cv::DMatch>>& KNN_m, double ratio_thresh, double mapScale, std::vector<double>& lisx, std::vector<double>& lisy, double& sumx, double& sumy);

}