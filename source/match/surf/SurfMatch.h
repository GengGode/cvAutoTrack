#pragma once
#include <opencv2/xfeatures2d.hpp>
#include "utils/Utils.h"
#include "algorithms/algorithms.include.h"
#include "resources/trackCache.h"

class Matcher
{
public:
    Matcher(double hessian_threshold = 1, int octaves = 1, int octave_layers = 1, bool extended = false, bool upright = true);
    ~Matcher() = default;
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
    float zoom;         //小地图的缩放
    int area_id;        //当前地区的id
    int sub_area_id;    //当前子地区的id
public:
    SurfMatch() = default;
    ~SurfMatch() = default;
    Matcher matcher;

    features all_map_features;

    bool isInit = false;
    bool isContinuity = false;

    int continuity_retry = 0;		//局部匹配重试次数
    const int max_continuity_retry = 3;		//最大重试次数

    bool is_success_match = false;

    void Init(std::shared_ptr<trackCache::CacheInfo> cache_info);
    void UnInit();
    bool UpdateMatch(cv::Mat mini_map);

    cv::Point2d CurrentPosition();
    int CurrentAreaId();
    int CurrentSubAreaId();
    float CurrentZoom();
    bool IsContinuity();
   
private:
    bool isMatchAllMap = true;

    void GetInfo(const std::vector<cv::DMatch> dmatchs, std::vector<cv::KeyPoint> keypoints_scene, cv::Mat mat);


#ifdef _DEBUG
    /**
     * @brief 绘制匹配的特征点，分组DMatch版本
     * @param img_scene 大地图的图像
     * @param keypoint_scene 大地图的特征点
     * @param img_object 小地图的图像
     * @param keypoint_object 小地图的特征点
     * @param matches 匹配的点对
    */
    static void draw_matched_keypoints(const cv::Mat &img_scene, const std::vector<cv::KeyPoint> &keypoint_scene, const cv::Mat &img_object, const std::vector<cv::KeyPoint> &keypoint_object, const std::vector<std::vector<cv::DMatch>> &good_matches);

    /**
     * @brief 绘制匹配的特征点，单DMatch版本
     * @param img_scene 大地图的图像
     * @param keypoint_scene 大地图的特征点
     * @param img_object 小地图的图像
     * @param keypoint_object 小地图的特征点
     * @param matches 匹配的点对
    */
    static void draw_matched_keypoints(const cv::Mat &img_scene, const std::vector<cv::KeyPoint> &keypoint_scene, const cv::Mat &img_object, const std::vector<cv::KeyPoint> &keypoint_object, const std::vector<cv::DMatch> &d_matches);

    /**
     * @brief 绘制匹配的特征点，无DMatch版本，注意点对需要匹配
     * @param img_scene 大地图的图像
     * @param keypoint_scene 大地图的特征点
     * @param img_object 小地图的图像
     * @param keypoint_object 小地图的特征点
    */
    void draw_matched_keypoints(const cv::Mat &img_scene, const std::vector<cv::KeyPoint> &keypoint_scene, const cv::Mat &img_object, const std::vector<cv::KeyPoint> &keypoint_object);
#endif // _DEBUG

    /**
     * @brief 匹配小地图，获取坐标，匹配点和变换
     * @param matcher 匹配器
     * @param mini_map_mat 小地图图像
     * @param map 大地图特征点
     * @param out_pos 匹配到的坐标
     * @param d_matchs 匹配到的点对
     * @param mat 变换矩阵
     * @return 是否匹配成功
    */
    static bool MatchMiniMap(Matcher &matcher, const cv::Mat &mini_map_mat, features &map_feature, cv::Point2d &out_pos, std::vector<cv::DMatch> &out_d_matchs, cv::Mat &out_mat);


    /**
     * @brief 1st 粗筛，使用最佳比次佳移除质量较差的点
     * @param keypoint_scene 大地图的特征点
     * @param keypoint_object 小地图的特征点
     * @param KNN_m 包含至少两条匹配结果的特征点对
     * @param out_good_matches 返回好的特征点对
     * @param ratio_thresh 最佳比次佳的差异，越小越严格
    */
    static void calc_good_matches(const std::vector<cv::KeyPoint> &keypoint_scene, const std::vector<cv::KeyPoint> &keypoint_object, const std::vector<std::vector<cv::DMatch>> &KNN_m, std::vector<cv::DMatch> &out_good_matches, float ratio_thresh = 0.8);

    /**
     * @brief 2nd 移除离群点，需要至少3个匹配点
     * @param ref_dmatchs 需要处理的特征点对
     * @param keypoint_scene 大地图的特征点
     * @param keypoint_object 小地图的特征点
     * @param max_destance 距离中心的最大距离，越小越严格
    */
    static void RemoveOutliers(std::vector<cv::DMatch> &ref_dmatchs, const std::vector<cv::KeyPoint> &keypoint_scene, const std::vector<cv::KeyPoint> &keypoint_object, float max_destance = 512);
    
    /**
     * @brief 配准给定的特征点
     * @param ref_dmatch 匹配点对，如果配准后有误差过大的点，则执行完成后该点对会被删除
     * @param keypoint_scene 大地图特征点
     * @param keypoint_object 小地图特征点
     * @return 变换矩阵
    */
    static cv::Mat KeyPoint_Registration(std::vector<cv::DMatch> &ref_dmatchs, const std::vector<cv::KeyPoint> &keypoint_scene, const std::vector<cv::KeyPoint> &keypoint_object);

};
