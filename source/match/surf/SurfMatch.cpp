#include "pch.h"
#include "SurfMatch.h"
#include "match/type/MatchType.h"
#include "resources/Resources.h"
#include "utils/Utils.h"


void SurfMatch::setMiniMap(cv::Mat miniMapMat)
{
    _miniMapMat = miniMapMat;
}

void SurfMatch::Init(std::shared_ptr<trackCache::CacheInfo> cache_info)
{
    if (isInit)return;
    map.keypoints = std::move(cache_info->key_points);
    map.descriptors = std::move(cache_info->descriptors);

    //如果图像非空，则设定调试图像
    if (Resources::getInstance().DbgMap.empty())
    {
        _mapMat = Resources::getInstance().DbgMap;
    }

    double hessian_threshold = cache_info->setting.hessian_threshold;
    int octaves = cache_info->setting.octaves;
    int octave_layers = cache_info->setting.octave_layers;
    bool extended = cache_info->setting.extended;
    bool upright = cache_info->setting.up_right;
    matcher = Match(hessian_threshold, octaves, octave_layers, extended, upright);
    isInit = true;
}

void SurfMatch::UnInit()
{
    if (!isInit)return;
    map.keypoints.clear();
    map.descriptors.release();
    isInit = false;
}

bool SurfMatch::match(cv::Point2d& pos)
{
    bool isSuccess = match_all_map(pos);
}

bool SurfMatch::match_all_map(cv::Point2d& pos)
{
}

Match::Match(double hessian_threshold, int octaves, int octave_layers, bool extended, bool upright)
{
    detector = cv::xfeatures2d::SURF::create(hessian_threshold, octaves, octave_layers, extended, upright);
    //matcher  = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
}

std::vector<std::vector<cv::DMatch>> Match::match(const cv::Mat& query_descriptors, const cv::Mat& train_descriptors)
{
    std::vector<std::vector<cv::DMatch>> match_group;
    matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    matcher->knnMatch(query_descriptors, train_descriptors, match_group, 2);
    return match_group;
}

std::vector<std::vector<cv::DMatch>> Match::match(KeyMatPoint& query_key_mat_point, KeyMatPoint& train_key_mat_point)
{
    return match(query_key_mat_point.descriptors, train_key_mat_point.descriptors);
}

bool Match::detect_and_compute(const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors)
{
    if (img.empty()) return  false;
    detector->detectAndCompute(img, cv::noArray(), keypoints, descriptors);
    if (keypoints.size() == 0) return false;
    return true;
}

bool Match::detect_and_compute(const cv::Mat& img, Match::KeyMatPoint& key_mat_point)
{
    return detect_and_compute(img, key_mat_point.keypoints, key_mat_point.descriptors);
}

