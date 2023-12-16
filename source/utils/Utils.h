#pragma once
#include <random>

#define NOMINMAX
#include <windows.h>


namespace TianLi::Utils
{
    struct MatchKeyPoint
    {
        cv::Point2d query;
        cv::Point2d train;
    };

    //从大图中裁剪小图，并保证小图不超出大图范围
    //@param map大图
    //@param map小图左上角坐标
    //@param 小图的尺寸
    cv::Mat get_some_map(const cv::Mat& map, const cv::Point& pos, int size_r);

    //清洗并计算坐标
    bool SPC(std::vector<double> lisx, std::vector<double> lisy, cv::Point2d& out);

    //向量运算封装
    double dis(cv::Point2d p);
    double stdev(std::vector<double> list);
    double stdev(std::vector<cv::Point2d> list);
    double stdev_abs(std::vector<double> list);

    //按照百分比去除边框
    cv::Mat crop_border(const cv::Mat& mat, double border);
    //按照标准差剔除列表的异常值
    std::vector<double> extract_valid(std::vector<double> list);
    //按照标准差剔除列表的异常点
    std::vector<cv::Point2d> extract_valid(std::vector<cv::Point2d> list);

    //消除特征点的漂移，有利于获取到特征点的统计特征【待重写】
    //@param keypoints 特征点对
    //@param scale 小地图缩放
    //@param x_list 横坐标
    //@param y_list 纵坐标
    void RemoveKeypointOffset(std::vector<MatchKeyPoint> keypoints, double scale, std::vector<double>& x_list, std::vector<double>& y_list);

    //获取列表的最大值
    int getMaxID(double lis[], int len);
    //获取列表的最小值
    int getMinID(double lis[], int len);
    //将向量转换为规格化向量
    std::vector<cv::Point2f> Vector2UnitVector(std::vector<cv::Point2f> pLis);
    //将向量转换为方向
    double Line2Angle(cv::Point2f p);
    //正向变换坐标
    //@param pos 输入坐标
    //@param origin 目标坐标的原点
    //@param scale 目标坐标的缩放
    cv::Point2d TransferAxes(cv::Point2d pos, cv::Point2d origin, double scale);

    //输入两个矩形，变换坐标
    //@param pos 输入坐标
    //@param inRect 原始坐标的矩形区域
    //@param outRect 新坐标的矩形区域
    //@return 变换后的坐标
    cv::Point2d TransferAxes(cv::Point2d pos, cv::Rect2d inRect, cv::Rect2d outRect);

    //反向变换坐标
    //@param pos 输入坐标
    //@param origin 原始坐标的原点
    //@param scale 原始坐标的缩放
    cv::Point2d TransferAxes_inv(cv::Point2d pos, cv::Point2d origin, double scale);

    //变换特殊地区的坐标
    //！！！即将弃用
    std::pair<cv::Point2d, int> ConvertSpecialMapsPosition(double x, double y);

    bool getRegValue_REG_SZ(HKEY root, std::wstring item, std::wstring key, std::string& ret, int max_length);

    bool getRegValue_DWORD(HKEY root, std::wstring item, std::wstring key, int& ret);

    // copy from openvslam
    std::mt19937 create_random_engine();
    std::vector<unsigned int> create_random_array(const size_t size, const unsigned int rand_min, const unsigned int rand_max);
    void normalize(const std::vector<cv::KeyPoint>& keypts, std::vector<cv::Point2d>& normalized_pts, cv::Mat& transform);

    template<typename T>
    static cv::Mat to_homogeneous(T& pt) {
        return (cv::Mat_<double>(3, 1) << pt.x, pt.y, 1.0);
    }

}