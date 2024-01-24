#include "pch.h"
#include "AutoTrack.h"

#include "ErrorCode.h"
#include "genshin/genshin.h"
#include "match/match.star.h"
#include "match/match.uid.h"
#include "resources/Resources.h"
#include "utils/Utils.h"
#include "utils/convect.string.h"
#include "algorithms/algorithms.direction.h"
#include "algorithms/algorithms.rotation.h"
#include "algorithms/filter/filter.kalman.h"
#include "frame/capture/capture.bitblt.h"
#include "resource/version.h"

ErrorCode& err = ErrorCode::getInstance();
Resources& res = Resources::getInstance();

AutoTrack::AutoTrack()
{
    err.enableWirteFile();

    genshin_avatar_position.target_map_world_center = res.map_relative_center;
    genshin_avatar_position.target_map_world_scale = res.map_relative_scale;

    genshin_handle.config.frame_source = std::make_shared<tianli::frame::capture::capture_bitblt>();
    genshin_handle.config.frame_source->initialization();
    genshin_avatar_position.config.pos_filter = std::make_shared<tianli::algorithms::filter::filter_kalman>();
}

bool AutoTrack::init()
{
    if (!genshin_minimap.is_init_finish)
    {
        genshin_minimap.is_run_init_start = true;
        TianLi::Genshin::Match::get_avatar_position(genshin_minimap, genshin_avatar_position);
        genshin_minimap.is_run_init_start = false;

        genshin_minimap.is_init_finish = true;
    }
    return genshin_minimap.is_init_finish;
}

bool AutoTrack::uninit()
{
    if (genshin_minimap.is_init_finish)
    {
        genshin_minimap.is_run_uninit_start = true;
        TianLi::Genshin::Match::get_avatar_position(genshin_minimap, genshin_avatar_position);
        genshin_minimap.is_run_uninit_start = false;

        genshin_minimap.is_init_finish = false;
    }
    return !genshin_minimap.is_init_finish;
}


        bool AutoTrack::SetHandle(long long int handle)
        {
            if (handle == 0)
            {
                genshin_handle.config.is_auto_find_genshin = true;
                return true;
            }
            else
            {
                genshin_handle.config.is_auto_find_genshin = false;
                genshin_handle.handle = (HWND)handle;
            }
            return IsWindow(genshin_handle.handle);
        }

        bool AutoTrack::SetWorldCenter(double x, double y)
        {
            genshin_avatar_position.target_map_world_center.x = x;
            genshin_avatar_position.target_map_world_center.y = y;
            return true;
        }

        bool AutoTrack::SetWorldScale(double scale)
        {
            genshin_avatar_position.target_map_world_scale = scale;
            return true;
        }

        bool AutoTrack::startServe()
        {
            return false;
        }

        bool AutoTrack::stopServe()
        {
            return false;
        }

        bool AutoTrack::SetDisableFileLog()
        {
            err.disableWirteFile();
            return true;
        }

        bool AutoTrack::SetEnableFileLog()
        {
            err.enableWirteFile();
            return true;
        }

        bool AutoTrack::DebugCapture()
        {
            return DebugCapturePath("Capture.png", 12);
        }

        bool AutoTrack::DebugCapturePath(const char* path_buff, int buff_size)
        {
            if (path_buff == NULL || buff_size < 1)
            {
                err = { 251, "路径缓存区为空指针或是路径缓存区大小为小于1" };
                return false;
            }

            if (genshin_screen.img_screen.empty())
            {
                err = { 252, "画面为空" };
                return false;
            }
            cv::Mat out_info_img = genshin_screen.img_screen.clone();
            switch (genshin_handle.config.frame_source->type)
            {
                case tianli::frame::frame_source::source_type::bitblt:
                    {
                        // 绘制paimon Rect
                        cv::rectangle(out_info_img, genshin_paimon.rect_paimon, cv::Scalar(0, 0, 255), 2);
                        // 绘制miniMap Rect
                        cv::rectangle(out_info_img, genshin_minimap.rect_minimap, cv::Scalar(0, 0, 255), 2);
                        cv::Rect Avatar = genshin_minimap.rect_avatar;
                        Avatar.x += genshin_minimap.rect_minimap.x;
                        Avatar.y += genshin_minimap.rect_minimap.y;

                        // 绘制avatar Rect
                        cv::rectangle(out_info_img, Avatar, cv::Scalar(0, 0, 255), 2);
                        // 绘制UID Rect
                        cv::rectangle(out_info_img, genshin_handle.rect_uid, cv::Scalar(0, 0, 255), 2);
                        break;
                    }
                case tianli::frame::frame_source::source_type::window_graphics:
                    {
                        // 绘制paimon Rect
                        cv::rectangle(out_info_img, genshin_paimon.rect_paimon, cv::Scalar(0, 0, 255), 2);
                        // 绘制miniMap Rect
                        cv::rectangle(out_info_img, genshin_minimap.rect_minimap, cv::Scalar(0, 0, 255), 2);
                        cv::Rect Avatar = genshin_minimap.rect_avatar;
                        Avatar.x += genshin_minimap.rect_minimap.x;
                        Avatar.y += genshin_minimap.rect_minimap.y;

                        // 绘制avatar Rect
                        cv::rectangle(out_info_img, Avatar, cv::Scalar(0, 0, 255), 2);
                        // 绘制UID Rect
                        cv::rectangle(out_info_img, genshin_handle.rect_uid, cv::Scalar(0, 0, 255), 2);
                    }
            }

#if (_MSC_VER && _MSVC_LANG <= 201703L) || (!_MSC_VER && __cplusplus <= 201703L)
    std::string last_time_str = global::format("{:%Y-%m-%d :%H:%M:%S}", std::chrono::system_clock::to_time_t(genshin_screen.last_time));
#else
    std::string last_time_str = global::format("{:%Y-%m-%d %H:%M:%S}", genshin_screen.last_time);
#endif

    cv::putText(out_info_img, last_time_str, cv::Point(out_info_img.cols / 2, out_info_img.rows / 2), 1, 1, cv::Scalar(128, 128, 128, 255), 1, 16, 0);
    auto err_msg_str = err.toJson();
    cv::putText(out_info_img, err_msg_str, cv::Point(0, out_info_img.rows / 2 - 100), 1, 1, cv::Scalar(128, 128, 128, 128), 1, 16, 0);

    auto path = utils::to_utf8(path_buff);

    bool rel = cv::imwrite(path_buff, out_info_img);

    if (!rel)
    {
        err = { 252, std::string("保存画面失败，请检查文件路径是否合法") + std::string(path_buff) };
        return false;
    }

    return clear_error_logs();
}

