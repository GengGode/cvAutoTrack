#pragma once
#include "algorithms.include.h"
#include "utils/Utils.h"

struct rotation_calculation_config
{
	bool error = false;
	error_info err = { 0,"" };
	
};

double lastRotate = 0;

void rotation_calculation(cv::Mat& giMiniMapRef, double& a, rotation_calculation_config& config);			//算法1，基于Alpha通道，高精度
void rotation_calculation_2nd(cv::Mat& giMiniMapRef, double& a, rotation_calculation_config& config);	//算法2，基于屏幕空间，低精度

void rotation_calculation(cv::Mat& giMiniMapRef, double&a, rotation_calculation_config& config)
{
	if (giMiniMapRef.channels() != 4)
		return rotation_calculation_2nd(giMiniMapRef, a, config);
	//取Alpha通道
	std::vector<cv::Mat>scr_channels;
	split(giMiniMapRef, scr_channels);
	cv::Mat alpha = scr_channels[3];

	double minVal;
	cv::minMaxIdx(alpha, &minVal, NULL, NULL, NULL);
	if(minVal == 255)		//没有透明通道
		return rotation_calculation_2nd(giMiniMapRef, a, config);

	cv::Point2i center(giMiniMapRef.size[1] / 2, giMiniMapRef.size[0] / 2);
	cv::Size2i miniMap_size = giMiniMapRef.size();
	//极坐标
	cv::warpPolar(alpha.clone(), alpha, cv::Size2i(512, 512), center, 360, cv::WARP_POLAR_LINEAR | cv::INTER_LINEAR);
	alpha = alpha(cv::Rect2d(
		miniMap_size.width * 0.1, 0,
		miniMap_size.width * 0.3, alpha.size[0]));
	cv::rotate(alpha.clone(), alpha, cv::ROTATE_90_COUNTERCLOCKWISE);
	//滤波
	cv::Scharr(alpha.clone(), alpha, CV_32FC1, 1, 0);

	//列求和&平滑
	cv::Mat sumOfImg;
	cv::reduce(alpha, sumOfImg, 0, cv::REDUCE_SUM, CV_32F);
	sumOfImg = cv::repeat(sumOfImg, 1, 2);
	cv::blur(sumOfImg.clone(), sumOfImg, cv::Size2i(5, 1), cv::Point2i(-1, -1), cv::BORDER_REPLICATE);
	sumOfImg = sumOfImg(cv::Rect2i(256, 0, 512, 1));

	//求角度
	float angles[2];
	cv::Point2i maxLoc,minLoc;
	cv::minMaxLoc(sumOfImg, NULL, NULL, &minLoc, &maxLoc);
	angles[0] = std::fmod((minLoc.x / 512.0) * 360, 360);
	angles[1] = std::fmod((maxLoc.x / 512.0) * 360, 360);

	if (angles[1] < angles[0])
		angles[1] += 360;



	if ((std::abs(angles[1] - angles[0] - 90) > 15))
		return rotation_calculation_2nd(giMiniMapRef, a, config);

	a =  -((angles[0] + angles[1]) / 2) + 90;
	if (a < -180) a += 360;
	lastRotate = a;
}

cv::Mat cvMatRoll(const cv::Mat& src,cv::Point2d shift)
{
	cv::Mat transFormMat = cv::Mat::zeros(2, 3, CV_32FC1);
	cv::Mat dst;
	transFormMat.at<float>(0, 0) = 1;
	transFormMat.at<float>(1, 1) = 1;
	transFormMat.at<float>(0, 2) = shift.x;
	transFormMat.at<float>(1, 2) = shift.y;
	cv::warpAffine(src, dst, transFormMat, src.size(), cv::INTER_NEAREST, cv::BORDER_WRAP);
	return dst;
}

void rotation_calculation_2nd(cv::Mat& giMiniMapRef, double& a, rotation_calculation_config& config)
{
	cv::Point2i center(giMiniMapRef.size[1] / 2, giMiniMapRef.size[0] / 2);
	cv::Size2i miniMap_size = giMiniMapRef.size();

	//极坐标
	cv::Mat img_object;
	cv::warpPolar(giMiniMapRef, img_object, cv::Size2i(512, 512), center, 360, cv::WARP_POLAR_LINEAR | cv::INTER_LINEAR);

	img_object = img_object(cv::Rect2d(
		miniMap_size.width * 0.1, 0,
		miniMap_size.width * 0.3, img_object.size[0]));
	cv::rotate(img_object.clone(), img_object, cv::ROTATE_90_COUNTERCLOCKWISE);

	//边缘提取
	img_object.convertTo(img_object, CV_32F);
	img_object /= 255;
	float alpha = 0.25;

	cv::Mat img_edge_arr[2] = {
		cv::abs(img_object * (1 - alpha) + alpha - cvMatRoll(img_object,cv::Point2d(-5,0))),
		cv::abs(img_object * (1 - alpha) + alpha - cvMatRoll(img_object,cv::Point2d(5,0)))
	};

	float angles[2];
	for (int i = 0; i < 2; i++)
	{
		//提取边缘信息
		cv::pow(img_edge_arr[i].clone(), 2, img_edge_arr[i]);
		img_edge_arr[i] = (img_edge_arr[i] < 0.001);
		std::vector<cv::Mat> bgr_planes;
		cv::split(img_edge_arr[i], bgr_planes);
		img_edge_arr[i] = bgr_planes[0] & bgr_planes[1] & bgr_planes[2];

		//形态学滤波
		cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Point(1, 3));
		cv::morphologyEx(img_edge_arr[i].clone(), img_edge_arr[i], cv::MORPH_OPEN, kernel);
		//列求和&平滑
		cv::Mat sumOfImg;
		cv::reduce(img_edge_arr[i], sumOfImg, 0, cv::REDUCE_SUM,CV_32F);
		sumOfImg = cv::repeat(sumOfImg, 1, 2);
		cv::blur(sumOfImg.clone(), sumOfImg, cv::Size2i(5, 1), cv::Point2i(-1, -1), cv::BORDER_REPLICATE);
		sumOfImg = sumOfImg(cv::Rect2i(256, 0, 512, 1));
		//求峰值
		cv::Point2i maxLoc;
		cv::minMaxLoc(sumOfImg, NULL, NULL, NULL, &maxLoc);
		angles[i] = std::fmod((maxLoc.x / 512.0) * 360, 360);
	}

	if (angles[1] < angles[0])
		angles[1] += 360;

	if ((std::abs(angles[1] - angles[0] - 90) > 20))
	{
		config.error = true;
		config.err = { 3006,"获取视角的误差过大" };
		a = lastRotate;
		return;
	}

	a = -((angles[0] + angles[1]) / 2) + 90;
	if (a < -180) a += 360;
	lastRotate = a;
	return;
}