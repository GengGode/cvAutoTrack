#pragma once
#include "algorithms/algorithms.include.h"
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <execution>

namespace tianli::algorithms::features_operate
{    
    static features merge(const features &features_upper, const features &features_lower, const cv::Point2f diff_offset = cv::Point2f(0, 0))
    {
        features features_merge;
        auto &[ids, kps, des] = features_merge;
        ids = features_upper.indexs;
        ids.push_back(features_upper.size());
        for (auto i : features_lower.indexs)
            ids.push_back(features_upper.size() + i);
        kps = features_upper.keypoints;
        std::for_each(features_lower.keypoints.begin(), features_lower.keypoints.end(), [&](const auto &kp)
            { kps.emplace_back(kp.pt + diff_offset, kp.size, kp.angle, kp.response, kp.octave, kp.class_id); });
        des = cv::Mat(features_upper.size() + features_lower.size(), 64, features_upper.descriptors.type());
        features_upper.descriptors.copyTo(des(cv::Rect(0, 0, 64, features_upper.size())));
        features_lower.descriptors.copyTo(des(cv::Rect(0, features_upper.size(), 64, features_lower.size())));
        return features_merge;
    }

    static std::vector<features> split(const features &featuress)
    {
        if (featuress.indexs.size() == 0)
            return { featuress };
        auto &[ids, kps, des] = featuress;
        std::vector<features> features_vec;
        auto iter_size = featuress.size();
        for (int rec_i = ids.size() - 1; rec_i >= 0; rec_i--)
        {
            int i = ids.at(rec_i);
            auto end_features_size = iter_size - i;
            auto begin = kps.cbegin() + i;
            auto end = begin + end_features_size;
            iter_size = iter_size - end_features_size;
            features new_features;
            new_features.keypoints = std::vector<cv::KeyPoint>(begin, end);
            new_features.descriptors = des(cv::Rect(0, i, 64, end_features_size)).clone();
            features_vec.insert(features_vec.begin(), new_features);
        }
        int i = 0;
        auto end_features_size = iter_size - i;
        auto begin = kps.cbegin() + i;
        auto end = begin + end_features_size;
        iter_size = iter_size - end_features_size;
        features end_features;
        end_features.keypoints = std::vector<cv::KeyPoint>(begin, end);
        end_features.descriptors = des(cv::Rect(0, i, 64, end_features_size)).clone();
        features_vec.insert(features_vec.begin(), end_features);
        return features_vec;
    }

    static features join(const features &fts, std::vector<std::shared_ptr<point_index>> result)
    {
        features res;
        res.keypoints = std::vector<cv::KeyPoint>(result.size());
        res.descriptors = cv::Mat(result.size(), 64, CV_32F);
        int index = 0;
        for (const auto &item : result)
        {
            auto keypoint_ptr = std::dynamic_pointer_cast<keypoint_index>(item);
            // 组合关键点
            res.keypoints[index] = *keypoint_ptr->kp;
            // 组合描述子
            auto descriptor = fts.descriptors.row(keypoint_ptr->index);
            descriptor.copyTo(res.descriptors.row(index));
            index++;
        }
        return res;
    }

