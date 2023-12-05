#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <iostream>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "resources/binary/resources.binary.h"
#include <fmt/format.h>
#include "algorithms/algorithms.features.h"
#include "resources/Resources.h"

using namespace tianli::algorithms::features;

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
        max_width = (std::max)(max_width, (int)kp.pt.x);
        max_height = (std::max)(max_height, (int)kp.pt.y); });

    cv::Mat mat;
    drawKeypoints(cv::Mat(max_height, max_width, CV_8UC3, cv::Scalar(128, 128, 128)), fts.keypoints, mat, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    cv::imwrite(fmt::format("out_key_.png"), mat);

    
}