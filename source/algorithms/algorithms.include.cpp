#include "pch.h"
#include "algorithms.include.h"

features::features(const std::vector<cv::KeyPoint> &keypoints, const cv::Mat &descriptors) :
    keypoints(keypoints), descriptors(descriptors)
{
    indexs.resize(keypoints.size());
    std::iota(indexs.begin(), indexs.end(), 0);
}
