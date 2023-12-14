#pragma once
#include "../algorithms.feature.h"

namespace tianli::algorithms::feature::create
{
    // NEED_TEST: 构造均匀分布的特征点，基于网格划分，在每个网格内至少采样到目标密度的特征点
    static features from_image(const cv::Mat& image, const cv::Mat& mask = cv::Mat(), const int target_density = 100)
    {
        const float grid_radius = 512.0;
        // 将图像根据尺寸划分为网格
        const size_t grid_width = std::ceil(image.size[1] / grid_radius);
        const size_t grid_height = std::ceil(image.size[0] / grid_radius);
        const size_t grid_size = grid_width * grid_height;

        //图像整体扩增边缘，以简化滑动窗口的计算
        int border_size = 32;
        cv::Mat paddingImg, paddingMask;
        cv::copyMakeBorder(image, paddingImg, border_size, border_size, border_size, border_size, cv::BORDER_REPLICATE);
        if (!mask.empty())
        {
            cv::copyMakeBorder(image, paddingImg, border_size, border_size, border_size, border_size, cv::BORDER_REPLICATE);
        }

        // 结果特征点
        features result_features;
        // 特征点的生成算子
        const auto& detector = cv::xfeatures2d::SURF::create(100, 1, 3, true, false);
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

                // 计算网格中的特征点
                cv::Mat grid_image = paddingImg(grid_rect);
                cv::Mat grid_mask;
                if (!mask.empty())
                {
                    grid_mask = paddingMask(grid_rect);
                }

                float threshold = 100.0;
                features grad_feature;
                do {
                    detector->setHessianThreshold(threshold);
                    grad_feature = feature::from_image(detector, grid_image, grid_mask, 0);
                    threshold /= 2.0;
                } while (threshold >= 1 && grad_feature.keypoints.size() < target_density);

                result_features = merge(result_features, grad_feature, cv::Point2d(grid_rect.x - border_size, grid_rect.y - border_size));
            }
        }

        return result_features;
    }

}