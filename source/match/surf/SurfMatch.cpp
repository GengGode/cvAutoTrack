#include "pch.h"
#include "SurfMatch.h"
#include "resources/Resources.h"
#include "utils/Utils.h"
#include "algorithms/algorithms.solve.linear.h"
#include "algorithms/features/features.operate.h"
#include "global/global.genshin.h"
#include <opencv2/calib3d.hpp>

using namespace tianli::algorithms;

void SurfMatch::Init(std::shared_ptr<trackCache::CacheInfo> cache_info)
{
    if (isInit)
        return;
    this->cache_info = cache_info;
    all_map_features = features(cache_info->key_points, cache_info->descriptors);

    // 如果图像非空，则设定调试图像
    if (Resources::getInstance().debug_map_image.empty())
    {
        _mapMat = Resources::getInstance().debug_map_image;
    }

    double hessian_threshold = cache_info->setting.hessian_threshold;
    int octave = cache_info->setting.octaves;
    int octave_layers = cache_info->setting.octave_layers;
    bool extended = cache_info->setting.extended;
    bool upright = cache_info->setting.up_right;
    matcher = Matcher(hessian_threshold, octave, octave_layers, extended, upright);
    isInit = true;
}

void SurfMatch::UnInit()
{
    if (!isInit)
        return;
    all_map_features.keypoints.clear();
    all_map_features.descriptors.release();
    isInit = false;
}

bool SurfMatch::UpdateMatch(cv::Mat mini_map)
{
    is_success_match = false;
    std::vector<cv::DMatch> d_matchs;
    cv::Mat m_mat;
    if (MatchMiniMap(matcher, mini_map, all_map_features, pos, d_matchs, m_mat) == true)
    {
        is_success_match = true;
        // 求解其他信息
        GetInfo(d_matchs, all_map_features.keypoints, m_mat);
    }
    return false;
}

bool SurfMatch::MatchMiniMap(Matcher& matcher, const cv::Mat& mini_map_img, features& map_feature, cv::Point2d& out_pos, std::vector<cv::DMatch>& out_d_matchs, cv::Mat& out_transform_mat)
{
    cv::Point2d map_pos;
    features mini_map_feature;
    cv::Size2i mini_map_pixel_size = mini_map_img.size();

    matcher.detect_and_compute(mini_map_img, mini_map_feature);

    // 预变换地图，统一为原点在中心，尺寸为200*200的地图
    mini_map_feature.transform(cv::Point2d(mini_map_pixel_size / 2), cv::Size2d(200.0 / mini_map_pixel_size.width, 200.0 / mini_map_pixel_size.height));

    // 没有提取到特征点直接返回，结果无效
    if (mini_map_feature.keypoints.empty())
    {
        return false;
    }
    // 匹配特征点
    std::vector<std::vector<cv::DMatch>> KNN_m = matcher.match(mini_map_feature, map_feature);
    // 01 粗筛，使用最优比次优方法获取质量高的匹配
    std::vector<cv::DMatch> d_matchs;
    calc_good_matches(map_feature.keypoints, mini_map_feature.keypoints, KNN_m, d_matchs);
    // 02 移除外点
    RemoveOutliers(d_matchs, map_feature.keypoints, mini_map_feature.keypoints, 500);

#ifdef _DEBUG
    debug_draw_match_keypoints_wrapper(map_feature, mini_map_img, mini_map_feature, d_matchs);
#endif // DEBUG

    // 少于2个点无法配准
    if (d_matchs.size() < 2)
    {
        return false;
    }
    // 03 使用中心配准的方法精筛特征点
    auto trans_mat = KeyPoint_Registration(d_matchs, map_feature.keypoints, mini_map_feature.keypoints);
    // 04 计算坐标
    out_pos = cv::Point2d(trans_mat.at<double>(0, 2), trans_mat.at<double>(1, 2));
    out_d_matchs = d_matchs;
    out_transform_mat = trans_mat;
    return true;
}

cv::Point2d SurfMatch::CurrentPosition()
{
    return pos;
}

int SurfMatch::CurrentAreaId()
{
    return area_id;
}

float SurfMatch::CurrentZoom()
{
    return zoom;
}

bool SurfMatch::IsContinuity()
{
    return isContinuity;
}

