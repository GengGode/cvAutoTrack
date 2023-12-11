#pragma once
#include <memory>
#include <vector>
#include <opencv2/opencv.hpp>

// cv::Point less define
namespace std
{
    template <>
    struct less<cv::Point>
    {
        bool operator()(const cv::Point &a, const cv::Point &b) const
        {
            return a.x < b.x || (a.x == b.x && a.y < b.y);
        }
    };
    template <>
    struct less<cv::Size>
    {
        bool operator()(const cv::Size &a, const cv::Size &b) const
        {
            return a.width < b.width || (a.width == b.width && a.height < b.height);
        }
    };
    template <>
    struct less<cv::Rect>
    {
        bool operator()(const cv::Rect &a, const cv::Rect &b) const
        {
            return a.x < b.x || (a.x == b.x && a.y < b.y) || (a.x == b.x && a.y == b.y && a.width < b.width) || (a.x == b.x && a.y == b.y && a.width == b.width && a.height < b.height);
        }
    };
} // namespace std

/// @brief 用来存储物品项的详细信息接口
class ItemInface
{
public:
    ItemInface() = default;
    ItemInface(const cv::Point2d &pos) : pos_(pos) {}
    ~ItemInface() = default;
    virtual cv::Point2d pos() { return pos_; }

private:
    cv::Point2d pos_;
};

struct MapSprite
{
    cv::Point2d pos;
    double scale;
    cv::Size size;
};

/// @brief 用来物品项集合的构造和查找接口
class ItemSetInface
{
public:
    ItemSetInface() = default;
    virtual ~ItemSetInface() = default;

public:
    /// @brief 根据范围查找物品项
    /// @param rect 范围
    /// @return std::vector<std::shared_ptr<ItemInface>> 物品项集合
    virtual std::vector<std::shared_ptr<ItemInface>> find(const cv::Rect2d &rect) = 0;

public:
    /// @brief 判断物品项集合是否为空
    /// @return
    virtual bool empty() { return true; }
};

class MapImageInface
{
public:
    MapImageInface() = default;
    virtual ~MapImageInface() = default;

public:
    virtual cv::Mat view(const cv::Rect &rect) = 0;
    // virtual cv::Mat view(const MapSprite &sprite) = 0;
};
