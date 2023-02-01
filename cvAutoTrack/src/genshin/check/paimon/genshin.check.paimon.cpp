#include "pch.h"
#include "genshin.check.paimon.h"
#include "../../../resources/Resources.h"

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
	double paimon_check_diff = 0;
	for (auto& paimon_key : paimon_keys)
	{
		auto& [_point, color] = paimon_key;
		auto [x, y] = rect_keypoint.tl() + _point;
		if (is_need_cvt)
		{
			auto screen_color_v4 = giPaimonRef.at<cv::Vec4b>(y, x);
			auto screen_color = cv::Vec3b(screen_color_v4[0], screen_color_v4[1], screen_color_v4[2]);
			auto color_diff = cv::norm(screen_color, color);
			paimon_check_diff = paimon_check_diff + color_diff / paimon_keys.size();
		}
		else
		{
			auto screen_color = giPaimonRef.at<cv::Vec3b>(y, x);
			auto color_diff = cv::norm(screen_color, color);
			paimon_check_diff = paimon_check_diff + color_diff / paimon_keys.size();
		}
	}
	// 超过阈值，匹配手柄模式
	if (paimon_check_diff > out_genshin_paimon.config.check_match_paimon_keypoint_params)
	{
		// 计算每一个点的颜色距离并累计平均
		double paimon_handle_check_diff = 0;
		for (auto& paimon_key : paimon_handle_keys)
		{
			auto& [_point, color] = paimon_key;
			auto [x, y] = rect_keypoint_handle.tl() + _point;
			if (is_need_cvt)
			{
				auto screen_color_v4 = giPaimonRef.at<cv::Vec4b>(y, x);
				auto screen_color = cv::Vec3b(screen_color_v4[0], screen_color_v4[1], screen_color_v4[2]);
				auto color_diff = cv::norm(screen_color, color);
				paimon_handle_check_diff = paimon_handle_check_diff + color_diff / paimon_keys.size();
			}
			else
			{
				auto screen_color = giPaimonRef.at<cv::Vec3b>(y, x);
				auto color_diff = cv::norm(screen_color, color);
				paimon_handle_check_diff = paimon_handle_check_diff + color_diff / paimon_keys.size();
			}
		}
		// 超过阈值，未能匹配到
		if (paimon_handle_check_diff > out_genshin_paimon.config.check_match_paimon_keypoint_params)
		{
			out_genshin_paimon.is_visial = false;
		}
		else
		{
			out_genshin_paimon.is_handle_mode = true;
			out_genshin_paimon.is_visial = true;
			out_genshin_paimon.rect_paimon = genshin_screen.config.rect_paimon_keypoint_handle;
		}
	}
	else
	{
		out_genshin_paimon.is_handle_mode = false;
		out_genshin_paimon.is_visial = true;
		out_genshin_paimon.rect_paimon = genshin_screen.config.rect_paimon_keypoint;
	}
	return true;
}

bool TianLi::Genshin::Check::check_paimon(const GenshinScreen& genshin_screen, GenshinPaimon& out_genshin_paimon)
{
	return check_paimon_impl(genshin_screen, out_genshin_paimon);
}