void SurfMatch::GetInfo(const std::vector<cv::DMatch> dmatchs, std::vector<cv::KeyPoint> keypoints_scene, cv::Mat mat)
{
    // 从矩阵反解缩放
    double zoom_x = std::sqrt(std::pow(mat.at<double>(0, 0), 2.0) + std::pow(mat.at<double>(0, 1), 2.0));
    double zoom_y = std::sqrt(std::pow(mat.at<double>(1, 0), 2.0) + std::pow(mat.at<double>(1, 1), 2.0));
    zoom = static_cast<float>((zoom_x + zoom_y) / 2.0);

    // 抽取keypoints
    std::vector<cv::KeyPoint> matched_keypoints;
    for_each(dmatchs.begin(), dmatchs.end(), [&](const cv::DMatch& dmatch) { matched_keypoints.emplace_back(keypoints_scene[dmatch.trainIdx]); });
    // 获取地区信息
    std::map<int, int> matched_area_ids;
    for_each(matched_keypoints.begin(), matched_keypoints.end(), [&](const cv::KeyPoint& keypoint) { matched_area_ids[keypoint.class_id]++; });

    int max_count = 0;
    int max_id = 0;
    for_each(matched_area_ids.begin(), matched_area_ids.end(), [&](const std::pair<int, int>& pair) {
        if (pair.second > max_count)
        {
            max_count = pair.second;
            max_id = pair.first;
        }
    });
    area_id = max_id;
}

