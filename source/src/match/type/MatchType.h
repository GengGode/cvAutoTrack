#pragma once
class Capture;
class Filter;

enum GenshinWindowClass
{
	None,
	Unity,
	Obs,
	Qt,
};

const std::vector<std::pair<std::wstring, GenshinWindowClass>> GenshinWindowNameList =
{
	{L"原神",GenshinWindowClass::Unity},
	{L"云·原神", GenshinWindowClass::Qt} ,
	{L"窗口投影（源） - 云·原神",GenshinWindowClass::Obs},
	{L"원신", GenshinWindowClass::Unity},
	{L"Genshin Impact", GenshinWindowClass::Unity},
	{L"原神-调试", GenshinWindowClass::None}
};
const cv::Rect MatchPaimonKeyPointRect =
{
	25,12,68,77
};
const cv::Rect MatchPaimonHandleKeyPointRect =
{
	89,42,57,64
};
const std::vector<std::pair<cv::Point, cv::Vec3b>> MatchPaimonKeyPointList =
{
{{42,19},{143,196,233}},
{{17,23},{255,255,255}},
{{29,29},{101, 72, 41}},
{{52,41},{255,255,255}},
{{32,47},{255,255,255}},
{{19,51},{224,238,250}},
{{49,58},{255,255,255}},
{{24,70},{164,112, 70}},
};

const std::vector<std::pair<cv::Point, cv::Vec3b>> MatchPaimonHandleKeyPointList =
{
{{35,16},{143,196,233}},
{{16,19},{255,255,255}},
{{24,24},{101, 71, 42}},
{{38,28},{255,255,255}},
{{27,39},{255,255,255}},
{{16,42},{224,238,250}},
{{42,46},{255,255,255}},
{{20,57},{159,109, 67}},
};

const cv::Rect MiniMapRect =
{
	59,15,218,218
};
const cv::Rect MiniMapHandleRect =
{
	117,45,182,182
};

struct GenshinHandleConfig
{
	bool is_auto_find_genshin = true;
	bool is_force_used_no_alpha = false;
	HWND genshin_handle = nullptr;
	std::shared_ptr<Capture> capture;

	std::vector<std::pair<std::wstring, GenshinWindowClass>> genshin_window_name_list = GenshinWindowNameList;
};
// 用于匹配原神窗口的句柄相关变量
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
	
	bool is_used_alpha = true;

	bool is_handle_mode = false;
	bool is_search_mode = false;

	cv::Rect rect_paimon;
	cv::Rect rect_minimap_cailb;
	cv::Rect rect_minimap = MiniMapRect;
	cv::Rect rect_minimap_handle = MiniMapHandleRect;
};
// 用于获取原神画面的相关变量
struct GenshinScreen
{
	cv::Rect rect_client;
	std::chrono::system_clock::time_point last_time = std::chrono::system_clock::now();

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
	double check_match_paimon_params = 0.80;
	double check_match_paimon_params_no_alpha = 0.50;
	std::vector<std::pair<cv::Point, cv::Vec3b>> paimon_check_vec = MatchPaimonKeyPointList;
	std::vector<std::pair<cv::Point, cv::Vec3b>> paimon_handle_check_vec = MatchPaimonHandleKeyPointList;
	double check_match_paimon_keypoint_params = 16;
	// 正常模式下匹配派蒙的rect位置
	cv::Rect rect_paimon_keypoint = MatchPaimonKeyPointRect;
	// 手柄模式下匹配派蒙的rect位置
	cv::Rect rect_paimon_keypoint_handle = MatchPaimonHandleKeyPointRect;
};
// 画面中派蒙的相关变量
struct GenshinPaimon
{
	bool is_visial = false;
	bool is_handle_mode = false;
	bool is_search_mode = false;
	cv::Rect rect_paimon;
	GenshinPaimonConfig config;
};
struct GenshinMinimapCailbConfig
{
	bool is_need_find = true;
	double check_match_minimap_cailb_params = 0.85;
	double check_match_minimap_cailb_params_no_alpha = 0.50;
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
	bool is_init_finish = false;
	bool is_run_init_start = false;
	bool is_run_uninit_start = false;
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
struct GenshinAvatarDirectionConfig
{

};
struct GenshinAvatarDirection
{
	double angle = 0;
	GenshinAvatarDirectionConfig config;
};

struct GenshinAvatarPositionConfig
{
	bool is_init_finish = false;
	bool is_lock_minimap_rotation = true;
	double minimap_rotation = 0;
	bool is_continuity = false;
	bool is_coveying = false;
	bool is_on_city = false;
	bool is_exist_last_match_minimap = false;
	cv::Mat img_last_match_minimap;
	bool is_use_filter = true;
	std::shared_ptr<Filter> pos_filter;
};
struct GenshinAvatarPosition
{
	cv::Point2d target_map_world_center;
	double target_map_world_scale = 1.0;
	cv::Point2d position;
	GenshinAvatarPositionConfig config;
};

struct GenshinViewerDirectionConfig
{
	bool is_lock_minimap_rotation = true;
	double minimap_rotation = 0;
};

struct GenshinViewerDirection
{
	double angle = 0;
	GenshinViewerDirectionConfig config;
};

struct GenshinMinimapDirectionConfig
{
	bool  is_defalut = true;
	bool is_skip = true;
};

struct GenshinMinimapDirection
{
	double angle = 0;
	GenshinMinimapDirectionConfig config;
};
struct GenshinStarsConfig
{
	double check_match_star_params = 0.85;
};
struct GenshinStars
{
	bool is_find = false;
	std::vector<cv::Point> points_star;
	GenshinStarsConfig config;
};

struct GenshinTagflagsConfig
{
	double check_match_star_params = 0.85;
};
struct GenshinTagflags
{
	bool is_find = false;
	std::vector<cv::Point> points_star;
	GenshinTagflagsConfig config;
};

struct GenshinUIDConfig
{
	double check_match_uid_params = 0.85;
};
struct GenshinUID
{
	int uid = 0;
	GenshinUIDConfig config;
};