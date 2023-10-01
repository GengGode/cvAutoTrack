#pragma once
#include "utils/Utils.h"

bool judgesIsOnCity(std::vector<TianLi::Utils::MatchKeyPoint> goodMatches, double minimap_scale);
std::pair<std::vector<cv::Point2d>,double> judges_scale(std::vector<TianLi::Utils::MatchKeyPoint> match_points, double scale_a, double scale_b);

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

class Match
{
public:
	struct KeyMatPoint
	{
		std::vector<cv::KeyPoint> keypoints;
		cv::Mat descriptors;
		bool empty() { return keypoints.size() == 0; }
		auto size() { return keypoints.size(); }
	};
public:
	Match(double hessian_threshold = 1, int octaves = 4, int octave_layers = 3, bool extended = false, bool upright = false);
	~Match() = default;
public:
	cv::Ptr<cv::xfeatures2d::SURF> detector;
	cv::Ptr<cv::DescriptorMatcher> matcher;
	KeyMatPoint query;
	KeyMatPoint train;
public:
	std::vector<std::vector<cv::DMatch>> match(const cv::Mat& query_descriptors, const cv::Mat& train_descriptors);
	std::vector<std::vector<cv::DMatch>> match(KeyMatPoint& query_key_mat_point, KeyMatPoint& train_key_mat_point);
	bool detect_and_compute(const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors);
	bool detect_and_compute(const cv::Mat& img, KeyMatPoint& key_mat_point);
};


class SurfMatch
{
	cv::Mat _mapMat;
	cv::Mat _miniMapMat;
	cv::Mat _miniMapLastMat;


	cv::Point2d pos;
	cv::Point2d last_pos;		// 上一次匹配的地点，匹配失败，返回上一次的结果
	cv::Rect rect_continuity_map;
public:
	SurfMatch() = default;
	~SurfMatch() = default;

public:
	Match matcher;
	
	Match::KeyMatPoint map, some_map, mini_map;

	bool isInit = false;
	bool isContinuity = false;
	bool isCoveying = false;
	bool isOnCity = false;

	int continuity_retry = 0;		//局部匹配重试次数
	const int max_continuity_retry = 3;		//最大重试次数
	
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
	//cv::Point2d match_all_map(bool& calc_is_faile,double& stdev, double minimap_scale_param = 1.0);

	cv::Point2d getLocalPos();
	bool getIsContinuity();

private:
	bool isMatchAllMap = true;
};

