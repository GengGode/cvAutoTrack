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
	bool calc_is_faile = false;
	is_success_match = false;
	isContinuity = true;

	// 尝试连续匹配，匹配角色附近小范围区域
	if (isContinuity)
	{
		for (int retry_times = 1; retry_times <= max_continuity_retry; retry_times++)
		{
			bool calc_continuity_is_faile = false;
			pos = match_continuity(calc_continuity_is_faile);

			if (!calc_continuity_is_faile)
				break;				//匹配成功，结束，否则重试
			else if (retry_times == max_continuity_retry)
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

	is_success_match = true;
}

bool judgesIsOnCity(std::vector<TianLi::Utils::MatchKeyPoint> goodMatches)
{
	auto cvPoint2dDistance = [](cv::Point2d a, cv::Point2d b){
		return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
	};
	auto get_average = [](const std::vector<float>& v) {
		return std::accumulate(v.begin(), v.end(), 0.0) / (float)v.size();
	};
	auto get_sigma = [](const std::vector<float>& v,const float average) {
		return sqrt(std::accumulate(v.begin(), v.end(), 0.0, [&](const float acc, const float x) {return acc + pow(x - average, 2); }) / (float)v.size());
	};

	std::vector<float> vec_distRatio;
	float distScene, distObject;
	int goodMatchesSize = goodMatches.size();

	if (goodMatchesSize < 5)
		return true;			//识别的点数少于5，可以认为在城里

	std::default_random_engine rand_engine;
	int rand_idx1, rand_idx2;
	for (int i = 0; i < 100; i++)
	{
		rand_idx1 = rand_engine() % goodMatchesSize;
		rand_idx2 = rand_engine() % goodMatchesSize;
		distScene = cvPoint2dDistance(goodMatches[rand_idx1].query, goodMatches[rand_idx2].query);
		distObject = cvPoint2dDistance(goodMatches[rand_idx1].train, goodMatches[rand_idx2].train);
		if (isfinite(distScene / distObject))
			vec_distRatio.emplace_back(distScene / distObject);
	}
	//计算距离比例的均值和标准差
	float e_distRatio, s_distRatio;
	e_distRatio = get_average(vec_distRatio);
	s_distRatio = get_sigma(vec_distRatio, e_distRatio);
	//剔除误差过大的距离，1倍sigma
	for (int i = 0; i < vec_distRatio.size(); i++)
	{
		if (abs(vec_distRatio[i]-e_distRatio) > 1 * s_distRatio)
		{
			vec_distRatio.erase(vec_distRatio.begin() + i);
			i--;
		}
	}
	if (vec_distRatio.size() == 0) return true;		//找不到追踪点，大概率在城内

	//重新计算距离比例均值，并判断是否接近城外缩放比例
	e_distRatio = get_average(vec_distRatio);

	if (e_distRatio < 1)
		return true;
	return false;
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
	cv::Mat someMap = TianLi::Utils::get_some_map(img_scene, pos, someSizeR);
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
	
	std::vector<TianLi::Utils::MatchKeyPoint> keypoint_on_city_list;
	TianLi::Utils::calc_good_matches(someMap, Kp_SomeMap, img_object, Kp_MiniMap, KNN_mTmp, ratio_thresh, keypoint_on_city_list);

	std::vector<double> lisx;
	std::vector<double> lisy;
	TianLi::Utils::remove_invalid(keypoint_on_city_list, 0.8667 , lisx, lisy);


	std::vector<cv::Point2d> list_on_city;
	for (int i = 0; i < keypoint_on_city_list.size(); i++)
	{
		list_on_city.push_back(cv::Point2d(lisx[i], lisy[i]));
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
	
	isOnCity = judgesIsOnCity(keypoint_on_city_list);

	cv::Point2d pos_continuity_on_city = TianLi::Utils::SPC(lisx, lisy);

	pos_continuity_on_city.x = (pos_continuity_on_city.x - someMap.cols / 2.0) / 2.0;
	pos_continuity_on_city.y = (pos_continuity_on_city.y - someMap.rows / 2.0) / 2.0;

	pos_on_city = cv::Point2d(pos_continuity_on_city.x + pos.x, pos_continuity_on_city.y + pos.y);

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
	cv::Mat someMap = TianLi::Utils::get_some_map(img_scene, pos, someSizeR);
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

	std::vector<TianLi::Utils::MatchKeyPoint> keypoint_not_on_city_list;
	TianLi::Utils::calc_good_matches(someMap, Kp_SomeMap, miniMap_scale, Kp_MiniMap, KNN_not_no_city, ratio_thresh, keypoint_not_on_city_list);

	std::vector<double> lisx;
	std::vector<double> lisy;
	TianLi::Utils::remove_invalid(keypoint_not_on_city_list, render_map_scale / minimap_scale_param, lisx, lisy);
	
	std::vector<cv::Point2d> list_not_on_city;
	for (int i = 0; i < keypoint_not_on_city_list.size(); i++)
	{
		list_not_on_city.push_back(cv::Point2d(lisx[i], lisy[i]));
	}
	list_not_on_city = TianLi::Utils::extract_valid(list_not_on_city);

	lisx.clear();
	lisy.clear();
	for (int i = 0; i < list_not_on_city.size(); i++)
	{
		lisx.push_back(list_not_on_city[i].x);
		lisy.push_back(list_not_on_city[i].y);
	}
	

	if (!judgesIsOnCity(keypoint_not_on_city_list))
	{
		isOnCity = false;
		cv::Point2d p = TianLi::Utils::SPC(lisx, lisy);
		pos_not_on_city = cv::Point2d(p.x + pos.x - someSizeR, p.y + pos.y - someSizeR);
		return pos_not_on_city;
	}
	
	cv::Point2d pos_on_city;
	someMap = TianLi::Utils::get_some_map(img_scene, pos, someSizeR);
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
	
	std::vector<TianLi::Utils::MatchKeyPoint> keypoint_on_city_list;
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

	isOnCity = judgesIsOnCity(keypoint_on_city_list);

	cv::Point2d p = TianLi::Utils::SPC(list_x_on_city, list_y_on_city);

	double x = (p.x - someMap.cols / 2.0) / 2.0;
	double y = (p.y - someMap.rows / 2.0) / 2.0;

	pos_on_city = cv::Point2d(x + pos.x, y + pos.y);
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
	double on_city_stdev = 0.0;
	double not_on_city_stdev = 0.0;

	if (isOnCity)
	{
		bool on_city_calc_is_faile = false;
		auto on_city_pos = match_all_map(on_city_calc_is_faile, on_city_stdev, 0.5);
		if (on_city_calc_is_faile != true && on_city_stdev < ALL_MAP__ON_CITY__STDEV_THRESH)
		{
			calc_is_faile = on_city_calc_is_faile;
			isOnCity = true;
			return on_city_pos;
		}
	}
	bool not_on_city_calc_is_faile = false;
	auto not_on_city_pos= match_all_map(not_on_city_calc_is_faile, not_on_city_stdev, 2.0);
	if (not_on_city_calc_is_faile != true && not_on_city_stdev < ALL_MAP__NOT_ON_CITY__STDEV_THRESH)
	{
		calc_is_faile = not_on_city_calc_is_faile;
		isOnCity = false;
		return not_on_city_pos;
	}
	double try_match_stdev=0.0;
	return match_all_map(calc_is_faile, try_match_stdev, 1.0);
}


cv::Point2d SurfMatch::match_all_map(bool& calc_is_faile, double& stdev, double minimap_scale_param)
{
	static cv::Mat all_map(_mapMat);
	cv::Point2d all_map_pos;

	cv::Mat img_object = crop_border(_miniMapMat, 0.15);
	cv::resize(img_object, img_object, cv::Size(0, 0), minimap_scale_param, minimap_scale_param, cv::INTER_CUBIC);

	// 小地图区域计算特征点
	detector->detectAndCompute(img_object, cv::noArray(), Kp_MiniMap, Dp_MiniMap);
	// 没有提取到特征点直接返回，结果无效
	if (Kp_MiniMap.size() == 0)
	{
		calc_is_faile = true;
		return all_map_pos;
	}
	// 匹配特征点
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<cv::DMatch> > KNN_m;
	matcher->knnMatch(Dp_MiniMap, Dp_Map, KNN_m, 2);

	std::vector<TianLi::Utils::MatchKeyPoint> keypoint_list;
	TianLi::Utils::calc_good_matches(all_map, Kp_Map, img_object, Kp_MiniMap, KNN_m, ratio_thresh, keypoint_list);

	std::vector<double> lisx;
	std::vector<double> lisy;
	TianLi::Utils::remove_invalid(keypoint_list, render_map_scale / minimap_scale_param, lisx, lisy);

	std::vector<cv::Point2d> list_not_on_city;
	for (int i = 0; i < keypoint_list.size(); i++)
	{
		list_not_on_city.push_back(cv::Point2d(lisx[i], lisy[i]));
	}
	list_not_on_city = TianLi::Utils::extract_valid(list_not_on_city);

	lisx.clear();
	lisy.clear();
	for (int i = 0; i < list_not_on_city.size(); i++)
	{
		lisx.push_back(list_not_on_city[i].x);
		lisy.push_back(list_not_on_city[i].y);
	}
	
	double x_stdev = TianLi::Utils::stdev_abs(lisx);
	double y_stdev = TianLi::Utils::stdev_abs(lisy);
	
	stdev = sqrt(x_stdev + y_stdev);
	
	// 没有最佳匹配结果直接返回，结果无效
	if (std::min(lisx.size(), lisy.size()) == 0)
	{
		calc_is_faile = true;
		return all_map_pos;
	}
	// 从最佳匹配结果中剔除异常点计算角色位置返回
	all_map_pos = TianLi::Utils::SPC(lisx, lisy);
	return all_map_pos;
}

cv::Point2d SurfMatch::getLocalPos()
{
	return pos;
}

bool SurfMatch::getIsContinuity()
{
	return isContinuity;
}
