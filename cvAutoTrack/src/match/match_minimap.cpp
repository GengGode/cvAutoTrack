#include "pch.h"
#include "match_minimap.h"
#include "../resources/Resources.h"

#include <Windows.h>
#include <shellscalingapi.h>
#pragma comment(lib, "Shcore.lib")

namespace TianLi::Match
{



	void get_genshin_screen(const GenshinHandle& genshin_handle, GenshinScreen& out_genshin_screen)
	{
		static HBITMAP hBmp;

		auto& giHandle = genshin_handle.handle;
		auto& giRect = genshin_handle.rect;
		auto& giRectClient = genshin_handle.rect_client;
		auto& giScale = genshin_handle.scale;
		auto& giFrame = out_genshin_screen.img_screen;

#ifdef TEST_LOCAL
		static cv::Mat img = cv::imread("C:\\Users\\GengG\\source\\repos\\Cv测试\\OpencvConsole\\img3.png", -1);
		giFrame = img;
#else
		if (genshin_handle.handle == NULL)
			return;

		//获取目标句柄的DC
		HDC hScreen = GetDC(giHandle);/* 对原神窗口的操作 */
		HDC hCompDC = CreateCompatibleDC(hScreen);

		//获取目标句柄的宽度和高度
		int	nWidth = static_cast<int>(std::round((giRectClient.right - giRectClient.left) / giScale));
		int	nHeight = static_cast<int>(std::round((giRectClient.bottom - giRectClient.top) / giScale));

		DeleteObject(hBmp);

		//创建Bitmap对象
		hBmp = CreateCompatibleBitmap(hScreen, nWidth, nHeight);//得到位图

		SelectObject(hCompDC, hBmp);
		BitBlt(hCompDC, 0, 0, nWidth, nHeight, hScreen, 0, 0, SRCCOPY);

		//释放对象
		DeleteDC(hScreen);
		DeleteDC(hCompDC);

		BITMAP bmp;

		//类型转换
		GetObject(hBmp, sizeof(BITMAP), &bmp);
		int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
		//int depth = bmp.bmBitsPixel == 1 ? 1 : 8;

		//mat操作

		giFrame.create(cv::Size(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8U, nChannels));

		GetBitmapBits(hBmp, bmp.bmHeight * bmp.bmWidth * nChannels, giFrame.data);

		if (giFrame.channels() == 3)
		{
			cvtColor(giFrame, giFrame, cv::COLOR_RGB2RGBA);
		}

		cv::resize(giFrame, giFrame, genshin_handle.size_frame);

#endif // TEST_LOCAL


		{
			if (giFrame.empty())return;

			out_genshin_screen.rect_client = cv::Rect(giRect.left, giRect.top, giRectClient.right - giRectClient.left, giRectClient.bottom - giRectClient.top);

			// 获取maybe区域
			out_genshin_screen.img_paimon_maybe = giFrame(genshin_handle.rect_paimon_maybe);
			out_genshin_screen.img_minimap_cailb_maybe = giFrame(genshin_handle.rect_minimap_cailb_maybe);
			out_genshin_screen.img_minimap_maybe = giFrame(genshin_handle.rect_minimap_maybe);
			out_genshin_screen.img_uid_maybe = giFrame(genshin_handle.rect_uid_maybe);
			out_genshin_screen.img_left_give_items_maybe = giFrame(genshin_handle.rect_left_give_items_maybe);
			out_genshin_screen.img_right_pick_items_maybe = giFrame(genshin_handle.rect_right_pick_items_maybe);

			out_genshin_screen.config.rect_paimon_maybe = genshin_handle.rect_paimon_maybe;
			out_genshin_screen.config.rect_minimap_cailb_maybe = genshin_handle.rect_minimap_cailb_maybe;
			out_genshin_screen.config.rect_minimap_maybe = genshin_handle.rect_minimap_maybe;


			out_genshin_screen.img_uid = giFrame(genshin_handle.rect_uid);

		}
	}


