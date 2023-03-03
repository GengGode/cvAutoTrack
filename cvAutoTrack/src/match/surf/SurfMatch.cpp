#include "pch.h"
#include "SurfMatch.h"
#include "match/type/MatchType.h"
#include "resources/Resources.h"
#include "utils/Utils.h"

bool save_map_keypoint_cache(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, double hessian_threshold, int octaves, int octave_layers, bool extended, bool upright)
{
	cv::Ptr<cv::xfeatures2d::SURF>  detector = cv::xfeatures2d::SURF::create(hessian_threshold, octaves, octave_layers, extended, upright);
	detector->detectAndCompute(Resources::getInstance().MapTemplate, cv::noArray(), keypoints, descriptors);

	cv::FileStorage fs("cvAutoTrack_Cache.xml", cv::FileStorage::WRITE);

	std::string build_time = __DATE__ " " __TIME__;
	fs << "build_time" << build_time;
	
	fs << "hessian_threshold" << hessian_threshold;
	fs << "octaves" << octaves;
	fs << "octave_layers" << octave_layers;
	fs << "extended" << extended;
	fs << "upright" << upright;
	
	fs << "keypoints" << keypoints;
	fs << "descriptors" << descriptors;
	fs.release();
	return true;
}
bool load_map_keypoint_cache(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, double hessian_threshold, int octaves, int octave_layers, bool extended, bool upright)
{
	if (std::filesystem::exists("cvAutoTrack_Cache.xml") == false)
	{
		return false;
	}
	cv::FileStorage fs("cvAutoTrack_Cache.xml", cv::FileStorage::READ);
	double r_hessian_threshold = 1;
	int    r_octaves = 1;
	int    r_octave_layers = 1;
	bool   r_extended = false;
	bool   r_upright = false;
	
	fs["hessian_threshold"]>> r_hessian_threshold;
	fs["octaves"]          >> r_octaves;
	fs["octave_layers"]    >> r_octave_layers;
	fs["extended"]         >> r_extended;
	fs["upright"]          >> r_upright;
	
	if (r_hessian_threshold != hessian_threshold || r_octaves != octaves || r_octave_layers != octave_layers || r_extended != extended || r_upright != upright)
	{
		return false;
	}
	
	fs["keypoints"] >> keypoints;
	fs["descriptors"] >> descriptors;
	fs.release();
	return true;
}

bool get_map_keypoint(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, double hessian_threshold, int octaves, int octave_layers, bool extended, bool upright)
{
	if (std::filesystem::exists("cvAutoTrack_Cache.xml") == false)
	{
		return save_map_keypoint_cache(keypoints, descriptors, hessian_threshold, octaves, octave_layers, extended, upright);
	}
	else
	{
		if (load_map_keypoint_cache(keypoints, descriptors, hessian_threshold, octaves, octave_layers, extended, upright) == false)
		{
			return save_map_keypoint_cache(keypoints, descriptors, hessian_threshold, octaves, octave_layers, extended, upright);
		}
		else
		{
			return true;
		}
	}
}

SurfMatch::SurfMatch()
{
	hisP[0] = cv::Point();
	hisP[1] = cv::Point();
	hisP[2] = cv::Point();
}

SurfMatch::~SurfMatch()
{
}

void SurfMatch::setMap(cv::Mat gi_map)
{
	_mapMat = gi_map;
}

void SurfMatch::setMiniMap(cv::Mat miniMapMat)
{
	_miniMapMat = miniMapMat;
}

void SurfMatch::Init()
{
	if (isInit)return;
	detector = cv::xfeatures2d::SURF::create(hessian_threshold, octaves, octave_layers, extended, upright);
	detector->detectAndCompute(_mapMat, cv::noArray(), Kp_Map, Dp_Map);
	isInit = true;
}

void SurfMatch::Init(std::vector<cv::KeyPoint>& gi_map_keypoints, cv::Mat& gi_map_descriptors)
{
	if (isInit)return;	
	detector = cv::xfeatures2d::SURF::create(hessian_threshold, octaves, octave_layers, extended, upright);
	Kp_Map = std::move(gi_map_keypoints);
	Dp_Map = std::move(gi_map_descriptors);
	isInit = true;
}

