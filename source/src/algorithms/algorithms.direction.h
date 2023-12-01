#pragma once
#include "algorithms.include.h"

struct direction_calculation_config
{
	bool error = false;
	error_info err = { 0,"" };
};

double last_direction = 0;

void direction_calculation(cv::Mat& giAvatarRef, double& a, direction_calculation_config& config)
{
	cv::resize(giAvatarRef, giAvatarRef, cv::Size(), 2, 2);
	std::vector<cv::Mat> lis;
	cv::split(giAvatarRef, lis);

	cv::Mat gray0;
	cv::Mat gray1;
	cv::Mat gray2;

	cv::threshold(lis[0], gray0, 240, 255, cv::THRESH_BINARY);
	cv::threshold(lis[1], gray1, 212, 255, cv::THRESH_BINARY);
	cv::threshold(lis[2], gray2, 25, 255, cv::THRESH_BINARY_INV);

	cv::Mat and12;
	cv::bitwise_and(gray1, gray2, and12, gray0);
	cv::resize(and12, and12, cv::Size(), 2, 2, 3);
	cv::Canny(and12, and12, 20, 3 * 20, 3);
	cv::circle(and12, cv::Point(cvCeil(and12.cols / 2), cvCeil(and12.rows / 2)), cvCeil(and12.cols / 4.5), cv::Scalar(0, 0, 0), -1);
	cv::Mat dilate_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::dilate(and12, and12, dilate_element);
	cv::Mat erode_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::erode(and12, and12, erode_element);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarcy;
	cv::Point2d center = cv::Point2d(and12.cols / 2.0, and12.rows / 2.0);

	cv::findContours(and12, contours, hierarcy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	std::vector<cv::Rect> boundRect(contours.size());  //定义外接矩形集合
	cv::Point2f rect[4];

	std::vector<cv::Point2d> AvatarKeyPoint;
	double AvatarKeyPointLine[3] = { 0 };
	std::vector<cv::Point2f> AvatarKeyLine;
	cv::Point2f KeyLine;

	if (contours.size() == 3)
	{
		for (int i = 0; i < 3; i++)
		{
			boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));
			AvatarKeyPoint.emplace_back(cv::Point2d(boundRect[i].x + boundRect[i].width / 2.0, boundRect[i].y + boundRect[i].height / 2.0));
		}

		AvatarKeyPointLine[0] = TianLi::Utils::dis(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
		AvatarKeyPointLine[1] = TianLi::Utils::dis(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
		AvatarKeyPointLine[2] = TianLi::Utils::dis(AvatarKeyPoint[1] - AvatarKeyPoint[0]);

		if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[2] && AvatarKeyPointLine[1] >= AvatarKeyPointLine[2])
		{
			AvatarKeyLine.emplace_back(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
			AvatarKeyLine.emplace_back(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
		}
		if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[1] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[1])
		{
			AvatarKeyLine.emplace_back(AvatarKeyPoint[1] - AvatarKeyPoint[0]);
			AvatarKeyLine.emplace_back(AvatarKeyPoint[1] - AvatarKeyPoint[2]);
		}
		if (AvatarKeyPointLine[1] >= AvatarKeyPointLine[0] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[0])
		{
			AvatarKeyLine.emplace_back(AvatarKeyPoint[0] - AvatarKeyPoint[1]);
			AvatarKeyLine.emplace_back(AvatarKeyPoint[0] - AvatarKeyPoint[2]);
		}

		AvatarKeyLine = TianLi::Utils::Vector2UnitVector(AvatarKeyLine);
		KeyLine = AvatarKeyLine[0] + AvatarKeyLine[1];

		a = TianLi::Utils::Line2Angle(KeyLine);
	}
	else if (contours.size() == 2)
	{
		for (int i = 0; i < 2; i++)
		{
			boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));
			AvatarKeyPoint.emplace_back(cv::Point2d(boundRect[i].x + boundRect[i].width / 2.0, boundRect[i].y + boundRect[i].height / 2.0));
		}
		// 两点取中点
		AvatarKeyPoint.emplace_back((AvatarKeyPoint[0] + AvatarKeyPoint[1]) / 2.0);
		// 中点基于图片中心对称位置
		cv::Point2d line = center - AvatarKeyPoint[2];
		a = TianLi::Utils::Line2Angle(line);
	}
	else
	{
		config.error = true;
		config.err = { 9,"提取小箭头特征误差过大" };
		a = last_direction;
		return;
	}
	last_direction = a;
}