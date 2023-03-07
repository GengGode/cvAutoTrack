#pragma once
#include "resource.h"
#include "utils/Utils.h"
#include <wincodec.h>
#include <opencv2/opencv.hpp>

bool save_map_keypoint_cache(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, double hessian_threshold = 1, int octaves = 1, int octave_layers = 1, bool extended = false, bool upright = false);
bool load_map_keypoint_cache(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, double hessian_threshold = 1, int octaves = 1, int octave_layers = 1, bool extended = false, bool upright = false);
bool get_map_keypoint(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, double hessian_threshold = 1, int octaves = 1, int octave_layers = 1, bool extended = false, bool upright = false);

bool judgesIsOnCity(std::vector<TianLi::Utils::MatchKeyPoint> goodMatches);

// 城镇外确认最小匹配点数量，大于该值即为城镇外
constexpr int NOT_ON_CITY__MIN_GOODMATCHS = 20;
// 城镇外尝试使用城镇内缩放计算的最小匹配点数量，小于该值即匹配失败
constexpr int NOT_ON_CITY__ON_CITY_MIN_GOODMATCHS = 8;
// 城镇内确认最小匹配点数量，大于该值即为城镇内
constexpr int ON_CITY__MIN_GOODMATCHS = 20;
// 全图匹配时城镇缩放下匹配点方差阈值，范围内即为城镇内
constexpr double ALL_MAP__ON_CITY__STDEV_THRESH = 50;
// 全图匹配时城镇外缩放下匹配点方差阈值，范围内即为城镇外
constexpr double ALL_MAP__NOT_ON_CITY__STDEV_THRESH = 500;

// 特征点匹配的剔除因子，越大越严格
constexpr double SURF_MATCH_RATIO_THRESH = 0.66;
// 地图和小地图野外的缩放比例，（大地图 / 小地图野外）得到，注意城镇内小地图是野外的两倍，所以是城镇内比例是1.3/2
constexpr double MAP_BOTH_SCALE_RATE = 1.3;
// 地图中取小部分区域的半径，目前为小地图标准半径
constexpr int DEFAULT_SOME_MAP_SIZE_R = 106;

class SurfMatch
{
	cv::Mat _mapMat;
	cv::Mat _miniMapMat;
	cv::Mat _miniMapLastMat;


	cv::Point2d pos;
	cv::Rect rect_continuity_map;
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
	std::vector<cv::KeyPoint> Kp_MiniMap, Kp_Map, Kp_SomeMap;
	cv::Mat Dp_MiniMap, Dp_Map, Dp_SomeMap;

	bool isInit = false;
	bool isContinuity = false;
	bool isCoveying = false;
	bool isOnCity = false;

	int max_continuity_retry = 3;
	
	bool is_success_match = false;

	void setMap(cv::Mat gi_map);
	void setMiniMap(cv::Mat miniMapMat);

	void Init();
	void Init(std::vector<cv::KeyPoint>& gi_map_keypoints, cv::Mat& gi_map_descriptors);
	void UnInit();
	void match();

	cv::Point2d match_continuity(bool& calc_continuity_is_faile);
	cv::Point2d match_continuity_on_city(bool& calc_continuity_is_faile);
	cv::Point2d match_continuity_not_on_city(bool& calc_continuity_is_faile);

	cv::Point2d match_no_continuity(bool& calc_is_faile);
	cv::Point2d match_no_continuity_1st(bool& calc_is_faile);

	//全图匹配
	cv::Point2d match_all_map(bool& calc_is_faile,double& stdev, double minimap_scale_param = 1.0);

	cv::Point2d getLocalPos();
	bool getIsContinuity();
private:

	cv::Mat crop_border(cv::Mat& mat, double border)
	{
		int crop_size = static_cast<int>((mat.rows + mat.cols) * 0.5 * border);
		return mat(cv::Rect(crop_size, crop_size, mat.cols - crop_size * 2, mat.rows - crop_size * 2));
	}
};