void SurfMatch::UnInit()
{
	if (!isInit)return;
	_mapMat.release();
	_mapMat = cv::Mat();
	Kp_Map.clear();
	Dp_Map.release();
	isInit = false;
}

void SurfMatch::match()
{
	cv::Point2d dp1 = hisP[1] - hisP[0];
	cv::Point2d dp2 = hisP[2] - hisP[1];

	bool calc_is_faile = false;
	is_success_match = false;
	isContinuity = false;

	// 角色移动连续性判断
	if (((TianLi::Utils::dis(dp1) + TianLi::Utils::dis(dp2)) < 2000) && (hisP[2].x > someSizeR && hisP[2].x < _mapMat.cols - someSizeR && hisP[2].y>someSizeR && hisP[2].y < _mapMat.rows - someSizeR))
	{
		isContinuity = true;
	}
	// 尝试连续匹配，匹配角色附近小范围区域
	if (isContinuity)
	{
		bool calc_continuity_is_faile = false;
		pos = match_continuity(calc_continuity_is_faile);
		// 连续匹配失败
		if (calc_continuity_is_faile)
		{
			isContinuity = false;
		}
	}
	// 直接非连续匹配，匹配整个大地图
	if (!isContinuity)
	{
		pos = match_no_continuity(calc_is_faile);
	}
	// 没有有效结果，结束
	if (calc_is_faile)
	{
		return;
	}

	hisP[0] = hisP[1];
	hisP[1] = hisP[2];
	hisP[2] = pos;
	is_success_match = true;

}

