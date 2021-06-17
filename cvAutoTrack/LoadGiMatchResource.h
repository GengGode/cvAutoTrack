#pragma once
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <vector>

// 此类是不导出的
class LoadGiMatchResource
{
public:
	LoadGiMatchResource(void);
	~LoadGiMatchResource(void);

public:
	cv::Mat PaimonTemplate;
	cv::Mat MapTemplate;
};

double dis(std::vector<cv::Point> &TransfornHistory);
double dis(cv::Point p);
cv::Point SPC(std::vector<double> lisx, double sumx, std::vector<double> lisy, double sumy);