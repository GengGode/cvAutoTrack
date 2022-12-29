#include "pch.h"
#include "genshin.screen.h"
#include "capture/Capture.h"
#include <chrono>

void TianLi::Genshin::get_genshin_screen(const GenshinHandle& genshin_handle, GenshinScreen& out_genshin_screen)
{
	static HBITMAP hBmp;

	//auto& giHandle = genshin_handle.handle;
	auto& giRect = genshin_handle.rect;
	auto& giRectClient = genshin_handle.rect_client;
	//auto& giScale = genshin_handle.scale;
	auto& giFrame = out_genshin_screen.img_screen;

#ifdef TEST_LOCAL
	static cv::Mat img = cv::imread("C:\\Users\\GengG\\source\\repos\\Cv测试\\OpencvConsole\\img3.png", -1);
	giFrame = img;
#else

	static auto last_time = std::chrono::system_clock::now();
	auto now_time = std::chrono::system_clock::now();
	if (now_time - last_time < 20ms)
	{
		return;
	}
	else
	{
		last_time = now_time;
	}

	genshin_handle.config.capture->capture(giFrame);
	
#endif // TEST_LOCAL


	{
		if (giFrame.empty())return;
		cv::resize(giFrame, giFrame, genshin_handle.size_frame);

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
