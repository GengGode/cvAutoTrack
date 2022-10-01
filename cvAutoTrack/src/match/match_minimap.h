#pragma once

struct GenshinScreenConfig
{
	// TianLi::Track::ScreenType screen_type = TianLi::Track::Bitblt;
	cv::Rect rect_paimon_maybe;
	cv::Rect rect_minimap_cailb_maybe;
	cv::Rect rect_minimap_maybe;

	bool is_handle_mode = false;

	cv::Rect rect_paimon;
	cv::Rect rect_minimap_cailb;
	cv::Rect rect_minimap;
};
struct GenshinScreen
{
	cv::Rect rect_client;

	cv::Mat img_screen;
	cv::Mat img_paimon_maybe;
	cv::Mat img_paimon;
	cv::Mat img_minimap_cailb_maybe;
	cv::Mat img_minimap_cailb;
	cv::Mat img_minimap_maybe;
	cv::Mat img_minimap;
	GenshinScreenConfig config;
};
struct GenshinPaimonConfig
{
	double check_match_paimon_params = 0.85;
};

struct GenshinPaimon
{
	bool is_visial = false;
	bool is_handle_mode = false;
	cv::Rect rect_paimon;
	GenshinPaimonConfig config;
};
struct GenshinMinimapCailbConfig
{
	bool is_need_find = true;
	double check_match_minimap_cailb_params = 0.85;
};

struct GenshinMinimapCailb
{
	bool is_visial = false;
	cv::Rect rect_minimap_cailb;
	GenshinMinimapCailbConfig config;
};
struct GenshinMinimapConfig
{
	bool is_need_cailb = true;
	bool is_find_paimon = false;

};

struct GenshinMinimap
{
	bool is_cailb = false;
	cv::Mat img_minimap;
	cv::Rect rect_minimap;
	cv::Point point_minimap_center;
	cv::Rect rect_avatar;
	cv::Mat img_avatar;
	cv::Rect rect_viewer;
	cv::Mat img_viewer;
	cv::Rect rect_stars;
	cv::Mat img_stars;

	GenshinMinimapConfig config;
};

void check_paimon(const GenshinScreen& genshin_screen, GenshinPaimon& out_genshin_paimon);

void match_minimap_cailb(const GenshinScreen& genshin_screen, GenshinMinimapCailb& out_genshin_minimap_cailb);

void splite_minimap(const GenshinScreen& genshin_screen, GenshinMinimap& out_genshin_minimap);