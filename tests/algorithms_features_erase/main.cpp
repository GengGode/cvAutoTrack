#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <iostream>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <fmt/format.h>
#include "algorithms/algorithms.features.h"
#include "algorithms/container/container.set.h"
#include "resources/Resources.h"

using namespace tianli::algorithms::features;
#include <psapi.h>


int main(int v, char *s[])
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
    std::sort(kps_x_sorted.begin(), kps_x_sorted.end(), [](const cv::KeyPoint &a, const cv::KeyPoint &b) {
        return a.pt.x < b.pt.x;
    });
    auto dis_min = 1.2;
    // 如果两个点的x坐标差值小于dis_min，并且y坐标差值小于dis_min，则删除y坐标较小的点
    for (auto it = kps_x_sorted.begin(); it != kps_x_sorted.end(); ++it)
    {
        auto it_next = it + 1;
        if (it_next == kps_x_sorted.end())
            break;
        if (std::abs(it->pt.x - it_next->pt.x) < dis_min && std::abs(it->pt.y - it_next->pt.y) < dis_min)
        {
            if (it->pt.y < it_next->pt.y)
                kps.erase(it);
            else
                kps.erase(it_next);
        }
    }

    std::cout << "erase features size:" << kps_x_sorted.size() << "\n";
}
