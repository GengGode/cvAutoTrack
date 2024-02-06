#include <opencv2/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include "capture.h"

#include <frame/capture/capture.bitblt.h>
#include <frame/capture/capture.include.h>
#include <frame/frame.include.h>
#include <iostream>
#include <utils/convect.string.h>

#define GEN_FUNC(name)                          \
    bool func_test_##name(const cv::Mat frame); \
    void test_##name()                          \
    {                                           \
        test(func_test_##name, 30);             \
    }                                           \
    bool func_test_##name(const cv::Mat frame)
#define GEN_FUNC_LOCAL(name)                    \
    bool func_test_##name(const cv::Mat frame); \
    void test_##name()                          \
    {                                           \
        test_local(func_test_##name);           \
    }

#include <Windows.h>


GEN_FUNC(calc_angle)
{
    auto& giFrame = frame;
    int x = giFrame.cols, y = giFrame.rows;
    double f = 1, fx = 1, fy = 1;

    if (static_cast<double>(x) / static_cast<double>(y) == 16.0 / 9.0)
    {

        // 正常，不做处理
        if (x != 1920 && y != 1080)
        {
            cv::resize(giFrame, giFrame, cv::Size(1920, 1080));
        }
    }
    else if (static_cast<double>(x) / static_cast<double>(y) > 16.0 / 9.0)
    {

        // 高型，以宽为比例

        // x = (y * 16) / 9;
        f = y / 1080.0;
        // 将giFrame缩放到1920*1080的比例
        fx = x / f;
        // 将图片缩放
        cv::resize(giFrame, giFrame, cv::Size(static_cast<int>(fx), 1080));
    }
    else if (static_cast<double>(x) / static_cast<double>(y) < 16.0 / 9.0)
    {

        // 宽型，以高为比例

        // x = (y * 16) / 9;
        f = x / 1920.0;
        // 将giFrame缩放到1920*1080的比例
        fy = y / f;
        // 将图片缩放
        cv::resize(giFrame, giFrame, cv::Size(1920, static_cast<int>(fy)));
    }
    int MiniMap_Rect_x = 90;
    int MiniMap_Rect_y = 25;
    int MiniMap_Rect_w = 323;
    int MiniMap_Rect_h = 323;

    cv::Mat minimap_mat = giFrame(cv::Rect(MiniMap_Rect_x, MiniMap_Rect_y, MiniMap_Rect_w, MiniMap_Rect_h));
    cv::Mat rang;
    cv::warpPolar(minimap_mat.clone(), rang, cv::Size2i(720, 720), cv::Point2d(MiniMap_Rect_w / 2, MiniMap_Rect_h / 2), 360, cv::WARP_POLAR_LINEAR | cv::INTER_LINEAR);
    cv::rotate(rang.clone(), rang, cv::ROTATE_90_COUNTERCLOCKWISE);
    // 只取下方的部分
    rang = rang(cv::Rect(0, 720 - 256, 720, 200));

    cv::Mat alpha;
    cv::cvtColor(rang, alpha, cv::COLOR_RGBA2GRAY);

    cv::Mat a = alpha(cv::Rect(0, 10, 720, 100));
    // 向下平移一格然后相减
    cv::Mat b = alpha(cv::Rect(0, 0, 720, 100));
    cv::Mat c;
    cv::subtract(a, b, c);
    c = c * 2;

    // 滤波
    cv::Scharr(alpha, alpha, CV_8UC1, 0, 1);

    cv::Mat mat_rgb;
    cv::cvtColor(rang, mat_rgb, cv::COLOR_RGBA2RGB);
    std::vector<cv::Mat> rgba;
    cv::split(rang, rgba);

    cv::Mat mat_lab;
    cv::cvtColor(mat_rgb, mat_lab, cv::COLOR_RGB2Lab);
    std::vector<cv::Mat> lab;
    cv::split(mat_lab, lab);

    cv::Mat mat_hsv;
    cv::cvtColor(mat_rgb, mat_hsv, cv::COLOR_RGB2HSV);
    std::vector<cv::Mat> hsv;
    cv::split(mat_hsv, hsv);

    cv::Mat v = 255.0 - hsv[1];
    cv::Mat g = lab[0] - lab[1] - hsv[0] * 0.5;
    cv::Mat t = g & alpha;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

    cv::GaussianBlur(t, t, cv::Size(3, 3), 0, 0);
    cv::Mat ss;
    cv::Scharr(lab[0], ss, CV_8UC1, 0, 1);
    cv::morphologyEx(ss - 80, ss, cv::MORPH_CLOSE, kernel);

    t = t * 2 - ss;

    // 开运算
    cv::morphologyEx(t, t, cv::MORPH_OPEN, kernel);

    cv::Mat t2 = (t(cv::Rect(0, 40, 720, 100)) - (c)) * 10;

    cv::imshow("c", c);
    cv::imshow("t", t);
    cv::imshow("t2", t2);

    cv::waitKey(1);
    return true;
}

int main()
{
    test_calc_angle();

    return 0;
}