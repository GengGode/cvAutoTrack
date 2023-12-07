#pragma once
#include "../algorithms.include.h"

namespace tianli::algorithms::match
{
    features from(cv::Mat image, cv::Mat mask ,double hessian_threshold = 100, int octaves= 4, int octave_layers =3, bool extended =false , bool upright =false)
    {
        features result_features;
        auto detector = cv::xfeatures2d::SURF::create(hessian_threshold, octaves, octave_layers, extended, upright);
        detector->detectAndCompute(image, mask, result_features.keypoints, result_features.descriptors);
        return result_features;
    }

    std::vector<std::vector<cv::DMatch>> match_features(features target_fts, features object_fts)
    {
        std::vector<std::vector<cv::DMatch>> match_group;
        auto matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
        matcher->knnMatch(query_descriptors, train_descriptors, match_group, 2);
        return match_group;
    }

} // namespace tianli::algorithms::match
