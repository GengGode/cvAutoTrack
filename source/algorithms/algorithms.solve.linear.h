#pragma once
#include "algorithms.include.h"

// 从匹配点对求解带仅缩放、dx和dy的线性方程
inline bool solve_linear_s_dx_dy(std::vector<cv::Point2d>& src, std::vector<cv::Point2d>& dst, double& s, double& dx, double& dy);

inline bool solve_linear_s_dx_dy(std::vector<cv::Point2d>& src, std::vector<cv::Point2d>& dst, double& s, double& dx, double& dy)
{
    // assert(src.size() == dst.size());
    // assert(src.size() >= 2);

    if (src.size() != dst.size() || src.size() < 2)
    {
        return false;
    }

    // 从匹配点对求解带仅缩放、dx和dy的线性方程
    // 1. 构造A
    cv::Mat A = cv::Mat::zeros(src.size() * 2, 3, CV_64F);
    cv::Mat b = cv::Mat::zeros(src.size() * 2, 1, CV_64F);
    for (int i = 0; i < src.size(); i++)
    {
        A.at<double>(i * 2, 0) = src[i].x;
        A.at<double>(i * 2, 1) = 1.;
        A.at<double>(i * 2, 2) = 0.;
        b.at<double>(i * 2, 0) = dst[i].x;

        A.at<double>(i * 2 + 1, 0) = src[i].y;
        A.at<double>(i * 2 + 1, 1) = 0.;
        A.at<double>(i * 2 + 1, 2) = 1.;
        b.at<double>(i * 2 + 1, 0) = dst[i].y;
    }
    // 最小二乘
    cv::Mat x = (A.t() * A).inv() * A.t() * b;
    // cv::solve + SVD
    // cv::Mat x;
    // cv::solve(A, b, x, cv::DECOMP_SVD);

    s = x.at<double>(0, 0);
    dx = x.at<double>(1, 0);
    dy = x.at<double>(2, 0);
    return true;
}