void SurfMatch::draw_matched_keypoints(const cv::Mat& img_scene, const std::vector<cv::KeyPoint>& keypoint_scene, const cv::Mat& img_object, const std::vector<cv::KeyPoint>& keypoint_object,
                                       const std::vector<std::vector<cv::DMatch>>& d_matches)
{
    cv::Mat img_matches, imgmap, imgminmap;
    drawKeypoints(img_scene, keypoint_scene, imgmap, cv::Scalar::all(-1));
    drawKeypoints(img_object, keypoint_object, imgminmap, cv::Scalar::all(-1));
    drawMatches(img_object, keypoint_object, img_scene, keypoint_scene, d_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<std::vector<char>>(),
                cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
}

void SurfMatch::draw_matched_keypoints(const cv::Mat& img_scene, const std::vector<cv::KeyPoint>& keypoint_scene, const cv::Mat& img_object, const std::vector<cv::KeyPoint>& keypoint_object,
                                       const std::vector<cv::DMatch>& d_matches)
{
    cv::Mat img_matches, imgmap, imgminmap;
    drawKeypoints(img_scene, keypoint_scene, imgmap, cv::Scalar::all(-1));
    drawKeypoints(img_object, keypoint_object, imgminmap, cv::Scalar::all(-1));
    drawMatches(img_object, keypoint_object, img_scene, keypoint_scene, d_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(),
                cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
}

void SurfMatch::draw_matched_keypoints(const cv::Mat& img_scene, const std::vector<cv::KeyPoint>& keypoint_scene, const cv::Mat& img_object, const std::vector<cv::KeyPoint>& keypoint_object)
{
    std::vector<cv::DMatch> d_matches;
    for (int i = 0; i < keypoint_object.size(); i++)
    {
        d_matches.push_back(cv::DMatch(i, i, 0));
    }
    draw_matched_keypoints(img_scene, keypoint_scene, img_object, keypoint_object, d_matches);
}

void SurfMatch::debug_draw_match_keypoints_wrapper(features& map_feature, const cv::Mat& mini_map_image, features& mini_map_feature, std::vector<cv::DMatch>& d_matchs)
{
    cv::Size2i mini_map_pixel_size = mini_map_image.size();
    features translated_map_feature = map_feature;
    translated_map_feature.transform(cache_info->setting.roi, cv::Rect(cv::Point2i(), Resources::getInstance().debug_map_image.size()));

    features translated_minimap_feature = mini_map_feature;
    translated_minimap_feature.transform(cv::Point2d(-100, -100), cv::Size2d(mini_map_pixel_size.width / 200.0, mini_map_pixel_size.height / 200.0));

    draw_matched_keypoints(Resources::getInstance().debug_map_image, translated_map_feature.keypoints, mini_map_image, translated_minimap_feature.keypoints, d_matchs);
}

void SurfMatch::calc_good_matches(const std::vector<cv::KeyPoint>& keypoint_scene, const std::vector<cv::KeyPoint>& keypoint_object, const std::vector<std::vector<cv::DMatch>>& KNN_m,
                                  std::vector<cv::DMatch>& out_good_matches, float ratio_thresh)
{
    for (size_t i = 0; i < KNN_m.size(); i++)
    {
        if (KNN_m[i][0].distance < ratio_thresh * KNN_m[i][1].distance)
        {
            out_good_matches.push_back(KNN_m[i][0]);
        }
    }
}

void SurfMatch::RemoveOutliers(std::vector<cv::DMatch>& ref_dmatchs, const std::vector<cv::KeyPoint>& keypoint_scene, const std::vector<cv::KeyPoint>& keypoint_object, float max_destance)
{
    // 使用中值作为预测值，筛选离群点
    std::vector<double> xlist;
    std::vector<double> ylist;

    for (const auto& d_match : ref_dmatchs)
    {
        xlist.emplace_back(keypoint_scene[d_match.trainIdx].pt.x);
        ylist.emplace_back(keypoint_scene[d_match.trainIdx].pt.y);
    }
    std::sort(xlist.begin(), xlist.end(), [](double a, double b) { return a < b; });
    std::sort(ylist.begin(), ylist.end(), [](double a, double b) { return a < b; });

    double median_x = xlist[ref_dmatchs.size() / 2];
    double median_y = ylist[ref_dmatchs.size() / 2];
    ref_dmatchs.erase(
        std::remove_if(ref_dmatchs.begin(), ref_dmatchs.end(),
                       [&](cv::DMatch dmatch) { return abs(keypoint_scene[dmatch.trainIdx].pt.x - median_x) > max_destance || (keypoint_scene[dmatch.trainIdx].pt.y - median_y) > max_destance; }),
        ref_dmatchs.end());
}

cv::Mat SurfMatch::KeyPoint_Registration(std::vector<cv::DMatch>& ref_dmatchs, const std::vector<cv::KeyPoint>& keypoint_scene, const std::vector<cv::KeyPoint>& keypoint_object)
{
    // 抽出匹配点
    cv::Mat matched_keypoints_scene(cv::Size(2, ref_dmatchs.size()), CV_32FC1);
    cv::Mat matched_keypoints_object(cv::Size(2, ref_dmatchs.size()), CV_32FC1);
    for (int i = 0; i < ref_dmatchs.size(); i++)
    {
        matched_keypoints_scene.at<float>(i, 0) = keypoint_scene[ref_dmatchs[i].trainIdx].pt.x;
        matched_keypoints_scene.at<float>(i, 1) = keypoint_scene[ref_dmatchs[i].trainIdx].pt.y;
        matched_keypoints_object.at<float>(i, 0) = keypoint_object[ref_dmatchs[i].queryIdx].pt.x;
        matched_keypoints_object.at<float>(i, 1) = keypoint_object[ref_dmatchs[i].queryIdx].pt.y;
    }
    // 求解变换矩阵
    cv::Mat out_mask;

    cv::Mat trans_mat = cv::estimateAffinePartial2D(matched_keypoints_object, matched_keypoints_scene, out_mask, cv::RANSAC, 10);

    // 剔除不匹配的点
    std::vector<cv::DMatch> good_matches;
    for (int i = 0; i < out_mask.rows; i++)
    {
        if (out_mask.at<uchar>(i, 0) == 1)
        {
            good_matches.push_back(ref_dmatchs[i]);
        }
    }
    ref_dmatchs = good_matches;
    return trans_mat;
}

Matcher::Matcher(double hessian_threshold, int octaves, int octave_layers, bool extended, bool upright)
{
    detector = cv::xfeatures2d::SURF::create(hessian_threshold, octaves, octave_layers, extended, upright);
    matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
}

std::vector<std::vector<cv::DMatch>> Matcher::match(const cv::Mat& query_descriptors, const cv::Mat& train_descriptors)
{
    std::vector<std::vector<cv::DMatch>> match_group;
    matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE);
    matcher->knnMatch(query_descriptors, train_descriptors, match_group, 2);
    return match_group;
}

std::vector<std::vector<cv::DMatch>> Matcher::match(features& query_key_mat_point, features& train_key_mat_point)
{
    return match(query_key_mat_point.descriptors, train_key_mat_point.descriptors);
}

bool Matcher::detect_and_compute(const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors)
{
    if (img.empty())
        return false;
    detector->detectAndCompute(img, cv::noArray(), keypoints, descriptors);
    if (keypoints.size() == 0)
        return false;
    return true;
}

bool Matcher::detect_and_compute(const cv::Mat& img, features& key_mat_point)
{
    return detect_and_compute(img, key_mat_point.keypoints, key_mat_point.descriptors);
}
