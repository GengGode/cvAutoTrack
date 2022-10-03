#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
//#include <opencv2/xfeatures2d/nonfree.hpp>
#include <iostream>
#include <Windows.h>

#include "FlowWork.h"
#include "ErrorCode.h"
#include "Kalmanfilter.h"
#include "capture/Capture.h"
#include "resources/Resources.h"
#include "match/match_minimap.h"

// 此类是不导出的
class AutoTrack {
public:
	AutoTrack(void);
	~AutoTrack(void);


	bool init();
	bool uninit();

	bool SetUseBitbltCaptureMode();
	bool SetUseDx11CaptureMode();

	bool SetHandle(long long int handle = 0);
	bool SetWorldCenter(double x, double y);
	bool SetWorldScale(double scale);
	bool GetTransform(float& x, float& y, float& a);
	bool GetTransformOfMap(float& x, float& y, float& a, int& mapId);
	bool GetPosition(double& x, double& y);
	bool GetPositionOfMap(double& x, double& y, int& mapId);
	bool GetDirection(double& a);
	bool GetRotation(double& a);
	//获取发现的神瞳坐标,isEnd为真则为当前画面中的最后一个神瞳
	bool GetStar(double& x, double& y, bool& isEnd);
	//获取发现的神瞳坐标，以json字符串格式
	bool GetStarJson(char* jsonBuff);
	bool GetUID(int& uid);
	bool GetInfoLoadPicture(char* path, int& uid, double& x, double& y, double& a);
	bool GetInfoLoadVideo(char* path, char* pathOutFile);
	int GetLastError();
	const char* GetLastErrorStr();
	int GetLastErrMsg(char* msg_buff, int buff_size);
	bool startServe();
	bool stopServe();

	bool DebugCapture();

private:
	Resources& res = Resources::getInstance();
	ErrorCode& err = ErrorCode::getInstance();
	FlowWork wPaimon;
	FlowWork wMiniMap;
	FlowWork wAvatar;
	FlowWork wRotating;
	FlowWork wStar;

	FlowWork wUID;

	FlowWork wForAfter;

	Kalmanfilter posFilter;
private:
	int error_code = 0;

private:
	bool is_init_end = false;

private:
	int minHessian = 400;
	double ratio_thresh = 0.66;
	double mapScale = 1.3;//1.3;
	int someSizeR = 106;
	double MatchMatScale = 2.0;

	//用户定义映射关系参数
	double UserWorldOrigin_X = 0;
	double UserWorldOrigin_Y = 0;
	double UserWorldScale = 1.0;

#define MAP_3_1


#ifdef MAP_3_0
	// 绝对世界中心 绝对世界缩放系数
	//World Center on AbsAllMap Coor
	double WorldCenter_X = 7400; //Abs 2871   * MapAbsScale = 7392.2870
	double WorldCenter_Y = 9432; //Abs 3015.5 * MapAbsScale = 7710.6335
	double WorldScale = 1.0; //Abs

	//相对绝对空间原点坐标
	//Map Origin Point on AbsAllMap Coor.
	double MapWorldAbsOffset_X = 7.7130; // 7400 - 7392.2870
	double MapWorldAbsOffset_Y = 1721.3665; // 9432 - 7710.6335

	//相对绝对空间缩放系数
	//Map and AbsAllMap Scale Value, Map * MapAbsScale = AbsAllMap.
	double MapAbsScale = 2.557; //from diff Image 67.40%

	//2021-09-07 是个固定值，不用变
	//2022-09-26 构造函数中初始化
	cv::Point2d MapWorldOffset;
#endif

#ifdef MAP_3_1
	// 绝对世界中心 天理坐标系原点在实际画布的位置
	// World Center on AbsAllMap Coor
	double WorldCenter_X = 13544; //Abs 4480   * MapAbsScale = 11455.36
	double WorldCenter_Y = 11480; //Abs 3015.5 * MapAbsScale = 7710.6335
	// 绝对世界缩放系数
	double WorldScale = 1.0; //Abs

	// 地图中心点的实际像素位置
	double MapPixelCenter_X = 4480;
	double MapPixelCenter_Y = 3015.5;

	// 地图中心点转换到绝对空间中的坐标
	// Map Origin Point on AbsAllMap Coor.
	// MapWorldAbsCenter = MapPixelCenter * MapAbsScale;
	double MapWorldAbsCenter_X = 11455.36;
	double MapWorldAbsCenter_Y = 7710.6335;

	//相对绝对空间原点偏移
	//Map Origin Point on AbsAllMap Coor.
	// MapWorldAbsOrigin = WorldCenter - MapWorldOffset
	double MapWorldAbsOffset_X = 2088.64; // 13544 - 11455.36
	double MapWorldAbsOffset_Y = 3769.3665; // 11480 - 7710.6335

	//相对绝对空间缩放系数
	//Map and AbsAllMap Scale Value, Map * MapAbsScale = AbsAllMap.
	double MapAbsScale = 2.557; //from diff Image 67.40%

	//2021-09-07 是个固定值，不用变
	//2022-09-26 构造函数中初始化
	// MapWorldOffset = MapWorldAbsOffset - WorldCenter
	cv::Point2d MapWorldOffset = cv::Point2d(2096.083, 3769.3665);
#endif
	//2022-07-13 还是没想起来
	double MapWorldScale = 1.0;

	// ???
	double screen_scale = 1;


private:
	cv::Ptr<cv::xfeatures2d::SURF> _detectorAllMap;
	//
	cv::Ptr<cv::xfeatures2d::SURF> _detectorSomeMap;
	//
	std::vector<cv::KeyPoint> _KeyPointAllMap;
	//std::vector<cv::KeyPoint>
	std::vector<cv::KeyPoint> _KeyPointSomeMap;
	//std::vector<cv::KeyPoint>
	std::vector<cv::KeyPoint> _KeyPointMiniMap;
	//cv::Mat
	cv::Mat _DataPointAllMap;
	//cv::Mat
	cv::Mat _DataPointSomeMap;
	//cv::Mat
	cv::Mat _DataPointMiniMap;
private:
	bool isOnCity = false;
	bool isContinuity = false;
	bool isConveying = false;
	cv::Point2d _TransformHistory[3];
	bool is_Auto_getHandle = true;
	bool isStarVisible = false;

private:
	double check_match_paimon_params = 0.9;
private:
	// GPU设备数量
	int gpuDeviceNumber = 0;
	// 使能GPU设备id
	int gpuDeviceId = -1;

