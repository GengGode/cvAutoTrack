#include "pch.h"
#include "SurfMatch.h"
#include "global/global.genshin.h"
#include "resources/Resources.h"
#include "utils/Utils.h"
#include "algorithms/algorithms.solve.linear.h"
#include "algorithms/features/features.operate.h"

using namespace tianli::algorithms;

void SurfMatch::Init(std::shared_ptr<trackCache::CacheInfo> cache_info)
{
    if (isInit)return;
    this->cache_info = cache_info;
    all_map_features = features(cache_info->key_points, cache_info->descriptors);

    //如果图像非空，则设定调试图像
    if (Resources::getInstance().debug_map_image.empty())
    {
        _mapMat = Resources::getInstance().debug_map_image;
    }

    double hessian_threshold = cache_info->setting.hessian_threshold;
    //double hessian_threshold = 1.0;
    int octave = cache_info->setting.octaves;
    int octave_layers = cache_info->setting.octave_layers;
    //int octave_layers = 1;
    bool extended = cache_info->setting.extended;
    bool upright = cache_info->setting.up_right;
    matcher = Matcher(hessian_threshold);
    // 
    isInit = true;
}

void SurfMatch::UnInit()
{
    if (!isInit)return;
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
        //求解其他信息
        GetInfo(d_matchs, all_map_features.keypoints, m_mat);
    }
    return false;
}

bool SurfMatch::MatchMiniMap(Matcher &matcher, const cv::Mat &mini_map_mat, features &map_feature, cv::Point2d& out_pos, std::vector<cv::DMatch> &out_d_matchs, cv::Mat &out_mat)
{
    cv::Point2d map_pos;
    features mini_map_feature;
    matcher.detect_and_compute(mini_map_mat, mini_map_feature);
    // 没有提取到特征点直接返回，结果无效
    if (mini_map_feature.keypoints.empty())
    {
        return false;
    }
    // 匹配特征点
    std::vector<std::vector<cv::DMatch>> KNN_m = matcher.match(mini_map_feature, map_feature);
    //01 粗筛，使用最优比次优方法获取质量高的匹配
    std::vector<cv::DMatch> d_matchs;
    calc_good_matches(map_feature.keypoints, mini_map_feature.keypoints, KNN_m, d_matchs);
    //02 移除外点
    RemoveOutliers(d_matchs, map_feature.keypoints, mini_map_feature.keypoints, 500);
    //少于4个点无法配准
    if (d_matchs.size() < 4) {
        return false;
    }

    //03 使用中心配准的方法精筛特征点
    auto trans_mat = KeyPoint_Registration(d_matchs, map_feature.keypoints, mini_map_feature.keypoints);
    //04 计算坐标
    out_pos = cv::Point2d(mini_map_mat.cols / 2.0, mini_map_mat.rows / 2.0);
    out_pos = TianLi::Utils::transform(out_pos,trans_mat);
    out_d_matchs = d_matchs;
    out_mat = trans_mat;
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
    //从矩阵反解缩放
    double zoom_x = std::sqrt(std::pow(mat.at<double>(0, 0),2.0) + std::pow(mat.at<double>(0,1),2.0));
    double zoom_y = std::sqrt(std::pow(mat.at<double>(1, 0),2.0) + std::pow(mat.at<double>(1,1),2.0));
    zoom = static_cast<float>((zoom_x + zoom_y) / 2.0);

    //抽取keypoints
    std::vector<cv::KeyPoint> matched_keypoints;
    for_each(dmatchs.begin(), dmatchs.end(), [&](const cv::DMatch& dmatch) {
        matched_keypoints.emplace_back(keypoints_scene[dmatch.trainIdx]);
    });
    //获取地区信息
    std::map<int, int> matched_area_ids;
    for_each(matched_keypoints.begin(), matched_keypoints.end(), [&](const cv::KeyPoint& keypoint) {
        matched_area_ids[keypoint.class_id]++;
        });
    
    int max_count = 0;
    int max_id = 0;
    for_each(matched_area_ids.begin(), matched_area_ids.end(), [&](const std::pair<int, int> &pair) {
        if (pair.second > max_count)
        {
            max_count = pair.second;
            max_id = pair.first;
        }
    });
    area_id = max_id;
}

