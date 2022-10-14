#include "pch.h"
#include "match_minimap.h"
#include "../resources/Resources.h"

#include <Windows.h>
#include <shellscalingapi.h>
#pragma comment(lib, "Shcore.lib")

namespace TianLi::Match
{

	GenshinHandle func_get_handle(HWND& in)
	{
		static GenshinHandle out;
		if (in == 0)
		{
			get_genshin_handle(out);
		}
		else
		{
			update_genshin_handle(in, out);
		}
		return out;
	};

	void get_genshin_handle(GenshinHandle& genshin_handle)
	{
		if (genshin_handle.config.is_auto_find_genshin)
		{
			//genshin_handle.handle = FindWindowA(NULL, "窗口位置调整");

			auto& giHandle = genshin_handle.handle;
			LPCWSTR giWindowName = { L"原神" };
			/* 对原神窗口的操作 */
			giHandle = FindWindowA("UnityWndClass", "原神");
			if (giHandle == NULL)
			{
				giWindowName = L"Genshin Impact";
				giHandle = FindWindowW(L"UnityWndClass", giWindowName); /* 匹配名称：原神 */
			}
			if (giHandle == NULL)
			{
				giWindowName = L"원신";
				giHandle = FindWindowW(L"UnityWndClass", giWindowName); /* 匹配名称：원신 */
			}
			if (giHandle == NULL)
			{
				giWindowName = L"\u539F\u795E";
				giHandle = FindWindowW(L"UnityWndClass", giWindowName); /* 匹配名称：原神 */
			}
			if (giHandle == NULL)
			{
				giWindowName = L"\uC6D0\uC2E0";
				giHandle = FindWindowW(L"UnityWndClass", giWindowName); /* 匹配名称：원신 */
			}

		}
		else
		{
			genshin_handle.handle = genshin_handle.config.genshin_handle;
		}
		if (genshin_handle.handle != 0)
		{
			genshin_handle.is_exist = true;
		}
		else
		{
			genshin_handle.is_exist = false;
#ifdef TEST_LOCAL
#else
			return;

#endif
		}
		// 判断窗口是否存在标题栏
		if (GetWindowLong(genshin_handle.handle, GWL_STYLE) & WS_CAPTION)
		{
			genshin_handle.is_exist_title_bar = true;
		}
		else
		{
			genshin_handle.is_exist_title_bar = false;
		}
		// 获取窗口大小
		GetWindowRect(genshin_handle.handle, &genshin_handle.rect);
		// 获取除标题栏区域大小
		GetClientRect(genshin_handle.handle, &genshin_handle.rect_client);
		// 获取缩放比例
		HMONITOR hMonitor = MonitorFromWindow(genshin_handle.handle, MONITOR_DEFAULTTONEAREST);
		UINT dpiX, dpiY;
		GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
		genshin_handle.scale = dpiX / 96.0;

		{
#ifdef TEST_LOCAL
			static cv::Mat img = cv::imread("C:\\Users\\GengG\\source\\repos\\Cv测试\\OpencvConsole\\img3.png", -1);
			int x = img.cols;
			int y = img.rows;
#else
			int x = genshin_handle.rect_client.right - genshin_handle.rect_client.left;
			int y = genshin_handle.rect_client.bottom - genshin_handle.rect_client.top;
#endif
			double f = 1, fx = 1, fy = 1;

			if (static_cast<double>(x) / static_cast<double>(y) == 16.0 / 9.0)
			{
				genshin_handle.size_frame = cv::Size(1920, 1080);
			}
			else if (static_cast<double>(x) / static_cast<double>(y) > 16.0 / 9.0)
			{

				//高型，以宽为比例

				// x = (y * 16) / 9;
				f = y / 1080.0;
				//将giFrame缩放到1920*1080的比例
				fx = x / f;
				// 将图片缩放
				genshin_handle.size_frame = cv::Size(static_cast<int>(fx), 1080);

			}
			else if (static_cast<double>(x) / static_cast<double>(y) < 16.0 / 9.0)
			{

				//宽型，以高为比例

				// x = (y * 16) / 9;
				f = x / 1920.0;
				//将giFrame缩放到1920*1080的比例
				fy = y / f;
				// 将图片缩放
				genshin_handle.size_frame = cv::Size(1920, static_cast<int>(fy));
			}
			x = genshin_handle.size_frame.width;
			y = genshin_handle.size_frame.height;
			// 派蒙可能性区域计算参数
			int paimon_mayArea_left = 0;
			int paimon_mayArea_top = 0;
			int paimon_mayArea_width = static_cast<int>(x * 0.10);
			int paimon_mayArea_height = static_cast<int>(y * 0.10);
			// 派蒙可能性区域
			cv::Rect Area_Paimon_mayArea(
				paimon_mayArea_left,
				paimon_mayArea_top,
				paimon_mayArea_width,
				paimon_mayArea_height);
			genshin_handle.rect_paimon_maybe = Area_Paimon_mayArea;

			// 小地图标定可能性区域计算参数
			int miniMap_Cailb_mayArea_left = static_cast<int>(x * 0.10);
			int miniMap_Cailb_mayArea_top = 0;
			int miniMap_Cailb_mayArea_width = static_cast<int>(x * 0.10);
			int miniMap_Cailb_mayArea_height = static_cast<int>(y * 0.10);
			// 小地图标定可能性区域
			cv::Rect Area_MiniMap_Cailb_mayArea(
				miniMap_Cailb_mayArea_left,
				miniMap_Cailb_mayArea_top,
				miniMap_Cailb_mayArea_width,
				miniMap_Cailb_mayArea_height);
			genshin_handle.rect_minimap_cailb_maybe = Area_MiniMap_Cailb_mayArea;

			// 小地图可能性区域计算参数
			int miniMap_mayArea_left = 0;
			int miniMap_mayArea_top = 0;
			int miniMap_mayArea_width = static_cast<int>(x * 0.18);
			int miniMap_mayArea_height = static_cast<int>(y * 0.22);
			// 小地图可能性区域
			cv::Rect Area_MiniMap_mayArea(
				miniMap_mayArea_left,
				miniMap_mayArea_top,
				miniMap_mayArea_width,
				miniMap_mayArea_height);
			genshin_handle.rect_minimap_maybe = Area_MiniMap_mayArea;

			// UID可能性区域计算参数
			int UID_mayArea_left = static_cast<int>(x * 0.88);
			int UID_mayArea_top = static_cast<int>(y * 0.97);
			int UID_mayArea_width = x - UID_mayArea_left;
			int UID_mayArea_height = y - UID_mayArea_top;
			// UID可能性区域
			cv::Rect Area_UID_mayArea(
				UID_mayArea_left,
				UID_mayArea_top,
				UID_mayArea_width,
				UID_mayArea_height);
			genshin_handle.rect_uid_maybe = Area_UID_mayArea;
			int UID_Rect_x = cvCeil(x - x * (1.0 - 0.865));
			int UID_Rect_y = cvCeil(y - 1080.0 * (1.0 - 0.9755));
			int UID_Rect_w = cvCeil(1920 * 0.11);
			int UID_Rect_h = cvCeil(1920 * 0.0938 * 0.11);


			genshin_handle.rect_uid = cv::Rect(UID_Rect_x, UID_Rect_y, UID_Rect_w, UID_Rect_h);

			// 左侧已获取物品可能性区域计算参数
			int leftGetItems_mayArea_left = static_cast<int>(x * 0.570);
			int leftGetItems_mayArea_top = static_cast<int>(y * 0.250);
			int leftGetItems_mayArea_width = static_cast<int>(x * 0.225);
			int leftGetItems_mayArea_height = static_cast<int>(y * 0.500);
			// 左侧已获取物品可能性区域
			cv::Rect Area_LeftGetItems_mayArea(
				leftGetItems_mayArea_left,
				leftGetItems_mayArea_top,
				leftGetItems_mayArea_width,
				leftGetItems_mayArea_height);
			genshin_handle.rect_left_give_items_maybe = Area_LeftGetItems_mayArea;

			// 右侧可捡取物品可能性区域计算参数
			int rightGetItems_mayArea_left = static_cast<int>(x * 0.050);
			int rightGetItems_mayArea_top = static_cast<int>(y * 0.460);
			int rightGetItems_mayArea_width = static_cast<int>(x * 0.160);
			int rightGetItems_mayArea_height = static_cast<int>(y * 0.480);
			// 右侧可捡取物品可能性区域
			cv::Rect Area_RightGetItems_mayArea(
				rightGetItems_mayArea_left,
				rightGetItems_mayArea_top,
				rightGetItems_mayArea_width,
				rightGetItems_mayArea_height);
			genshin_handle.rect_right_pick_items_maybe = Area_RightGetItems_mayArea;
		}
	}

	void update_genshin_handle(const HWND& old_handle, GenshinHandle& out_genshin_handle)
	{
		static unsigned char tick_count = 0;
		if (IsWindowVisible(old_handle))
		{
			if (tick_count < 30)
			{
				tick_count++;
			}
			else
			{
				tick_count = 0;
				get_genshin_handle(out_genshin_handle);
			}
		}
		else
		{
			get_genshin_handle(out_genshin_handle);
		}
		return;
	}


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