// cvAutoTrack.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "framework.h"
#include "cvAutoTrack.h"

#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include "LoadGiMatchResource.h"

#include "resource.h"

#include <iostream>

// 这是已导出类的构造函数。
CcvAutoTrack::CcvAutoTrack()
{
	_detectorAllMap = new cv::Ptr<cv::xfeatures2d::SURF>;
	_detectorSomeMap = new cv::Ptr<cv::xfeatures2d::SURF>;
	_KeyPointAllMap = new std::vector<cv::KeyPoint>;
	_KeyPointSomeMap = new std::vector<cv::KeyPoint>;
	_KeyPointMiniMap = new std::vector<cv::KeyPoint>;
	_DataPointAllMap = new cv::Mat;
	_DataPointSomeMap = new cv::Mat;
	_DataPointMiniMap = new cv::Mat;

	_TransfornHistory = new std::vector<cv::Point>;
	__TransfornHistory = new void*[3];
	for (int i = 0; i < 3; i++)
	{
		__TransfornHistory[i] = new cv::Point;
	}

	_giMatchResource = new LoadGiMatchResource;
	_giHandle = new HWND;
	_giRect = new RECT;
	_giClientRect = new RECT;
	_giClientSize = new cv::Size;
	_giFrame = new cv::Mat;
	_giPaimonRef = new cv::Mat;
	_giMiniMapRef = new cv::Mat;
    return;
}

CcvAutoTrack::~CcvAutoTrack(void)
{
	delete _detectorAllMap;
	delete _detectorSomeMap;
	delete _KeyPointAllMap;
	delete _KeyPointSomeMap;
	delete _KeyPointMiniMap;
	delete _DataPointAllMap;
	delete _DataPointSomeMap;
	delete _DataPointMiniMap;

	delete _TransfornHistory;
	delete[] __TransfornHistory;

	delete _giMatchResource;
	delete _giHandle;
	delete _giRect;
	delete _giClientRect;
	delete _giClientSize;
	delete _giFrame;
	delete _giPaimonRef;
	delete _giMiniMapRef;
}

bool CcvAutoTrack::init()
{
	if (!is_init_end)
	{
		LoadGiMatchResource& giMatchResource = *(LoadGiMatchResource*)_giMatchResource;
		cv::Ptr<cv::xfeatures2d::SURF>& detectorAllMap = *(cv::Ptr<cv::xfeatures2d::SURF>*) _detectorAllMap;
		std::vector<cv::KeyPoint>& KeyPointAllMap = *(std::vector<cv::KeyPoint>*)_KeyPointAllMap;
		cv::Mat& DataPointAllMap = *(cv::Mat*)_DataPointAllMap;

		detectorAllMap = cv::xfeatures2d::SURF::create(minHessian);
		detectorAllMap->detectAndCompute(giMatchResource.MapTemplate, cv::noArray(), KeyPointAllMap, DataPointAllMap);

		is_init_end = true;
	}
	return is_init_end;
}

bool CcvAutoTrack::uninit()
{
	if (is_init_end)
	{
		delete _detectorAllMap;
		_detectorAllMap = nullptr;
		delete _KeyPointAllMap;
		_KeyPointAllMap = nullptr;
		delete _DataPointAllMap;
		_DataPointAllMap = nullptr;
		is_init_end = false;
	}
	return !is_init_end;
}

