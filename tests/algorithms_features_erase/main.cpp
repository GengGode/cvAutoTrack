#include "resources/Resources.h"
#include "algorithms/container/container.set.h"
#include "algorithms/features/features.operate.h"
#include <Windows.h>
#include <fmt/format.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

using namespace tianli::algorithms::features_operate;
#include <psapi.h>

int main(int v, char* s[])
{
    features fts;
    std::shared_ptr<trackCache::CacheInfo> cache_info;
    if (load_cache(cache_info) == false)
        return -1;
    fts.keypoints = cache_info->key_points;
    fts.descriptors = cache_info->descriptors;
    std::cout << "features size:" << fts.size() << "\n";

    auto kps = fts.keypoints;
    auto kps_x_sorted = kps;
    std::sort(kps_x_sorted.begin(), kps_x_sorted.end(), [](const cv::KeyPoint& a, const cv::KeyPoint& b) { return a.pt.x < b.pt.x; });
    auto dis_min = 100;
    // 如果两个点的x坐标差值小于dis_min，并且y坐标差值小于dis_min，跳过y坐标小的点
    std::vector<cv::KeyPoint> kps_erase;
    for (auto it = kps_x_sorted.begin(); it != kps_x_sorted.end(); ++it)
    {
        auto it_next = it + 1;
        if (it_next == kps_x_sorted.end())
            break;
        if (std::abs(it->pt.x - it_next->pt.x) < dis_min && std::abs(it->pt.y - it_next->pt.y) < dis_min)
        {
            if (it->pt.y < it_next->pt.y)
                continue;
        }
        kps_erase.push_back(*it);
    }

    std::cout << "erase features size:" << kps_erase.size() << "\n";
}
