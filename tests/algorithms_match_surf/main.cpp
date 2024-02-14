#include "resources/Resources.h"
#include "algorithms/container/container.set.h"
#include "algorithms/features/features.operate.h"
#include "algorithms/match/match.surf.h"
#include <fmt/format.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include <Windows.h>

using namespace tianli::algorithms::features_operate;

int main(int v, char* s[])
{
    features fts;
    std::shared_ptr<trackCache::CacheInfo> cache_info;
    if (load_cache("./cvAutoTrack_Cache.dat", cache_info) == false)
        return 0;
    fts.keypoints = cache_info->key_points;
    fts.descriptors = cache_info->descriptors;
    std::cout << "features size:" << fts.size() << "\n";

    int max_width = 0;
    int max_height = 0;
    std::for_each(fts.keypoints.begin(), fts.keypoints.end(), [&](auto& kp) {
        kp.size = 100;
        kp.octave = 10;
        max_width = (std::max)(max_width, (int)kp.pt.x);
        max_height = (std::max)(max_height, (int)kp.pt.y);
    });

    cv::Mat mat = cv::Mat(max_height, max_width, CV_8UC3, cv::Scalar(128, 128, 128));
    drawKeypoints(mat, fts.keypoints, mat, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    cv::imwrite(fmt::format("out_key_.png"), mat);

    std::vector<std::shared_ptr<point_index>> key_point_objects;
    for (auto& kp : cache_info->key_points)
    {
        key_point_objects.push_back(std::make_shared<keypoint_index>(kp, 1));
    }

    auto item_set = tianli::algorithms::container::tree(key_point_objects);
    item_set.print();

    auto some_key_points = item_set.find(cv::Rect(0, 0, 100, 100));
    std::cout << "some_key_points size:" << some_key_points.size() << "\n";
    auto some_fts = join(fts, some_key_points);
    std::cout << "some_fts size:" << some_fts.size() << "\n";

    auto true_r = tianli::algorithms::match::match_features(some_fts, some_fts);
    std::cout << "match size:" << true_r.size() << "\n";
    auto r = tianli::algorithms::match::match_features(fts, some_fts);
    std::cout << "match size:" << r.size() << "\n";
}
