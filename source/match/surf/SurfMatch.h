#pragma once
#include "utils/Utils.h"
#include "resources/trackCache.h"

class Match
{
public:
    struct KeyMatPoint
    {
        std::vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
        bool empty() { return keypoints.size() == 0; }
        auto size() { return keypoints.size(); }
    };
public:
    Match(double hessian_threshold = 1, int octaves = 1, int octave_layers = 1, bool extended = false, bool upright = true);
    ~Match() = default;
public:
    cv::Ptr<cv::xfeatures2d::SURF> detector;
    cv::Ptr<cv::DescriptorMatcher> matcher;
    KeyMatPoint query;
    KeyMatPoint train;
public:
    std::vector<std::vector<cv::DMatch>> match(const cv::Mat& query_descriptors, const cv::Mat& train_descriptors);
    std::vector<std::vector<cv::DMatch>> match(KeyMatPoint& query_key_mat_point, KeyMatPoint& train_key_mat_point);
    bool detect_and_compute(const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors);
    bool detect_and_compute(const cv::Mat& img, KeyMatPoint& key_mat_point);
};



class SurfMatch
{
    cv::Mat _mapMat;
    cv::Mat _miniMapMat;

    //缓存信息
    trackCache::CacheInfo* cache_info;

    cv::Point2d pos;
    cv::Point2d last_pos;		// 上一次匹配的地点，匹配失败，返回上一次的结果
public:
    SurfMatch() = default;
    ~SurfMatch() = default;

public:
    Match matcher;

    Match::KeyMatPoint map;
    Match::KeyMatPoint some_map;
    Match::KeyMatPoint mini_map;

    bool isInit = false;
    bool isContinuity = false;
    bool isCoveying = false;

    int continuity_retry = 0;		//局部匹配重试次数
    const int max_continuity_retry = 3;		//最大重试次数

    bool is_success_match = false;

    void setMiniMap(cv::Mat miniMapMat);
    void Init(std::shared_ptr<trackCache::CacheInfo> cache_info);
    void UnInit();
    bool match(cv::Point2d& pos);

private:
    bool match_all_map(cv::Point2d& pos);

};

