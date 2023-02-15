#include "pch.h"
#include "genshin.check.paimon.h"
#include "../../../resources/Resources.h"

auto get_point_vec3b(const cv::Mat& mat, int x, int y, bool is_need_cvt)
{
	if (is_need_cvt)
	{
		auto screen_color_v4 = mat.at<cv::Vec4b>(y, x);
		return cv::Vec3b(screen_color_v4[0], screen_color_v4[1], screen_color_v4[2]);
	}
	return mat.at<cv::Vec3b>(y, x);

}
double cala_keypoint_diff(const cv::Mat& mat,const cv::Rect& roi,const std::vector<std::pair<cv::Point, cv::Vec3b>> paimon_keys, bool is_need_cvt)
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
bool check_paimon_search_impl(const GenshinScreen& genshin_screen, GenshinPaimon& out_genshin_paimon)
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
#ifdef _DEBUG
				std::cout << "模版匹配手柄成功" << std::endl;
#endif // DEBUG
			}
		}
		else
		{
			out_genshin_paimon.is_visial = false;
#ifdef _DEBUG
			std::cout << "模板匹配失败" << std::endl;
#endif // DEBUG

		}
	}
	else
	{
		out_genshin_paimon.is_handle_mode = false;
		out_genshin_paimon.is_visial = true;
		out_genshin_paimon.rect_paimon = cv::Rect(rect_origin.tl() + paimon_match_maxLoc, paimon_template.size());
#ifdef _DEBUG
		std::cout << "模版匹配键鼠成功" << std::endl;
#endif // DEBUG
	}
	return true;
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

	// 尝试匹配键鼠标模板
	if (genshin_screen.config.is_hairtail_screen)			//带鱼屏，切片
	{
		cv::Rect clipRect(cv::Point(72, 0), cv::Point(giPaimonRef.cols, giPaimonRef.rows));
		giPaimonRef = giPaimonRef(clipRect);
#ifdef _DEBUG
		std::cout << "带鱼屏模式" << std::endl;
#endif // DEBUG
	}
	double paimon_check_diff = cala_keypoint_diff(giPaimonRef, rect_keypoint, paimon_keys, is_need_cvt);
	if (paimon_check_diff < out_genshin_paimon.config.check_match_paimon_keypoint_params)
	{
		out_genshin_paimon.is_handle_mode = false;
		out_genshin_paimon.is_visial = true;
		out_genshin_paimon.rect_paimon = genshin_screen.config.rect_paimon_keypoint;
		if (genshin_screen.config.is_hairtail_screen)
			out_genshin_paimon.rect_paimon += cv::Point(72, 0);
#ifdef _DEBUG
		std::cout << "快速模式匹配键鼠成功" << std::endl;
#endif // DEBUG

		return true;
	}

	// 匹配失败，改为匹配手柄模板
	giPaimonRef = genshin_screen.img_paimon_maybe;
	double paimon_handle_check_diff = cala_keypoint_diff(giPaimonRef, rect_keypoint_handle, paimon_handle_keys, is_need_cvt);
	if (paimon_handle_check_diff < out_genshin_paimon.config.check_match_paimon_keypoint_params)
	{
		out_genshin_paimon.is_handle_mode = true;
		out_genshin_paimon.is_visial = true;
		out_genshin_paimon.rect_paimon = genshin_screen.config.rect_paimon_keypoint_handle;
#ifdef _DEBUG
		std::cout << "快速模式匹配手柄成功" << std::endl;
#endif // DEBUG
		return true;
	}

	// 正常模式和手柄模式都未匹配到，尝试搜索匹配
	return check_paimon_search_impl(genshin_screen, out_genshin_paimon);
}

bool TianLi::Genshin::Check::check_paimon(const GenshinScreen& genshin_screen, GenshinPaimon& out_genshin_paimon)
{
	return check_paimon_impl(genshin_screen, out_genshin_paimon);
}