// Test2Project1.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "framework.h"
#include "Test2Project1.h"
#include "resource.h"

#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

using namespace cv;

// 这是导出变量的一个示例
extern "C" TEST2PROJECT1_API int nTest2Project1=0;

// 这是导出函数的一个示例。
TEST2PROJECT1_API int fnTest2Project1(void)
{
    return 0;
}

BOOL HBitmap2Mat(HBITMAP& _hBmp, cv::Mat& _mat)
{
	//BITMAP操作
	BITMAP bmp;
	GetObject(_hBmp, sizeof(BITMAP), &bmp);
	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
	//mat操作
	cv::Mat v_mat;
	v_mat.create(cvSize(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8UC3, nChannels));
	//v_mat.create(cvSize(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8UC3, nChannels));
	GetBitmapBits(_hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, v_mat.data);
	_mat = v_mat;
	if (nChannels == 4)
	{
		cv::cvtColor(v_mat, _mat, CV_RGBA2RGB);
		return TRUE;
	}
	return FALSE;
}

// 这是已导出类的构造函数。
GGenshinImpactMatch::GGenshinImpactMatch(void)
{
	gMat = new void*[gLoadDllResMaxCount];
	gHmp = new void*[gLoadDllResMaxCount];

	for (int i = 0; i < gLoadDllResMaxCount; i++)
	{
		gMat[i] = new cv::Mat;
		gHmp[i] = new HBITMAP;
	}

	if (!gLoadSource())throw "Dll Resource Image Load Faile!";
}

GGenshinImpactMatch::~GGenshinImpactMatch(void)
{
	for (int i = 0; i < gLoadDllResMaxCount; i++)
	{
		delete gMat[i];
		delete gHmp[i];
	}
	delete[] gMat;
	delete[] gHmp;
}

bool GGenshinImpactMatch::setSource(void * _source)
{
	cv::Mat& source = *(cv::Mat*)_source;


	return false;
}

bool GGenshinImpactMatch::setTarget(void * _target)
{
	cv::Mat& target = *(cv::Mat*)_target;

	return false;
}

void * GGenshinImpactMatch::getSource(int _id)
{
	if (0 < _id&&_id < gLoadDllResMaxCount)
	{
		return gMat[_id];
	}
	else
	{
		return gMat[0];
	}
	return nullptr;
}

int GGenshinImpactMatch::getMaxCount()
{
	return gLoadDllResMaxCount;
}


GPoint GGenshinImpactMatch::getPosition()
{
	return gPosition;
}

