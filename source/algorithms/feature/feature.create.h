#pragma once
#include "../algorithms.feature.h"
#include <execution>

namespace tianli::algorithms::feature::create
{
    // NEED_TEST: 构造均匀分布的特征点，基于网格划分，在每个网格内至少采样到目标密度的特征点
    static features from_image(const cv::Mat &image, const int target_density = 100, const int grid_radius = 512, const cv::Mat &mask = cv::Mat())
    {
        // 将图像根据尺寸划分为网格
        const size_t grid_width = std::ceil(image.size[1] / grid_radius);
        const size_t grid_height = std::ceil(image.size[0] / grid_radius);
        const size_t grid_size = grid_width * grid_height;
        // std::vector<cv::Rect> grid_rects(grid_size);
        // std::vector<features> grid_features(grid_size);
        std::vector<std::pair<cv::Rect, features>> grid_features(grid_size);
        //  结果特征点
        features result_features;
        // 特征点的生成算子
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
                const cv::Rect grid_rect = cv::Rect(grid_x, grid_y, grid_width, grid_height);
                grid_features[grid_index].first = grid_rect;
            }
        }

        std::for_each(std::execution::par_unseq, grid_features.begin(), grid_features.end(), [&](std::pair<cv::Rect, features> &grid_feature) 
                      {
                          const auto &detector = cv::xfeatures2d::SURF::create(100, 1, 3, false, true);
                          float threshold = 100.0;
                          features feature;
                          for (int i = 0; i < 10; i++)
                          {
                              if (threshold < 1 || feature.keypoints.size() > target_density)
                                  break;
                              detector->setHessianThreshold(threshold);
                              feature = feature::from_image(detector, image, grid_feature.first, mask);
                              threshold /= 2.0;
                          } 
                          grid_feature.second = feature;});

        for (int i = 0; i < grid_size; i++)
            result_features = merge(result_features, grid_features[i].second, cv::Point2d(grid_features[i].first.x, grid_features[i].first.y));
        return result_features;
    }
}