	// 界面处于手柄模式
	bool isHandleGameMode = false;

	Capture* capture = nullptr;

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
	cv::Mat giStarRef;
	cv::Rect Area_Paimon_mayArea;
	cv::Rect Area_Minimap_mayArea;
	cv::Rect Area_UID_mayArea;
	cv::Rect Area_Avatar_mayArea;

	GenshinHandle genshin_handle;
	GenshinScreen genshin_screen;
	GenshinPaimon genshin_paimon;
	GenshinMinimapCailb genshin_minimap_cailb;
	GenshinMinimap genshin_minimap;
  
private:
	bool getAutoTrackIsInit();
	bool getGengshinImpactWnd();
	bool getGengshinImpactRect();
	bool getGengshinImpactScale();
	bool getGengshinImpactScreen();

	bool getPaimonRefMat();
	bool getMiniMapRefMat();
	bool getMiniMapRefMat_Bitblt();
	bool getUIDRefMat();
	bool getAvatarRefMat();

private:
	bool clear_error_logs();
private:
	bool check_paimon(cv::Rect& paimon_rect);
	//bool check_minimap(cv::Rect minimap_rect);	
	//bool check_uid(cv::Rect uid_rect);
	//bool check_avatar(cv::Rect avatar_rect);
	//bool check_star(cv::Rect star_rect);
};

// using namespace TianLi::Utils;