bool GGenshinImpactMatch::gLoadSource()
{
	if (gLoadSourceGetRes())
	{
		for (int i = 0; i < gLoadDllResMaxCount; i++)
		{
			if (gLoadSourceBitMap(gHmp[i], gMat[i]) == false)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

bool GGenshinImpactMatch::gLoadSourceGetRes()
{
	*(HBITMAP*)gHmp[0] = LoadBitmap(GetModuleHandle(L"TEST2PROJECT1.dll"), MAKEINTRESOURCE(IDB_BITMAP1));
	if (gHmp[0] == NULL)throw"LoadSource Get Res ID:IDB_BITMAP1 Dll HBitmap faile";
	*(HBITMAP*)gHmp[1] = LoadBitmap(GetModuleHandle(L"TEST2PROJECT1.dll"), MAKEINTRESOURCE(IDB_BITMAP2));
	if (gHmp[1] == NULL)throw"LoadSource Get Res ID:IDB_BITMAP2 Dll HBitmap faile";
	*(HBITMAP*)gHmp[2] = LoadBitmap(GetModuleHandle(L"TEST2PROJECT1.dll"), MAKEINTRESOURCE(IDB_BITMAP3));
	if (gHmp[2] == NULL)throw"LoadSource Get Res ID:IDB_BITMAP3 Dll HBitmap faile";
	*(HBITMAP*)gHmp[3] = LoadBitmap(GetModuleHandle(L"TEST2PROJECT1.dll"), MAKEINTRESOURCE(IDB_BITMAP4));
	if (gHmp[3] == NULL)throw"LoadSource Get Res ID:IDB_BITMAP4 Dll HBitmap faile";
	*(HBITMAP*)gHmp[4] = LoadBitmap(GetModuleHandle(L"TEST2PROJECT1.dll"), MAKEINTRESOURCE(IDB_BITMAP5));
	if (gHmp[4] == NULL)throw"LoadSource Get Res ID:IDB_BITMAP5 Dll HBitmap faile";
	*(HBITMAP*)gHmp[5] = LoadBitmap(GetModuleHandle(L"TEST2PROJECT1.dll"), MAKEINTRESOURCE(IDB_BITMAP6));
	if (gHmp[5] == NULL)throw"LoadSource Get Res ID:IDB_BITMAP6 Dll HBitmap faile";
	*(HBITMAP*)gHmp[6] = LoadBitmap(GetModuleHandle(L"TEST2PROJECT1.dll"), MAKEINTRESOURCE(IDB_BITMAP7));
	if (gHmp[6] == NULL)throw"LoadSource Get Res ID:IDB_BITMAP7 Dll HBitmap faile";
	*(HBITMAP*)gHmp[7] = LoadBitmap(GetModuleHandle(L"TEST2PROJECT1.dll"), MAKEINTRESOURCE(IDB_BITMAP8));
	if (gHmp[7] == NULL)throw"LoadSource Get Res ID:IDB_BITMAP8 Dll HBitmap faile";
	*(HBITMAP*)gHmp[8] = LoadBitmap(GetModuleHandle(L"TEST2PROJECT1.dll"), MAKEINTRESOURCE(IDB_BITMAP9));
	if (gHmp[8] == NULL)throw"LoadSource Get Res ID:IDB_BITMAP9 Dll HBitmap faile";
	*(HBITMAP*)gHmp[9] = LoadBitmap(GetModuleHandle(L"TEST2PROJECT1.dll"), MAKEINTRESOURCE(IDB_BITMAP10));
	if (gHmp[9] == NULL)throw"LoadSource Get Res ID:IDB_BITMAP10 Dll HBitmap faile";
	*(HBITMAP*)gHmp[10]= LoadBitmap(GetModuleHandle(L"TEST2PROJECT1.dll"), MAKEINTRESOURCE(IDB_BITMAP11));
	if (gHmp[10] == NULL)throw"LoadSource Get Res ID:IDB_BITMAP11 Dll HBitmap faile";
	*(HBITMAP*)gHmp[11] = LoadBitmap(GetModuleHandle(L"TEST2PROJECT1.dll"), MAKEINTRESOURCE(IDB_BITMAP12));
	if (gHmp[11] == NULL)throw"LoadSource Get Res ID:IDB_BITMAP12 Dll HBitmap faile";
	*(HBITMAP*)gHmp[12] = LoadBitmap(GetModuleHandle(L"TEST2PROJECT1.dll"), MAKEINTRESOURCE(IDB_BITMAP13));
	if (gHmp[12] == NULL)throw"LoadSource Get Res ID:IDB_BITMAP13 Dll HBitmap faile";
	*(HBITMAP*)gHmp[13] = LoadBitmap(GetModuleHandle(L"TEST2PROJECT1.dll"), MAKEINTRESOURCE(IDB_BITMAP14));
	if (gHmp[13] == NULL)throw"LoadSource Get Res ID:IDB_BITMAP14 Dll HBitmap faile";
	return true;
}

bool GGenshinImpactMatch::gLoadSourceBitMap(void * _hBmp, void * _mat)
{
	return HBitmap2Mat(*(HBITMAP*)_hBmp, *(cv::Mat*)_mat);
}

GSize::GSize():GSize(0,0){}

GSize::GSize(int _width, int _height):width(_width),height(_height){}

void GSize::toArray(int _array[2])
{
	_array[0] = width;
	_array[1] = height;
}

GPoint::GPoint() :GPoint(0, 0) {}

GPoint::GPoint(int _x, int _y) :x(_x), y(_y) {}

void GPoint::toArray(int _array[2])
{
	_array[0] = x;
	_array[1] = y;
}

