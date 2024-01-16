#include "algorithms.include.h"
#include "pch.h"

inline void features::TransferAxes(const cv::Point2f &origin, const double scale)
{
    for (auto &keypoint : keypoints)
    {
        keypoint.pt = cv::Point2d((keypoint.pt - origin) * scale);
    }
}

void features::TransferAxes(const cv::Rect2d& inRect, const cv::Rect2d& outRect)
{
    for (auto &keypoint : keypoints)
    {
        //输入>输出的缩放
        auto scale = cv::Point2d(outRect.width / inRect.width, outRect.height / inRect.height);
        //输入>输出的平移
        cv::Rect2d scaled_inRect = cv::Rect2d(inRect.x * scale.x, inRect.y * scale.y,
            inRect.width * scale.x, inRect.height * scale.y);
        auto translate = cv::Point2d(outRect.x - scaled_inRect.x, outRect.y - scaled_inRect.y);
        //坐标换算
        keypoint.pt = cv::Point2d(keypoint.pt.x * scale.x, keypoint.pt.y * scale.y) + translate;
    }
}

void features::TransformAxes(const cv::Mat& trans_mat)
{
    for (auto &keypoint : keypoints)
    {
        cv::Mat pos_vec = (cv::Mat_<double>(3, 1) << keypoint.pt.x, keypoint.pt.y, 1);
        cv::Mat pos_vec_translated = trans_mat * pos_vec;
        keypoint.pt = cv::Point2d(pos_vec_translated.at<double>(0, 0) / pos_vec_translated.at<double>(2, 0), pos_vec_translated.at<double>(1, 0) / pos_vec_translated.at<double>(2, 0));
    }
}