#ifdef _DEBUG
void SurfMatch::draw_matched_keypoints(const cv::Mat &img_scene, const std::vector<cv::KeyPoint> &keypoint_scene, const cv::Mat &img_object, const std::vector<cv::KeyPoint> &keypoint_object, const std::vector<std::vector<cv::DMatch>> &d_matches)
{
    cv::Mat img_matches, imgmap, imgminmap;
    drawKeypoints(img_scene, keypoint_scene, imgmap, cv::Scalar::all(-1));
    drawKeypoints(img_object, keypoint_object, imgminmap, cv::Scalar::all(-1));
    drawMatches(img_object, keypoint_object, img_scene, keypoint_scene, d_matches, img_matches,
        cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<std::vector<char>>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
}

void SurfMatch::draw_matched_keypoints(const cv::Mat &img_scene, const std::vector<cv::KeyPoint> &keypoint_scene, const cv::Mat &img_object, const std::vector<cv::KeyPoint> &keypoint_object, const std::vector<cv::DMatch> &d_matches)
{
    cv::Mat img_matches, imgmap, imgminmap;
    drawKeypoints(img_scene, keypoint_scene, imgmap, cv::Scalar::all(-1));
    drawKeypoints(img_object, keypoint_object, imgminmap, cv::Scalar::all(-1));
    drawMatches(img_object, keypoint_object, img_scene, keypoint_scene, d_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
}

void SurfMatch::draw_matched_keypoints(const cv::Mat &img_scene, const std::vector<cv::KeyPoint> &keypoint_scene, const cv::Mat &img_object, const std::vector<cv::KeyPoint> &keypoint_object)
{
    std::vector<cv::DMatch> d_matches;
    for (int i = 0; i < keypoint_object.size(); i++)
    {
        d_matches.push_back(cv::DMatch(i, i, 0));
    }
    draw_matched_keypoints(img_scene, keypoint_scene, img_object, keypoint_object, d_matches);
}
#endif // _DEBUG

void SurfMatch::calc_good_matches(const std::vector<cv::KeyPoint> &keypoint_scene, const std::vector<cv::KeyPoint> &keypoint_object,
    const std::vector<std::vector<cv::DMatch>> &KNN_m,
    std::vector<cv::DMatch> &out_good_matches, float ratio_thresh)
{
    for (size_t i = 0; i < KNN_m.size(); i++)
    {
        if (KNN_m[i][0].distance < ratio_thresh * KNN_m[i][1].distance)
        {
            out_good_matches.push_back(KNN_m[i][0]);
        }
    }
}

void SurfMatch::RemoveOutliers(std::vector<cv::DMatch> &ref_dmatchs, const std::vector<cv::KeyPoint> &keypoint_scene, const std::vector<cv::KeyPoint> &keypoint_object, float max_destance)
{
    //使用中值作为预测值，筛选离群点
    std::vector<double> xlist;
    std::vector<double> ylist; 

    for (const auto& d_match:ref_dmatchs)
    {
        xlist.emplace_back(keypoint_scene[d_match.trainIdx].pt.x);
        ylist.emplace_back(keypoint_scene[d_match.trainIdx].pt.y);
    }
    std::sort(xlist.begin(), xlist.end(), [](double a, double b) {
        return a < b; });
    std::sort(ylist.begin(), ylist.end(), [](double a, double b) {
        return a < b; });

    double median_x = xlist[ref_dmatchs.size() / 2];
    double median_y = ylist[ref_dmatchs.size() / 2];
    ref_dmatchs.erase(std::remove_if(ref_dmatchs.begin(), ref_dmatchs.end(), [&](cv::DMatch dmatch) {
        return abs(keypoint_scene[dmatch.trainIdx].pt.x - median_x) > max_destance || (keypoint_scene[dmatch.trainIdx].pt.y - median_y) > max_destance; }), ref_dmatchs.end());
}

cv::Mat SurfMatch::KeyPoint_Registration(std::vector<cv::DMatch> &ref_dmatchs, const std::vector<cv::KeyPoint> &keypoint_scene, const std::vector<cv::KeyPoint> &keypoint_object)
{
    //抽出匹配点
    std::vector<cv::Point2f> matched_keypoints_scene;
    std::vector<cv::Point2f> matched_keypoints_object;
    for_each(ref_dmatchs.begin(), ref_dmatchs.end(), [&](const cv::DMatch& d_match) {
        matched_keypoints_scene.push_back(keypoint_scene[d_match.trainIdx].pt);
        matched_keypoints_object.push_back(keypoint_object[d_match.queryIdx].pt);
    });
    //求解变换矩阵
    std::vector<int> out_mask;
    cv::Mat trans_mat = cv::findHomography(matched_keypoints_object, matched_keypoints_scene,out_mask,cv::RANSAC,10);
    
    //剔除不匹配的点
    std::vector<cv::DMatch> good_matches;
    for (int i = 0; i < out_mask.size(); i++)
    {
        if (out_mask[i] == 1)
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

std::vector<std::vector<cv::DMatch>> Matcher::match(const cv::Mat &query_descriptors, const cv::Mat &train_descriptors)
{
    std::vector<std::vector<cv::DMatch>> match_group;
    matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE);
    matcher->knnMatch(query_descriptors, train_descriptors, match_group, 2);
    return match_group;
}

std::vector<std::vector<cv::DMatch>> Matcher::match(features &query_key_mat_point, features &train_key_mat_point)
{
    return match(query_key_mat_point.descriptors, train_key_mat_point.descriptors);
}

bool Matcher::detect_and_compute(const cv::Mat &img, std::vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors)
{
    if (img.empty()) return  false;
    detector->detectAndCompute(img, cv::noArray(), keypoints, descriptors);
    if (keypoints.size() == 0) return false;
    return true;
}

bool Matcher::detect_and_compute(const cv::Mat &img, features &key_mat_point)
{
    return detect_and_compute(img, key_mat_point.keypoints, key_mat_point.descriptors);
}