cv::Point2d  SurfMatch::match_continuity(bool& calc_continuity_is_faile)
{
	cv::Point2d pos_continuity;

	if (isOnCity == false)
	{
		pos_continuity = match_continuity_not_on_city(calc_continuity_is_faile);
	}
	else
	{
		pos_continuity = match_continuity_on_city(calc_continuity_is_faile);
	}
	
	if (calc_continuity_is_faile)
	{
		pos_continuity = match_continuity_not_on_city(calc_continuity_is_faile);
	}

	return pos_continuity;
}
/// <summary>
/// 匹配 在城镇内
/// </summary>
/// <param name="calc_continuity_is_faile"></param>
/// <returns></returns>
cv::Point2d SurfMatch::match_continuity_on_city(bool& calc_continuity_is_faile)
{
	static cv::Mat img_scene(_mapMat);
	//const auto minimap_scale_param = 2.0;

	cv::Point2d pos_on_city;
	
	cv::Mat img_object = crop_border(_miniMapMat,0.15);

	//在城镇中
	/***********************/
	//重新从完整中地图取出角色周围部分地图
	cv::Mat someMap(img_scene(cv::Rect(static_cast<int>(hisP[2].x - someSizeR), static_cast<int>(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)));
	cv::Mat MiniMap(img_object);

	cv::resize(someMap, someMap, cv::Size(someSizeR * 4, someSizeR * 4), cv::INTER_CUBIC);
	//resize(MiniMap, MiniMap, cv::Size(0, 0), minimap_scale_param, minimap_scale_param, cv::INTER_CUBIC);

	detectorSomeMap = cv::xfeatures2d::SURF::create(hessian_threshold, octaves, octave_layers, extended, upright);
	detectorSomeMap->detectAndCompute(someMap, cv::noArray(), Kp_SomeMap, Dp_SomeMap);
	detectorSomeMap->detectAndCompute(MiniMap, cv::noArray(), Kp_MiniMap, Dp_MiniMap);

	if (Kp_SomeMap.size() <= 2 || Kp_MiniMap.size() <= 2)
	{
		calc_continuity_is_faile = true;
		return pos_on_city;
	}

	cv::Ptr<cv::DescriptorMatcher> matcherTmp = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<cv::DMatch> > KNN_mTmp;

	matcherTmp->knnMatch(Dp_MiniMap, Dp_SomeMap, KNN_mTmp, 2);

	//TianLi::Utils::calc_good_matches(someMap, Kp_SomeMap, img_object, Kp_MiniMap, KNN_mTmp, ratio_thresh, 0.8667/*/ minimap_scale_param*/, lisx, lisy, sumx, sumy);
	
	std::vector<TianLi::Utils::KeyPoint> keypoint_on_city_list;
	TianLi::Utils::calc_good_matches(someMap, Kp_SomeMap, img_object, Kp_MiniMap, KNN_mTmp, ratio_thresh, keypoint_on_city_list);

	std::vector<double> lisx;
	std::vector<double> lisy;
	TianLi::Utils::remove_invalid(keypoint_on_city_list, 0.8667 , lisx, lisy);


	std::vector<cv::Point2d> list_on_city;
	for (int i = 0; i < keypoint_on_city_list.size(); i++)
	{
		list_on_city.push_back(cv::Point2d(lisx[i], lisx[i]));
	}
	list_on_city = TianLi::Utils::extract_valid(list_on_city);

	lisx.clear();
	lisy.clear();
	for (int i = 0; i < list_on_city.size(); i++)
	{
		lisx.push_back(list_on_city[i].x);
		lisy.push_back(list_on_city[i].y);
	}
	
	if (std::max(lisx.size(), lisy.size()) <= NOT_ON_CITY__ON_CITY_MIN_GOODMATCHS)
	{
		calc_continuity_is_faile = true;
		return pos_on_city;
	}
	
	//// 根据匹配点的方差判断点集是否合理，偏差过大即为无效数据
	//if (TianLi::Utils::is_valid_keypoints(lisx, sumx, lisy, sumy, someSizeR/4.0) == false)
	//{
	//	calc_continuity_is_faile = true;
	//	return pos_on_city;
	//}

	if (std::min(lisx.size(), lisy.size()) >= ON_CITY__MIN_GOODMATCHS)
	{
		isOnCity = true;
	}
	else
	{
		isOnCity = false;
	}

	cv::Point2d pos_continuity_on_city = TianLi::Utils::SPC(lisx, lisy);

	pos_continuity_on_city.x = (pos_continuity_on_city.x - someMap.cols / 2.0) / 2.0;
	pos_continuity_on_city.y = (pos_continuity_on_city.y - someMap.rows / 2.0) / 2.0;

	pos_on_city = cv::Point2d(pos_continuity_on_city.x + hisP[2].x, pos_continuity_on_city.y + hisP[2].y);

	return pos_on_city;
}
/// <summary>
/// 匹配 在野外
/// </summary>
/// <param name="calc_continuity_is_faile"></param>
/// <returns></returns>
cv::Point2d SurfMatch::match_continuity_not_on_city(bool& calc_continuity_is_faile)
{
	static cv::Mat img_scene(_mapMat);
	const auto minimap_scale_param = 2.0;

	cv::Point2d pos_not_on_city;

	cv::Mat img_object = crop_border(_miniMapMat, 0.15);
	//不在城镇中时
	cv::Mat someMap(img_scene(cv::Rect(static_cast<int>(hisP[2].x - someSizeR), static_cast<int>(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)));
	cv::Mat miniMap(img_object);
	cv::Mat miniMap_scale = img_object.clone();
	
	cv::resize(miniMap_scale, miniMap_scale, cv::Size(0, 0), minimap_scale_param, minimap_scale_param, cv::INTER_CUBIC);

	detectorSomeMap = cv::xfeatures2d::SURF::create(hessian_threshold, octaves, octave_layers, extended, upright);
	detectorSomeMap->detectAndCompute(someMap, cv::noArray(), Kp_SomeMap, Dp_SomeMap);
	detectorSomeMap->detectAndCompute(miniMap_scale, cv::noArray(), Kp_MiniMap, Dp_MiniMap);

	// 如果搜索范围内可识别特征点数量为0，则认为计算失败
	if (Kp_SomeMap.size() <= 2 || Kp_MiniMap.size() <= 2)
	{
		calc_continuity_is_faile = true;
		return pos_not_on_city;
	}
	cv::Ptr<cv::DescriptorMatcher> matcher_not_on_city = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<cv::DMatch> > KNN_not_no_city;

	matcher_not_on_city->knnMatch(Dp_MiniMap, Dp_SomeMap, KNN_not_no_city, 2);

	std::vector<TianLi::Utils::KeyPoint> keypoint_not_on_city_list;
	TianLi::Utils::calc_good_matches(someMap, Kp_SomeMap, miniMap_scale, Kp_MiniMap, KNN_not_no_city, ratio_thresh, keypoint_not_on_city_list);

	std::vector<double> lisx;
	std::vector<double> lisy;
	TianLi::Utils::remove_invalid(keypoint_not_on_city_list, render_map_scale / minimap_scale_param, lisx, lisy);
	
	std::vector<cv::Point2d> list_not_on_city;
	for (int i = 0; i < keypoint_not_on_city_list.size(); i++)
	{
		list_not_on_city.push_back(cv::Point2d(lisx[i], lisx[i]));
	}
	list_not_on_city = TianLi::Utils::extract_valid(list_not_on_city);

	lisx.clear();
	lisy.clear();
	for (int i = 0; i < list_not_on_city.size(); i++)
	{
		lisx.push_back(list_not_on_city[i].x);
		lisy.push_back(list_not_on_city[i].y);
	}
	
	// 如果范围内最佳匹配特征点对数量大于4，则认为不可能处于城镇之中，位于城镇之外
	if (std::min(lisx.size(), lisy.size()) > NOT_ON_CITY__MIN_GOODMATCHS)
	{
		isOnCity = false;
		cv::Point2d p = TianLi::Utils::SPC(lisx, lisy);
		pos_not_on_city = cv::Point2d(p.x + hisP[2].x - someSizeR, p.y + hisP[2].y - someSizeR);
		return pos_not_on_city;
	}
	
	//// 根据匹配点的方差判断点集是否合理，偏差过大即为无效数据
	//if (TianLi::Utils::is_valid_keypoints(lisx, sumx, lisy, sumy, someSizeR / 4.0) == true)
	//{
	//	isOnCity = false;
	//	cv::Point2d p = TianLi::Utils::SPC(lisx, sumx, lisy, sumy);
	//	pos_not_on_city = cv::Point2d(p.x + hisP[2].x - someSizeR, p.y + hisP[2].y - someSizeR);
	//	return pos_not_on_city;
	//}
	
	cv::Point2d pos_on_city;
	
	img_scene(cv::Rect(static_cast<int>(hisP[2].x - someSizeR), static_cast<int>(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)).copyTo(someMap);

	cv::resize(someMap, someMap, cv::Size(someSizeR * 4, someSizeR * 4), cv::INTER_CUBIC);

	detectorSomeMap = cv::xfeatures2d::SURF::create(hessian_threshold, octaves, octave_layers, extended, upright);
	detectorSomeMap->detectAndCompute(someMap, cv::noArray(), Kp_SomeMap, Dp_SomeMap);
	detectorSomeMap->detectAndCompute(miniMap, cv::noArray(), Kp_MiniMap, Dp_MiniMap);

	if (Kp_SomeMap.size() == 0 || Kp_MiniMap.size() == 0)
	{
		calc_continuity_is_faile = true;
		return pos_not_on_city;
}

	cv::Ptr<cv::DescriptorMatcher> matcher_mabye_on_city = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<cv::DMatch> > KNN_mabye_on_city;

	matcher_mabye_on_city->knnMatch(Dp_MiniMap, Dp_SomeMap, KNN_mabye_on_city, 2);
	
	std::vector<TianLi::Utils::KeyPoint> keypoint_on_city_list;
	TianLi::Utils::calc_good_matches(someMap, Kp_SomeMap, miniMap, Kp_MiniMap, KNN_mabye_on_city, ratio_thresh, keypoint_on_city_list);

	std::vector<double> list_x_on_city;
	std::vector<double> list_y_on_city;
	TianLi::Utils::remove_invalid(keypoint_on_city_list, 0.8667, list_x_on_city, list_y_on_city);

	std::vector<cv::Point2d> list_on_city;
	for (int i = 0; i < keypoint_on_city_list.size(); i++)
	{
		list_on_city.push_back(cv::Point2d(list_x_on_city[i], list_y_on_city[i]));
	}
	list_on_city = TianLi::Utils::extract_valid(list_on_city);
	
	list_x_on_city.clear();
	list_y_on_city.clear();
	for (int i = 0; i < list_on_city.size(); i++)
	{
		list_x_on_city.push_back(list_on_city[i].x);
		list_y_on_city.push_back(list_on_city[i].y);
	}

	if (std::min(list_x_on_city.size(), list_y_on_city.size()) <= NOT_ON_CITY__ON_CITY_MIN_GOODMATCHS)
	{
		calc_continuity_is_faile = true;
		return pos_not_on_city;
	}

	if (std::min(list_x_on_city.size(), list_y_on_city.size()) >= ON_CITY__MIN_GOODMATCHS)
	{
		isOnCity = true;
	}
	else
	{
		isOnCity = false;
	}

	cv::Point2d p = TianLi::Utils::SPC(list_x_on_city, list_y_on_city);

	double x = (p.x - someMap.cols / 2.0) / 2.0;
	double y = (p.y - someMap.rows / 2.0) / 2.0;

	pos_on_city = cv::Point2d(x + hisP[2].x, y + hisP[2].y);
	return pos_on_city;
}
/// <summary>
/// 匹配 不连续 全局匹配
/// </summary>
/// <param name="calc_is_faile"></param>
/// <returns></returns>
cv::Point2d SurfMatch::match_no_continuity(bool& calc_is_faile)
{
#ifdef _2TH_MATCH
	return match_no_continuity_2nd(calc_is_faile);
#else
	return match_no_continuity_1st(calc_is_faile);
#endif // _2TH_MATCH
}


/// <summary>
/// 非连续匹配，从大地图中确定角色位置
/// 直接通过SURF特征点匹配精确定位角色位置
/// </summary>
/// <param name="calc_is_faile">匹配结果是否有效</param>
/// <returns></returns>
cv::Point2d SurfMatch::match_no_continuity_1st(bool& calc_is_faile)
{
	static cv::Mat img_scene(_mapMat);
	const auto minimap_scale_param = 2.0;
	cv::Point2d pos_continuity_no;

	cv::Mat img_object = crop_border(_miniMapMat, 0.15);
	cv::resize(img_object, img_object, cv::Size(0, 0), minimap_scale_param, minimap_scale_param,cv::INTER_CUBIC);
	
	// 小地图区域计算特征点
	detector->detectAndCompute(img_object, cv::noArray(), Kp_MiniMap, Dp_MiniMap);
	// 没有提取到特征点直接返回，结果无效
	if (Kp_MiniMap.size() == 0)
	{
		calc_is_faile = true;
		return pos_continuity_no;
	}
	// 匹配特征点
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<cv::DMatch> > KNN_m;
	matcher->knnMatch(Dp_MiniMap, Dp_Map, KNN_m, 2);

	std::vector<TianLi::Utils::KeyPoint> keypoint_list;
	TianLi::Utils::calc_good_matches(img_scene, Kp_Map, img_object, Kp_MiniMap, KNN_m, ratio_thresh, keypoint_list);

	std::vector<double> lisx;
	std::vector<double> lisy;
	TianLi::Utils::remove_invalid(keypoint_list, render_map_scale / minimap_scale_param, lisx, lisy);

	std::vector<cv::Point2d> list_not_on_city;
	for (int i = 0; i < keypoint_list.size(); i++)
	{
		list_not_on_city.push_back(cv::Point2d(lisx[i], lisx[i]));
	}
	list_not_on_city = TianLi::Utils::extract_valid(list_not_on_city);

	lisx.clear();
	lisy.clear();
	for (int i = 0; i < list_not_on_city.size(); i++)
	{
		lisx.push_back(list_not_on_city[i].x);
		lisy.push_back(list_not_on_city[i].y);
	}
	
	// 没有最佳匹配结果直接返回，结果无效
	if (std::min(lisx.size(), lisy.size()) == 0)
	{
		calc_is_faile = true;
		return pos_continuity_no;
	}
	// 从最佳匹配结果中剔除异常点计算角色位置返回
	pos_continuity_no = TianLi::Utils::SPC(lisx, lisy);
	return pos_continuity_no;
}


cv::Point2d SurfMatch::getLocalPos()
{
	return pos;
}

bool SurfMatch::getIsContinuity()
{
	return isContinuity;
}
