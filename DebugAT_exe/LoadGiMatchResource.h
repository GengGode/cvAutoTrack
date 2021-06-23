#pragma once
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <vector>

#include "resource.h"

// 此类是不导出的
class LoadGiMatchResource
{
public:
	LoadGiMatchResource(void);
	~LoadGiMatchResource(void);

public:
	cv::Mat PaimonTemplate;
	cv::Mat MapTemplate;
	cv::Mat AvatarTemplate;
	cv::Mat UID;
	cv::Mat UIDnumber[10];

private:
	void UIDnumberRGBA2A();
};

double dis(std::vector<cv::Point> &TransfornHistory);
double dis(cv::Point p);
cv::Point SPC(std::vector<double> lisx, double sumx, std::vector<double> lisy, double sumy);
int getMaxID(double lis[], int len);
int getMinID(double lis[], int len);

std::vector<cv::Point2f> Vector2UnitVector(std::vector<cv::Point2f> pLis);
double Line2Angle(cv::Point2f p);
