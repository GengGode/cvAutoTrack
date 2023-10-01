#include "pch.h"
#include "genshin.match.position.h"

#include "resources/Resources.h"
#include "Match/surf/SurfMatch.h"
#include "filter/kalman/Kalman.h"

cv::Mat to_color(cv::Mat& img_object)
{
	cv::Mat color_mat;
	int s_len = static_cast<int>((img_object.cols + img_object.rows) * 0.25 * 0.8);
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
	cv::resize(roi, roi_color, cv::Size(1, 1), cv::INTER_AREA);

	roi_color.at<cv::Vec4b>(0, 0)[3] = 255;

	cv::resize(img_object, color_mat, cv::Size(5, 5), cv::INTER_AREA);
	return roi_color;

}
// 初步定位：根据颜色确定角色在大地图的哪个方位
cv::Point match_find_direction_in_all(cv::Mat& _mapMat, cv::Mat& _MiniMapMat)
{
	static cv::Mat color_map;
	static bool is_first = true;
	if (is_first)
	{
		cv::resize(_mapMat, color_map, cv::Size(), 0.01, 0.01, cv::INTER_CUBIC);
		is_first = false;
	}

	int crop_border = static_cast<int>((_MiniMapMat.rows + _MiniMapMat.cols) * 0.5 * 0.15);
	cv::Mat img_object(_MiniMapMat(cv::Rect(crop_border, crop_border, _MiniMapMat.cols - crop_border * 2, _MiniMapMat.rows - crop_border * 2)));
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
cv::Point match_find_block_in_direction(cv::Mat& _mapMat, cv::Mat& _MiniMapMat, cv::Point pos_first_match)
{
	// 0 18 22 38
	const static cv::Rect yellow_rect = cv::Rect(0, 18, 22, 38);
	if (yellow_rect.contains(pos_first_match))
	{
		return cv::Point(-1, 0);
	}
	UNREFERENCED_PARAMETER(_mapMat);
	UNREFERENCED_PARAMETER(_MiniMapMat);
	return cv::Point(0, 0);
}
// 确定位置：根据所在区块的结果精确匹配角色位置
cv::Point2d match_yellow_block(cv::Mat& _mapMat, cv::Mat& _MiniMapMat)
{
	cv::Point pos_first_match = match_find_direction_in_all(_mapMat, _MiniMapMat);
	cv::Point pos_block_match = match_find_block_in_direction(_mapMat, _MiniMapMat, pos_first_match);
	cv::Point2d pos_continuity_no;
	bool calc_is_faile = false;
	//cv::Point2d pos = match_find_pos_in_block(_mapMat, _MiniMapMat, pos_block_match, pos_continuity_no, calc_is_faile);
	if (calc_is_faile)
	{
		return cv::Point2d(-1, -1);
	}
	return cv::Point2d(0, 0);
}
// 确定位置：根据所在区块的结果精确匹配角色位置
cv::Point2d match_find_position_in_block(cv::Point pos_second_match, bool& calc_is_faile)
{
	if (pos_second_match.x == -1)
	{
		return cv::Point2d(0, 0);
	}
	else
	{
		UNREFERENCED_PARAMETER(calc_is_faile);
		//return match_no_continuity_1st(calc_is_faile);
		return cv::Point2d(0, 0);
	}
}
/// <summary>
/// 非连续匹配，从大地图中确定角色位置 v2.0
/// 根据某小地图整体颜色判断角色的大致位置，然后再根据大致位置进行精确匹配
/// </summary>
/// <param name="calc_is_faile">匹配结果是否有效</param>
/// <returns></returns>
cv::Point2d match_no_continuity_2nd(bool& calc_is_faile)
{
	cv::Point pos_first_match;
	cv::Point pos_second_match;
	cv::Point2d pos_continuity_no;
	// 初步定位：根据颜色确定角色在大地图的哪个方位
	//pos_first_match = match_find_direction_in_all(_mapMat, _miniMapMat);
	// 确定区块：根据初步定位的结果再遍历该方位的区块，确定所在区块
	//pos_second_match = match_find_block_in_direction(_mapMat, _miniMapMat, pos_first_match);
	// 确定位置：根据所在区块的结果精确匹配角色位置
	pos_continuity_no = match_find_position_in_block(pos_second_match, calc_is_faile);
	// 返回结果
	return pos_continuity_no;
}


void TianLi::Genshin::Match::get_avatar_position(const GenshinMinimap& genshin_minimap, GenshinAvatarPosition& out_genshin_position)
{
	static SurfMatch surf_match;
	static bool is_init = false;
	if (genshin_minimap.is_run_init_start == true || is_init == false)
	{
		if (is_init)return;
		Resources::getInstance().install();


		std::vector<cv::KeyPoint> gi_map_keypoints;
		cv::Mat gi_map_descriptors;

		surf_match.setMap(Resources::getInstance().MapTemplate);
		get_map_keypoint(gi_map_keypoints, gi_map_descriptors);
		surf_match.Init(gi_map_keypoints, gi_map_descriptors);

		is_init = true;
		return;
	}

	if (genshin_minimap.is_run_uninit_start == true)
	{
		surf_match.UnInit();
		Resources::getInstance().release();
		is_init = false;
		return;
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
	out_genshin_position.config.is_continuity = surf_match.isContinuity;
	out_genshin_position.config.is_coveying = surf_match.isCoveying;
	out_genshin_position.config.is_on_city = surf_match.isOnCity;

	if (out_genshin_position.config.is_use_filter)
	{
		cv::Point2d pos = out_genshin_position.position;
		cv::Point2d filt_pos;
		if (out_genshin_position.config.is_coveying || out_genshin_position.config.is_continuity == false)
		{
			filt_pos = out_genshin_position.config.pos_filter->re_init_filterting(pos);
		}
		else
		{
			filt_pos = out_genshin_position.config.pos_filter->filterting(pos);
		}
		out_genshin_position.position = filt_pos;
	}

	if (surf_match.is_success_match)
	{
		out_genshin_position.config.img_last_match_minimap = genshin_minimap.img_minimap.clone();
		out_genshin_position.config.is_exist_last_match_minimap = true;
	}
	else
	{
		out_genshin_position.config.is_exist_last_match_minimap = false;
	}
}