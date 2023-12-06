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


double test_time(int max_width, int max_height, std::vector<std::shared_ptr<point_index>> key_point_objects)
{
    int test_cycle = 100;
    int test_count = 0;

    auto start = std::chrono::high_resolution_clock::now();

    int mem_used_before = 0;
    int mem_used_after = 0;

    PROCESS_MEMORY_COUNTERS pmc;
    auto mem_info = GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    if (mem_info)
    {
        mem_used_before = pmc.WorkingSetSize;
    }

    for (int i = 0; i < test_cycle; i++)
    {
        test_count++;
        auto item_set = tianli::algorithms::container::tree(key_point_objects);

        mem_info = GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
        if (mem_info)
        {
            mem_used_after = pmc.WorkingSetSize;
        }

        double mem_used = (mem_used_after - mem_used_before) / 1024.0 / 1024.0;
        std::cout << "test_count:" << test_count << " mem_used:" << mem_used << std::endl;
    }

    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000000.0 / 100.0;
}
int main(int v, char *s[])
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

    std::vector<std::shared_ptr<point_index>> key_point_objects;
    for (auto &kp : cache_info->key_points)
    {
        key_point_objects.push_back(std::make_shared<keypoint_index>(kp, 1));
    }

    auto item_set = tianli::algorithms::container::tree(key_point_objects);
    item_set.print();

    auto r = test_time(max_width, max_height, key_point_objects);
    std::cout << "time:" << r << " ms\n";
}
