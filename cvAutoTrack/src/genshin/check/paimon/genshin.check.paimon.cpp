#include "pch.h"
#include "genshin.check.paimon.h"
#include "../../../resources/Resources.h"

namespace TianLi::dev
{
	
	bool check_paimon_2nd(const GenshinScreen& genshin_screen, GenshinPaimon& out_genshin_paimon)
	{
		auto paimon_keys = out_genshin_paimon.config.paimon_check_vec;
		auto paimon_handle_keys = out_genshin_paimon.config.paimon_handle_check_vec;

		auto giPaimonRef = genshin_screen.img_paimon_maybe;
		// 判空退出
		if (giPaimonRef.empty()) return false;

		// 计算每一个点的颜色距离并累计平均
		double paimon_check_diff = 0;
		for (auto& paimon_key : paimon_keys)
		{
			auto& [_point, color] = paimon_key;
			auto& [x, y] = _point;
			auto screen_color = giPaimonRef.at<cv::Vec3b>(y, x);
			auto color_diff = cv::norm(screen_color, color);
			paimon_check_diff = paimon_check_diff + color_diff / paimon_keys.size();
		}
		// 超过阈值，匹配手柄模式
		if (paimon_check_diff > out_genshin_paimon.config.check_match_paimon_keypoint_params)
		{
			// 计算每一个点的颜色距离并累计平均
			double paimon_handle_check_diff = 0;
			for (auto& paimon_key : paimon_handle_keys)
			{
				auto& [_point, color] = paimon_key;
				auto& [x, y] = _point;
				auto screen_color = giPaimonRef.at<cv::Vec3b>(y, x);
				auto color_diff = cv::norm(screen_color, color);
				paimon_check_diff = paimon_check_diff + color_diff / paimon_handle_keys.size();
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
	
}
bool check_paimon_impl(const GenshinScreen& genshin_screen, GenshinPaimon& out_genshin_paimon)
{
	static std::vector<cv::Mat> split_paimon_template;
	// 派蒙正比例二值模板 黑白
	static cv::Mat paimon_template;
	// 派蒙手柄比例二值模板 黑白
	static cv::Mat paimon_template_handle_mode;
	// 派蒙正比例灰度模板 灰度
	static cv::Mat paimon_template_no_alpha;
	// 派蒙手柄比例灰度模板 灰度
	static cv::Mat paimon_template_no_alpha_handle_mode;
	static bool is_first = true;
	if (is_first)
	{
		cv::Mat paimon;
		Resources::getInstance().PaimonTemplate.copyTo(paimon);
		cv::resize(paimon, paimon, cv::Size(68, 77));
		cv::cvtColor(paimon, paimon_template_no_alpha, cv::COLOR_RGBA2GRAY);
		cv::split(paimon, split_paimon_template);

		paimon_template = split_paimon_template[3];
		//paimon_template_no_alpha = split_paimon_template[0];
		//paimon_template_no_alpha.copyTo(paimon_template_no_alpha, paimon_template);
		cv::resize(paimon_template, paimon_template_handle_mode, cv::Size(), 1.0 / 1.20, 1.0 / 1.20);
		cv::resize(paimon_template_no_alpha, paimon_template_no_alpha_handle_mode, cv::Size(), 1.0 / 1.20, 1.0 / 1.20);
		//paimon_template_no_alpha_handle_mode.copyTo(paimon_template_no_alpha_handle_mode, paimon_template_handle_mode);
		is_first = false;
	}
	auto giPaimonRef = genshin_screen.img_paimon_maybe;
	auto& rect_origin = genshin_screen.config.rect_paimon_maybe;
	// 识别用模板 正比例  包含alpha和no_alpha
	auto template_not_handle_mode = paimon_template;
	// 识别用模板 手柄比例  包含alpha和no_alpha
	auto template_handle_mode = paimon_template_handle_mode;
	// 匹配遮罩 正比例  alpha下是空，no_alpha下是 派蒙手柄比例二值模板 黑白
	auto template_mask_not_handle_mode = cv::Mat();
	// 匹配遮罩 手柄比例  alpha下是空，no_alpha下是 派蒙手柄比例二值模板 黑白
	auto template_mask_handle_mode = cv::Mat();
	// 判空退出
	if (giPaimonRef.empty() || paimon_template_handle_mode.empty()) return false;
	if (giPaimonRef.cols < paimon_template.cols || giPaimonRef.rows < paimon_template.rows) return false;

	// 设置阈值取值 根据是否使用alpha图层
	double check_match_paimon_param = out_genshin_paimon.config.check_match_paimon_params;
	if (genshin_screen.config.is_used_alpha == false)
	{
		cv::cvtColor(genshin_screen.img_paimon_maybe, giPaimonRef, cv::COLOR_RGBA2GRAY);
		// no_alpha下为 灰度模板
		template_not_handle_mode = paimon_template_no_alpha;
		template_handle_mode = paimon_template_no_alpha_handle_mode;
		// no_alpha下为 派蒙手柄比例二值模板 黑白
		template_mask_not_handle_mode = paimon_template;
		template_mask_handle_mode = paimon_template_handle_mode;
		check_match_paimon_param = out_genshin_paimon.config.check_match_paimon_params_no_alpha;
	}

	// 拆分图层
	std::vector<cv::Mat>  split_paimon;
	cv::split(giPaimonRef, split_paimon);

	cv::Mat template_result;
	// TODO HOTCODE
	cv::matchTemplate(split_paimon.back(), template_not_handle_mode, template_result, cv::TM_CCOEFF_NORMED, template_mask_not_handle_mode);

	double paimon_match_minVal, paimon_match_maxVal;
	cv::Point paimon_match_minLoc, paimon_match_maxLoc;
	cv::minMaxLoc(template_result, &paimon_match_minVal, &paimon_match_maxVal, &paimon_match_minLoc, &paimon_match_maxLoc);

	// 如果小于阈值，则尝试判断是否为手柄模式，否则为检测到派蒙
	if (paimon_match_maxVal < check_match_paimon_param || paimon_match_maxVal == 1)
	{
		if (paimon_match_maxVal > 0.2)
		{
			cv::Mat template_handle_mode_result;
			// TODO HOTCODE
			cv::matchTemplate(split_paimon.back(), template_handle_mode, template_handle_mode_result, cv::TM_CCOEFF_NORMED, template_mask_handle_mode);

			double paimon_match_handle_mode_minVal, paimon_match_handle_mode_maxVal;
			cv::Point paimon_match_handle_mode_minLoc, paimon_match_handle_mode_maxLoc;
			cv::minMaxLoc(template_handle_mode_result, &paimon_match_handle_mode_minVal, &paimon_match_handle_mode_maxVal, &paimon_match_handle_mode_minLoc, &paimon_match_handle_mode_maxLoc);
			if (paimon_match_handle_mode_maxVal > check_match_paimon_param)
			{
				out_genshin_paimon.is_handle_mode = true;
				out_genshin_paimon.is_visial = true;
				out_genshin_paimon.rect_paimon = cv::Rect(rect_origin.tl() + paimon_match_handle_mode_maxLoc, paimon_template_handle_mode.size());
			}
		}
		else
		{
			out_genshin_paimon.is_visial = false;
		}
	}
	else
	{
		out_genshin_paimon.is_handle_mode = false;
		out_genshin_paimon.is_visial = true;
		out_genshin_paimon.rect_paimon = cv::Rect(rect_origin.tl() + paimon_match_maxLoc, paimon_template.size());
	}
	return true;
}

bool TianLi::Genshin::Check::check_paimon(const GenshinScreen& genshin_screen, GenshinPaimon& out_genshin_paimon)
{
#ifdef TIANLI_DEV
		return check_paimon_2nd(genshin_screen, out_genshin_paimon);
#else
		return check_paimon_impl(genshin_screen, out_genshin_paimon);
#endif // TIANLI_DEV
}