    static features from_image(const cv::Ptr<cv::xfeatures2d::SURF> &detector, const cv::Mat &image, const cv::Mat &mask = cv::Mat())
    {
        const int border_size = 100;
        features border_fts;
        cv::Mat border;
        cv::Mat border_mask;
        cv::copyMakeBorder(image, border, border_size, border_size, border_size, border_size, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
        if (mask.empty() == false)
            cv::copyMakeBorder(mask, border_mask, border_size, border_size, border_size, border_size, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

        detector->detectAndCompute(border, border_mask.empty() ? cv::noArray() : border_mask, border_fts.keypoints, border_fts.descriptors);
        std::vector<cv::KeyPoint> keypoints;
        std::vector<int> selected_row_indexs;
        for (int i = 0; i < border_fts.keypoints.size(); i++)
        {
            auto &kp = border_fts.keypoints[i];
            if (kp.pt.x < border_size || kp.pt.y < border_size || kp.pt.x > image.cols + border_size || kp.pt.y > image.rows + border_size)
                continue;
            keypoints.emplace_back(kp.pt - cv::Point2f(border_size, border_size), kp.size, kp.angle, kp.response, kp.octave, kp.class_id);
            selected_row_indexs.push_back(i);
        }

        features fts;
        fts.keypoints = keypoints;
        fts.descriptors = cv::Mat(keypoints.size(), border_fts.descriptors.cols, CV_32F);
        for (int i = 0; i < selected_row_indexs.size(); i++)
            border_fts.descriptors.row(selected_row_indexs[i]).copyTo(fts.descriptors.row(i));
        return fts;
    }
    
    static features from_image(const cv::Ptr<cv::xfeatures2d::SURF> &detector, const cv::Mat &image, const cv::Rect &roi, const cv::Mat &mask = cv::Mat())
    {
        const int border_size = 100;
        features border_fts;

        cv::Rect border_rect = cv::Rect(0, 0, image.cols, image.rows) & cv::Rect(roi.x - border_size, roi.y - border_size, roi.width + 2 * border_size, roi.height + 2 * border_size);
        cv::Rect border_mask_rect = border_rect;

        cv::Mat border_img = image(border_rect);
        cv::Mat border_mask = mask.empty() ? cv::Mat() : mask(border_mask_rect);

        //获取图像四边需要填充的像素数
        int pt = -(std::min)(0, roi.y - border_size);
        int pd = (std::max)(0, roi.y + roi.height + border_size - image.rows);
        int pl = -(std::min)(0, roi.x - border_size);
        int pr = (std::max)(0, roi.x + roi.width + border_size - image.cols);
        //填充图像
        cv::copyMakeBorder(border_img, border_img, pt, pd, pl, pr, cv::BORDER_REPLICATE);
        if (mask.empty() == false) cv::copyMakeBorder(border_mask, border_mask, pt, pd, pl, pr, cv::BORDER_REPLICATE);

        detector->detectAndCompute(border_img, border_mask.empty() ? cv::noArray() : border_mask, border_fts.keypoints, border_fts.descriptors);
        std::vector<cv::KeyPoint> keypoints;
        std::vector<int> selected_row_indexs;
        for (int i = 0; i < border_fts.keypoints.size(); i++)
        {
            auto &kp = border_fts.keypoints[i];
            if (kp.pt.x < border_size || kp.pt.y < border_size || kp.pt.x > image.cols + border_size || kp.pt.y > image.rows + border_size)
                continue;
            keypoints.emplace_back(kp.pt - cv::Point2f(border_size, border_size), kp.size, kp.angle, kp.response, kp.octave, kp.class_id);
            selected_row_indexs.push_back(i);
        }

        features fts;
        fts.keypoints = keypoints;
        fts.descriptors = cv::Mat(keypoints.size(), border_fts.descriptors.cols, CV_32F);
        for (int i = 0; i < selected_row_indexs.size(); i++)
            border_fts.descriptors.row(selected_row_indexs[i]).copyTo(fts.descriptors.row(i));
        return fts;
    }

    // NEED_TEST: 构造均匀分布的特征点，基于网格划分，在每个网格内至少采样到目标密度的特征点   
    static features from_image(const cv::Mat &image, const int target_density = 100, const int grid_radius = 512, const cv::Mat &mask = cv::Mat(),const double surf_begin_hessian_threshold=100,const int surf_octaves = 1,const int surf_octaveLayers = 3,const  bool surf_extended = true,const bool surf_upright = false)
    {
        const double target_density_rate = 1.0 / (grid_radius * grid_radius) * target_density;
        // 将图像根据尺寸划分为网格，以均分为原则，尽可能接近给定的半径
        const size_t grids_row_count = static_cast<int>(std::ceil(1.0 * image.rows / grid_radius));
        const size_t grids_col_count = static_cast<int>(std::ceil(1.0 * image.cols / grid_radius));
        const cv::Size2d grid_size(image.cols*1.0 / grids_col_count, image.rows*1.0 / grids_row_count);
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
                    feature = from_image(detector, image, grid_feature.first, mask);
                    threshold /= 2.0;
                }
                grid_feature.second = feature;
            });

        for (int i = 0; i < grid_count; i++)
            result_features = merge(result_features, grid_features[i].second, cv::Point2d(grid_features[i].first.x, grid_features[i].first.y));
        return result_features;
    }
}