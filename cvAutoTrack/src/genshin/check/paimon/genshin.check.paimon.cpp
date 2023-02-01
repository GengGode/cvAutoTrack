#include "pch.h"
#include "genshin.check.paimon.h"
#include "../../../resources/Resources.h"

auto get_point_vec3b(const cv::Mat& const mat, int x, int y, bool is_need_cvt)
{
	if (is_need_cvt)
	{
		auto screen_color_v4 = mat.at<cv::Vec4b>(y, x);
		return cv::Vec3b(screen_color_v4[0], screen_color_v4[1], screen_color_v4[2]);
	}
	return mat.at<cv::Vec3b>(y, x);

}
double cala_keypoint_diff(const cv::Mat& const mat,const cv::Rect& roi,const std::vector<std::pair<cv::Point, cv::Vec3b>> paimon_keys, bool is_need_cvt)
{
	double paimon_check_diff = 0;
	for (auto& paimon_key : paimon_keys)
	{
		auto& [_point, color] = paimon_key;
		auto [x, y] = roi.tl() + _point;
		auto screen_color = get_point_vec3b(mat, x, y, is_need_cvt);
		auto color_diff = cv::norm(screen_color, color);
		paimon_check_diff = paimon_check_diff + color_diff / paimon_keys.size();
	}
	return paimon_check_diff;
}

bool check_paimon_impl(const GenshinScreen& genshin_screen, GenshinPaimon& out_genshin_paimon)
{
	auto paimon_keys = out_genshin_paimon.config.paimon_check_vec;
	auto paimon_handle_keys = out_genshin_paimon.config.paimon_handle_check_vec;
	auto rect_keypoint = genshin_screen.config.rect_paimon_keypoint;
	auto rect_keypoint_handle = genshin_screen.config.rect_paimon_keypoint_handle;
	auto giPaimonRef = genshin_screen.img_paimon_maybe;
	// 判空退出
	if (giPaimonRef.empty()) return false;

	// 是否需要从4通道取3通道
	bool is_need_cvt = false;
	if (giPaimonRef.channels() == 4)
	{
		is_need_cvt = true;
	}

	// 计算每一个点的颜色距离并累计平均
	double paimon_check_diff = cala_keypoint_diff(giPaimonRef, rect_keypoint, paimon_keys, is_need_cvt);
	// 超过阈值，匹配手柄模式
	if (paimon_check_diff < out_genshin_paimon.config.check_match_paimon_keypoint_params)
	{
		out_genshin_paimon.is_handle_mode = false;
		out_genshin_paimon.is_visial = true;
		out_genshin_paimon.rect_paimon = genshin_screen.config.rect_paimon_keypoint;
		return true;
	}
	// 计算每一个点的颜色距离并累计平均
	double paimon_handle_check_diff = cala_keypoint_diff(giPaimonRef, rect_keypoint_handle, paimon_handle_keys, is_need_cvt);
	// 超过阈值，未能匹配到
	if (paimon_handle_check_diff < out_genshin_paimon.config.check_match_paimon_keypoint_params)
	{
		out_genshin_paimon.is_handle_mode = true;
		out_genshin_paimon.is_visial = true;
		out_genshin_paimon.rect_paimon = genshin_screen.config.rect_paimon_keypoint_handle;
		return true;
	}
	out_genshin_paimon.is_visial = false;
	return true;
}

bool TianLi::Genshin::Check::check_paimon(const GenshinScreen& genshin_screen, GenshinPaimon& out_genshin_paimon)
{
	return check_paimon_impl(genshin_screen, out_genshin_paimon);
}