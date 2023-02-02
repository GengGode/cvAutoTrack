#include "pch.h"
#include "match_minimap.h"
#include "../resources/Resources.h"

#include <Windows.h>
#include <shellscalingapi.h>
#pragma comment(lib, "Shcore.lib")


namespace TianLi::Match
{
	bool match_minimap_cailb(const GenshinScreen& genshin_screen, GenshinMinimapCailb& out_genshin_minimap_cailb)
	{
		static std::vector<cv::Mat> split_minimap_cailb_template;
		static cv::Mat minimap_cailb_template;
		static cv::Mat minimap_cailb_template_handle_mode;
		static cv::Mat minimap_cailb_template_no_alpha;
		static cv::Mat minimap_cailb_template_no_alpha_handle_mode;
		static bool is_first = true;
		if (is_first)
		{
			cv::Mat minimap_cailb;
			cv::resize(Resources::getInstance().MinimapCailbTemplate, minimap_cailb, cv::Size(), 0.8, 0.8);
			cv::split(minimap_cailb, split_minimap_cailb_template);
			minimap_cailb_template = split_minimap_cailb_template[3];
			minimap_cailb_template_no_alpha = split_minimap_cailb_template[0];
			cv::resize(split_minimap_cailb_template[3], minimap_cailb_template_handle_mode, cv::Size(), 1 / 1.2, 1 / 1.2, cv::INTER_CUBIC);
			cv::resize(split_minimap_cailb_template[3], minimap_cailb_template_no_alpha_handle_mode, cv::Size(), 1.0 / 1.2, 1.0 / 1.2);
			is_first = false;
		}

		auto giMinimapCailbRef = genshin_screen.img_minimap_cailb_maybe;
		auto& rect_origin = genshin_screen.config.rect_minimap_cailb_maybe;
		auto& is_handle_mode = genshin_screen.config.is_handle_mode;

		auto& template_not_handle_mode = split_minimap_cailb_template[3];
		auto& template_handle_mode = minimap_cailb_template_handle_mode;
		
		if (giMinimapCailbRef.empty() || minimap_cailb_template_handle_mode.empty()) return false;
		if (giMinimapCailbRef.cols < split_minimap_cailb_template[3].cols || giMinimapCailbRef.rows < split_minimap_cailb_template[3].rows) return false;

		// 设置阈值取值 根据是否使用alpha图层
		double check_match_minimap_cailb_param = out_genshin_minimap_cailb.config.check_match_minimap_cailb_params;
		if (genshin_screen.config.is_used_alpha == false)
		{
			cv::cvtColor(genshin_screen.img_minimap_cailb_maybe, giMinimapCailbRef, cv::COLOR_RGBA2GRAY);
			template_not_handle_mode = minimap_cailb_template_no_alpha;
			template_handle_mode = minimap_cailb_template_no_alpha_handle_mode;
			check_match_minimap_cailb_param = out_genshin_minimap_cailb.config.check_match_minimap_cailb_params_no_alpha;
		}

		
		std::vector<cv::Mat>  split_minimap_cailb;
		cv::split(giMinimapCailbRef, split_minimap_cailb);

		cv::Mat template_result;
		if (is_handle_mode)
		{
			// TODO HOTCODE
			cv::matchTemplate(split_minimap_cailb.back(), template_handle_mode, template_result, cv::TM_CCOEFF_NORMED);
		}
		else
		{
			// TODO HOTCODE
			cv::matchTemplate(split_minimap_cailb.back(), template_not_handle_mode, template_result, cv::TM_CCOEFF_NORMED);
		}


		double minimap_cailb_match_minVal, minimap_cailb_match_maxVal;
		cv::Point minimap_cailb_match_minLoc, minimap_cailb_match_maxLoc;
		cv::minMaxLoc(template_result, &minimap_cailb_match_minVal, &minimap_cailb_match_maxVal, &minimap_cailb_match_minLoc, &minimap_cailb_match_maxLoc);

		if (minimap_cailb_match_maxVal < check_match_minimap_cailb_param || minimap_cailb_match_maxVal == 1)
		{
			out_genshin_minimap_cailb.is_visial = false;
		}
		else
		{
			out_genshin_minimap_cailb.is_visial = true;
			if (is_handle_mode)
			{
				out_genshin_minimap_cailb.rect_minimap_cailb = cv::Rect(rect_origin.tl() + minimap_cailb_match_maxLoc, template_handle_mode.size());
			}
			else
			{
				out_genshin_minimap_cailb.rect_minimap_cailb = cv::Rect(rect_origin.tl() + minimap_cailb_match_maxLoc, template_not_handle_mode.size());
			}
		}

		return true;
	}

	bool splite_minimap(const GenshinScreen& genshin_screen, GenshinMinimap& out_genshin_minimap)
	{
		auto& paimon_rect = genshin_screen.config.rect_paimon;
		// 检测不到派蒙，返回false
		if (paimon_rect.empty()) return false;
		
		// 矩形
		auto minimap_rect = genshin_screen.config.is_handle_mode ? genshin_screen.config.rect_minimap_handle : genshin_screen.config.rect_minimap;
		// 中心点
		auto minimap_center = cv::Point(minimap_rect.x + (minimap_rect.width) / 2, minimap_rect.y + (minimap_rect.height) / 2);
		out_genshin_minimap.img_minimap = genshin_screen.img_screen(minimap_rect);
		out_genshin_minimap.rect_minimap = minimap_rect;
		out_genshin_minimap.point_minimap_center = minimap_center;
		
		int Avatar_Rect_x = cvRound(minimap_rect.width * 0.4);
		int Avatar_Rect_y = cvRound(minimap_rect.height * 0.4);
		int Avatar_Rect_w = cvRound(minimap_rect.width * 0.2);
		int Avatar_Rect_h = cvRound(minimap_rect.height * 0.2);

		out_genshin_minimap.rect_avatar = cv::Rect(Avatar_Rect_x, Avatar_Rect_y, Avatar_Rect_w, Avatar_Rect_h);
		out_genshin_minimap.img_avatar = out_genshin_minimap.img_minimap(out_genshin_minimap.rect_avatar);

		int Viewer_Rect_x = cvRound(minimap_rect.width * 0.2);
		int Viewer_Rect_y = cvRound(minimap_rect.height * 0.2);
		int Viewer_Rect_w = cvRound(minimap_rect.width * 0.6);
		int Viewer_Rect_h = cvRound(minimap_rect.height * 0.6);

		out_genshin_minimap.rect_viewer = cv::Rect(Viewer_Rect_x, Viewer_Rect_y, Viewer_Rect_w, Viewer_Rect_h);
		out_genshin_minimap.img_viewer = out_genshin_minimap.img_minimap(out_genshin_minimap.rect_viewer);

		int Stars_Rect_x = cvRound(minimap_rect.width * 0.165);
		int Stars_Rect_y = cvRound(minimap_rect.height * 0.165);
		int Stars_Rect_w = cvRound(minimap_rect.width * 0.67);
		int Stars_Rect_h = cvRound(minimap_rect.height * 0.67);

		out_genshin_minimap.rect_stars = cv::Rect(Stars_Rect_x, Stars_Rect_y, Stars_Rect_w, Stars_Rect_h);
		out_genshin_minimap.img_stars = out_genshin_minimap.img_minimap(out_genshin_minimap.rect_stars);

		return true;
	}
}