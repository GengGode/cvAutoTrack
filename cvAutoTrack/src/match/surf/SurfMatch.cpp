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

	return pos_continuity;
}

cv::Point2d SurfMatch::match_continuity_on_city(bool& calc_continuity_is_faile)
{
	cv::Point2d pos_on_city;

	cv::Mat img_scene(_mapMat);
	int cut_bord = static_cast<int>((_minMapMat.rows + _minMapMat.cols) * 0.5 * 0.15);
	cv::Mat img_object(_minMapMat(cv::Rect(cut_bord, cut_bord, _minMapMat.cols - cut_bord*2, _minMapMat.rows - cut_bord*2)));

	//在城镇中
		/***********************/
		//重新从完整中地图取出角色周围部分地图
	cv::Mat someMap(img_scene(cv::Rect(static_cast<int>(hisP[2].x - someSizeR), static_cast<int>(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)));
	cv::Mat minMap(img_object);

	resize(someMap, someMap, cv::Size(someSizeR * 4, someSizeR * 4), cv::INTER_CUBIC);
	resize(minMap, minMap, cv::Size(someSizeR * 4, someSizeR * 4), cv::INTER_CUBIC);
	

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
#ifdef _DEBUG
	return match_no_continuity_2nd(calc_is_faile);
#else
	return match_no_continuity_1st(calc_is_faile);
#endif // _DEBUG
}
/// <summary>
/// 非连续匹配，从大地图中确定角色位置
/// 直接通过SURF特征点匹配精确定位角色位置
/// </summary>
/// <param name="calc_is_faile">匹配结果是否有效</param>
/// <returns></returns>
cv::Point2d SurfMatch::match_no_continuity_1st(bool& calc_is_faile)
{
	cv::Point2d pos_continuity_no;

	// TODO: 可优化为static
	cv::Mat img_scene(_mapMat);
	cv::Mat img_object(_minMapMat(cv::Rect(30, 30, _minMapMat.cols - 60, _minMapMat.rows - 60)));
	// 小地图区域计算特征点
	detector->detectAndCompute(img_object, cv::noArray(), Kp_MinMap, Dp_MinMap);
	// 没有提取到特征点直接返回，结果无效
	if (Kp_MinMap.size() == 0)
	{
		calc_is_faile = true;
		return pos_continuity_no;
	}
	// 匹配特征点
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<cv::DMatch> > KNN_m;
	matcher->knnMatch(Dp_MinMap, Dp_Map, KNN_m, 2);

	std::vector<double> lisx;
	std::vector<double> lisy;
	double sumx = 0;
	double sumy = 0;
	// 计算最佳匹配结果
	TianLi::Utils::calc_good_matches(img_scene, Kp_Map, img_object, Kp_MinMap, KNN_m, ratio_thresh, render_map_scale, lisx, lisy, sumx, sumy);
	// 没有最佳匹配结果直接返回，结果无效
	if (std::min(lisx.size(), lisy.size()) == 0)
	{
		calc_is_faile = true;
		return pos_continuity_no;
	}
	// 从最佳匹配结果中剔除异常点计算角色位置返回
	pos_continuity_no = TianLi::Utils::SPC(lisx, sumx, lisy, sumy);
	return pos_continuity_no;
}
cv::Mat to_color(cv::Mat& img_object)
{
	cv::Mat color_mat;
	int s_len = (img_object.cols + img_object.rows) * 0.25 * 0.8;
	cv::Mat roi_tl = img_object(cv::Rect(0, 0, s_len, s_len));
	cv::Mat roi_tr = img_object(cv::Rect(img_object.cols - s_len, 0, s_len, s_len));
	cv::Mat roi_bl = img_object(cv::Rect(0, img_object.rows - s_len, s_len, s_len));
	cv::Mat roi_br = img_object(cv::Rect(img_object.cols - s_len, img_object.rows - s_len, s_len, s_len));
	
	cv::Mat roi_tl_color;
	cv::Mat roi_tr_color;
	cv::Mat roi_bl_color;
	cv::Mat roi_br_color;
	
	cv::resize(roi_tl, roi_tl_color, cv::Size(3, 3), cv::INTER_AREA);
	cv::resize(roi_tr, roi_tr_color, cv::Size(3, 3), cv::INTER_AREA);
	cv::resize(roi_bl, roi_bl_color, cv::Size(3, 3), cv::INTER_AREA);
	cv::resize(roi_br, roi_br_color, cv::Size(3, 3), cv::INTER_AREA);
	
	cv::Mat roi = cv::Mat::zeros(6, 6, img_object.type());
	roi_tl_color.copyTo(roi(cv::Rect(0, 0, 3, 3)));
	roi_tr_color.copyTo(roi(cv::Rect(3, 0, 3, 3)));
	roi_bl_color.copyTo(roi(cv::Rect(0, 3, 3, 3)));
	roi_br_color.copyTo(roi(cv::Rect(3, 3, 3, 3)));
	
	cv::Mat roi_color;
	cv::resize(roi, roi_color, cv::Size(1,1), cv::INTER_AREA);
	
	roi_color.at<cv::Vec4b>(0,0)[3] = 255;
	
	cv::resize(img_object, color_mat, cv::Size(5,5),cv::INTER_AREA);
	return roi_color;
	
}
// 初步定位：根据颜色确定角色在大地图的哪个方位
cv::Point match_find_direction_in_all(cv::Mat& _mapMat, cv::Mat& _minMapMat)
{
	static cv::Mat color_map;
	static bool is_first = true;
	if (is_first)
	{
		cv::resize(_mapMat, color_map, cv::Size(), 0.01, 0.01);
		is_first = false;
	}
	
	cv::Mat img_object(_minMapMat(cv::Rect(30, 30, _minMapMat.cols - 60, _minMapMat.rows - 60)));
	// 对小地图进行颜色提取
	cv::Mat color = to_color(img_object);
	// 模板匹配
	cv::Mat result;
	cv::matchTemplate(color_map, color, result, cv::TM_CCOEFF_NORMED);
	// 找到最大值和最小值的位置
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
	// 计算角色在大地图的位置
	cv::Point pos = cv::Point(maxLoc.x * 100 + 50, maxLoc.y * 100 + 50);
	return pos;
}
// 确定区块：根据初步定位的结果再遍历该方位的区块，确定所在区块
cv::Point match_find_block_in_direction(cv::Mat& _mapMat, cv::Mat& _minMapMat, cv::Point pos_first_match)
{
	// 0 18 22 38
	const static cv::Rect yellow_rect = cv::Rect(0, 18, 22, 38);
	if (yellow_rect.contains(pos_first_match))
	{
		return cv::Point(-1, 0);
	}

	return cv::Point(0, 0);
}
// 确定位置：根据所在区块的结果精确匹配角色位置
cv::Point2d match_yellow_block(cv::Mat& _mapMat, cv::Mat& _minMapMat)
{
	cv::Point pos_first_match = match_find_direction_in_all(_mapMat, _minMapMat);
	cv::Point pos_block_match = match_find_block_in_direction(_mapMat, _minMapMat, pos_first_match);
	cv::Point2d pos_continuity_no;
	bool calc_is_faile = false;
	//cv::Point2d pos = match_find_pos_in_block(_mapMat, _minMapMat, pos_block_match, pos_continuity_no, calc_is_faile);
	if (calc_is_faile)
	{
		return cv::Point2d(-1, -1);
	}
	return cv::Point2d(0,0);
}
// 确定位置：根据所在区块的结果精确匹配角色位置
cv::Point2d SurfMatch::match_find_position_in_block(cv::Mat& _mapMat, cv::Mat& _minMapMat, cv::Point pos_second_match, bool& calc_is_faile)
{
	if (pos_second_match.x == -1)
	{
		return cv::Point2d(0, 0);
	}
	else
	{
		return match_no_continuity_1st(calc_is_faile);
	}
}
/// <summary>
/// 非连续匹配，从大地图中确定角色位置 v2.0
/// 根据某小地图整体颜色判断角色的大致位置，然后再根据大致位置进行精确匹配
/// </summary>
/// <param name="calc_is_faile">匹配结果是否有效</param>
/// <returns></returns>
cv::Point2d SurfMatch::match_no_continuity_2nd(bool& calc_is_faile)
{
	cv::Point pos_first_match;
	cv::Point pos_second_match;
	cv::Point2d pos_continuity_no;
	// 初步定位：根据颜色确定角色在大地图的哪个方位
	pos_first_match = match_find_direction_in_all(_mapMat, _minMapMat);
	// 确定区块：根据初步定位的结果再遍历该方位的区块，确定所在区块
	pos_second_match = match_find_block_in_direction(_mapMat, _minMapMat, pos_first_match);
	// 确定位置：根据所在区块的结果精确匹配角色位置
	pos_continuity_no = match_find_position_in_block(_mapMat, _minMapMat, pos_second_match, calc_is_faile);
	// 返回结果
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