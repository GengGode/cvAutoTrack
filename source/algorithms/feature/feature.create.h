#pragma once
#include "../algorithms.feature.h"


namespace tianli::algorithms::feature::create
{
    // TODO: 构造均匀分布的特征点，基于网格划分，在每个网格内至少采样到目标密度的特征点
    static features from_image(const cv::Mat &image, const cv::Mat &mask = cv::Mat(), const int target_density = 100)
    {
        const cv::Ptr<cv::xfeatures2d::SURF> &detector = cv::xfeatures2d::SURF::create();
        const cv::Ptr<cv::xfeatures2d::SURF> &level_0_detector = cv::xfeatures2d::SURF::create(100, 1, 1, true, false);
        const cv::Ptr<cv::xfeatures2d::SURF> &level_1_detector = cv::xfeatures2d::SURF::create(50, 1, 2, true, false);
        const cv::Ptr<cv::xfeatures2d::SURF> &level_2_detector = cv::xfeatures2d::SURF::create(25, 1, 3, true, false);
        const cv::Ptr<cv::xfeatures2d::SURF> &level_3_detector = cv::xfeatures2d::SURF::create(2, 1, 4, true, false);

        features fts;
        detector->detectAndCompute(image, mask, fts.keypoints, fts.descriptors);
        return fts;
    }

}