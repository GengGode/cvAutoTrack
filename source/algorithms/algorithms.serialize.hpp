#pragma once
#include <opencv2/core.hpp>
// cereal
#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>
namespace cereal
{
    // cv::Rect2i
    template <class Archive> void serialize(Archive& ar, cv::Rect2i& rect)
    {
        ar(rect.x, rect.y, rect.width, rect.height);
    }
    // cv::Rect2f
    template <class Archive> void serialize(Archive& ar, cv::Rect2f& rect)
    {
        ar(rect.x, rect.y, rect.width, rect.height);
    }
    // cv::Rect2d
    template <class Archive> void serialize(Archive& ar, cv::Rect2d& rect)
    {
        ar(rect.x, rect.y, rect.width, rect.height);
    }
    // cv::Point2i
    template <class Archive> void serialize(Archive& ar, cv::Point2i& point)
    {
        ar(point.x, point.y);
    }
    // cv::Point2f
    template <class Archive> void serialize(Archive& ar, cv::Point2f& point)
    {
        ar(point.x, point.y);
    }
    // cv::Point2d
    template <class Archive> void serialize(Archive& ar, cv::Point2d& point)
    {
        ar(point.x, point.y);
    }
    // cv::KeyPoint
    template <class Archive> void serialize(Archive& ar, cv::KeyPoint& keypoint)
    {
        ar(keypoint.angle, keypoint.class_id, keypoint.octave, keypoint.pt, keypoint.response, keypoint.size);
    }
    // cv::Mat
    template <class Archive> void save(Archive& ar, const cv::Mat& mat)
    {
        int rows = mat.rows;
        int cols = mat.cols;
        int type = mat.type();
        ar(rows, cols, type);
        size_t dataSize = mat.total() * mat.elemSize();
        ar(cereal::binary_data(mat.ptr(), dataSize));
    }
    template <class Archive> void load(Archive& ar, cv::Mat& mat)
    {
        int rows, cols, type;
        ar(rows, cols, type);
        mat.create(rows, cols, type);
        size_t dataSize = mat.total() * mat.elemSize();
        ar(cereal::binary_data(mat.ptr(), dataSize));
    }
} // namespace cereal