bool AutoTrack::GetTransformOfMap(double& x, double& y, double& a, int& mapId)
{
    double x2 = 0, y2 = 0, a2 = 0;
    int mapId2 = 0;
    if (!genshin_minimap.is_init_finish)
    {
        init(); // 初始化
    }

    if (!GetPositionOfMap(x2, y2, mapId2))
    {
        return false;
    }

    GetDirection(a2);
    x = x2;
    y = y2;
    a = a2;
    mapId = mapId2;
    return clear_error_logs();
}

bool AutoTrack::GetPosition(double& x, double& y)
{
    if (try_get_genshin_windows() == false)
    {
        return false;
    }
    if (!genshin_minimap.is_init_finish)
    {
        init();
    }
    if (getMiniMapRefMat() == false)
    {
        // err = { 1001, "获取坐标时，没有识别到paimon" };
        return false;
    }

    if (genshin_minimap.img_minimap.empty())
    {
        err = { 5, "原神小地图区域为空" };
        return false;
    }
    genshin_minimap.config.is_find_paimon = true;

    TianLi::Genshin::Match::get_avatar_position(genshin_minimap, genshin_avatar_position);

    cv::Point2d pos = genshin_avatar_position.position;

    x = pos.x;
    y = pos.y;

    return clear_error_logs();
}

bool AutoTrack::GetPositionOfMap(double& x, double& y, int& mapId)
{
    mapId = 0;
    // 新算法直接输出原始坐标，所以不需要做额外处理
    bool isSuccess = GetPosition(x, y);
    if (isSuccess != true)
    {
        return false;
    }
    return clear_error_logs();
}

bool AutoTrack::GetDirection(double& a)
{
    if (try_get_genshin_windows() == false)
    {
        return false;
    }
    if (getMiniMapRefMat() == false)
    {
        // err = { 2001, "获取角色朝向时，没有识别到paimon" };
        return false;
    }
    if (genshin_minimap.rect_avatar.empty())
    {
        err = { 11, "原神角色小箭头区域为空" };
        return false;
    }

    tianli::global::direction_calculation_config config;
    direction_calculation(genshin_minimap.img_avatar, a, config);
    if (config.error)
    {
        err = config.err;
        return false;
    }

    return clear_error_logs();
}

bool AutoTrack::GetRotation(double& a)
{
    if (try_get_genshin_windows() == false)
    {
        return false;
    }
    if (getMiniMapRefMat() == false)
    {
        // err = { 3001, "获取视角朝向时，没有识别到paimon" };
        return false;
    }

    tianli::global::rotation_calculation_config config;
    rotation_calculation(genshin_minimap.img_minimap, a, config);
    if (config.error)
    {
        err = config.err;
        return false;
    }

    return clear_error_logs();
}