	bool check_paimon(const GenshinScreen& genshin_screen, GenshinPaimon& out_genshin_paimon)
	{
		static std::vector<cv::Mat> split_paimon_template;
		static cv::Mat paimon_template_handle_mode;
		static bool is_first = true;
		if (is_first)
		{
			cv::Mat paimon;
			Resources::getInstance().PaimonTemplate.copyTo(paimon);
			cv::resize(paimon, paimon, cv::Size(68, 77));
			cv::split(paimon, split_paimon_template);
			cv::resize(split_paimon_template[3], paimon_template_handle_mode, cv::Size(), 1.0 / 1.20, 1.0 / 1.20);
			is_first = false;
		}
		auto& giPaimonRef = genshin_screen.img_paimon_maybe;
		auto& rect_origin = genshin_screen.config.rect_paimon_maybe;

		if (giPaimonRef.empty() || paimon_template_handle_mode.empty()) return false;
		if (giPaimonRef.cols < split_paimon_template[3].cols || giPaimonRef.rows < split_paimon_template[3].rows) return false;

		std::vector<cv::Mat>  split_paimon;
		cv::split(giPaimonRef, split_paimon);

		cv::Mat template_result;
		// TODO HOTCODE
		cv::matchTemplate(split_paimon[3], split_paimon_template[3], template_result, cv::TM_CCOEFF_NORMED);

		double paimon_match_minVal, paimon_match_maxVal;
		cv::Point paimon_match_minLoc, paimon_match_maxLoc;
		cv::minMaxLoc(template_result, &paimon_match_minVal, &paimon_match_maxVal, &paimon_match_minLoc, &paimon_match_maxLoc);

		// 如果小于阈值，则尝试判断是否为手柄模式，否则为检测到派蒙
		if (paimon_match_maxVal < out_genshin_paimon.config.check_match_paimon_params || paimon_match_maxVal == 1)
		{
			if (paimon_match_maxVal > 0.5)
			{
				cv::Mat template_handle_mode_result;
				// TODO HOTCODE
				cv::matchTemplate(split_paimon[3], paimon_template_handle_mode, template_handle_mode_result, cv::TM_CCOEFF_NORMED);

				double paimon_match_handle_mode_minVal, paimon_match_handle_mode_maxVal;
				cv::Point paimon_match_handle_mode_minLoc, paimon_match_handle_mode_maxLoc;
				cv::minMaxLoc(template_handle_mode_result, &paimon_match_handle_mode_minVal, &paimon_match_handle_mode_maxVal, &paimon_match_handle_mode_minLoc, &paimon_match_handle_mode_maxLoc);
				if (paimon_match_handle_mode_maxVal > out_genshin_paimon.config.check_match_paimon_params)
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
			out_genshin_paimon.rect_paimon = cv::Rect(rect_origin.tl() + paimon_match_maxLoc, split_paimon_template[3].size());
		}
		return true;
	}
	bool match_minimap_cailb(const GenshinScreen& genshin_screen, GenshinMinimapCailb& out_genshin_minimap_cailb)
	{
		static std::vector<cv::Mat> split_minimap_cailb_template;
		static cv::Mat minimap_cailb_template_handle_mode;
		static bool is_first = true;
		if (is_first)
		{
			cv::Mat minimap_cailb;
			cv::resize(Resources::getInstance().MinimapCailbTemplate, minimap_cailb, cv::Size(), 0.8, 0.8);
			cv::split(minimap_cailb, split_minimap_cailb_template);
			cv::resize(split_minimap_cailb_template[3], minimap_cailb_template_handle_mode, cv::Size(), 1 / 1.2, 1 / 1.2, cv::INTER_CUBIC);
			is_first = false;
		}

		auto& giMinimapCailbRef = genshin_screen.img_minimap_cailb_maybe;
		auto& rect_origin = genshin_screen.config.rect_minimap_cailb_maybe;
		auto& is_handle_mode = genshin_screen.config.is_handle_mode;

		if (giMinimapCailbRef.empty() || minimap_cailb_template_handle_mode.empty()) return false;
		if (giMinimapCailbRef.cols < split_minimap_cailb_template[3].cols || giMinimapCailbRef.rows < split_minimap_cailb_template[3].rows) return false;

		std::vector<cv::Mat>  split_minimap_cailb;
		cv::split(giMinimapCailbRef, split_minimap_cailb);

		cv::Mat template_result;
		if (!is_handle_mode)
		{
			// TODO HOTCODE
			cv::matchTemplate(split_minimap_cailb[3], split_minimap_cailb_template[3], template_result, cv::TM_CCOEFF_NORMED);
		}
		else
		{
			// TODO HOTCODE
			cv::matchTemplate(split_minimap_cailb[3], minimap_cailb_template_handle_mode, template_result, cv::TM_CCOEFF_NORMED);
		}


		double minimap_cailb_match_minVal, minimap_cailb_match_maxVal;
		cv::Point minimap_cailb_match_minLoc, minimap_cailb_match_maxLoc;
		cv::minMaxLoc(template_result, &minimap_cailb_match_minVal, &minimap_cailb_match_maxVal, &minimap_cailb_match_minLoc, &minimap_cailb_match_maxLoc);

		if (minimap_cailb_match_maxVal < out_genshin_minimap_cailb.config.check_match_minimap_cailb_params || minimap_cailb_match_maxVal == 1)
		{
			out_genshin_minimap_cailb.is_visial = false;
		}
		else
		{
			out_genshin_minimap_cailb.is_visial = true;
			if (!is_handle_mode)
			{
				out_genshin_minimap_cailb.rect_minimap_cailb = cv::Rect(rect_origin.tl() + minimap_cailb_match_maxLoc, split_minimap_cailb_template[3].size());
			}
			else
			{
				out_genshin_minimap_cailb.rect_minimap_cailb = cv::Rect(rect_origin.tl() + minimap_cailb_match_maxLoc, minimap_cailb_template_handle_mode.size());
			}
		}

		return true;
	}
	bool splite_minimap(const GenshinScreen& genshin_screen, GenshinMinimap& out_genshin_minimap)
	{
		auto& paimon_rect = genshin_screen.config.rect_paimon;
		auto& minimap_cailb_rect = genshin_screen.config.rect_minimap_cailb;

		if (paimon_rect.empty() || minimap_cailb_rect.empty()) return false;

		// 1. 以paimon为基准，计算出minimap的左边界
		auto minimap_left = paimon_rect.x + paimon_rect.width / 2;
		// 2. 以minimap_cailb为基准，计算出minimap的右边界
		auto minimap_right = minimap_cailb_rect.x + minimap_cailb_rect.width / 2;
		// 3. 以paimon和minimap_cailb为基准，计算出minimap的上边界
		auto minimap_top = (paimon_rect.y + minimap_cailb_rect.y) / 2;
		// 4. 计算出minimap的宽度
		auto minimap_width = minimap_right - minimap_left;
		// 5. 计算出minimap的高度
		auto minimap_height = minimap_width;
		// 6. 计算出minimap的下边界	
		auto minimap_bottom = minimap_top + minimap_height;
		// 7. 计算出minimap的左上角坐标
		auto minimap_left_top = cv::Point(minimap_left, minimap_top);
		// 8. 计算出minimap的右下角坐标
		auto minimap_right_bottom = cv::Point(minimap_right, minimap_bottom);
		// 9. 计算出minimap的矩形区域
		auto minimap_rect = cv::Rect(minimap_left_top, minimap_right_bottom);
		// 10. 以minimap_rect为基准，计算出minimap的中心点
		auto minimap_center = cv::Point(minimap_rect.x + minimap_rect.width / 2, minimap_rect.y + minimap_rect.height / 2);

		out_genshin_minimap.img_minimap = genshin_screen.img_screen(minimap_rect);
		out_genshin_minimap.rect_minimap = minimap_rect;
		out_genshin_minimap.point_minimap_center = minimap_center;

		int Avatar_Rect_x = cvRound(minimap_width * 0.4);
		int Avatar_Rect_y = cvRound(minimap_height * 0.4);
		int Avatar_Rect_w = cvRound(minimap_width * 0.2);
		int Avatar_Rect_h = cvRound(minimap_height * 0.2);

		out_genshin_minimap.rect_avatar = cv::Rect(Avatar_Rect_x, Avatar_Rect_y, Avatar_Rect_w, Avatar_Rect_h);
		out_genshin_minimap.img_avatar = out_genshin_minimap.img_minimap(out_genshin_minimap.rect_avatar);

		int Viewer_Rect_x = cvRound(minimap_width * 0.2);
		int Viewer_Rect_y = cvRound(minimap_height * 0.2);
		int Viewer_Rect_w = cvRound(minimap_width * 0.6);
		int Viewer_Rect_h = cvRound(minimap_height * 0.6);

		out_genshin_minimap.rect_viewer = cv::Rect(Viewer_Rect_x, Viewer_Rect_y, Viewer_Rect_w, Viewer_Rect_h);
		out_genshin_minimap.img_viewer = out_genshin_minimap.img_minimap(out_genshin_minimap.rect_viewer);

		int Stars_Rect_x = cvRound(minimap_width * 0.165);
		int Stars_Rect_y = cvRound(minimap_height * 0.165);
		int Stars_Rect_w = cvRound(minimap_width * 0.67);
		int Stars_Rect_h = cvRound(minimap_height * 0.67);

		out_genshin_minimap.rect_stars = cv::Rect(Stars_Rect_x, Stars_Rect_y, Stars_Rect_w, Stars_Rect_h);
		out_genshin_minimap.img_stars = out_genshin_minimap.img_minimap(out_genshin_minimap.rect_stars);

		return true;
	}
}