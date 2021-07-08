#include "pch.h"
#include "GenshinImpactScreen.h"

GenshinImpactScreen::GenshinImpactScreen()
{
	getNowTimeMs();
	updateTime();
}

GenshinImpactScreen * GenshinImpactScreen::getInstance()
{
	static GenshinImpactScreen GIS_Instance = GenshinImpactScreen();
	return &GIS_Instance;
}

Mat GenshinImpactScreen::getGenshinImpactScreen()
{
	getNowTimeMs();
	if (NowTimeMs - LastTimeMs >= 24)
	{
		
		return getGenshinImpactScreen_NewGet();
	}
	else
	{
		return getGenshinImpactScreen_GetAfter();
	}

}

void GenshinImpactScreen::updateTime()
{
	LastTimeMs = NowTimeMs;
}

void GenshinImpactScreen::getNowTimeMs()
{
	NowTimeMs = clock();
}

Mat GenshinImpactScreen::getGenshinImpactScreen_GetAfter()
{
	return Screen;
}

Mat GenshinImpactScreen::getGenshinImpactScreen_NewGet()
{
	static HBITMAP	hBmp;
	BITMAP bmp;
	Mat giFarme;

	DeleteObject(hBmp);

	if (giHandle == NULL)
	{
		err = 12;//窗口句柄失效
		return false;
	}
	if (!IsWindow(giHandle))
	{
		err = 12;//窗口句柄失效
		return false;
	}
	//获取目标句柄的窗口大小RECT
	GetWindowRect(giHandle, &giRect);/* 对原神窗口的操作 */

	//获取目标句柄的DC
	HDC hScreen = GetDC(giHandle);/* 对原神窗口的操作 */
	HDC hCompDC = CreateCompatibleDC(hScreen);

	//获取目标句柄的宽度和高度
	int	nWidth = (int)((screen_scale) * (giRect.right - giRect.left));
	int	nHeight = (int)((screen_scale) * (giRect.bottom - giRect.top));

	//创建Bitmap对象
	hBmp = CreateCompatibleBitmap(hScreen, nWidth, nHeight);//得到位图

	SelectObject(hCompDC, hBmp); //不写就全黑
	BitBlt(hCompDC, 0, 0, nWidth, nHeight, hScreen, 0, 0, SRCCOPY);

	//释放对象
	DeleteDC(hScreen);
	DeleteDC(hCompDC);

	//类型转换
	//这里获取位图的大小信息,事实上也是兼容DC绘图输出的范围
	GetObject(hBmp, sizeof(BITMAP), &bmp);

	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;

	//mat操作
	giFrame.create(cv::Size(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8U, nChannels));

	GetBitmapBits(hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, giFrame.data);

	giFrame = giFrame(cv::Rect(giClientRect.left, giClientRect.top, giClientSize.width, giClientSize.height));

	if (giFrame.empty())
	{
		err = 3;
		return giFrame;
	}
	return giFrame;
}
