#pragma once
#include "algorithms.include.h"

namespace tianli::algorithms::features
{
    struct features
    {
        std::vector<size_t> indexs;
        std::vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;

        size_t size() const { return keypoints.size(); }

        static features from_image(const cv::Ptr<cv::xfeatures2d::SURF> &detector, const cv::Mat &image, const cv::Mat &mask = cv::Mat())
        {
            features fts;
            detector->detectAndCompute(image, mask, fts.keypoints, fts.descriptors);
            return fts;
        }
    };

    static features merge(const features &features_upper, const features &features_lower)
    {
        features features_merge;
        auto &[ids, kps, des] = features_merge;
        ids = features_upper.indexs;
        ids.push_back(features_upper.size());
        for (auto i : features_lower.indexs)
            ids.push_back(features_upper.size() + i);
        kps = features_upper.keypoints;
        kps.insert(kps.end(), features_lower.keypoints.begin(), features_lower.keypoints.end());
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

} // namespace tianli::algorithms
