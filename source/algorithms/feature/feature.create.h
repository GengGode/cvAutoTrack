#pragma once
#include "../algorithms.feature.h"

namespace tianli::algorithms::feature::create
{
    // NEED_TEST: 构造均匀分布的特征点，基于网格划分，在每个网格内至少采样到目标密度的特征点
    static features from_image(const cv::Mat &image, const cv::Mat &mask = cv::Mat(), const int target_density = 100)
    {
        const cv::Ptr<cv::xfeatures2d::SURF> &detector = cv::xfeatures2d::SURF::create();
        const cv::Ptr<cv::xfeatures2d::SURF> &level_0_detector = cv::xfeatures2d::SURF::create(100, 1, 3, true, false);
        const cv::Ptr<cv::xfeatures2d::SURF> &level_1_detector = cv::xfeatures2d::SURF::create(50, 1, 3, true, false);
        const cv::Ptr<cv::xfeatures2d::SURF> &level_2_detector = cv::xfeatures2d::SURF::create(25, 1, 3, true, false);
        const cv::Ptr<cv::xfeatures2d::SURF> &level_3_detector = cv::xfeatures2d::SURF::create(2, 1, 3, true, false);
        const float grid_radius = 512.0;
        // 将图像根据尺寸划分为网格
        const size_t grid_width = std::ceil(image.size[1] / grid_radius);
        const size_t grid_height = std::ceil(image.size[0] / grid_radius);
        const size_t grid_size = grid_width * grid_height;
        // 采样网格中的特征点
        std::vector<features> grid_features(grid_size);
        std::vector<features> grid_features_level_0(grid_size);
        std::vector<features> grid_features_level_1(grid_size);
        std::vector<features> grid_features_level_2(grid_size);
        std::vector<features> grid_features_level_3(grid_size);
        std::vector<cv::Rect> grid_rects(grid_size);

        std::vector<features> res_features(grid_size);

        // 图像整体扩增边缘，以简化滑动窗口的计算
        int border_size = 32;
        cv::Mat padding_image;
        cv::copyMakeBorder(image, padding_image, border_size, border_size, border_size, border_size, cv::BORDER_REPLICATE);

        for (int w_index = 0; w_index < grid_width; w_index++)
        {
            for (int h_index = 0; h_index < grid_height; h_index++)
            {
                // 计算网格索引
                const size_t grid_index = w_index * grid_height + h_index;
                // 计算网格范围
                const size_t grid_x = w_index * grid_radius;
                const size_t grid_y = h_index * grid_radius;
                const size_t grid_width = grid_x + grid_radius > image.size[1] ? image.size[1] - grid_x : grid_radius;
                const size_t grid_height = grid_y + grid_radius > image.size[0] ? image.size[0] - grid_y : grid_radius;
                // 放大滑动窗口以适配边界区域

                const cv::Rect grid_rect(grid_x, grid_y, grid_width + 2 * border_size, grid_height + 2 * border_size);
                grid_rects[grid_index] = grid_rect;
                // 计算网格中的特征点
                cv::Mat grid_image = padding_image(grid_rect);
                cv::Mat grid_mask;
                if (!mask.empty())
                    grid_mask = padding_image(grid_rect);
                else
                    grid_mask = cv::Mat(grid_image.rows, grid_image.cols, CV_8UC1);
                grid_features[grid_index] = feature::from_image(detector, grid_image, grid_mask);
                grid_features_level_0[grid_index] = feature::from_image(level_0_detector, grid_image, grid_mask);
                grid_features_level_1[grid_index] = feature::from_image(level_1_detector, grid_image, grid_mask);
                grid_features_level_2[grid_index] = feature::from_image(level_2_detector, grid_image, grid_mask);
                grid_features_level_3[grid_index] = feature::from_image(level_3_detector, grid_image, grid_mask);

                if (grid_features[grid_index].size() < target_density)
                {
                    res_features[grid_index] = grid_features[grid_index];
                    continue;
                }
                if (grid_features_level_0[grid_index].size() < target_density)
                {
                    res_features[grid_index] = grid_features_level_0[grid_index];
                    continue;
                }
                if (grid_features_level_1[grid_index].size() < target_density)
                {
                    res_features[grid_index] = grid_features_level_1[grid_index];
                    continue;
                }
                if (grid_features_level_2[grid_index].size() < target_density)
                {
                    res_features[grid_index] = grid_features_level_2[grid_index];
                    continue;
                }
                res_features[grid_index] = grid_features_level_3[grid_index];
            }
        }

        // 合并所有特征点
        features result_features;
        for (int i = 0; i < grid_size; i++)
        {
            auto &grid_feature = res_features[i];
            auto &grid_rect = grid_rects[i];
            result_features = merge(result_features, grid_feature, cv::Point2f(grid_rect.x - border_size, grid_rect.y - border_size));
        }
        return result_features;
    }

}