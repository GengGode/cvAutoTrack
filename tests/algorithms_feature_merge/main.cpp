#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <iostream>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "resources/binary/resources.binary.h"
#include <fmt/format.h>
#include "algorithms/algorithms.features.h"

using namespace tianli::algorithms::features;

cv::Mat get(int i)
{
    using namespace TianLi::Resources::Binary;
    auto image_name = get_image_name(i);
    auto image_binary = get_image_binary(image_name);

    std::vector<uchar> image_binary_vector(image_binary.data, image_binary.data + image_binary.size);
    auto image = cv::imdecode(image_binary_vector, cv::IMREAD_UNCHANGED);
    return image;
}

cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create();

features gen(int i)
{
    features fts = features::from_image(detector, get(i));
    cv::Mat mat;
    drawKeypoints(get(i), fts.keypoints, mat, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    cv::imwrite(fmt::format("org_key_{}.png", i), mat);
    return fts;
}

int main()
{
    auto f1 = gen(0);
    auto f2 = gen(1);
    auto f3 = gen(2);
    auto f4 = gen(3);
    std::cout << "features 0 size:" << f1.size() << "\n";
    std::cout << "features 1 size:" << f2.size() << "\n";
    std::cout << "features 2 size:" << f3.size() << "\n";
    std::cout << "features 3 size:" << f4.size() << "\n";

    auto fts = merge(f1, f2);
    fts = merge(fts, f3);
    fts = merge(fts, f4);
    std::cout << fts.size() << "\n";
    cv::Mat mat;
    drawKeypoints(get(1), fts.keypoints, mat, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    cv::imwrite(fmt::format("out_key_.png"), mat);
    auto fts_s = split(fts);
    for (int i = 0; i < fts_s.size(); i++)
    {
        cv::Mat mat;
        drawKeypoints(get(i), fts_s[i].keypoints, mat, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        cv::imwrite(fmt::format("out_key_{}.png", i), mat);
    }
}