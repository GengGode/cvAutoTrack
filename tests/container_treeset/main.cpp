#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <iostream>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "resources/binary/resources.binary.h"
#include <fmt/format.h>
#include "algorithms/algorithms.features.h"
#include "resources/Resources.h"
#include "tianli.realsky/tianli.core/core.map/item.set.h"
using namespace tianli::algorithms::features;

double test_time(int max_width, int max_height, std::vector<std::shared_ptr<ItemInface>> key_point_objects)
{
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 100; i++)
    {
        auto item_set = MapSet(cv::Rect2d(0, 0, max_width, max_height), key_point_objects);
    }

    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000000.0 / 100.0;
}
int main()
{
    features fts;
    std::shared_ptr<trackCache::CacheInfo> cache_info;
    if (load_cache(cache_info) == false)
        return 0;
    fts.keypoints = cache_info->key_points;
    fts.descriptors = cache_info->descriptors;
    std::cout << "features size:" << fts.size() << "\n";

    int max_width = 0;
    int max_height = 0;
    std::for_each(fts.keypoints.begin(), fts.keypoints.end(), [&](auto &kp)
                  {
                    kp.size = 100;
                    kp.octave = 10;
        max_width = (std::max)(max_width, (int)kp.pt.x);
        max_height = (std::max)(max_height, (int)kp.pt.y); });

    cv::Mat mat = cv::Mat(max_height, max_width, CV_8UC3, cv::Scalar(128, 128, 128));
    drawKeypoints(mat, fts.keypoints, mat, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    cv::imwrite(fmt::format("out_key_.png"), mat);

    std::vector<std::shared_ptr<ItemInface>> key_point_objects;
    for (auto &kp : cache_info->key_points)
    {
        key_point_objects.push_back(std::make_shared<KeyPointObject>(kp,1));
    }

    auto item_set = MapSet(cv::Rect2d(0, 0, max_width, max_height), key_point_objects);
    item_set.print();

    auto r =test_time(max_width, max_height, key_point_objects);
    std::cout << "time:" << r << " ms\n";
}
