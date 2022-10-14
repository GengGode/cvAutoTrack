#pragma once
#include <opencv2/opencv.hpp>
class DiffMatch
{
	cv::Mat _minimap_last_mat;
	cv::Mat _minimap_now_mat;
	
	double _render_map_scale = 1.3;

	cv::Point2d _diff_pos;
public:
	DiffMatch();
	~DiffMatch();
public:
	void setInitMap(cv::Mat minimap_last_mat);
	void setMiniMap(cv::Mat minimap_now_mat);
	
	void match();
	cv::Point2d match(cv::Mat minimap_now_mat);
	cv::Point2d getDiffPos();
};
