#pragma once
#include "algorithms.include.h"
#include <opencv2/core/mat.hpp>

namespace tianli::algorithms::feature
{
    static features from_image(const cv::Ptr<cv::xfeatures2d::SURF> &detector, const cv::Mat &image, const cv::Rect &roi, const cv::Mat &mask = cv::Mat())
    {
        const int border_size = 100;
        features border_fts;
        cv::Rect border_rect = cv::Rect(0, 0, image.cols, image.rows) & cv::Rect(roi.x - border_size, roi.y - border_size, roi.width + 2 * border_size, roi.height + 2 * border_size);
        cv::Rect border_mask_rect = border_rect;

        cv::Mat border = image(border_rect);
        cv::Mat border_mask = mask.empty() ? cv::Mat() : mask(border_mask_rect);

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
        fts.descriptors = cv::Mat(keypoints.size(), 64, CV_32F);
        for (int i = 0; i < selected_row_indexs.size(); i++)
            fts.descriptors.row(i).copyTo(border_fts.descriptors.row(selected_row_indexs[i]));
        return fts;
    }

    static features from_image(const cv::Ptr<cv::xfeatures2d::SURF> &detector, const cv::Mat &image, const cv::Mat &mask = cv::Mat())
    {
        const int border_size = 100;
        features border_fts;
        cv::Mat border;
        cv::Mat border_mask;
        cv::copyMakeBorder(image, border, border_size, border_size, border_size, border_size, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
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
        fts.descriptors = cv::Mat(keypoints.size(), 64, CV_32F);
        for (int i = 0; i < selected_row_indexs.size(); i++)
            fts.descriptors.row(i).copyTo(border_fts.descriptors.row(selected_row_indexs[i]));
        return fts;
    }

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
        des = cv::Mat(features_upper.size() + features_lower.size(), 64, CV_16FC4);
        des(cv::Rect(0, 0, 64, features_upper.size())) = features_upper.descriptors.clone();
        des(cv::Rect(0, features_upper.size(), 64, features_lower.size())) = features_lower.descriptors.clone();
        return features_merge;
    }

    static std::vector<features> split(const features &featuress)
    {
        if (featuress.indexs.size() == 0)
            return {featuress};
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
} // namespace tianli::algorithms
