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
    inline void TransformAxes(const cv::Point2f &origin, const cv::Size2f& scale)
    {
        for (auto &keypoint : keypoints)
        {
            keypoint.pt = cv::Point2d(keypoint.pt - origin);
            keypoint.pt = cv::Point2d(keypoint.pt.x * scale.width, keypoint.pt.y * scale.height);
        }
    }

    //转换features的坐标系，使用两个Rect表示前后的坐标系
    inline void TransformAxes(const cv::Rect2d &inRect, const cv::Rect2d &outRect)
    {
        for (auto &keypoint : keypoints)
        {
            //输入>输出的缩放
            auto scale = cv::Point2d(outRect.width / inRect.width, outRect.height / inRect.height);
            //输入>输出的平移
            cv::Rect2d scaled_inRect = cv::Rect2d(inRect.x * scale.x, inRect.y * scale.y,
                inRect.width * scale.x, inRect.height * scale.y);
            auto translate = cv::Point2d(outRect.x - scaled_inRect.x, outRect.y - scaled_inRect.y);
            //坐标换算
            keypoint.pt = cv::Point2d(keypoint.pt.x * scale.x, keypoint.pt.y * scale.y) + translate;
        }
    }

    //转换features的坐标系，使用矩阵
    inline void TransformAxes(const cv::Mat &trans_mat)
    {
        for (auto &keypoint : keypoints)
        {
            cv::Mat pos_vec = (cv::Mat_<double>(3, 1) << keypoint.pt.x, keypoint.pt.y, 1);
            cv::Mat pos_vec_translated = trans_mat * pos_vec;
            keypoint.pt = cv::Point2d(pos_vec_translated.at<double>(0, 0) / pos_vec_translated.at<double>(2, 0), pos_vec_translated.at<double>(1, 0) / pos_vec_translated.at<double>(2, 0));
        }
    }
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
