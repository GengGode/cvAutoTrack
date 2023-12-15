#pragma once
#include "../algorithms.feature.h"
#include <execution>

namespace tianli::algorithms::feature::create
{
    // NEED_TEST: 构造均匀分布的特征点，基于网格划分，在每个网格内至少采样到目标密度的特征点
    static features from_image(const cv::Mat &image, const int target_density = 100, const int grid_radius = 512, const cv::Mat &mask = cv::Mat())
    {
        const float target_density_rate = 1.0 / (grid_radius * grid_radius) * target_density;
        // 将图像根据尺寸划分为网格，为了避免出现过小的块，宽高会减去一半边长，以实现将不足一半边长的块向前合并
        const size_t grid_row_count = std::max(1, static_cast<int>(std::ceil((image.size[1] - grid_radius / 2) / grid_radius)));
        const size_t grid_col_count = std::max(1, static_cast<int>(std::ceil((image.size[0] - grid_radius / 2) / grid_radius)));
        const size_t grid_size = grid_row_count * grid_col_count;

        std::vector<std::pair<cv::Rect, features>> grid_features(grid_size);
        //  结果特征点
        features result_features;
        for (int grid_row = 0; grid_row < grid_row_count; grid_row++)
        {
            for (int grid_col = 0; grid_col < grid_col_count; grid_col++)
            {
                // 计算网格索引
                const size_t grid_index = grid_row * grid_col_count + grid_col;
                // 计算网格范围
                const size_t grid_x = grid_row * grid_radius;
                const size_t grid_y = grid_col * grid_radius;
                const size_t grid_width = grid_row - 1 ? image.size[1] - grid_x : grid_radius;
                const size_t grid_height = grid_col - 1 ? image.size[0] - grid_y : grid_radius;

                const cv::Rect grid_rect = cv::Rect(grid_x, grid_y, grid_width, grid_height);
                grid_features[grid_index].first = grid_rect;
            }
        }

        std::for_each(std::execution::par_unseq, grid_features.begin(), grid_features.end(), [&](std::pair<cv::Rect, features> &grid_feature)
            {
                const int fixed_target_density = std::ceil(target_density_rate * grid_feature.first.area());
                const auto &detector = cv::xfeatures2d::SURF::create(100, 1, 3, true, false);
                float threshold = 100.0;
                features feature;
                for (int i = 0; i < 10; i++)
                {
                    if (threshold < 1 || feature.keypoints.size() > fixed_target_density)
                        break;
                    detector->setHessianThreshold(threshold);
                    feature = feature::from_image(detector, image, grid_feature.first, mask);
                    threshold /= 2.0;
                }
                grid_feature.second = feature; });

        for (int i = 0; i < grid_size; i++)
            result_features = merge(result_features, grid_features[i].second, cv::Point2d(grid_features[i].first.x, grid_features[i].first.y));
        return result_features;
    }
}