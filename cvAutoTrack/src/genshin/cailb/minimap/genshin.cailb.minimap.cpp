#include "pch.h"
#include "genshin.cailb.minimap.h"

bool cailb_minimap_impl(const GenshinScreen& genshin_screen, GenshinMinimap& out_genshin_minimap)
{
	auto& paimon_rect = genshin_screen.config.rect_paimon;
	// not find paimon 
	if (paimon_rect.empty()) return false;

	// rect
	auto minimap_rect = genshin_screen.config.is_handle_mode ? genshin_screen.config.rect_minimap_handle : genshin_screen.config.rect_minimap;
	// center point
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

bool TianLi::Genshin::Cailb::cailb_minimap(const GenshinScreen& genshin_screen, GenshinMinimap& out_genshin_minimap)
{
	return cailb_minimap_impl(genshin_screen, out_genshin_minimap);
}
