#pragma once
#include "global/global.genshin.h"

#include "../../../resources/Resources.h"
namespace tianli::global
{
    struct check_paimon_search_params
    {
        std::vector<cv::Mat> split_paimon_template;
        // 派蒙正比例二值模板 黑白
        cv::Mat paimon_template;
        // 派蒙手柄比例二值模板 黑白
        cv::Mat paimon_template_handle_mode;
        // 派蒙正比例灰度模板 灰度
        cv::Mat paimon_template_no_alpha;
        // 派蒙手柄比例灰度模板 灰度
        cv::Mat paimon_template_no_alpha_handle_mode;

        void init(Resources& res)
        {
            cv::Mat paimon;
            res.PaimonTemplate.copyTo(paimon);
            cv::resize(paimon, paimon, cv::Size(68, 77));
            cv::cvtColor(paimon, paimon_template_no_alpha, cv::COLOR_RGBA2GRAY);
            cv::split(paimon, split_paimon_template);

            paimon_template = split_paimon_template[3];
            // paimon_template_no_alpha = split_paimon_template[0];
            // paimon_template_no_alpha.copyTo(paimon_template_no_alpha, paimon_template);
            cv::resize(paimon_template, paimon_template_handle_mode, cv::Size(), 1.0 / 1.20, 1.0 / 1.20);
            cv::resize(paimon_template_no_alpha, paimon_template_no_alpha_handle_mode, cv::Size(), 1.0 / 1.20, 1.0 / 1.20);
            // paimon_template_no_alpha_handle_mode.copyTo(paimon_template_no_alpha_handle_mode, paimon_template_handle_mode);
        }
    };
} // namespace tianli::global

namespace TianLi::Genshin::Check
{
    bool check_paimon(const tianli::global::GenshinScreen& genshin_screen, tianli::global::GenshinPaimon& out_genshin_paimon);
}
