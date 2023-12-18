#pragma once
#include "utils/Utils.h"
#include "algorithms/algorithms.include.h"
#include "resources/trackCache.h"

class Match
{
public:
    Match(double hessian_threshold = 1, int octaves = 1, int octave_layers = 1, bool extended = false, bool upright = true);
    ~Match() = default;
public:
    cv::Ptr<cv::xfeatures2d::SURF> detector;
    cv::Ptr<cv::DescriptorMatcher> matcher;
    features query;
    features train;
public:
    std::vector<std::vector<cv::DMatch>> match(const cv::Mat &query_descriptors, const cv::Mat &train_descriptors);
    std::vector<std::vector<cv::DMatch>> match(features &query_key_mat_point, features &train_key_mat_point);
    bool detect_and_compute(const cv::Mat &img, std::vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors);
    bool detect_and_compute(const cv::Mat &img, features &key_mat_point);
};

class SurfMatch
{
    cv::Mat _mapMat;
    cv::Mat _miniMapMat;

    //缓存信息
    std::shared_ptr<trackCache::CacheInfo> cache_info;

    cv::Point2d pos;
    cv::Point2d last_pos;		// 上一次匹配的地点，匹配失败，返回上一次的结果
public:
    SurfMatch() = default;
    ~SurfMatch() = default;

public:
    Match matcher;

    features all_map_features;
    features some_map_features;
    features mini_map_features;

    bool isInit = false;
    bool isContinuity = false;
    bool isCoveying = false;

    int continuity_retry = 0;		//局部匹配重试次数
    const int max_continuity_retry = 3;		//最大重试次数

    bool is_success_match = false;

    void Init(std::shared_ptr<trackCache::CacheInfo> cache_info);
    void UnInit();
    bool GetNextPosition(cv::Mat mini_map, cv::Point2d &position);

    /**
     * @brief 使用自行实现的ransac算法进行匹配
     * @param calc_is_faile 是否计算失败
     * @param affine_mat_out 输出的仿射矩阵
     * @param max_iter_num 最大迭代次数
     * @return 匹配的位置
    */
    cv::Point2d match_ransac(bool &calc_is_faile, cv::Mat &affine_mat_out,
        int max_iter_num);

    cv::Point2d match_all_map(Match &matcher, const cv::Mat &mini_map_mat, features &mini_map, features &map, bool &calc_is_faile);

    cv::Point2d getLocalPos();
    bool getIsContinuity();



   
private:
    bool isMatchAllMap = true;

    double check_inliers(
        cv::Mat &H_21, std::vector<bool> &is_inlier_match,
        std::vector<cv::KeyPoint> &undist_keypts_1, std::vector<cv::KeyPoint> &undist_keypts_2,
        std::vector<TianLi::Utils::MatchKeyPoint> &matches_12
    );

    /**
     * @brief 绘制匹配的特征点
     * @param img_scene 大地图的图像
     * @param keypoint_scene 大地图的特征点
     * @param img_object 小地图的图像
     * @param keypoint_object 小地图的特征点
     * @param matches 匹配的点对
    */
    void draw_matched_keypoints(const cv::Mat &img_scene, const std::vector<cv::KeyPoint> &keypoint_scene, const cv::Mat &img_object, const std::vector<cv::KeyPoint> &keypoint_object, const std::vector<cv::DMatch> &good_matches);

    static void calc_good_matches(const cv::Mat &img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat &img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<std::vector<cv::DMatch>> &KNN_m, double ratio_thresh, std::vector<TianLi::Utils::MatchKeyPoint> &good_keypoints, std::vector<cv::DMatch> &good_matches);

};
