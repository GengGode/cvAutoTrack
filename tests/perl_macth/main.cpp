#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <iostream>
#include "frame/capture/capture.bitblt.h"
#include "algorithms/filter/filter.kalman.h"
#include "global/global.include.h"
#include "global/global.genshin.h"
#include "global/record/record.stdlog.h"
#include "resources/trackCache.h"
#include "match/surf/SurfMatch.h"
#include "utils/convect.string.h"
#include "genshin/genshin.handle.h"
#include "genshin/genshin.screen.h"
#include "genshin\check\paimon\genshin.check.paimon.h"
#include "genshin\cailb\minimap\genshin.cailb.minimap.h"
#include <filesystem>
#include "algorithms/algorithms.visual.odometer.h"
#include "resources/Resources.h"
#include <fmt/format.h>
#include <opencv2/core/utils/logger.hpp>
tianli::global::GenshinHandle genshin_handle;
tianli::global::GenshinScreen genshin_screen;
tianli::global::GenshinPaimon genshin_paimon;
tianli::global::GenshinMinimap genshin_minimap;
tianli::global::GenshinAvatarPosition genshin_avatar_position;

auto logger = std::make_shared<tianli::global::record::std_logger>();
void get_avatar_position(const tianli::global::GenshinMinimap &genshin_minimap, tianli::global::GenshinAvatarPosition &out_genshin_position)
{
    static SurfMatch surf_match;
    static bool is_init = false;
    if (genshin_minimap.is_run_init_start == true || is_init == false)
    {
        if (is_init)
            return;

        std::vector<cv::KeyPoint> gi_map_keypoints;
        cv::Mat gi_map_descriptors;
        std::shared_ptr<trackCache::CacheInfo> cache_info;
        load_cache(cache_info);
        surf_match.Init(cache_info);

        is_init = true;
        return;
    }

    if (genshin_minimap.is_run_uninit_start == true)
    {
        surf_match.UnInit();
        is_init = false;
        return;
    }

    if (genshin_minimap.config.is_find_paimon == false)
    {
        return;
    }

    if (genshin_minimap.img_minimap.empty())
    {
        return;
    }

    surf_match.setMiniMap(genshin_minimap.img_minimap);

    logger->perl("match");

    surf_match.match();
    logger->perl_end("match");

    out_genshin_position.position = surf_match.getLocalPos();
    out_genshin_position.config.is_continuity = surf_match.isContinuity;
    out_genshin_position.config.is_coveying = surf_match.isCoveying;

    if (out_genshin_position.config.is_use_filter)
    {
        cv::Point2d pos = out_genshin_position.position;
        cv::Point2d filt_pos;
        // u_k: 视觉里程计控制量；od_valid: 视觉里程计是否有效；ms_valid: 全局匹配是否有效
        auto u_k = cv::Point2d(0, 0);
        auto od_valid = control_odometer_calculation(genshin_minimap.img_minimap, u_k, tianli::global::odometer_config());
        auto ms_valid = surf_match.is_success_match;

        // 里程计能更就更；全局匹配能配就配
        if (od_valid)
        {
            filt_pos = out_genshin_position.config.pos_filter->predict(u_k);
        }
        if (ms_valid)
        {
            filt_pos = out_genshin_position.config.pos_filter->update(pos);
        }

        // 特判tp之后
        if (!od_valid && ms_valid)
        {
            filt_pos = out_genshin_position.config.pos_filter->re_init_filterting(pos);
        }

        out_genshin_position.position = filt_pos;
    }

    if (surf_match.is_success_match)
    {
        out_genshin_position.config.img_last_match_minimap = genshin_minimap.img_minimap.clone();
        out_genshin_position.config.is_exist_last_match_minimap = true;
    }
    else
    {
        out_genshin_position.config.is_exist_last_match_minimap = false;
    }
}

bool test()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    TianLi::Genshin::get_genshin_handle(genshin_handle);
    if (genshin_handle.handle == NULL)
    {

        return false;
    }

    genshin_handle.config.frame_source->set_capture_handle(genshin_handle.handle);

    TianLi::Genshin::get_genshin_screen(genshin_handle, genshin_screen);
    if (genshin_screen.img_screen.empty())
    {
        return false;
    }

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

    if (genshin_minimap.img_minimap.empty())
    {
        return false;
    }

    genshin_minimap.config.is_find_paimon = true;

    get_avatar_position(genshin_minimap, genshin_avatar_position);

    cv::Point2d pos = genshin_avatar_position.position;
    return true;
}

int main()
{
    fmt::print("Hello, world!\n");

    genshin_handle.config.frame_source = std::make_shared<tianli::frame::capture::capture_bitblt>();
    genshin_handle.config.frame_source->initialization();
    genshin_avatar_position.config.pos_filter = std::make_shared<tianli::algorithms::filter::filter_kalman>();

    // 实现30fps call test
    auto beg_time = std::chrono::steady_clock::now();
    auto end_time = std::chrono::steady_clock::now();
    auto cost_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - beg_time).count();
    while (true)
    {
        beg_time = std::chrono::steady_clock::now();
        logger->perl("call");
        test();
        logger->perl_end("call");
        end_time = std::chrono::steady_clock::now();
        cost_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - beg_time).count();
        if (cost_time < 33)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(33 - cost_time));
        }
    }
}