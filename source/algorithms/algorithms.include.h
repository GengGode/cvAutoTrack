#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>

struct features
{
    std::vector<size_t> indexs;
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    features() = default;
    features(const std::vector<cv::KeyPoint>& keypoints, const cv::Mat& descriptors) : keypoints(keypoints), descriptors(descriptors) {}
    features(const std::vector<cv::KeyPoint>& keypoints, const cv::Mat& descriptors, const std::vector<size_t>& indexs) : keypoints(keypoints), descriptors(descriptors), indexs(indexs) {}
    ~features() = default;

    size_t group_count() const { return indexs.size(); }
    size_t size() const { return keypoints.size(); }
    size_t rows() const { return descriptors.rows; }
    size_t cols() const { return descriptors.cols; }
    bool empty() const { return keypoints.empty(); }

    //转换features的坐标系，先平移，再缩放
    void TransferAxes(const cv::Point2f &origin, const double scale);

    //转换features的坐标系，使用两个Rect表示前后的坐标系
    void TransferAxes(const cv::Rect2d &inRect, const cv::Rect2d& outRect);

    //转换features的坐标系，使用矩阵
    void TransformAxes(const cv::Mat& trans_mat);
};

class point_index
{
public:
    point_index() = default;
    point_index(const cv::Point2d& pos) : _point(pos) {}
    ~point_index() = default;
    virtual cv::Point2d point() { return _point; }

private:
    cv::Point2d _point;
};

class keypoint_index : public point_index
{
public:
    keypoint_index(std::shared_ptr<cv::KeyPoint> keypoint, size_t index) : point_index(keypoint->pt), kp(keypoint), index(index) {}
    keypoint_index(cv::KeyPoint keypoint, size_t index) : point_index(keypoint.pt), kp(std::make_shared<cv::KeyPoint>(keypoint)), index(index) {}
    ~keypoint_index() = default;
    cv::Point2d point() override { return kp->pt; }
    std::shared_ptr<cv::KeyPoint> kp;
    size_t index = 0;
};

/// @brief 用来物品项集合的构造和查找接口
class set
{
public:
    set() = default;
    virtual ~set() = default;

public:
    /// @brief 根据范围查找物品项
    /// @param rect 范围
    /// @return std::vector<std::shared_ptr<ItemInface>> 物品项集合
    virtual std::vector<std::shared_ptr<point_index>> find(const cv::Rect2d& rect) = 0;

public:
    /// @brief 判断物品项集合是否为空
    /// @return
    virtual bool empty() { return true; }
};