bool AutoTrack::GetUID(int& uid)
{
    if (try_get_genshin_windows() == false)
    {
        return false;
    }

    cv::Mat& giUIDRef = genshin_screen.img_uid;

    std::vector<cv::Mat> channels;

    split(giUIDRef, channels);

    if (genshin_handle.config.frame_source->type == tianli::frame::frame_source::source_type::window_graphics)
    {
        cv::cvtColor(giUIDRef, giUIDRef, cv::COLOR_RGBA2GRAY);
    }
    else
    {
        giUIDRef = channels[3];
    }

    tianli::global::uid_calculation_config config;
    uid_calculation(giUIDRef, uid, config);
    if (config.error)
    {
        err = config.err;
        return false;
    }

    return clear_error_logs();
}

bool AutoTrack::GetAllInfo(double& x, double& y, int& mapId, double& a, double& r, int& uid)
{
    if (try_get_genshin_windows() == false)
    {
        return false;
    }
    if (!genshin_minimap.is_init_finish)
    {
        init();
    }
    if (getMiniMapRefMat() == false)
    {
        // err = { 1001, "获取所有信息时，没有识别到paimon" };
        return false;
    }
    if (genshin_minimap.img_minimap.empty())
    {
        err = { 5, "原神小地图区域为空" };
        return false;
    }
    if (genshin_minimap.rect_avatar.empty())
    {
        err = { 11, "原神角色小箭头区域为空" };
        return false;
    }

    // x,y,mapId
    {
        genshin_minimap.config.is_find_paimon = true;
        GetPositionOfMap(x, y, mapId);
    }

    // a
    {
        tianli::global::direction_calculation_config config;
        direction_calculation(genshin_minimap.img_avatar, a, config);
        if (config.error)
        {
            err = config.err;
        }
    }
    // r
    {
        tianli::global::rotation_calculation_config config;
        rotation_calculation(genshin_minimap.img_minimap, r, config);
        if (config.error)
        {
            err = config.err;
        }
    }
    cv::Mat& giUIDRef = genshin_screen.img_uid;
    // uid
    {
        std::vector<cv::Mat> channels;

        cv::split(giUIDRef, channels);

        if (genshin_handle.config.frame_source->type == tianli::frame::frame_source::source_type::window_graphics)
        {
            cv::cvtColor(giUIDRef, giUIDRef, cv::COLOR_RGBA2GRAY);
        }
        else
        {
            giUIDRef = channels[3];
        }

        tianli::global::uid_calculation_config config;
        uid_calculation(giUIDRef, uid, config);
        if (config.error)
        {
            err = config.err;
        }
    }

    return clear_error_logs();
}

bool AutoTrack::try_get_genshin_windows()
{
    if (!clear_error_logs())
    {
        err = { 0, "正常退出" };
        return false;
    }
    if (!getGengshinImpactWnd())
    {
        err = { 101, "未能找到原神窗口句柄" };
        return false;
    }
    if (!getGengshinImpactScreen())
    {
        err = { 103, "获取原神画面失败" };
        return false;
    }
    return true;
}

bool AutoTrack::getGengshinImpactWnd()
{
    TianLi::Genshin::get_genshin_handle(genshin_handle);
    if (genshin_handle.config.frame_source->mode != tianli::frame::frame_source::source_mode::handle)
    {
        return true;
    }
    if (genshin_handle.handle == NULL)
    {
        err = { 10, "无效句柄或指定句柄所指向窗口不存在" };
        return false;
    }

    genshin_handle.config.frame_source->set_capture_handle(genshin_handle.handle);

    return true;
}

bool AutoTrack::getGengshinImpactScreen()
{
    TianLi::Genshin::get_genshin_screen(genshin_handle, genshin_screen);
    if (genshin_screen.img_screen.empty())
    {
        err = { 433, "截图失败" };
        return false;
    }
    return true;
}

bool AutoTrack::getMiniMapRefMat()
{
    genshin_minimap.img_minimap = genshin_screen.img_screen(genshin_minimap.rect_minimap);

    if (genshin_handle.config.frame_source->type == tianli::frame::frame_source::source_type::window_graphics || genshin_handle.config.is_force_used_no_alpha)
    {
        genshin_screen.config.is_used_alpha = false;
    }
    else
    {
        genshin_screen.config.is_used_alpha = true;
    }

    // 检测派蒙 -> 直接计算小地图坐标

    if (TianLi::Genshin::Check::check_paimon(genshin_screen, genshin_paimon) == false)
    {
        return false;
    }
    if (genshin_paimon.is_visial == false)
    {
        return false;
    }

    genshin_screen.config.rect_paimon = genshin_paimon.rect_paimon;
    genshin_screen.config.is_handle_mode = genshin_paimon.is_handle_mode;
    genshin_screen.config.is_search_mode = genshin_paimon.is_search_mode;

    if (TianLi::Genshin::Cailb::cailb_minimap(genshin_screen, genshin_minimap) == false)
    {
        return false;
    }

    return true;
}