//bool AutoTrack::GetInfoLoadPicture(char* path, int& uid, double& x, double& y, double& a)
//{
//	std::string pathStr(path);
//
//	giFrame = cv::imread(pathStr);
//
//	if (giFrame.empty())
//	{
//		err = 201;
//		return false;
//	}
//
//	getPaimonRefMat();
//
//	cv::Mat paimonTemplate;
//
//
//	res.PaimonTemplate.copyTo(paimonTemplate);
//
//	cv::Mat tmp;
//	//	giPaimonRef = giFrame(cv::Rect(0, 0, cvCeil(giFrame.cols / 20), cvCeil(giFrame.rows / 10)));
//	//
//	//#ifdef _DEBUG
//	//	cv::namedWindow("test", cv::WINDOW_FREERATIO);
//	//	cv::imshow("test", giPaimonRef);
//	//#endif
//
//	if (giPaimonRef.channels() != 4)
//	{
//		cv::cvtColor(paimonTemplate, paimonTemplate, cv::COLOR_RGBA2GRAY);
//		cv::cvtColor(giPaimonRef, giPaimonRef, cv::COLOR_RGB2GRAY);
//	}
//
//	cv::matchTemplate(paimonTemplate, giPaimonRef, tmp, cv::TM_CCOEFF_NORMED);
//
//	double minVal, maxVal;
//	cv::Point minLoc, maxLoc;
//	cv::minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
//#ifdef _DEBUG
//	cv::namedWindow("test2", cv::WINDOW_FREERATIO);
//	cv::imshow("test2", tmp);
//	//std::cout << "Paimon Match: " << minVal << "," << maxVal << std::endl;
//#endif
//	if (maxVal < 0.34 || maxVal == 1)
//	{
//		err = 202;
//		return false;
//	}
//
//	getMiniMapRefMat();
//
//	cv::Mat img_scene(res.MapTemplate);
//	cv::Mat img_object(giMiniMapRef(cv::Rect(30, 30, giMiniMapRef.cols - 60, giMiniMapRef.rows - 60)));
//
//	cv::cvtColor(img_scene, img_scene, cv::COLOR_RGBA2RGB);
//
//	if (img_object.empty())
//	{
//		err = 203;
//		return false;
//	}
//
//	getAvatarRefMat();
//
//	if (giAvatarRef.empty())
//	{
//		err = 204;//未能取到小箭头区域
//		return false;
//	}
//
//	cv::resize(giAvatarRef, giAvatarRef, cv::Size(), 2, 2);
//	std::vector<cv::Mat> lis;
//	cv::split(giAvatarRef, lis);
//
//	cv::Mat gray0;
//	cv::Mat gray1;
//	cv::Mat gray2;
//
//	cv::threshold(lis[0], gray0, 240, 255, cv::THRESH_BINARY);
//	cv::threshold(lis[1], gray1, 212, 255, cv::THRESH_BINARY);
//	cv::threshold(lis[2], gray2, 25, 255, cv::THRESH_BINARY_INV);
//
//	cv::Mat and12;
//	cv::bitwise_and(gray1, gray2, and12);// , gray0);
//	cv::resize(and12, and12, cv::Size(), 2, 2, 3);
//	cv::Canny(and12, and12, 20, 3 * 20, 3);
//	cv::circle(and12, cv::Point(cvCeil(and12.cols / 2), cvCeil(and12.rows / 2)), cvCeil(and12.cols / 4.5), cv::Scalar(0, 0, 0), -1);
//	cv::Mat dilate_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
//	cv::dilate(and12, and12, dilate_element);
//	cv::Mat erode_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
//	cv::erode(and12, and12, erode_element);
//
//	std::vector<std::vector<cv::Point>> contours;
//	std::vector<cv::Vec4i> hierarcy;
//
//	cv::findContours(and12, contours, hierarcy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
//
//	std::vector<cv::Rect> boundRect(contours.size());  //定义外接矩形集合
//	cv::Point2f rect[4];
//
//	std::vector<cv::Point2d> AvatarKeyPoint;
//	double AvatarKeyPointLine[3] = { 0 };
//	std::vector<cv::Point2f> AvatarKeyLine;
//	cv::Point2f KeyLine;
//
//	if (contours.size() != 3)
//	{
//		err = 205;//提取小箭头特征误差过大
//		//return false;
//	}
//	else
//	{
//		for (int i = 0; i < 3; i++)
//		{
//			boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));
//			AvatarKeyPoint.push_back(cv::Point(cvRound(boundRect[i].x + boundRect[i].width / 2), cvRound(boundRect[i].y + boundRect[i].height / 2)));
//		}
//
//		AvatarKeyPointLine[0] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
//		AvatarKeyPointLine[1] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
//		AvatarKeyPointLine[2] = dis(AvatarKeyPoint[1] - AvatarKeyPoint[0]);
//
//		if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[2] && AvatarKeyPointLine[1] >= AvatarKeyPointLine[2])
//		{
//			AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
//			AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
//		}
//		if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[1] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[1])
//		{
//			AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[0]);
//			AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[2]);
//		}
//		if (AvatarKeyPointLine[1] >= AvatarKeyPointLine[0] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[0])
//		{
//			AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[1]);
//			AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[2]);
//		}
//
//		AvatarKeyLine = Vector2UnitVector(AvatarKeyLine);
//		KeyLine = AvatarKeyLine[0] + AvatarKeyLine[1];
//
//		a = Line2Angle(KeyLine);
//	}
//
//
//
//	if (getUIDRefMat() == false)
//	{
//		err = 206;
//		//return false;
//	}
//	else
//	{
//		std::vector<cv::Mat> channels;
//
//		split(giUIDRef, channels);
//		giUIDRef = channels[0];
//
//		int _uid = 0;
//		int _NumBit[9] = { 0 };
//
//		int bitCount = 1;
//		cv::Mat matchTmp;
//		cv::Mat Roi;
//		cv::Mat checkUID = res.UID;
//
//#ifdef _DEBUG
//		//if (checkUID.rows > Roi.rows)
//		//{
//		//	cv::resize(checkUID, checkUID, cv::Size(), Roi.rows/ checkUID.rows);
//		//}
//#endif
//		giUIDRef.copyTo(Roi);
//
//		if (checkUID.rows > Roi.rows)
//		{
//			cv::resize(Roi, Roi, cv::Size(cvRound(1.0 * checkUID.rows / Roi.rows * Roi.cols), checkUID.rows), 0);
//		}
//
//		cv::matchTemplate(Roi, checkUID, matchTmp, cv::TM_CCOEFF_NORMED);
//
//		minVal = 0;
//		maxVal = 0;
//		minLoc = cv::Point(0, 0);
//		maxLoc = cv::Point(0, 0);
//		//寻找最佳匹配位置
//		cv::minMaxLoc(matchTmp, &minVal, &maxVal, &minLoc, &maxLoc);
//		if (maxVal > 0.75)
//		{
//			int x_uid_ = maxLoc.x + checkUID.cols + 7;
//			int y_uid_ = 0;
//			double tmplis[10] = { 0 };
//			int tmplisx[10] = { 0 };
//			for (int p = 8; p >= 0; p--)
//			{
//				_NumBit[p] = 0;
//				for (int i = 0; i < 10; i++)
//				{
//					cv::Rect r(x_uid_, y_uid_, res.UIDnumber[i].cols + 2, giUIDRef.rows);//180-46/9->140/9->16->16*9=90+54=144
//					if (x_uid_ + r.width > giUIDRef.cols)
//					{
//						r = cv::Rect(giUIDRef.cols - res.UIDnumber[i].cols - 2, y_uid_, res.UIDnumber[i].cols + 2, giUIDRef.rows);
//					}
//
//					cv::Mat numCheckUID = res.UIDnumber[i];
//					Roi = giUIDRef(r);
//
//					cv::matchTemplate(Roi, numCheckUID, matchTmp, cv::TM_CCOEFF_NORMED);
//
//					double minVali, maxVali;
//					cv::Point minLoci, maxLoci;
//					//寻找最佳匹配位置
//					cv::minMaxLoc(matchTmp, &minVali, &maxVali, &minLoci, &maxLoci);
//
//					tmplis[i] = maxVali;
//					tmplisx[i] = maxLoci.x + numCheckUID.cols - 1;
//					if (maxVali > 0.91)
//					{
//						_NumBit[p] = i;
//						x_uid_ = x_uid_ + maxLoci.x + numCheckUID.cols - 1;
//						break;
//					}
//					if (i == 10 - 1)
//					{
//						_NumBit[p] = getMaxID(tmplis, 10);
//						x_uid_ = x_uid_ + tmplisx[_NumBit[p]];
//					}
//				}
//			}
//		}
//
//		_uid = 0;
//		for (int i = 0; i < 9; i++)
//		{
//			_uid += _NumBit[i] * bitCount;
//			bitCount = bitCount * 10;
//		}
//		if (_uid == 0)
//		{
//			err = 207;//未能在UID区域检测到有效UID
//			//return false;
//		}
//		uid = _uid;
//	}
//
//
//
//
//	if (!is_init_end)
//	{
//		init();//初始化
//	}
//	if (img_object.empty())
//	{
//		err = 208;//原神小地图区域为空或者区域长宽小于60px
//		return false;
//	}
//
//	isContinuity = false;
//	isConveying = false;
//
//	cv::Point2d* hisP = _TransformHistory;
//
//	cv::Point2d pos;
//
//	if (dis(hisP[2] - hisP[1]) < 1000)
//	{
//		if (hisP[2].x > someSizeR && hisP[2].x < img_scene.cols - someSizeR && hisP[2].y>someSizeR && hisP[2].y < img_scene.rows - someSizeR)
//		{
//			isContinuity = true;
//		}
//	}
//
//	if (isContinuity)
//	{
//		if (isOnCity == false)
//		{
//			cv::Mat someMap(img_scene(cv::Rect(cvRound(hisP[2].x - someSizeR), cvRound(hisP[2].y - someSizeR), cvRound(someSizeR * 2), cvRound(someSizeR * 2))));
//			cv::Mat minMap(img_object);
//
//
//			cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap = _detectorSomeMap;
//			std::vector<cv::KeyPoint>& KeyPointSomeMap = _KeyPointSomeMap;
//			cv::Mat& DataPointSomeMap = _DataPointSomeMap;
//			std::vector<cv::KeyPoint>& KeyPointMiniMap = _KeyPointMiniMap;
//			cv::Mat& DataPointMiniMap = _DataPointMiniMap;
//
//			detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
//			detectorSomeMap->detectAndCompute(someMap, cv::noArray(), KeyPointSomeMap, DataPointSomeMap);
//			detectorSomeMap->detectAndCompute(minMap, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);
//
//
//
//			if (KeyPointMiniMap.size() == 0 || KeyPointSomeMap.size() == 0)
//			{
//				isContinuity = false;
//			}
//			else
//			{
//				cv::Ptr<cv::DescriptorMatcher> matcher_on_city_not = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
//				std::vector< std::vector<cv::DMatch> > KNN_m_on_city_not;
//
//				matcher_on_city_not->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_m_on_city_not, 2);
//
//				std::vector<double> lisx;
//				std::vector<double> lisy;
//				double sumx = 0;
//				double sumy = 0;
//
//				calc_good_matches(someMap, KeyPointSomeMap, img_object, KeyPointMiniMap, KNN_m_on_city_not, ratio_thresh, mapScale, lisx, lisy, sumx, sumy);
//
//				if (min(lisx.size(), lisy.size()) <= 4)
//				{
//					//有可能处于城镇中
//
//					/***********************/
//					//重新从完整中地图取出角色周围部分地图
//					img_scene(cv::Rect(cvCeil(hisP[2].x - someSizeR), cvCeil(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)).copyTo(someMap);
//					//Mat minMap(img_object);
//
//					resize(someMap, someMap, cv::Size(someSizeR * 4, someSizeR * 4));
//					//resize(minMap, minMap, Size(), MatchMatScale, MatchMatScale, 1);
//
//					detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
//					detectorSomeMap->detectAndCompute(someMap, cv::noArray(), KeyPointSomeMap, DataPointSomeMap);
//					//detectorSomeMap->detectAndCompute(minMap, noArray(), KeyPointMinMap, DataPointMinMap);
//					if (KeyPointSomeMap.size() == 0 || KeyPointMiniMap.size() == 0)
//					{
//						isContinuity = false;
//					}
//					else
//					{
//						cv::Ptr<cv::DescriptorMatcher> matcher_on_city_maybe = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
//						std::vector< std::vector<cv::DMatch> > KNN_m_on_city_maybe;
//
//						matcher_on_city_maybe->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_m_on_city_maybe, 2);
//						//std::vector<double> lisx;
//						//std::vector<double> lisy;
//						lisx.clear();
//						lisy.clear();
//						//double sumx = 0;
//						//double sumy = 0;
//						sumx = 0;
//						sumy = 0;
//
//						calc_good_matches(someMap, KeyPointSomeMap, img_object, KeyPointMiniMap, KNN_m_on_city_maybe, ratio_thresh, 0.8667, lisx, lisy, sumx, sumy);
//
//						if (min(lisx.size(), lisy.size()) <= 4)
//						{
//							isContinuity = false;
//						}
//						else
//						{
//							if (min(lisx.size(), lisy.size()) >= 10)
//							{
//								isOnCity = true;
//							}
//							else
//							{
//								isOnCity = false;
//							}
//
//							cv::Point2d pos_on_city_maybe = SPC(lisx, sumx, lisy, sumy);
//
//							double x_on_city_maybe = (pos_on_city_maybe.x - someMap.cols / 2.0) / 2.0;
//							double y_on_city_maybe = (pos_on_city_maybe.y - someMap.rows / 2.0) / 2.0;
//
//							pos = cv::Point2d(x_on_city_maybe + hisP[2].x, y_on_city_maybe + hisP[2].y);
//						}
//
//					}
//				}
//				else
//				{
//					isOnCity = false;
//
//					cv::Point2d p_on_city_not = SPC(lisx, sumx, lisy, sumy);
//
//					pos = cv::Point2d(p_on_city_not.x + hisP[2].x - someSizeR, p_on_city_not.y + hisP[2].y - someSizeR);
//				}
//			}
//		}
//		else
//		{
//			//在城镇中
//				/***********************/
//				//重新从完整中地图取出角色周围部分地图
//			cv::Mat someMap(img_scene(cv::Rect(cvCeil(hisP[2].x - someSizeR), cvCeil(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)));
//			cv::Mat minMap(img_object);
//
//
//			cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap = _detectorSomeMap;
//			std::vector<cv::KeyPoint>& KeyPointSomeMap = _KeyPointSomeMap;
//			cv::Mat& DataPointSomeMap = _DataPointSomeMap;
//			std::vector<cv::KeyPoint>& KeyPointMiniMap = _KeyPointMiniMap;
//			cv::Mat& DataPointMiniMap = _DataPointMiniMap;
//
//
//			resize(someMap, someMap, cv::Size(someSizeR * 4, someSizeR * 4));
//			//resize(minMap, minMap, Size(), MatchMatScale, MatchMatScale, 1);
//
//			detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
//			detectorSomeMap->detectAndCompute(someMap, cv::noArray(), KeyPointSomeMap, DataPointSomeMap);
//			detectorSomeMap->detectAndCompute(minMap, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);
//
//			if (KeyPointSomeMap.size() != 0 && KeyPointMiniMap.size() != 0)
//			{
//				cv::Ptr<cv::DescriptorMatcher> matcher_on_city = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
//				std::vector< std::vector<cv::DMatch> > KNN_m_on_city;
//
//				matcher_on_city->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_m_on_city, 2);
//				std::vector<double> lisx;
//				std::vector<double> lisy;
//				double sumx = 0;
//				double sumy = 0;
//
//				calc_good_matches(someMap, KeyPointSomeMap, img_object, KeyPointMiniMap, KNN_m_on_city, ratio_thresh, 0.8667, lisx, lisy, sumx, sumy);
//
//				if (max(lisx.size(), lisy.size()) > 4)
//				{
//					if (min(lisx.size(), lisy.size()) >= 10)
//					{
//						isOnCity = true;
//					}
//					else
//					{
//						isOnCity = false;
//					}
//
//					cv::Point2d pos_on_city = SPC(lisx, sumx, lisy, sumy);
//
//					double x_on_city = (pos_on_city.x - someMap.cols / 2.0) / 2.0;
//					double y_on_city = (pos_on_city.y - someMap.rows / 2.0) / 2.0;
//
//					pos = cv::Point2d(x_on_city + hisP[2].x, y_on_city + hisP[2].y);
//				}
//				else
//				{
//					isContinuity = false;
//				}//if (max(lisx.size(), lisy.size()) > 4)
//			}
//			else
//			{
//				isContinuity = false;
//			}//if (KeyPointSomeMap.size() != 0 && KeyPointMinMap.size() != 0)
//		}
//	}
//	else
//	{
//		isConveying = true;
//	}
//
//	if (!isContinuity)
//	{
//		cv::Ptr<cv::xfeatures2d::SURF>& detectorAllMap = _detectorAllMap;
//		std::vector<cv::KeyPoint>& KeyPointAllMap = _KeyPointAllMap;
//		cv::Mat& DataPointAllMap = _DataPointAllMap;
//		std::vector<cv::KeyPoint>& KeyPointMiniMap = _KeyPointMiniMap;
//		cv::Mat& DataPointMiniMap = _DataPointMiniMap;
//
//		detectorAllMap->detectAndCompute(img_object, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);
//
//		if (KeyPointMiniMap.size() == 0)
//		{
//			err = { 4, "小地图未能计算出可识别特征点" };//未能匹配到特征点
//			return false;
//		}
//		else
//		{
//			cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
//			std::vector< std::vector<cv::DMatch> > KNN_m;
//
//			matcher->knnMatch(DataPointMiniMap, DataPointAllMap, KNN_m, 2);
//
//			std::vector<double> lisx;
//			std::vector<double> lisy;
//			double sumx = 0;
//			double sumy = 0;
//
//			calc_good_matches(img_scene, KeyPointAllMap, img_object, KeyPointMiniMap, KNN_m, ratio_thresh, mapScale, lisx, lisy, sumx, sumy);
//
//
//
//			if (lisx.size() == 0 || lisy.size() == 0)
//			{
//				err = { 4, "未能匹配到特征点" };
//				return false;
//			}
//			else
//			{
//				pos = SPC(lisx, sumx, lisy, sumy);
//				isConveying = true;
//			}
//		}
//	}
//	cv::Point2d filt_pos;
//	if (isConveying || !isContinuity)
//	{
//		filt_pos = posFilter.reinitfilterting(pos);
//		//isConveying = false;
//	}
//	else
//	{
//		filt_pos = posFilter.filterting(pos);
//	}
//
//
//	hisP[0] = hisP[1];
//	hisP[1] = hisP[2];
//	hisP[2] = filt_pos;
//
//	pos = TransferTianLiAxes(pos * MapAbsScale, MapWorldOffset, MapWorldScale);
//
//	pos = TransferUserAxes(pos, UserWorldOrigin_X, UserWorldOrigin_Y, UserWorldScale);
//
//	x = (float)(pos.x);
//	y = (float)(pos.y);
//
//	err = 0;
//	return true;
//
//}
//
//bool AutoTrack::GetInfoLoadVideo(char* path, char* pathOutFile)
//{
//	std::string pathStr(path);
//	std::string pathOutFileStr(pathOutFile);
//
//	cv::VideoCapture Video;
//	Video.open(pathStr);
//
//	if (!Video.isOpened())
//	{
//		err = 301;
//		return false;
//	}
//
//	FILE* fpSave;
//	errno_t errOpenFile;
//	errOpenFile = fopen_s(&fpSave, pathOutFileStr.c_str(), "w");
//	if (errOpenFile != 0)
//	{
//		err = 302;
//		return false;
//	}
//#define TestJson
//#ifdef TestJson
//
//#else
//	fprintf_s(fpSave, "{\"curveName\":\"新的画布\",\"curve_list\":[{\"lineName\":\"新的线条\",\"curve_poi\":[");
//#endif
//
//	int uid = 0;
//	double x = 0;
//	double y = 0;
//	double a = 0;
//
//	int timeNumber = 0;
//
//	bool isCycle = true;
//	while (isCycle)
//	{
//		Video >> giFrame;
//
//		if (giFrame.empty())
//		{
//			isCycle = false;
//			break;
//		}
//
//		getPaimonRefMat();
//
//		cv::Mat paimonTemplate;
//
//		res.PaimonTemplate.copyTo(paimonTemplate);
//
//		cv::Mat tmp;
//		giPaimonRef = giFrame(cv::Rect(0, 0, cvCeil(giFrame.cols / 20), cvCeil(giFrame.rows / 10)));
//
//#ifdef _DEBUG
//		cv::namedWindow("test", cv::WINDOW_FREERATIO);
//		cv::imshow("test", giPaimonRef);
//#endif
//
//		if (giPaimonRef.channels() != 4)
//		{
//			cv::cvtColor(paimonTemplate, paimonTemplate, cv::COLOR_RGBA2GRAY);
//			cv::cvtColor(giPaimonRef, giPaimonRef, cv::COLOR_RGB2GRAY);
//		}
//
//		cv::matchTemplate(paimonTemplate, giPaimonRef, tmp, cv::TM_CCOEFF_NORMED);
//
//		double minVal, maxVal;
//		cv::Point minLoc, maxLoc;
//		cv::minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
//#ifdef _DEBUG
//		cv::namedWindow("test2", cv::WINDOW_FREERATIO);
//		cv::imshow("test2", tmp);
//		//std::cout << "Paimon Match: " << minVal << "," << maxVal << std::endl;
//#endif
//		if (maxVal < 0.9 || maxVal == 1)
//		{
//			continue;
//		}
//
//		getMiniMapRefMat();
//
//		cv::Mat img_scene(res.MapTemplate);
//		cv::Mat img_object(giMiniMapRef(cv::Rect(30, 30, giMiniMapRef.cols - 60, giMiniMapRef.rows - 60)));
//
//		cv::cvtColor(img_scene, img_scene, cv::COLOR_RGBA2RGB);
//
//		if (img_object.empty())
//		{
//			continue;
//		}
//
//		getAvatarRefMat();
//
//		if (giAvatarRef.empty())
//		{
//			//err = 204;//未能取到小箭头区域
//			//break;// return false;
//		}
//		else
//		{
//			cv::resize(giAvatarRef, giAvatarRef, cv::Size(), 2, 2);
//			std::vector<cv::Mat> lis;
//			cv::split(giAvatarRef, lis);
//
//			cv::Mat gray0;
//			cv::Mat gray1;
//			cv::Mat gray2;
//
//			cv::threshold(lis[0], gray0, 240, 255, cv::THRESH_BINARY);
//			cv::threshold(lis[1], gray1, 212, 255, cv::THRESH_BINARY);
//			cv::threshold(lis[2], gray2, 25, 255, cv::THRESH_BINARY_INV);
//
//			cv::Mat and12;
//			cv::bitwise_and(gray1, gray2, and12);// , gray0);
//			cv::resize(and12, and12, cv::Size(), 2, 2, 3);
//			cv::Canny(and12, and12, 20, 3 * 20, 3);
//			cv::circle(and12, cv::Point(cvCeil(and12.cols / 2), cvCeil(and12.rows / 2)), cvCeil(and12.cols / 4.5), cv::Scalar(0, 0, 0), -1);
//			cv::Mat dilate_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
//			cv::dilate(and12, and12, dilate_element);
//			cv::Mat erode_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
//			cv::erode(and12, and12, erode_element);
//
//			std::vector<std::vector<cv::Point>> contours;
//			std::vector<cv::Vec4i> hierarcy;
//
//			cv::findContours(and12, contours, hierarcy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
//
//			std::vector<cv::Rect> boundRect(contours.size());  //定义外接矩形集合
//			cv::Point2f rect[4];
//
//			std::vector<cv::Point2d> AvatarKeyPoint;
//			double AvatarKeyPointLine[3] = { 0 };
//			std::vector<cv::Point2f> AvatarKeyLine;
//			cv::Point2f KeyLine;
//
//			if (contours.size() != 3)
//			{
//				//err = 205;//提取小箭头特征误差过大
//				//break;//return false;
//			}
//			else
//			{
//				for (int i = 0; i < 3; i++)
//				{
//					boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));
//					AvatarKeyPoint.push_back(cv::Point(cvRound(boundRect[i].x + boundRect[i].width / 2), cvRound(boundRect[i].y + boundRect[i].height / 2)));
//				}
//
//				AvatarKeyPointLine[0] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
//				AvatarKeyPointLine[1] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
//				AvatarKeyPointLine[2] = dis(AvatarKeyPoint[1] - AvatarKeyPoint[0]);
//
//				if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[2] && AvatarKeyPointLine[1] >= AvatarKeyPointLine[2])
//				{
//					AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
//					AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
//				}
//				if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[1] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[1])
//				{
//					AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[0]);
//					AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[2]);
//				}
//				if (AvatarKeyPointLine[1] >= AvatarKeyPointLine[0] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[0])
//				{
//					AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[1]);
//					AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[2]);
//				}
//
//				AvatarKeyLine = Vector2UnitVector(AvatarKeyLine);
//				KeyLine = AvatarKeyLine[0] + AvatarKeyLine[1];
//
//				a = Line2Angle(KeyLine);
//			}
//		}
//
//
//
//
//
//		if (getUIDRefMat() == false)
//		{
//			continue;
//		}
//		else
//		{
//			std::vector<cv::Mat> channels;
//
//			split(giUIDRef, channels);
//			giUIDRef = channels[0];
//
//			int _uid = 0;
//			int _NumBit[9] = { 0 };
//
//			int bitCount = 1;
//			cv::Mat matchTmp;
//			cv::Mat Roi;
//			cv::Mat checkUID = res.UID;
//
//#ifdef _DEBUG
//			//if (checkUID.rows > Roi.rows)
//			//{
//			//	cv::resize(checkUID, checkUID, cv::Size(), Roi.rows/ checkUID.rows);
//			//}
//#endif
//			giUIDRef.copyTo(Roi);
//
//			if (checkUID.rows > Roi.rows)
//			{
//				cv::resize(Roi, Roi, cv::Size(cvRound(1.0 * checkUID.rows / Roi.rows * Roi.cols), checkUID.rows), 0);
//			}
//
//			cv::matchTemplate(Roi, checkUID, matchTmp, cv::TM_CCOEFF_NORMED);
//
//			minVal = 0;
//			maxVal = 0;
//			minLoc = cv::Point(0, 0);
//			maxLoc = cv::Point(0, 0);
//			//寻找最佳匹配位置
//			cv::minMaxLoc(matchTmp, &minVal, &maxVal, &minLoc, &maxLoc);
//			if (maxVal > 0.75)
//			{
//				int x_uid_ = maxLoc.x + checkUID.cols + 7;
//				int y_uid_ = 0;
//				double tmplis[10] = { 0 };
//				int tmplisx[10] = { 0 };
//				for (int p = 8; p >= 0; p--)
//				{
//					_NumBit[p] = 0;
//					for (int i = 0; i < 10; i++)
//					{
//						cv::Rect r(x_uid_, y_uid_, res.UIDnumber[i].cols + 2, giUIDRef.rows);//180-46/9->140/9->16->16*9=90+54=144
//						if (x_uid_ + r.width > giUIDRef.cols)
//						{
//							r = cv::Rect(giUIDRef.cols - res.UIDnumber[i].cols - 2, y_uid_, res.UIDnumber[i].cols + 2, giUIDRef.rows);
//						}
//
//						cv::Mat numCheckUID = res.UIDnumber[i];
//						Roi = giUIDRef(r);
//
//						cv::matchTemplate(Roi, numCheckUID, matchTmp, cv::TM_CCOEFF_NORMED);
//
//						double minVali, maxVali;
//						cv::Point minLoci, maxLoci;
//						//寻找最佳匹配位置
//						cv::minMaxLoc(matchTmp, &minVali, &maxVali, &minLoci, &maxLoci);
//
//						tmplis[i] = maxVali;
//						tmplisx[i] = maxLoci.x + numCheckUID.cols - 1;
//						if (maxVali > 0.91)
//						{
//							_NumBit[p] = i;
//							x_uid_ = x_uid_ + maxLoci.x + numCheckUID.cols - 1;
//							break;
//						}
//						if (i == 10 - 1)
//						{
//							_NumBit[p] = getMaxID(tmplis, 10);
//							x_uid_ = x_uid_ + tmplisx[_NumBit[p]];
//						}
//					}
//				}
//			}
//
//			_uid = 0;
//			for (int i = 0; i < 9; i++)
//			{
//				_uid += _NumBit[i] * bitCount;
//				bitCount = bitCount * 10;
//			}
//			if (_uid == 0)
//			{
//				//err = 207;//未能在UID区域检测到有效UID
//				//return false;
//			}
//			uid = _uid;
//		}
//
//
//
//
//
//		if (!is_init_end)
//		{
//			init();//初始化
//		}
//		if (img_object.empty())
//		{
//			continue;
//		}
//
//		isContinuity = false;
//		isConveying = false;
//
//		cv::Point2d* hisP = _TransformHistory;
//
//		cv::Point2d pos;
//
//		if (dis(hisP[2] - hisP[1]) < 1000)
//		{
//			if (hisP[2].x > someSizeR && hisP[2].x < img_scene.cols - someSizeR && hisP[2].y>someSizeR && hisP[2].y < img_scene.rows - someSizeR)
//			{
//				isContinuity = true;
//			}
//		}
//
//		if (isContinuity)
//		{
//			if (isOnCity == false)
//			{
//				cv::Mat someMap(img_scene(cv::Rect(cvRound(hisP[2].x - someSizeR), cvRound(hisP[2].y - someSizeR), cvRound(someSizeR * 2), cvRound(someSizeR * 2))));
//				cv::Mat minMap(img_object);
//
//
//				cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap = _detectorSomeMap;
//				std::vector<cv::KeyPoint>& KeyPointSomeMap = _KeyPointSomeMap;
//				cv::Mat& DataPointSomeMap = _DataPointSomeMap;
//				std::vector<cv::KeyPoint>& KeyPointMiniMap = _KeyPointMiniMap;
//				cv::Mat& DataPointMiniMap = _DataPointMiniMap;
//
//				detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
//				detectorSomeMap->detectAndCompute(someMap, cv::noArray(), KeyPointSomeMap, DataPointSomeMap);
//				detectorSomeMap->detectAndCompute(minMap, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);
//
//
//
//				if (KeyPointMiniMap.size() == 0 || KeyPointSomeMap.size() == 0)
//				{
//					isContinuity = false;
//				}
//				else
//				{
//					cv::Ptr<cv::DescriptorMatcher> matcher_on_city_not = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
//					std::vector< std::vector<cv::DMatch> > KNN_m_on_city_not;
//
//					matcher_on_city_not->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_m_on_city_not, 2);
//
//					std::vector<double> lisx;
//					std::vector<double> lisy;
//					double sumx = 0;
//					double sumy = 0;
//
//					calc_good_matches(someMap, KeyPointSomeMap, img_object, KeyPointMiniMap, KNN_m_on_city_not, ratio_thresh, mapScale, lisx, lisy, sumx, sumy);
//
//					if (min(lisx.size(), lisy.size()) <= 4)
//					{
//						//有可能处于城镇中
//
//						/***********************/
//						//重新从完整中地图取出角色周围部分地图
//						img_scene(cv::Rect(cvCeil(hisP[2].x - someSizeR), cvCeil(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)).copyTo(someMap);
//						//Mat minMap(img_object);
//
//						resize(someMap, someMap, cv::Size(someSizeR * 4, someSizeR * 4));
//						//resize(minMap, minMap, Size(), MatchMatScale, MatchMatScale, 1);
//
//						detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
//						detectorSomeMap->detectAndCompute(someMap, cv::noArray(), KeyPointSomeMap, DataPointSomeMap);
//						//detectorSomeMap->detectAndCompute(minMap, noArray(), KeyPointMinMap, DataPointMinMap);
//						if (KeyPointSomeMap.size() == 0 || KeyPointMiniMap.size() == 0)
//						{
//							isContinuity = false;
//						}
//						else
//						{
//							cv::Ptr<cv::DescriptorMatcher> matcher_on_city_maybe = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
//							std::vector< std::vector<cv::DMatch> > KNN_m_on_city_maybe;
//
//							matcher_on_city_maybe->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_m_on_city_maybe, 2);
//							//std::vector<double> lisx;
//							//std::vector<double> lisy;
//							lisx.clear();
//							lisy.clear();
//							//double sumx = 0;
//							//double sumy = 0;
//							sumx = 0;
//							sumy = 0;
//
//							calc_good_matches(someMap, KeyPointSomeMap, img_object, KeyPointMiniMap, KNN_m_on_city_maybe, ratio_thresh, 0.8667, lisx, lisy, sumx, sumy);
//
//							if (min(lisx.size(), lisy.size()) <= 4)
//							{
//								isContinuity = false;
//							}
//							else
//							{
//								if (min(lisx.size(), lisy.size()) >= 10)
//								{
//									isOnCity = true;
//								}
//								else
//								{
//									isOnCity = false;
//								}
//
//								cv::Point2d pos_on_city_maybe = SPC(lisx, sumx, lisy, sumy);
//
//								double x_on_city_maybe = (pos_on_city_maybe.x - someMap.cols / 2.0) / 2.0;
//								double y_on_city_maybe = (pos_on_city_maybe.y - someMap.rows / 2.0) / 2.0;
//
//								pos = cv::Point2d(x_on_city_maybe + hisP[2].x, y_on_city_maybe + hisP[2].y);
//							}
//
//						}
//					}
//					else
//					{
//						isOnCity = false;
//
//						cv::Point2d p_on_city_not = SPC(lisx, sumx, lisy, sumy);
//
//						pos = cv::Point2d(p_on_city_not.x + hisP[2].x - someSizeR, p_on_city_not.y + hisP[2].y - someSizeR);
//					}
//				}
//			}
//			else
//			{
//				//在城镇中
//					/***********************/
//					//重新从完整中地图取出角色周围部分地图
//				cv::Mat someMap(img_scene(cv::Rect(cvCeil(hisP[2].x - someSizeR), cvCeil(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)));
//				cv::Mat minMap(img_object);
//
//
//				cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap = _detectorSomeMap;
//				std::vector<cv::KeyPoint>& KeyPointSomeMap = _KeyPointSomeMap;
//				cv::Mat& DataPointSomeMap = _DataPointSomeMap;
//				std::vector<cv::KeyPoint>& KeyPointMiniMap = _KeyPointMiniMap;
//				cv::Mat& DataPointMiniMap = _DataPointMiniMap;
//
//
//				resize(someMap, someMap, cv::Size(someSizeR * 4, someSizeR * 4));
//				//resize(minMap, minMap, Size(), MatchMatScale, MatchMatScale, 1);
//
//				detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
//				detectorSomeMap->detectAndCompute(someMap, cv::noArray(), KeyPointSomeMap, DataPointSomeMap);
//				detectorSomeMap->detectAndCompute(minMap, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);
//
//				if (KeyPointSomeMap.size() != 0 && KeyPointMiniMap.size() != 0)
//				{
//					cv::Ptr<cv::DescriptorMatcher> matcher_on_city = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
//					std::vector< std::vector<cv::DMatch> > KNN_m_on_city;
//
//					matcher_on_city->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_m_on_city, 2);
//					std::vector<double> lisx;
//					std::vector<double> lisy;
//					double sumx = 0;
//					double sumy = 0;
//
//					calc_good_matches(someMap, KeyPointSomeMap, img_object, KeyPointMiniMap, KNN_m_on_city, ratio_thresh, 0.8667, lisx, lisy, sumx, sumy);
//
//					if (max(lisx.size(), lisy.size()) > 4)
//					{
//						if (min(lisx.size(), lisy.size()) >= 10)
//						{
//							isOnCity = true;
//						}
//						else
//						{
//							isOnCity = false;
//						}
//
//						cv::Point2d pos_on_city = SPC(lisx, sumx, lisy, sumy);
//
//						double x_on_city = (pos_on_city.x - someMap.cols / 2.0) / 2.0;
//						double y_on_city = (pos_on_city.y - someMap.rows / 2.0) / 2.0;
//
//						pos = cv::Point2d(x_on_city + hisP[2].x, y_on_city + hisP[2].y);
//					}
//					else
//					{
//						isContinuity = false;
//					}//if (max(lisx.size(), lisy.size()) > 4)
//				}
//				else
//				{
//					isContinuity = false;
//				}//if (KeyPointSomeMap.size() != 0 && KeyPointMinMap.size() != 0)
//			}
//		}
//		else
//		{
//			isConveying = true;
//		}
//
//		if (!isContinuity)
//		{
//			cv::Ptr<cv::xfeatures2d::SURF>& detectorAllMap = _detectorAllMap;
//			std::vector<cv::KeyPoint>& KeyPointAllMap = _KeyPointAllMap;
//			cv::Mat& DataPointAllMap = _DataPointAllMap;
//			std::vector<cv::KeyPoint>& KeyPointMiniMap = _KeyPointMiniMap;
//			cv::Mat& DataPointMiniMap = _DataPointMiniMap;
//
//			detectorAllMap->detectAndCompute(img_object, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);
//
//			if (KeyPointMiniMap.size() == 0)
//			{
//				err = { 4, "小地图未能计算出可识别特征点" };//未能匹配到特征点
//				return false;
//			}
//			else
//			{
//				cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
//				std::vector< std::vector<cv::DMatch> > KNN_m;
//
//				matcher->knnMatch(DataPointMiniMap, DataPointAllMap, KNN_m, 2);
//
//				std::vector<double> lisx;
//				std::vector<double> lisy;
//				double sumx = 0;
//				double sumy = 0;
//
//				calc_good_matches(img_scene, KeyPointAllMap, img_object, KeyPointMiniMap, KNN_m, ratio_thresh, mapScale, lisx, lisy, sumx, sumy);
//
//
//
//				if (lisx.size() == 0 || lisy.size() == 0)
//				{
//					err = { 4, "未能匹配到特征点" };
//					return false;
//				}
//				else
//				{
//					pos = SPC(lisx, sumx, lisy, sumy);
//					isConveying = true;
//				}
//			}
//		}
//		cv::Point2d filt_pos;
//		if (isConveying || !isContinuity)
//		{
//			filt_pos = posFilter.reinitfilterting(pos);
//			//isConveying = false;
//		}
//		else
//		{
//			filt_pos = posFilter.filterting(pos);
//		}
//
//
//		hisP[0] = hisP[1];
//		hisP[1] = hisP[2];
//		hisP[2] = filt_pos;
//
//		pos = TransferTianLiAxes(pos * MapAbsScale, MapWorldOffset, MapWorldScale);
//
//		pos = TransferUserAxes(pos, UserWorldOrigin_X, UserWorldOrigin_Y, UserWorldScale);
//
//		x = (float)(pos.x);
//		y = (float)(pos.y);
//
//#ifdef TestJson
//		fprintf_s(fpSave, "%d %d %lf %lf %lf\n", timeNumber, uid, x, y, a);
//#else
//		if (timeNumber != 0)
//		{
//			fprintf_s(fpSave, ",\n");
//		}
//		else
//		{
//			fprintf_s(fpSave, "\n");
//		}
//		fprintf_s(fpSave, "{\"x\": %lf,\"y\": %lf,\"z\":0.0}", x, y);
//#endif
//		timeNumber++;
//	}
//
//
//#ifdef TestJson
//
//#else
//	fprintf_s(fpSave, "]}]}");
//#endif
//
//	std::fclose(fpSave);
//	Video.release();
//	err = 0;
//	return true;
//}