bool CcvAutoTrack::GetTransforn(float & x, float & y, float & a)
{
	if (!is_init_end)
	{
		error_code = 1;//未初始化
		return false;
	}
	// 判断原神窗口不存在直接返回false，不对参数做任何修改
	if (getGengshinImpactWnd())
	{
		getGengshinImpactRect();
		getGengshinImpactScreen();

		cv::Mat& giFrame = *(cv::Mat*)_giFrame;

		if (!giFrame.empty())
		{
			getPaimonRefMat();

			cv::Mat& giPaimonRef = *(cv::Mat*)_giPaimonRef;
			LoadGiMatchResource& giMatchResource = *(LoadGiMatchResource*)_giMatchResource;
			
			cv::Mat paimonTemplate;
			cv::resize(giMatchResource.PaimonTemplate, paimonTemplate, giPaimonRef.size());

			cv::Mat tmp;
			cv::matchTemplate(paimonTemplate, giPaimonRef, tmp, cv::TM_CCOEFF_NORMED);

			double minVal, maxVal;
			cv::Point minLoc, maxLoc;
			
			cv::minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
			
			if (minVal < 0.36 || maxVal == 1)
			{
				error_code = 6;//未能匹配到派蒙
				return false;
			}

			getMiniMapRefMat();

			cv::Mat& giMiniMapRef = *(cv::Mat*)_giMiniMapRef;
			
			/******************************/

			cv::Mat img_scene(giMatchResource.MapTemplate);
			cv::Mat img_object(giMiniMapRef(cv::Rect(30, 30, giMiniMapRef.cols - 60, giMiniMapRef.rows - 60)));

			cv::cvtColor(img_scene, img_scene, CV_RGBA2RGB);

			if (img_object.empty())
			{
				error_code = 5;//原神小地图区域为空或者区域长宽小于60px
				return false;
			}

			isContinuity = false;

			std::vector<cv::Point>& TransfornHistory = *(std::vector<cv::Point>*)_TransfornHistory;
			cv::Point*& hisP = *(cv::Point**) __TransfornHistory;
			cv::Point pos;

			if ((dis(hisP[1] - hisP[0]) + dis(hisP[2] - hisP[1])) < 2000)
			{
				if (hisP[2].x > someSizeR && hisP[2].x < img_scene.cols - someSizeR && hisP[2].y>someSizeR && hisP[2].y < img_scene.rows - someSizeR)
				{
					isContinuity = true;
					if (isContinuity)
					{
						cv::Mat someMap(img_scene(cv::Rect(hisP[2].x - someSizeR, hisP[2].y - someSizeR, someSizeR * 2, someSizeR * 2)));
						cv::Mat minMap(img_object);

						//resize(someMap, someMap, Size(), MatchMatScale, MatchMatScale, 1);
						//resize(minMap, minMap, Size(), MatchMatScale, MatchMatScale, 1);

						cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap = *(cv::Ptr<cv::xfeatures2d::SURF>*)_detectorSomeMap;
						std::vector<cv::KeyPoint>& KeyPointSomeMap = *(std::vector<cv::KeyPoint>*)_KeyPointSomeMap;
						cv::Mat& DataPointSomeMap = *(cv::Mat*)_DataPointSomeMap;
						std::vector<cv::KeyPoint>& KeyPointMiniMap = *(std::vector<cv::KeyPoint>*)_KeyPointMiniMap;
						cv::Mat& DataPointMiniMap = *(cv::Mat*)_DataPointMiniMap;

						detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
						detectorSomeMap->detectAndCompute(someMap, cv::noArray(), KeyPointSomeMap, DataPointSomeMap);
						detectorSomeMap->detectAndCompute(minMap, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);
						cv::Ptr<cv::DescriptorMatcher> matcherTmp = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
						std::vector< std::vector<cv::DMatch> > KNN_mTmp;
						std::vector<cv::DMatch> good_matchesTmp;
						matcherTmp->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_mTmp, 2);
						std::vector<double> lisx;
						std::vector<double> lisy;
						double sumx = 0;
						double sumy = 0;
						for (size_t i = 0; i < KNN_mTmp.size(); i++)
						{
							if (KNN_mTmp[i][0].distance < ratio_thresh * KNN_mTmp[i][1].distance)
							{
								good_matchesTmp.push_back(KNN_mTmp[i][0]);
								// 这里有个bug回卡进来，进入副本或者切换放大招时偶尔触发
								lisx.push_back(((minMap.cols / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.x)*mapScale + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.x));
								lisy.push_back(((minMap.rows / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.y)*mapScale + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.y));
								sumx += lisx.back();
								sumy += lisy.back();
							}
						}
						cv::Mat img_matches, imgmap, imgminmap;
						drawKeypoints(someMap, KeyPointSomeMap, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
						drawKeypoints(img_object, KeyPointMiniMap, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

						drawMatches(img_object, KeyPointMiniMap, someMap, KeyPointSomeMap, good_matchesTmp, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

						if (lisx.size() <= 4 || lisy.size() <= 4)
						{
							isContinuity = false;
						}
						else
						{
							double meanx = sumx / lisx.size(); //均值
							double meany = sumy / lisy.size(); //均值
							cv::Point p = SPC(lisx, sumx, lisy, sumy);

							int x = (int)meanx;
							int y = (int)meany;

							x = p.x;
							y = p.y;

							pos = cv::Point(x + hisP[2].x - someSizeR, y + hisP[2].y - someSizeR);
						}
					}
				}
			}
			if (!isContinuity)
			{
				cv::Ptr<cv::xfeatures2d::SURF>& detectorAllMap = *(cv::Ptr<cv::xfeatures2d::SURF>*)_detectorAllMap;
				std::vector<cv::KeyPoint>& KeyPointAllMap = *(std::vector<cv::KeyPoint>*)_KeyPointAllMap;
				cv::Mat& DataPointAllMap = *(cv::Mat*)_DataPointAllMap;
				std::vector<cv::KeyPoint>& KeyPointMiniMap = *(std::vector<cv::KeyPoint>*)_KeyPointMiniMap;
				cv::Mat& DataPointMiniMap = *(cv::Mat*)_DataPointMiniMap;

				detectorAllMap->detectAndCompute(img_object, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);
				cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
				std::vector< std::vector<cv::DMatch> > KNN_m;
				//std::vector<DMatch> good_matches;
				matcher->knnMatch(DataPointMiniMap, DataPointAllMap, KNN_m, 2);

				std::vector<double> lisx;
				std::vector<double> lisy;
				double sumx = 0;
				double sumy = 0;
				for (size_t i = 0; i < KNN_m.size(); i++)
				{
					if (KNN_m[i][0].distance < ratio_thresh * KNN_m[i][1].distance)
					{
						//good_matches.push_back(KNN_m[i][0]);
						lisx.push_back(((img_object.cols / 2 - KeyPointMiniMap[KNN_m[i][0].queryIdx].pt.x)*mapScale + KeyPointAllMap[KNN_m[i][0].trainIdx].pt.x));
						lisy.push_back(((img_object.rows / 2 - KeyPointMiniMap[KNN_m[i][0].queryIdx].pt.y)*mapScale + KeyPointAllMap[KNN_m[i][0].trainIdx].pt.y));
						sumx += lisx.back();
						sumy += lisy.back();
					}
				}
				if (lisx.size() == 0 || lisy.size() == 0)
				{
					error_code = 4;//未能匹配到特征点
					return false;
				}
				else
				{
					pos = SPC(lisx, sumx, lisy, sumy);
				}
			}

			hisP[0] = hisP[1];
			hisP[1] = hisP[2];
			hisP[2] = pos;

			/******************************/

			x = (float)(pos.x);
			y = (float)(pos.y);
			
			error_code = 0;
			return true;
		}
		else
		{
			error_code = 3;//窗口画面为空
			return false;
		}

	}
	else
	{
		error_code = 2;//未能找到原神窗口句柄
		return false;
	}
}

bool CcvAutoTrack::GetUID(int &uid)
{
	// 判断原神窗口不存在直接返回false，不对参数做任何修改
	if (getGengshinImpactWnd())
	{
		getGengshinImpactRect();
		getGengshinImpactScreen();

		cv::Mat& giFrame = *(cv::Mat*)_giFrame;

		if (!giFrame.empty())
		{
			getPaimonRefMat();

			cv::Mat& giPaimonRef = *(cv::Mat*)_giPaimonRef;
			LoadGiMatchResource& giMatchResource = *(LoadGiMatchResource*)_giMatchResource;

			cv::Mat paimonTemplate;
			cv::resize(giMatchResource.PaimonTemplate, paimonTemplate, giPaimonRef.size());

			cv::Mat tmp;
			cv::matchTemplate(paimonTemplate, giPaimonRef, tmp, cv::TM_CCOEFF_NORMED);

			double minVal, maxVal;
			cv::Point minLoc, maxLoc;

			cv::minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);

			if (minVal < 0.36 || maxVal == 1)
			{
				error_code = 6;//未能匹配到派蒙
				return false;
			}

			error_code = 0;
			return true;
		}
		else
		{
			error_code = 3;//窗口画面为空
			return false;
		}
	}
	else
	{
		error_code = 2;//未能找到原神窗口句柄
		return false;
	}
}

int CcvAutoTrack::GetLastError()
{
	return error_code;
}

bool CcvAutoTrack::getGengshinImpactWnd()
{
	HWND& giHandle = *(HWND*)_giHandle;
	giHandle = FindWindowA("UnityWndClass", "原神");/* 对原神窗口的操作 */
	return (giHandle != NULL ? true : false);
}

void CcvAutoTrack::getGengshinImpactRect()
{
	HWND& giHandle = *(HWND*)_giHandle;
	RECT& giRect = *(RECT*)_giRect;
	RECT& giClientRect = *(RECT*)_giClientRect;
	cv::Size& giClientSize = *(cv::Size*)_giClientSize;

	GetWindowRect(giHandle, &giRect);
	GetClientRect(giHandle, &giClientRect);

	int x_offset = GetSystemMetrics(SM_CXDLGFRAME);
	int y_offset = GetSystemMetrics(SM_CYDLGFRAME) + GetSystemMetrics(SM_CYCAPTION);

	giClientSize.width = giClientRect.right - giClientRect.left;// -x_offset;
	giClientSize.height = giClientRect.bottom - giClientRect.top;// -y_offset;
}

void CcvAutoTrack::getGengshinImpactScreen()
{
	HWND& giHandle = *(HWND*)_giHandle;
	RECT& giRect = *(RECT*)_giRect;
	RECT& giClientRect = *(RECT*)_giClientRect;
	cv::Size& giClientSize = *(cv::Size*)_giClientSize;
	cv::Mat& giFrame = *(cv::Mat*)_giFrame;

	static HBITMAP	hBmp;
	BITMAP bmp;

	DeleteObject(hBmp);

	if (giHandle == NULL)return;

	//获取目标句柄的窗口大小RECT
	GetWindowRect(giHandle, &giRect);/* 对原神窗口的操作 */

	//获取目标句柄的DC
	HDC hScreen = GetDC(giHandle);/* 对原神窗口的操作 */
	HDC hCompDC = CreateCompatibleDC(hScreen);

	//获取目标句柄的宽度和高度
	int	nWidth = giRect.right - giRect.left;
	int	nHeight = giRect.bottom - giRect.top;

	//创建Bitmap对象
	hBmp = CreateCompatibleBitmap(hScreen, nWidth, nHeight);//得到位图

	SelectObject(hCompDC, hBmp); //不写就全黑
	BitBlt(hCompDC, 0, 0, nWidth, nHeight, hScreen, 0, 0, SRCCOPY);

	//释放对象
	DeleteDC(hScreen);
	DeleteDC(hCompDC);

	//类型转换
	GetObject(hBmp, sizeof(BITMAP), &bmp);

	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;

	//mat操作
	giFrame.create(cv::Size(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8U, nChannels));

	GetBitmapBits(hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, giFrame.data);

	giFrame = giFrame(cv::Rect(giClientRect.left, giClientRect.top, giClientSize.width, giClientSize.height));
}

void CcvAutoTrack::getPaimonRefMat()
{
	cv::Mat& giFrame = *(cv::Mat*)_giFrame;
	cv::Mat& giPaimonRef = *(cv::Mat*)_giPaimonRef;

	int Paimon_Rect_x = cvCeil(giFrame.cols*0.0135);
	int Paimon_Rect_y = cvCeil(giFrame.cols*0.006075);
	int Paimon_Rect_w = cvCeil(giFrame.cols*0.035);
	int Paimon_Rect_h = cvCeil(giFrame.cols*0.0406);

	giPaimonRef = giFrame(cv::Rect(Paimon_Rect_x, Paimon_Rect_y, Paimon_Rect_w, Paimon_Rect_h));
}

void CcvAutoTrack::getMiniMapRefMat()
{
	cv::Mat& giFrame = *(cv::Mat*)_giFrame;
	cv::Mat& giMiniMapRef = *(cv::Mat*)_giMiniMapRef;

	int MiniMap_Rect_x = cvCeil(giFrame.cols*0.032);
	int MiniMap_Rect_y = cvCeil(giFrame.cols*0.01);
	int MiniMap_Rect_w = cvCeil(giFrame.cols*0.11);
	int MiniMap_Rect_h = cvCeil(giFrame.cols*0.11);

	giMiniMapRef = giFrame(cv::Rect(MiniMap_Rect_x, MiniMap_Rect_y, MiniMap_Rect_w, MiniMap_Rect_h));
}


CcvAutoTrack* _at = new CcvAutoTrack();

bool __stdcall init()
{
	return _at->init();
}
bool __stdcall GetTransforn(float &x, float &y, float &a)
{
	return _at->GetTransforn(x, y, a);
}
bool __stdcall GetUID(int &uid)
{
	return _at->GetUID(uid);
}
int __stdcall GetLastErr()
{
	return _at->GetLastError();
}
bool __stdcall uninit()
{
	return _at->uninit();
}
