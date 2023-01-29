#pragma once
#include "resource.h"
#include <wincodec.h>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>

bool save_map_keypoint_cache(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, double hessian_threshold = 1, int octaves = 1, int octave_layers = 1, bool extended = false, bool upright = false);
bool load_map_keypoint_cache(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, double hessian_threshold = 1, int octaves = 1, int octave_layers = 1, bool extended = false, bool upright = false);
bool get_map_keypoint(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, double hessian_threshold = 1, int octaves = 1, int octave_layers = 1, bool extended = false, bool upright = false);

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
	double hessian_threshold = 1;
	int octaves = 4;
	int octave_layers = 3;
	bool extended = false;
	bool upright = false;
		
	cv::Ptr<cv::xfeatures2d::SURF> detector, detectorSomeMap;
	std::vector<cv::KeyPoint> Kp_MinMap, Kp_Map, Kp_SomeMap;
	cv::Mat Dp_MinMap, Dp_Map, Dp_SomeMap;

	bool isInit = false;
	bool isContinuity = false;
	bool isOnCity = false;
	
	bool is_success_match = false;

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
	cv::Point2d match_no_continuity_2th(bool& calc_is_faile);

	cv::Point2d SURFMatch(cv::Mat minMapMat);
	cv::Point2d getLocalPos();
	bool getIsContinuity();
};

