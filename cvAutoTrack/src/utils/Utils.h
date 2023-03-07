#pragma once

namespace TianLi::Utils
{
	struct MatchKeyPoint
	{
		cv::Point2d query;
		cv::Point2d train;
	};

	cv::Mat get_some_map(const cv::Mat& map, const cv::Point& pos, int size_r);

	double dis(cv::Point2d p);
	cv::Point2d SPC(std::vector<double> lisx, std::vector<double> lisy);
	
	double stdev(std::vector<double> list);
	double stdev(std::vector<cv::Point2d> list);
	double stdev_abs(std::vector<double> list);

	cv::Mat crop_border(const cv::Mat& mat, double border);

	std::vector<double> extract_valid(std::vector<double> list);
	std::vector<cv::Point2d> extract_valid(std::vector<cv::Point2d> list);

	void remove_invalid(std::vector<MatchKeyPoint> keypoints, double scale, std::vector<double>& x_list, std::vector<double>& y_list);


	
	int getMaxID(double lis[], int len);
	int getMinID(double lis[], int len);

	std::vector<cv::Point2f> Vector2UnitVector(std::vector<cv::Point2f> pLis);
	double Line2Angle(cv::Point2f p);
	cv::Point2d TransferTianLiAxes(cv::Point2d pos, cv::Point2d origin, double scale);
	cv::Point2d TransferUserAxes(cv::Point2d pos, double x, double y, double scale);
	cv::Point2d TransferTianLiAxes_Tr(cv::Point2d pos, cv::Point2d origin, double scale);
	cv::Point2d TransferUserAxes_Tr(cv::Point2d pos, double x, double y, double scale);
	
	std::pair<cv::Point2d, int> TransferTianLiAxes(double x, double y);


	void draw_good_matches(const cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<cv::DMatch>& good_matches);
	
	void calc_good_matches(const cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<std::vector<cv::DMatch>>& KNN_m, double ratio_thresh, std::vector<MatchKeyPoint>& good_keypoints);
	
}