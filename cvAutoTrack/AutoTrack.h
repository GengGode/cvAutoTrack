#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <iostream>
#include <Windows.h>

#include "LoadGiMatchResource.h"
#include "FlowWork.h"
#include "ErrorCode.h"

#define AUTO_TRACK_DEBUG_DELAY 1

// 此类是不导出的
class AutoTrack {
public:
	AutoTrack(void);
	~AutoTrack(void);

	bool init();
	bool SetHandle(long long int handle);
	bool SetWorldCenter(double x, double y);
	bool SetWorldScale(double scale);
	bool GetTransform(float &x, float &y, float &a);
	bool GetPosition(double &x, double &y);
	bool GetDirection(double &a);
	bool GetUID(int &uid);
	bool GetInfoLoadPicture(char* path, int &uid, double &x, double &y, double &a);
	int GetLastError();
	const char* GetLastErrorStr();
	bool uninit();

private:
	LoadGiMatchResource giMatchResource;
	ErrorCode& err = ErrorCode::getInstance();
	FlowWork wPaimon;
	FlowWork wMiniMap;
	FlowWork wAvatar;

	FlowWork wUID;

	FlowWork wForAfter;

private:
	int error_code = 0;

private:
	bool is_init_end = false;

private:
	int minHessian = 400;
	float ratio_thresh = 0.66f;
	float mapScale = 1.68f;//1.3;
	int someSizeR = 106;
	float MatchMatScale = 2.0;

	double screen_scale = 1;

	double UserWorldOrigin_X = 0;
	double UserWorldOrigin_Y = 0;
	double UserWorldScale = 1.0;

	//World Center on AbsAllMap Coor
	double WorldCenter_X = 5452;
	double WorldCenter_Y = 9432;
	double WorldScale = 1.0;

	//Map Origin Point on AbsAllMap Coor.
	double MapWorldAbsOrigin_X = 8866;
	double MapWorldAbsOrigin_Y = 11457;

	//Map and AbsAllMap Scale Value, Map * MapAbsScale = AbsAllMap.
	double MapAbsScale = 2;

	cv::Point2d MapWorldOffset = cv::Point2d(3412, 2025);
	double MapWorldOffset_X = 3412;
	double MapWorldOffset_Y = 2025;

	double MapWorldScale = 1.0;

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
	bool is_Auto_getHandle = true;

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
	bool getAutoTrackIsInit();
	bool getGengshinImpactWnd();
	bool getGengshinImpactRect();
	bool getGengshinImpactScale();
	bool getGengshinImpactScreen();

	bool getPaimonRefMat();
	bool getMiniMapRefMat();
	bool getUIDRefMat();
	bool getAvatarRefMat();


private:

};
