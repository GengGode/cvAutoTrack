#pragma once
#include "algorithms.include.h"
#include "utils/Utils.h"

struct rotation_calculation_config
{
	bool error = false;
	error_info err = { 0,"" };
	
};
void rotation_calculation(cv::Mat& giMiniMapRef, double&a, rotation_calculation_config& config)
{
	cv::Mat img_object(giMiniMapRef(cv::Rect(40, 40, giMiniMapRef.cols - 80, giMiniMapRef.rows - 80)));

	if (img_object.channels() != 4)
	{
		config.error = true;
		config.err = { 3003,"获取视角朝向时，原神小地图区域没有取到透明通道" };
		return;
	}

	//if (capture->mode == Capture::DirectX)
	//{
	//	config.error = true;
	//	config.err = { 3004,"DX模式下，原神小地图区域无法取到透明通道" };
	//	return;
	//}

	std::vector<cv::Mat>scr_channels;

	split(img_object, scr_channels);

	cv::Mat Alpha = scr_channels[3];

	Alpha = 255.0 - Alpha;

	Alpha = Alpha * 2;

	cv::threshold(Alpha, Alpha, 150, 0, cv::THRESH_TOZERO_INV);
	cv::threshold(Alpha, Alpha, 50, 0, cv::THRESH_TOZERO);
	cv::threshold(Alpha, Alpha, 50, 255, cv::THRESH_BINARY);

	cv::Point center = cv::Point(Alpha.cols / 2, Alpha.rows / 2);
	double min_radius = center.x < center.y ? center.x : center.y;

	cv::circle(Alpha, center, static_cast<int>(min_radius * 1.21), cv::Scalar(0, 0, 0), static_cast<int>(min_radius * 0.42));
	cv::circle(Alpha, center, static_cast<int>(min_radius * 0.3), cv::Scalar(0, 0, 0), -1);

	cv::Mat dilate_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));
	cv::dilate(Alpha, Alpha, dilate_element);
	cv::Mat erode_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));
	cv::erode(Alpha, Alpha, erode_element);

	erode_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));
	cv::erode(Alpha, Alpha, erode_element);
	dilate_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));
	cv::dilate(Alpha, Alpha, dilate_element);

	//传入黑白图
	//根据白块部分计算视角中心坐标
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarcy;

	cv::findContours(Alpha, contours, hierarcy, 0, 1);

	std::vector<cv::Rect> boundRect(contours.size());  //定义外接矩形集合

	if (contours.size() == 0)
	{
		config.error = true;
		config.err = { 3005 ,"获取视角朝向时，没有提取出视角扇形区域" };
		return;
	}

	cv::Point p;
	int maxBlack = 0;
	int maxId = 0;

	for (int i = 0; i < contours.size(); i++)
	{
		if (contours[i].size() > maxBlack)
		{
			maxBlack = static_cast<int>(contours[i].size());
			maxId = i;
		}
		boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));

	}

	p = cv::Point(boundRect[maxId].x + boundRect[maxId].width / 2, boundRect[maxId].y + boundRect[maxId].height / 2);

	// double res;
#ifdef _DEBUG
	circle(Alpha, p, 3, cv::Scalar(255, 0, 0));
	line(Alpha, p, cv::Point(img_object.cols / 2, img_object.rows / 2), cv::Scalar(0, 255, 0));
	cv::imshow("Img", Alpha);
#endif
	p = p - cv::Point(img_object.cols / 2, img_object.rows / 2);

	a = TianLi::Utils::Line2Angle(p);
}