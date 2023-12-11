#pragma once
#include <map>
#include <regex>
#include <future>
#include <ranges>
#include <vector>
#include <numeric>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include "map.inface.h"

class MapResource : public MapImageInface
{
public:
    MapResource() = default;
    MapResource(const std::filesystem::path &path, std::string target_name, cv::Point map_origin, cv::Point origin_index)
        : map_origin(map_origin), origin_index(origin_index) {}
    ~MapResource() = default;

public:
    void set_origin_index(cv::Point origin_index)
    {
        origin_index = origin_index;
        origin = blocks[origin_index].rect.tl() + cv::Point(1024, 1024);
    }
    void set_map_origin(cv::Point map_origin) { map_origin = map_origin; }
    cv::Point get_abs_origin() { return abs_origin(); }
    cv::Rect get_min_rect() { return min_rect; }
    cv::Rect abs(const cv::Rect &r) { return r - min_rect.tl() + abs_origin(); }

public:
    void load(const std::filesystem::path &path, std::string target_name, cv::Point map_origin, cv::Point origin_index)
    {
        this->map_origin = map_origin;
        this->origin_index = origin_index;
        if (std::filesystem::exists(path) == false)
            return; // 文件夹不存在

        std::vector<std::pair<std::string, cv::Point>> target_files;
        for (auto &p : std::filesystem::directory_iterator(path))
        {
            auto result = parse_file_name(p.path().filename().string(), target_name);
            if (result == std::nullopt)
                continue;
            auto &[name, xy] = result.value();
            cv::Mat img = cv::imread((path / name).string());
            insert(size_normalize(img), xy);
        }
        if (blocks.find(origin_index) == blocks.end())
            return; // 原点图片不存在
        origin = blocks[origin_index].rect.tl() + cv::Point(1024, 1024);
    }

    void insert(const cv::Mat &image, const cv::Point &index)
    {
        blocks.insert({index, {index, image, cv::Rect((-index.y - 1) * 2048, (-index.x - 1) * 2048, 2048, 2048)}});
        min_rect = gen_bounding_rect();
    }

    // 获取地图图片
    cv::Mat view() { return view_abs(gen_bounding_rect()); }
    // 获取地图局部，以地图原点坐标系
    cv::Mat view(const cv::Rect &rect) { return view(rect, find_indexs(rect)); }
    // 获取地图局部，以图片左上角绝对坐标系
    cv::Mat view_abs(const cv::Rect &rect) { return view_abs(rect, find_indexs_abs(rect)); }

private:
    cv::Mat view(const cv::Rect &rect, const std::vector<cv::Point> index_s)
    {
        return view_abs(to_abs(rect), index_s);
    }

    cv::Mat view_abs(const cv::Rect &rect, const std::vector<cv::Point> index_s)
    {
        cv::Mat map = cv::Mat::zeros(rect.size(), CV_8UC3);
        for (auto &index : index_s)
            map = view_abs(rect, index, map);
        return map;
    }

    cv::Mat view_abs(const cv::Rect &rect, const cv::Point index, cv::Mat map = cv::Mat())
    {
        if (map.empty())
            map = cv::Mat::zeros(rect.size(), CV_8UC3);
        cv::Rect r = rect & blocks[index].rect;
        // 如果交集面积为0，直接返回
        if (r.area() == 0)
            return map;
        // 获取相对于地图图片的范围
        cv::Rect r1 = r - blocks[index].rect.tl();
        // 获取相对于区块图片的范围
        cv::Rect r2 = r - rect.tl();
        blocks[index].image(r1).copyTo(map(r2));
        return map;
    }

    cv::Mat view_async(const cv::Rect &rect, const std::vector<cv::Point> index_s)
    {
        cv::Mat map = cv::Mat::zeros(rect.size(), CV_8UC3);
        std::list<std::future<void>> futures;
        for (auto &index : index_s)
        {
            cv::Rect r = rect & blocks[index].rect;
            if (r.area() == 0)
                continue;
            cv::Rect r1 = r - blocks[index].rect.tl();
            cv::Rect r2 = r - rect.tl();
            futures.emplace_back(std::async(std::launch::async, [r1, r2, index, this, &map]
                                            { blocks[index].image(r1).copyTo(map(r2)); }));
        }
        for (auto &f : futures)
            f.get();
        return map;
    }

    // 图片大小归一化
    cv::Mat size_normalize(cv::Mat &img)
    {
        if (img.cols != 2048 || img.rows != 2048)
            // 为了防止边缘出现淡化现象，放大时使用 INTER_NEAREST 插值
            cv ::resize(img, img, cv::Size(2048, 2048), 0, 0, (img.cols > 512 || img.rows > 512) ? cv::INTER_NEAREST : cv::INTER_CUBIC);
        return img;
    }

    std::optional<std::pair<std::string, cv::Point>> parse_file_name(const std::string &name, const std::string &target_name)
    {
        std::regex reg("UI_" + target_name + "_(-?\\d+)_(-?\\d+).png");
        std::smatch result;
        if (std::regex_match(name, result, reg) == false)
            return std::nullopt;
        int x = std::stoi(result[1].str());
        int y = std::stoi(result[2].str());
        return std::make_pair(name, cv::Point(x, y));
    }

    cv::Rect gen_bounding_rect()
    {
        return std::accumulate(blocks.begin(), blocks.end(), cv::Rect(0, 0, 16, 16), [](const cv::Rect &rect, const auto &block)
                               { return rect | block.second.rect; });
    }

    std::vector<cv::Point> find_indexs(const cv::Rect &rect) { return find_indexs_abs(to_abs(rect)); }
    std::vector<cv::Point> find_indexs_abs(const cv::Rect &rect)
    {
        if (auto r = rect & min_rect; r.area() == 0)
            return {};
        // 直接遍历所有区块，获取存在交集的区块的索引
        std::vector<cv::Point> indexs;
        std::ranges::copy_if(blocks | std::views::keys, std::back_inserter(indexs), [&](const cv::Point &index)
                             { return (rect & blocks[index].rect).area() > 0; });
        return indexs;
    }

private:
    // 原点图片在vector中的索引
    cv::Point origin_index;
    // 图片原点位置，相对于地图图片左上角
    cv::Point origin;
    // 地图原点位置，相对于图片原点
    cv::Point map_origin;
    // 地图原点在图片左上角的绝对位置
    cv::Point abs_origin() { return origin + map_origin; }
    cv::Point to_abs(const cv::Point &p) { return p + abs_origin(); }
    cv::Rect to_abs(const cv::Rect &r) { return r + abs_origin(); }

private:
    struct Block
    {
        cv::Point index;
        cv::Mat image;
        cv::Rect rect;
    };
    std::map<cv::Point, Block> blocks;
    cv::Rect min_rect;
};
