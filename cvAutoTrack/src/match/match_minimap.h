#pragma once

struct GenshinHandleConfig
{
	bool is_auto_find_genshin = true;
	HWND genshin_handle = nullptr;

};
struct GenshinHandle
{
	bool is_exist = false;
	HWND handle;
	RECT rect;
	RECT rect_client;
	double scale;
	bool is_exist_title_bar = true;
	cv::Size size_frame;
	cv::Rect rect_paimon_maybe;
	cv::Rect rect_paimon;
	cv::Rect rect_minimap_cailb_maybe;
	cv::Rect rect_minimap_cailb;
	cv::Rect rect_minimap_maybe;
	cv::Rect rect_minimap;
	cv::Rect rect_avatar_maybe;
	cv::Rect rect_avatar;
	cv::Rect rect_uid_maybe;
	cv::Rect rect_uid;
	cv::Rect rect_star_maybe;
	cv::Rect rect_star;
	cv::Rect rect_map_flag_maybe;
	cv::Rect rect_map_flag;
	cv::Rect rect_map_flag_icon_maybe;
	cv::Rect rect_map_flag_icon;
	cv::Rect rect_left_give_items_maybe;
	cv::Rect rect_left_give_items;
	cv::Rect rect_right_pick_items_maybe;
	cv::Rect rect_right_pick_items;
	GenshinHandleConfig config;
};
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
	cv::Mat img_avatar_maybe;
	cv::Mat img_avatar;
	cv::Mat img_uid_maybe;
	cv::Mat img_uid;
	cv::Mat img_star_maybe;
	cv::Mat img_star;
	cv::Mat img_map_flag_maybe;
	cv::Mat img_map_flag;
	cv::Mat img_map_flag_icon_maybe;
	cv::Mat img_map_flag_icon;
	cv::Mat img_left_give_items_maybe;
	cv::Mat img_left_give_items;
	cv::Mat img_right_pick_items_maybe;
	cv::Mat img_right_pick_items;
	cv::Mat img_hp_maybe;
	cv::Mat img_hp;
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
namespace TianLi::Match
{
	GenshinHandle func_get_handle(HWND& in);

	void get_genshin_handle(GenshinHandle& genshin_handle);

	void update_genshin_handle(const HWND& old_handle, GenshinHandle& out_genshin_handle);

	void get_genshin_screen(const GenshinHandle& genshin_handle, GenshinScreen& out_genshin_screen);

	bool check_paimon(const GenshinScreen& genshin_screen, GenshinPaimon& out_genshin_paimon);

	bool match_minimap_cailb(const GenshinScreen& genshin_screen, GenshinMinimapCailb& out_genshin_minimap_cailb);

	bool splite_minimap(const GenshinScreen& genshin_screen, GenshinMinimap& out_genshin_minimap);
}