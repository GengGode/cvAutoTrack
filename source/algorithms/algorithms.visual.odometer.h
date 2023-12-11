#pragma once
#include "algorithms.include.h"

// 视觉里程计
// 每次小地图更新时调用
// init：传入小地图与大地图的像素缩放比例
// when mini map update：尝试与上一张小地图进行匹配，匹配成功，则返回控制量dx dy

// TODO：把这个缩放比例使用仿射/自己组Ax=B求解，并测试性能。
// TODO：封装一下，此版本测试filter可行性

cv::Mat last_mini_map;
bool inited = false;

bool orb_match(cv::Mat &img1, cv::Mat &img2, cv::Point2f &offset);

void set_mini_map(const cv::Mat &giMiniMapRef)
{
    last_mini_map = giMiniMapRef.clone();
    inited = true;
}

bool control_odometer_calculation(const cv::Mat &giMiniMapRef, cv::Point2d &control, odometer_config &config)
{
    if (!inited) {
        // JUST INIT IT
        last_mini_map = giMiniMapRef.clone();
        inited = true;
        control = cv::Point2d(0, 0);
        return false;
    }
    else {
        auto curr_mini_map = giMiniMapRef.clone();
        // use orb match to get the u, aka offset
        cv::Point2f offset;
        if (orb_match(last_mini_map, curr_mini_map, offset)) {
            control = cv::Point2d(- offset.x * config.scale, -offset.y * config.scale);
            last_mini_map = curr_mini_map.clone();
            return true;
        }
        else {
            control = cv::Point2d(0, 0);
            return false;
        }
    }
}

// 草，surfmatch跟定位功能耦合有、严重
// 这里再造一个orb match + 最优比次优
// 几乎必然是同比例的，返回偏移量
bool orb_match(cv::Mat &img1, cv::Mat &img2, cv::Point2f &offset)
{
#ifdef _DEBUG
    auto beg_time = std::chrono::steady_clock::now();
#endif
    // 不crop了，因为上一张会被crop两次。
    auto img1_cp = TianLi::Utils::crop_border(img1, 0.15);
    auto img2_cp = TianLi::Utils::crop_border(img2, 0.15);
    // resize 是等比例缩放，就不超了

    // 分别计算orb特征点
    cv::Ptr<cv::ORB> orb = cv::ORB::create(5000);
    std::vector<cv::KeyPoint> kp1, kp2;
    cv::Mat desp1, desp2;
    orb->detectAndCompute(img1_cp, cv::Mat(), kp1, desp1);
    orb->detectAndCompute(img2_cp, cv::Mat(), kp2, desp2);
    
    if (desp1.empty() || desp2.empty()) {
        return false;
    }
    
    // 首先采用knnMatch剔除最近匹配点距离与次近匹配点距离比率大于0.6的舍去
    cv::BFMatcher matcher(cv::NORM_HAMMING);
    std::vector<std::vector<cv::DMatch>> matches;
    matcher.knnMatch(desp1, desp2, matches, 2);
    // 解决没有匹配点的情况
    if (matches.size() == 0) {
        return false;
    }
    std::vector<cv::DMatch> good_matches;
    for (int i = 0; i < matches.size(); i++) {
        if (matches[i][0].distance < 0.6 * matches[i][1].distance) {
            good_matches.push_back(matches[i][0]);
        }
    }

    if (good_matches.size() == 0) {
        return false;
    }

    // auto img2_copy = img2_cp.clone();
    // 画出good matches，然后保存
    // cv::drawMatches(img1_cp, kp1, img2_cp, kp2, good_matches, img2_copy);
    // cv::imwrite("good_matches.jpg", img2_copy);

    // 计算偏移量，直接取平均
    cv::Point2f sum_offset(0, 0);
    for (int i = 0; i < good_matches.size(); i++) {
        sum_offset += kp2[good_matches[i].trainIdx].pt - kp1[good_matches[i].queryIdx].pt;
    }
    offset = cv::Point2f(sum_offset.x / good_matches.size(), sum_offset.y / good_matches.size());
#ifdef _DEBUG
    auto end_time = std::chrono::steady_clock::now();
    auto time_cost = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - beg_time).count();
    std::cout << "orb match time cost: " << time_cost << " ms" << std::endl;
#endif
    return true;
}