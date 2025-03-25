#pragma once
#include "global/global.genshin.h"

#include "../../../resources/Resources.h"

namespace tianli::global
{
    struct match_minimap_cailb_params
    {
        std::vector<cv::Mat> split_minimap_cailb_template;
        cv::Mat minimap_cailb_template;
        cv::Mat minimap_cailb_template_handle_mode;
        cv::Mat minimap_cailb_template_no_alpha;
        cv::Mat minimap_cailb_template_no_alpha_handle_mode;

        void init(Resources& res)
        {
            cv::Mat minimap_cailb;
            cv::resize(res.MinimapCailbTemplate, minimap_cailb, cv::Size(), 0.8, 0.8);
            cv::split(minimap_cailb, split_minimap_cailb_template);
            minimap_cailb_template = split_minimap_cailb_template[3];
            minimap_cailb_template_no_alpha = split_minimap_cailb_template[0];
            cv::resize(split_minimap_cailb_template[3], minimap_cailb_template_handle_mode, cv::Size(), 1 / 1.2, 1 / 1.2, cv::INTER_CUBIC);
            cv::resize(split_minimap_cailb_template[3], minimap_cailb_template_no_alpha_handle_mode, cv::Size(), 1.0 / 1.2, 1.0 / 1.2);
        }
    };
} // namespace tianli::global
namespace TianLi::Genshin::Cailb
{
    bool cailb_minimap(const tianli::global::GenshinScreen& genshin_screen, tianli::global::GenshinMinimap& out_genshin_minimap);
}
