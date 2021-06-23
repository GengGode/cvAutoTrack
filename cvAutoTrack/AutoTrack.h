#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <iostream>
#include <Windows.h>

#include "LoadGiMatchResource.h"

#define AUTO_TRACK_DEBUG_DELAY 1

// 此类是不导出的
class AutoTrack {
public:
	AutoTrack(void);
	~AutoTrack(void);

	bool init();
	bool GetTransform(float &x, float &y, float &a);
	bool GetUID(int &uid);
	int GetLastError();
	bool uninit();

private:
	LoadGiMatchResource giMatchResource;

private:
	int error_code = 0;

private:
	bool is_init_end = false;

private:
	int minHessian = 400;
	float ratio_thresh = 0.66f;


	float mapScale = 1.3f;//1.3;


	int someSizeR = 106;
	float MatchMatScale = 2.0;
private:
	//cv::Ptr<cv::xfeatures2d::SURF>
	void* _detectorAllMap = nullptr;
	//cv::Ptr<cv::xfeatures2d::SURF>
	void* _detectorSomeMap = nullptr;
	//std::vector<cv::KeyPoint>
	void* _KeyPointAllMap = nullptr;
	//std::vector<cv::KeyPoint>
	void* _KeyPointSomeMap = nullptr;
	//std::vector<cv::KeyPoint>
	void* _KeyPointMiniMap = nullptr;
	//cv::Mat
	void* _DataPointAllMap = nullptr;
	//cv::Mat
	void* _DataPointSomeMap = nullptr;
	//cv::Mat
	void* _DataPointMiniMap = nullptr;

private:
	bool isContinuity = false;
	cv::Point2f _TransformHistory[3];

private:
	HWND giHandle;
	RECT giRect;
	RECT giClientRect;
	cv::Size giClientSize;
	cv::Mat giFrame;
	cv::Mat giPaimonRef;
	cv::Mat giMiniMapRef;
	cv::Mat giUIDRef;
	cv::Mat giAvatarRef;

private:
	bool getGengshinImpactWnd();
	void getGengshinImpactRect();
	void getGengshinImpactScreen();
	void getPaimonRefMat();
	void getMiniMapRefMat();
	void getUIDRefMat();
	void getAvatarRefMat();

private:
	double screen_scale = 1;
	void getScreenScale();
	double getAvatarAngle();
};
