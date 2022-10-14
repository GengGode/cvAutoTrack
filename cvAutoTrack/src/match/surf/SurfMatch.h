#pragma once
#include "resource.h"
#include <wincodec.h>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
class SurfMatch
{
	cv::Mat _mapMat;
	cv::Mat _minMapMat;
	cv::Mat _minMapLastMat;

	double ratio_thresh = 0.66;
	double render_map_scale = 1.3;//1.3;
	int someSizeR = 106;
	double MatchMatScale = 2.0;
	

	cv::Point2d hisP[3];

	cv::Point2d pos;
public:
	SurfMatch();
	~SurfMatch();

public:
	int minHessian = 400;

	cv::Ptr<cv::xfeatures2d::SURF> detector, detectorSomeMap;
	std::vector<cv::KeyPoint> Kp_MinMap, Kp_Map, Kp_SomeMap;
	cv::Mat Dp_MinMap, Dp_Map, Dp_SomeMap;

	bool isInit = false;
	bool isContinuity = false;
	bool isOnCity = false;

	void setMap(cv::Mat gi_map);
	void setMiniMap(cv::Mat minMapMat);

	void Init();
	void Init(std::vector<cv::KeyPoint>& gi_map_keypoints, cv::Mat& gi_map_descriptors);
	void UnInit();
	void match();

	cv::Point2d match_continuity(bool& calc_continuity_is_faile);
	cv::Point2d match_continuity_on_city(bool& calc_continuity_is_faile);
	cv::Point2d match_continuity_not_on_city(bool& calc_continuity_is_faile);

	cv::Point2d match_no_continuity(bool& calc_is_faile);

	cv::Point2d SURFMatch(cv::Mat minMapMat);
	cv::Point2d getLocalPos();
	bool getIsContinuity();
};

