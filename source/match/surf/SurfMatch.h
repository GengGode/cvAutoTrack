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
    void match();

    /**
     * @brief 使用自行实现的ransac算法进行匹配
     * @param calc_is_faile 是否计算失败
     * @param affine_mat_out 输出的仿射矩阵
     * @param max_iter_num 最大迭代次数
     * @return 匹配的位置
    */
    cv::Point2d match_ransac(bool& calc_is_faile, cv::Mat& affine_mat_out, 
                             int max_iter_num);

    cv::Point2d match_continuity(bool& calc_continuity_is_faile);
    cv::Point2d match_continuity_on_city(bool& calc_continuity_is_faile);
    cv::Point2d match_continuity_not_on_city(bool& calc_continuity_is_faile);

    cv::Point2d match_no_continuity(bool& calc_is_faile);
    cv::Point2d match_no_continuity_1st(bool& calc_is_faile);

    cv::Point2d getLocalPos();
    bool getIsContinuity();

private:
    bool isMatchAllMap = true;

    double check_inliers(
        cv::Mat& H_21, std::vector<bool>& is_inlier_match,
        std::vector<cv::KeyPoint>& undist_keypts_1, std::vector<cv::KeyPoint>& undist_keypts_2,
        std::vector<TianLi::Utils::MatchKeyPoint>& matches_12
    );

};

