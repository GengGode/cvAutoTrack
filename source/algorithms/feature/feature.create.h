#pragma once
#include "../algorithms.feature.h"
#include <execution>

namespace tianli::algorithms::feature::create
{
    // NEED_TEST: 构造均匀分布的特征点，基于网格划分，在每个网格内至少采样到目标密度的特征点   
    static features from_image(const cv::Mat &image, const int target_density = 100, const int grid_radius = 512, const cv::Mat &mask = cv::Mat(),const double surf_begin_hessian_threshold=100,const int surf_octaves = 1,const int surf_octaveLayers = 3,const  bool surf_extended = true,const bool surf_upright = false)
    {
        const double target_density_rate = 1.0 / (grid_radius * grid_radius) * target_density;
        // 将图像根据尺寸划分为网格，以均分为原则，尽可能接近给定的半径
        const size_t grids_row_count = static_cast<int>(std::ceil(1.0 * image.rows / grid_radius));
        const size_t grids_col_count = static_cast<int>(std::ceil(1.0 * image.cols / grid_radius));
        const cv::Size2d grid_size(image.cols / grids_col_count, image.rows / grids_row_count);
        const size_t grid_count = grids_row_count * grids_col_count;

        std::vector<std::pair<cv::Rect, features>> grid_features(grid_count);
        //  结果特征点
        features result_features;
        for (int grid_row_index = 0; grid_row_index < grids_row_count; grid_row_index++)
        {
            for (int grid_col_index = 0; grid_col_index < grids_col_count; grid_col_index++)
            {
                // 计算网格范围
                const int32_t grid_x = static_cast<int>(grid_col_index * grid_size.width);
                const int32_t grid_y = static_cast<int>(grid_row_index * grid_size.height);
                const int32_t grid_width = static_cast<int>((grid_col_index + 1) * grid_size.width) - grid_x;
                const int32_t grid_height = static_cast<int>((grid_row_index + 1) * grid_size.height) - grid_y;
                const cv::Rect grid_rect = cv::Rect(grid_x, grid_y, grid_width, grid_height);
                // 计算网格索引
                const size_t grid_index = grid_row_index * grids_col_count + grid_col_index;
                grid_features[grid_index].first = grid_rect;
            }
        }

        std::for_each(std::execution::par_unseq, grid_features.begin(), grid_features.end(), [&](std::pair<cv::Rect, features> &grid_feature)
            {
                const int fixed_target_density = static_cast<int>(std::ceil(target_density_rate * grid_feature.first.area()));
                const auto &detector = cv::xfeatures2d::SURF::create(surf_begin_hessian_threshold, surf_octaves, surf_octaveLayers, surf_extended, surf_upright);
                double threshold = surf_begin_hessian_threshold;
                features feature;
                for (int i = 0; i < 10; i++)
                {
                    if (threshold < 1 || feature.keypoints.size() > fixed_target_density)
                        break;
                    detector->setHessianThreshold(threshold);
                    feature = feature::from_image(detector, image, grid_feature.first, mask);
                    threshold /= 2.0;
                }
                grid_feature.second = feature;
            });

        for (int i = 0; i < grid_count; i++)
            result_features = merge(result_features, grid_features[i].second, cv::Point2d(grid_features[i].first.x, grid_features[i].first.y));
        return result_features;
    }
}