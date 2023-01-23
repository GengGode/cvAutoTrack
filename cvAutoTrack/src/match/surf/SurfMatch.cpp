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

void SurfMatch::setMiniMap(cv::Mat minMapMat)
{
	_minMapMat = minMapMat;
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

	//角色移动连续性判断
	if (((TianLi::Utils::dis(dp1) + TianLi::Utils::dis(dp2)) < 2000) && (hisP[2].x > someSizeR && hisP[2].x < _mapMat.cols - someSizeR && hisP[2].y>someSizeR && hisP[2].y < _mapMat.rows - someSizeR))
	{
		isContinuity = true;
	}

	if (isContinuity)
	{
		bool calc_continuity_is_faile = false;

		pos = match_continuity(calc_continuity_is_faile);

		if (calc_continuity_is_faile)
		{
			isContinuity = false;
		}
	}

	if (!isContinuity)
	{
		pos = match_no_continuity(calc_is_faile);
	}

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

	return pos_continuity;
}

cv::Point2d SurfMatch::match_continuity_on_city(bool& calc_continuity_is_faile)
{
	cv::Point2d pos_on_city;

	cv::Mat img_scene(_mapMat);
	cv::Mat img_object(_minMapMat(cv::Rect(30, 30, _minMapMat.cols - 60, _minMapMat.rows - 60)));

	//在城镇中
		/***********************/
		//重新从完整中地图取出角色周围部分地图
	cv::Mat someMap(img_scene(cv::Rect(static_cast<int>(hisP[2].x - someSizeR), static_cast<int>(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)));
	cv::Mat minMap(img_object);

	resize(someMap, someMap, cv::Size(someSizeR * 4, someSizeR * 4));

	detectorSomeMap = cv::xfeatures2d::SURF::create(hessian_threshold, octaves, octave_layers, extended, upright);
	detectorSomeMap->detectAndCompute(someMap, cv::noArray(), Kp_SomeMap, Dp_SomeMap);
	detectorSomeMap->detectAndCompute(minMap, cv::noArray(), Kp_MinMap, Dp_MinMap);

	if (Kp_SomeMap.size() <= 2 || Kp_MinMap.size() <= 2)
	{
		calc_continuity_is_faile = true;
		return pos_on_city;
	}

	cv::Ptr<cv::DescriptorMatcher> matcherTmp = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<cv::DMatch> > KNN_mTmp;

	matcherTmp->knnMatch(Dp_MinMap, Dp_SomeMap, KNN_mTmp, 2);
	std::vector<double> lisx;
	std::vector<double> lisy;
	double sumx = 0;
	double sumy = 0;

	TianLi::Utils::calc_good_matches(someMap, Kp_SomeMap, img_object, Kp_MinMap, KNN_mTmp, ratio_thresh, 0.8667, lisx, lisy, sumx, sumy);

	if (std::max(lisx.size(), lisy.size()) <= 4)
	{
		calc_continuity_is_faile = true;
		return pos_on_city;
	}

	if (std::min(lisx.size(), lisy.size()) >= 10)
	{
		isOnCity = true;
	}
	else
	{
		isOnCity = false;
	}

	cv::Point2d pos_continuity_on_city = TianLi::Utils::SPC(lisx, sumx, lisy, sumy);

	pos_continuity_on_city.x = (pos_continuity_on_city.x - someMap.cols / 2.0) / 2.0;
	pos_continuity_on_city.y = (pos_continuity_on_city.y - someMap.rows / 2.0) / 2.0;

	pos_on_city = cv::Point2d(pos_continuity_on_city.x + hisP[2].x, pos_continuity_on_city.y + hisP[2].y);

	return pos_on_city;
}

cv::Point2d SurfMatch::match_continuity_not_on_city(bool& calc_continuity_is_faile)
{
	cv::Point2d pos_not_on_city;

	cv::Mat img_scene(_mapMat);
	cv::Mat img_object(_minMapMat(cv::Rect(30, 30, _minMapMat.cols - 60, _minMapMat.rows - 60)));

	//不在城镇中时
	cv::Mat someMap(img_scene(cv::Rect(static_cast<int>(hisP[2].x - someSizeR), static_cast<int>(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)));
	cv::Mat minMap(img_object);

	detectorSomeMap = cv::xfeatures2d::SURF::create(hessian_threshold, octaves, octave_layers, extended, upright);
	detectorSomeMap->detectAndCompute(someMap, cv::noArray(), Kp_SomeMap, Dp_SomeMap);
	detectorSomeMap->detectAndCompute(minMap, cv::noArray(), Kp_MinMap, Dp_MinMap);

	// 如果搜索范围内可识别特征点数量为0，则认为计算失败
	if (Kp_SomeMap.size() == 0 || Kp_MinMap.size() <= 2)
	{
		calc_continuity_is_faile = true;
		return pos_not_on_city;
	}
	cv::Ptr<cv::DescriptorMatcher> matcher_not_on_city = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<cv::DMatch> > KNN_not_no_city;

	matcher_not_on_city->knnMatch(Dp_MinMap, Dp_SomeMap, KNN_not_no_city, 2);
	std::vector<double> lisx;
	std::vector<double> lisy;
	double sumx = 0;
	double sumy = 0;

	TianLi::Utils::calc_good_matches(someMap, Kp_SomeMap, img_object, Kp_MinMap, KNN_not_no_city, ratio_thresh, render_map_scale, lisx, lisy, sumx, sumy);

	// 如果范围内最佳匹配特征点对数量大于4，则认为不可能处于城镇之中，位于城镇之外
	if (std::min(lisx.size(), lisy.size()) > 4)
	{
		isOnCity = false;

		cv::Point2d p = TianLi::Utils::SPC(lisx, sumx, lisy, sumy);
		pos_not_on_city = cv::Point2d(p.x + hisP[2].x - someSizeR, p.y + hisP[2].y - someSizeR);
	}
	else
	{

		//有可能处于城镇中

		/***********************/
		//重新从完整中地图取出角色周围部分地图
		img_scene(cv::Rect(static_cast<int>(hisP[2].x - someSizeR), static_cast<int>(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)).copyTo(someMap);
		//Mat minMap(img_object);

		resize(someMap, someMap, cv::Size(someSizeR * 4, someSizeR * 4));
		//resize(minMap, minMap, Size(), MatchMatScale, MatchMatScale, 1);

		detectorSomeMap = cv::xfeatures2d::SURF::create(hessian_threshold, octaves, octave_layers, extended, upright);
		detectorSomeMap->detectAndCompute(someMap, cv::noArray(), Kp_SomeMap, Dp_SomeMap);
		//detectorSomeMap->detectAndCompute(minMap, noArray(), Kp_MinMap, Dp_MinMap);
		if (Kp_SomeMap.size() == 0 || Kp_MinMap.size() == 0)
		{
			calc_continuity_is_faile = true;
			return pos_not_on_city;
		}

		cv::Ptr<cv::DescriptorMatcher> matcher_mabye_on_city = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
		std::vector< std::vector<cv::DMatch> > KNN_mabye_on_city;

		matcher_mabye_on_city->knnMatch(Dp_MinMap, Dp_SomeMap, KNN_mabye_on_city, 2);

		std::vector<double> list_x_on_city;
		std::vector<double> list_y_on_city;
		double sum_x_on_city = 0;
		double sum_y_on_city = 0;

		TianLi::Utils::calc_good_matches(someMap, Kp_SomeMap, img_object, Kp_MinMap, KNN_mabye_on_city, ratio_thresh, 0.8667, list_x_on_city, list_y_on_city, sum_x_on_city, sum_y_on_city);

		if (std::min(list_x_on_city.size(), list_y_on_city.size()) <= 4)
		{
			calc_continuity_is_faile = true;
			return pos_not_on_city;
		}

		if (std::min(list_x_on_city.size(), list_y_on_city.size()) >= 10)
		{
			isOnCity = true;
		}
		else
		{
			isOnCity = false;
		}

		cv::Point2d p = TianLi::Utils::SPC(list_x_on_city, sum_x_on_city, list_y_on_city, sum_y_on_city);

		double x = (p.x - someMap.cols / 2.0) / 2.0;
		double y = (p.y - someMap.rows / 2.0) / 2.0;

		pos_not_on_city = cv::Point2d(x + hisP[2].x, y + hisP[2].y);

	}

	return pos_not_on_city;
}

cv::Point2d SurfMatch::match_no_continuity(bool& calc_is_faile)
{
	cv::Point2d pos_continuity_no;

	// TODO: 可优化为static
	cv::Mat img_scene(_mapMat);
	cv::Mat img_object(_minMapMat(cv::Rect(30, 30, _minMapMat.cols - 60, _minMapMat.rows - 60)));

	detector->detectAndCompute(img_object, cv::noArray(), Kp_MinMap, Dp_MinMap);

	if (Kp_MinMap.size() == 0)
	{
		calc_is_faile = true;
		return pos_continuity_no;
	}

	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<cv::DMatch> > KNN_m;

	matcher->knnMatch(Dp_MinMap, Dp_Map, KNN_m, 2);

	std::vector<double> lisx;
	std::vector<double> lisy;
	double sumx = 0;
	double sumy = 0;

	TianLi::Utils::calc_good_matches(img_scene, Kp_Map, img_object, Kp_MinMap, KNN_m, ratio_thresh, render_map_scale, lisx, lisy, sumx, sumy);

	if (std::min(lisx.size(), lisy.size()) == 0)
	{
		calc_is_faile = true;
		return pos_continuity_no;
	}

	pos_continuity_no = TianLi::Utils::SPC(lisx, sumx, lisy, sumy);

	return pos_continuity_no;
}


cv::Point2d SurfMatch::SURFMatch(cv::Mat minMapMat)
{
	return cv::Point2d();
}

cv::Point2d SurfMatch::getLocalPos()
{
	return pos;
}

bool SurfMatch::getIsContinuity()
{
	return isContinuity;
}

void get_avatar_position(const GenshinMinimap& genshin_minimap, GenshinAvatarPosition& out_genshin_position)
{
	static SurfMatch surf_match;
	static bool is_init = false;
	if (!is_init)
	{
		std::vector<cv::KeyPoint> gi_map_keypoints;
		cv::Mat gi_map_descriptors;
		
		get_map_keypoint(gi_map_keypoints, gi_map_descriptors, surf_match.hessian_threshold, surf_match.octaves, surf_match.octave_layers, surf_match.extended, surf_match.upright);

		surf_match.setMap(Resources::getInstance().MapTemplate);

		surf_match.detector = cv::xfeatures2d::SURF::create(surf_match.hessian_threshold, surf_match.octaves, surf_match.octave_layers, surf_match.extended, surf_match.upright);
		surf_match.detectorSomeMap = cv::xfeatures2d::SURF::create(surf_match.hessian_threshold, surf_match.octaves, surf_match.octave_layers, surf_match.extended, surf_match.upright);

		surf_match.Init(gi_map_keypoints, gi_map_descriptors);

		is_init = true;
	}


	if (genshin_minimap.config.is_find_paimon == false)
	{
		return;
	}

	if (genshin_minimap.img_minimap.empty())
	{
		return;
	}

	surf_match.setMiniMap(genshin_minimap.img_minimap);

	surf_match.match();

	out_genshin_position.position = surf_match.getLocalPos();
}