#include "pch.h"
#include "AutoTrack.h"
#include "ErrorCode.h"
#include "capture/dxgi/Dxgi.h"
#include "capture/bitblt/Bitblt.h"
//#include "capture/gdi/Gdi.h"
#include "filter/kalman/Kalman.h"
#include "filter/smooth/Smooth.h"
//#include "filter/mean/Mean.h"
//#include "filter/median/Median.h"
#include "utils/Utils.h"
#include "match/Match.h"

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>


AutoTrack::AutoTrack()
{
	genshin_handle.handle = 0;
	

	MapWorldOffset.x = MapWorldAbsOffset_X - WorldCenter_X;
	MapWorldOffset.y = MapWorldAbsOffset_Y - WorldCenter_Y;
	MapWorldScale = WorldScale;

	capture = new Bitblt();
	capture->init();
	
	filter = new Kalman();

	wForAfter.append(this, &AutoTrack::clear_error_logs, 0, "正常退出");
	wForAfter.append(this, &AutoTrack::getGengshinImpactWnd, 101, "未能找到原神窗口句柄");
	wForAfter.append(this, &AutoTrack::getGengshinImpactRect, 102, "获取原神窗口大小失败");
	wForAfter.append(this, &AutoTrack::getGengshinImpactScreen, 103, "获取原神画面失败");

}

AutoTrack::~AutoTrack(void)
{
	delete capture;
}

bool AutoTrack::init()
{
	if (!genshin_minimap.is_init_finish)
	{
		genshin_minimap.is_run_init_start = true;
		TianLi::Match::get_avatar_position(genshin_minimap, genshin_avatar_position);
		genshin_minimap.is_run_init_start = false;
		
		genshin_minimap.is_init_finish = true;
	}
	return genshin_minimap.is_init_finish;
}

bool AutoTrack::uninit()
{
	if (genshin_minimap.is_init_finish)
	{
		genshin_minimap.is_run_uninit_start = true;
		TianLi::Match::get_avatar_position(genshin_minimap, genshin_avatar_position);
		genshin_minimap.is_run_uninit_start = false;
		
		genshin_minimap.is_init_finish = false;
	}
	return !genshin_minimap.is_init_finish;
}

bool AutoTrack::SetUseBitbltCaptureMode()
{
	if (capture == nullptr)
	{
		capture = new Bitblt();
		return true;
	}
	if (capture->mode == Capture::Bitblt)
	{
		return true;
	}

	delete capture;
	capture = new Bitblt();

	return true;
}

bool AutoTrack::SetUseDx11CaptureMode()
{
	if (capture == nullptr)
	{
		capture = new Dxgi();
		return true;
	}

	if (capture->mode == Capture::DirectX)
	{
		return true;
	}

	delete capture;
	capture = new Dxgi();

	return true;
}

bool AutoTrack::SetHandle(long long int handle)
{
	if (handle == 0)
	{
		genshin_handle.config.is_auto_find_genshin = true;
		return true;
	}
	else
	{
		genshin_handle.config.is_auto_find_genshin = false;
		genshin_handle.handle = (HWND)handle;
	}
	return IsWindow(genshin_handle.handle);
}

bool AutoTrack::SetWorldCenter(double x, double y)
{
	UserWorldOrigin_X = x;
	UserWorldOrigin_Y = y;
	return true;
}

bool AutoTrack::SetWorldScale(double scale)
{
	UserWorldScale = scale;
	return true;
}

bool AutoTrack::startServe()
{
	return false;
}

bool AutoTrack::stopServe()
{
	return false;
}

bool AutoTrack::GetVersion(char* version_buff, int buff_size)
{
	if (version_buff == NULL || buff_size < 1)
	{
		err = { 291,"缓存区为空指针或是缓存区大小为小于1" };
		return false;
	}
	if (TianLi::Version::build_version.size() > buff_size)
	{
		err = { 292,"缓存区大小不足" };
		return false;
	}
	strcpy_s(version_buff, buff_size, TianLi::Version::build_version.c_str());
	return true;
}

bool AutoTrack::GetCompileTime(char* time_buff, int buff_size)
{
	if (time_buff == NULL || buff_size < 1)
	{
		err = { 291,"缓存区为空指针或是缓存区大小为小于1" };
		return false;
	}
	if (TianLi::Version::build_time.size() > buff_size)
	{
		err = { 292,"缓存区大小不足" };
		return false;
	}
	strcpy_s(time_buff, buff_size, TianLi::Version::build_time.c_str());	
	return true;
}

bool AutoTrack::DebugCapture()
{
	if (giFrame.empty())
	{
		err = { 251,"画面为空" };
		return false;
	}
	cv::Mat out_info_img = giFrame.clone();
	switch (capture->mode)
	{
	case Capture::Bitblt:
	{
		// 绘制paimon Rect
		cv::rectangle(out_info_img, genshin_screen.config.rect_paimon, cv::Scalar(0, 0, 255), 2);
		// 绘制miniMap Rect
		cv::rectangle(out_info_img, genshin_screen.config.rect_minimap, cv::Scalar(0, 0, 255), 2);
		cv::Rect Avatar = Area_Avatar_mayArea;
		Avatar.x += genshin_screen.config.rect_minimap.x;
		Avatar.y += genshin_screen.config.rect_minimap.y;

		// 绘制avatar Rect
		cv::rectangle(out_info_img, Avatar, cv::Scalar(0, 0, 255), 2);
		// 绘制UID Rect
		cv::rectangle(out_info_img, Area_UID_mayArea, cv::Scalar(0, 0, 255), 2);
		break;
	}
	case Capture::DirectX:
	{
		// 绘制paimon Rect
		cv::rectangle(out_info_img, Area_Paimon_mayArea, cv::Scalar(0, 0, 255), 2);
		// 绘制miniMap Rect
		cv::rectangle(out_info_img, Area_Minimap_mayArea, cv::Scalar(0, 0, 255), 2);
		cv::Rect Avatar = Area_Avatar_mayArea;
		Avatar.x += Area_Minimap_mayArea.x;
		Avatar.y += Area_Minimap_mayArea.y;

		// 绘制avatar Rect
		cv::rectangle(out_info_img, Avatar, cv::Scalar(0, 0, 255), 2);
		// 绘制UID Rect
		cv::rectangle(out_info_img, Area_UID_mayArea, cv::Scalar(0, 0, 255), 2);
	}
	}
	
	bool rel = cv::imwrite("Capture.png", out_info_img);

	if (!rel)
	{
		err = { 252,"保存画面失败 " };
		return false;
	}	
	return true;
}

bool AutoTrack::DebugCapturePath(const char* path_buff, int buff_size)
{
	if (path_buff == NULL || buff_size < 1)
	{
		err = { 251,"路径缓存区为空指针或是路径缓存区大小为小于1" };
		return false;
	}

	if (giFrame.empty())
	{
		err = { 252,"画面为空" };
		return false;
	}
	cv::Mat out_info_img = giFrame.clone();
	switch (capture->mode)
	{
	case Capture::Bitblt:
	{
		// 绘制paimon Rect
		cv::rectangle(out_info_img, genshin_screen.config.rect_paimon, cv::Scalar(0, 0, 255), 2);
		// 绘制miniMap Rect
		cv::rectangle(out_info_img, genshin_screen.config.rect_minimap, cv::Scalar(0, 0, 255), 2);
		cv::Rect Avatar = Area_Avatar_mayArea;
		Avatar.x += genshin_screen.config.rect_minimap.x;
		Avatar.y += genshin_screen.config.rect_minimap.y;

		// 绘制avatar Rect
		cv::rectangle(out_info_img, Avatar, cv::Scalar(0, 0, 255), 2);
		// 绘制UID Rect
		cv::rectangle(out_info_img, Area_UID_mayArea, cv::Scalar(0, 0, 255), 2);
		break;
	}
	case Capture::DirectX:
	{
		// 绘制paimon Rect
		cv::rectangle(out_info_img, Area_Paimon_mayArea, cv::Scalar(0, 0, 255), 2);
		// 绘制miniMap Rect
		cv::rectangle(out_info_img, Area_Minimap_mayArea, cv::Scalar(0, 0, 255), 2);
		cv::Rect Avatar = Area_Avatar_mayArea;
		Avatar.x += Area_Minimap_mayArea.x;
		Avatar.y += Area_Minimap_mayArea.y;

		// 绘制avatar Rect
		cv::rectangle(out_info_img, Avatar, cv::Scalar(0, 0, 255), 2);
		// 绘制UID Rect
		cv::rectangle(out_info_img, Area_UID_mayArea, cv::Scalar(0, 0, 255), 2);
	}
	}

	bool rel = cv::imwrite(path_buff, out_info_img);

	if (!rel)
	{
		err = { 252,std::string("保存画面失败，请检查文件路径是否合法")+std::string(path_buff)};
		return false;
	}
	return true;
}

bool AutoTrack::GetTransformOfMap(double& x, double& y, double& a, int& mapId)
{
	double x2 = 0, y2 = 0, a2 = 0;
	int mapId2 = 0;
	if (!genshin_minimap.is_init_finish)
	{
		init();//初始化
	}

	/*
	分别判断是否成功获取，避免前一个error_code被后一个error_code覆盖
	而导致本函数返回false（表示失败）但error_code为0（表示成功）。
	*/
	if (!GetPositionOfMap(x2, y2, mapId2))
	{
		return false;
	}
	if (!GetDirection(a2))
	{
		return false;
	}
	x = x2;
	y = y2;
	a = a2;
	mapId = mapId2;
	return clear_error_logs();
}

bool AutoTrack::GetPosition(double& x, double& y)
{
	if (wForAfter.run() == false)
	{
		return false;
	}
	if (!genshin_minimap.is_init_finish)
	{
		err = { 1, "没有初始化" };
		return false;
	}

	if (capture->mode == Capture::Bitblt)
	{
		
		if (getMiniMapRefMat_Bitblt()==false)
		{
			err = { 1001, "Bitblt模式下获取坐标时，没有识别到paimon，建议使用DX模式" };
			return false;
		}
	}
	else
	{
		cv::Rect paimon_rect;
		if (!check_paimon(paimon_rect))
		{
			err = { 1002, "获取坐标时，没有识别到paimon" };
			return false;
		}

		getMiniMapRefMat_Dx();
	}

	if (giMiniMapRef.empty())
	{
		err = { 5, "原神小地图区域为空" };
		return false;
	}
	genshin_minimap.config.is_find_paimon = true;

	TianLi::Match::get_avatar_position(genshin_minimap, genshin_avatar_position);
	
	cv::Point2d pos = genshin_avatar_position.position;
	isConveying = genshin_avatar_position.config.is_cov;
	isContinuity = genshin_avatar_position.config.is_con;

	cv::Point2d filt_pos;
	if (isConveying || !isContinuity)
	{
		filt_pos = filter->re_init_filterting(pos);
	}
	else
	{
		filt_pos = filter->filterting(pos);
	}

	cv::Point2d abs_pos = TianLi::Utils::TransferTianLiAxes(filt_pos * MapAbsScale, MapWorldOffset, MapWorldScale);

	cv::Point2d user_pos = TianLi::Utils::TransferUserAxes(abs_pos, UserWorldOrigin_X, UserWorldOrigin_Y, UserWorldScale);

	x = user_pos.x;
	y = user_pos.y;

	return clear_error_logs();
}

bool AutoTrack::GetPositionOfMap(double& x, double& y, int& mapId)
{
	mapId = 0;
	cv::Point2d pos_tr;
	bool res_pos = GetPosition(x, y);
	if (res_pos != true)
	{
		return false;
	}

	pos_tr = TianLi::Utils::TransferUserAxes_Tr(cv::Point2d(x, y), UserWorldOrigin_X, UserWorldOrigin_Y, UserWorldScale);
	pos_tr = TianLi::Utils::TransferTianLiAxes_Tr(pos_tr, MapWorldOffset, MapWorldScale);
	pos_tr = pos_tr / MapAbsScale;

	//cv::Size size_DiXiaCengYan(1250, 1016);
	//cv::Size size_YuanXiaGong(2400, 2401); 5544 
	//cv::Size size_YuanXiaGong_Un(800, 450);
	cv::Rect rect_DiXiaCengYan(0, 0, 1250, 1016);
	cv::Rect rect_YuanXiaGong(0, 5543, 2400, 2401);
	{
		double _x = pos_tr.x;
		double _y = pos_tr.y;
		// 渊下宫
		if (_x > 0 && _x <= 0 + 2400 && _y > 5543 && _y <= 5543 + 2401)
		{
			mapId = 1;
		}
		// 地下层岩
		if (_x > 0 && _x <= 0 + 1250 && _y > 0 && _y <= 0 + 1016)
		{
			mapId = 2;
		}

		switch (mapId)
		{
		case 0:
		{
			break;
		}
		case 1:
		{
			_x = _x - 0;
			_y = _y - 5543;
			cv::Point2d pos = TianLi::Utils::TransferTianLiAxes(cv::Point2d(_x, _y), cv::Point2d(0, 0), MapWorldScale);
			pos = TianLi::Utils::TransferUserAxes(pos, 0, 0, 1);
			x = pos.x;
			y = pos.y;
			break;
		}
		case 2:
		{
			_x = _x - 0;
			_y = _y - 0;
			cv::Point2d pos = TianLi::Utils::TransferTianLiAxes(cv::Point2d(_x, _y), cv::Point2d(0, 0), MapWorldScale);
			pos = TianLi::Utils::TransferUserAxes(pos, 0, 0, 1);
			x = pos.x;
			y = pos.y;
			break;
		}
		default:
			break;
		}
	}
	return clear_error_logs();
}

bool AutoTrack::GetDirection(double& a)
{
	if (wForAfter.run() == false)
	{
		return false;
	}
	
	if (capture->mode == Capture::Bitblt)
	{
		if (getMiniMapRefMat_Bitblt() == false)
		{
			err = { 2001, "Bitblt模式下获取角色朝向时，没有识别到paimon" };
			return false;
		}
	}
	else
	{
		cv::Rect paimon_rect;
		if (!check_paimon(paimon_rect))
		{
			err = { 2002 ,"获取角色朝向时，没有识别到Paimon" };
			return false;
		}

		getMiniMapRefMat();
	}

	if (!getAvatarRefMat())
	{
		return false;
	}

	if (giAvatarRef.empty())
	{
		err = { 11,"原神角色小箭头区域为空" };
		return false;
	}

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
			AvatarKeyPoint.push_back(cv::Point2d(boundRect[i].x + boundRect[i].width / 2.0, boundRect[i].y + boundRect[i].height / 2.0));
		}

		AvatarKeyPointLine[0] = TianLi::Utils::dis(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
		AvatarKeyPointLine[1] = TianLi::Utils::dis(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
		AvatarKeyPointLine[2] = TianLi::Utils::dis(AvatarKeyPoint[1] - AvatarKeyPoint[0]);

		if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[2] && AvatarKeyPointLine[1] >= AvatarKeyPointLine[2])
		{
			AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
			AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
		}
		if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[1] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[1])
		{
			AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[0]);
			AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[2]);
		}
		if (AvatarKeyPointLine[1] >= AvatarKeyPointLine[0] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[0])
		{
			AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[1]);
			AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[2]);
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
			AvatarKeyPoint.push_back(cv::Point2d(boundRect[i].x + boundRect[i].width / 2.0, boundRect[i].y + boundRect[i].height / 2.0));
		}
		// 两点取中点
		AvatarKeyPoint.push_back((AvatarKeyPoint[0] + AvatarKeyPoint[1]) / 2.0);
		// 中点基于图片中心对称位置
		cv::Point2d line = center - AvatarKeyPoint[2];
		a = TianLi::Utils::Line2Angle(line);
	}
	else
	{
		err = { 9,"提取小箭头特征误差过大" };
		return false;
	}
	return clear_error_logs();
}

bool AutoTrack::GetRotation(double& a)
{
	if (wForAfter.run() == false)
	{
		return false;
	}

	if (capture->mode == Capture::Bitblt)
	{
		if (getMiniMapRefMat_Bitblt() == false)
		{
			err = { 3001, "Bitblt模式下获取视角朝向时，没有识别到paimon" };
			return false;
		}
	}
	else
	{
		cv::Rect paimon_rect;
		if (!check_paimon(paimon_rect))
		{
			err = { 3002 ,"获取视角朝向时，没有识别到Paimon" };
			return false;
		}

		getMiniMapRefMat_Dx();
	}
	
	cv::Mat img_object(giMiniMapRef(cv::Rect(40, 40, giMiniMapRef.cols - 80, giMiniMapRef.rows - 80)));

	if (img_object.channels() != 4)
	{
		err = { 3003,"获取视角朝向时，原神小地图区域没有取到透明通道" };
		return false;
	}

	if (capture->mode == Capture::DirectX)
	{
		err = { 3004,"DX模式下，原神小地图区域无法取到透明通道" };
		return false;
	}

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
		err = { 3005 ,"获取视角朝向时，没有提取出视角扇形区域" };
		return false;
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

	return clear_error_logs();
}

bool AutoTrack::GetStar(double& x, double& y, bool& isEnd)
{
	static bool isNotSee = false;
	static vector<cv::Point2d> pos;
	static int seeId = 0;

	int MAXLOOP = 0;
	bool isLoopMatch = false;
	cv::Mat tmp;
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	double scale = 1.3;

	if (isNotSee)
	{
		if (isOnCity)
		{
			scale = 0.8667;
		}
		x = pos[seeId].x * scale;
		y = pos[seeId].y * scale;
		seeId++;
		if (seeId == pos.size())
		{
			isEnd = true;
			isNotSee = false;
		}
		else
		{
			isEnd = false;
			isNotSee = true;
		}
		return clear_error_logs();
	}
	else
	{
		pos.clear();
		seeId = 0;

		if (wForAfter.run() == false)
		{
			return false;
		}

		getPaimonRefMat();
		
		if (capture->mode == Capture::Bitblt)
		{
			if (getMiniMapRefMat_Bitblt() == false)
			{
				err = { 4001, "Bitblt模式下获取神瞳时，没有识别到paimon" };
				return false;
			}
		}
		else
		{
			cv::Rect paimon_rect;
			if (!check_paimon(paimon_rect))
			{
				err = { 4002, "获取神瞳时，没有识别到paimon" };
				return false;
			}

			getMiniMapRefMat_Dx();
		}
		
		if (giMiniMapRef.empty())
		{
			err = { 5, "原神小地图区域为空" };
			return false;
		}

		cv::cvtColor(giMiniMapRef(cv::Rect(36, 36, giMiniMapRef.cols - 72, giMiniMapRef.rows - 72)),
			giStarRef, cv::COLOR_RGBA2GRAY);


		matchTemplate(res.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
		minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
		//cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif
		if (maxVal < 0.66)
		{
			isStarVisible = false;
		}
		else
		{
			isLoopMatch = true;
			isStarVisible = true;
			pos.push_back(cv::Point2d(maxLoc) -
				cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) +
				cv::Point2d(res.StarTemplate.cols / 2, res.StarTemplate.rows / 2));
		}

		while (isLoopMatch)
		{
			giStarRef(cv::Rect(maxLoc.x, maxLoc.y, res.StarTemplate.cols, res.StarTemplate.rows)) = cv::Scalar(0, 0, 0);
			matchTemplate(res.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
			minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
			//cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif

			if (maxVal < 0.66)
			{
				isLoopMatch = false;
			}
			else
			{
				pos.push_back(cv::Point2d(maxLoc) -
					cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) +
					cv::Point2d(res.StarTemplate.cols / 2, res.StarTemplate.rows / 2));
			}

			MAXLOOP > 10 ? isLoopMatch = false : MAXLOOP++;
		}


		if (isStarVisible == true)
		{
			if (isOnCity)
			{
				scale = 0.8667;
			}
			x = pos[seeId].x * scale;
			y = pos[seeId].y * scale;
			seeId++;
			if (seeId == pos.size())
			{
				isEnd = true;
				isNotSee = false;
			}
			else
			{
				isEnd = false;
				isNotSee = true;
			}
			return clear_error_logs();
		}
		err = { 601,"获取神瞳失败，未确定原因" };
		return false;
	}
}

bool AutoTrack::GetStarJson(char* jsonBuff)
{
	int MAXLOOP = 0;
	bool isLoopMatch = false;
	cv::Mat tmp;
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	vector<cv::Point2d> pos;
	double scale = 1.3;


	if (wForAfter.run() == false)
	{
		return false;
	}

	getPaimonRefMat();
	
	if (capture->mode == Capture::Bitblt)
	{
		if (getMiniMapRefMat_Bitblt() == false)
		{
			err = { 4001, "Bitblt模式下获取神瞳时，没有识别到paimon" };
			return false;
		}
	}
	else
	{
		cv::Rect paimon_rect;
		if (!check_paimon(paimon_rect))
		{
			err = { 4002, "获取神瞳时，没有识别到paimon" };
			return false;
		}

		getMiniMapRefMat_Dx();
	}
	
	if (giMiniMapRef.empty())
	{
		err = { 5, "原神小地图区域为空" };
		return false;
	}
	
	//一个bug 未开游戏而先开应用，开游戏时触发
	cv::cvtColor(giMiniMapRef(cv::Rect(36, 36, giMiniMapRef.cols - 72, giMiniMapRef.rows - 72)),
		giStarRef, cv::COLOR_RGBA2GRAY);


	matchTemplate(res.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
	minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
	//cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif
	if (maxVal < 0.66)
	{
		isStarVisible = false;
	}
	else
	{
		isLoopMatch = true;
		isStarVisible = true;
		pos.push_back(cv::Point2d(maxLoc) -
			cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) +
			cv::Point2d(res.StarTemplate.cols / 2, res.StarTemplate.rows / 2));
	}

	while (isLoopMatch)
	{
		giStarRef(cv::Rect(maxLoc.x, maxLoc.y, res.StarTemplate.cols, res.StarTemplate.rows)) = cv::Scalar(0, 0, 0);
		matchTemplate(res.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
		minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
		//cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif
		if (maxVal < 0.66)
		{
			isLoopMatch = false;
		}
		else
		{
			pos.push_back(cv::Point2d(maxLoc) -
				cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) +
				cv::Point2d(res.StarTemplate.cols / 2, res.StarTemplate.rows / 2));
		}

		MAXLOOP > 10 ? isLoopMatch = false : MAXLOOP++;
	}

	if (isOnCity)
	{
		scale = 0.8667;
	}

	if (isStarVisible == true)
	{

		sprintf_s(jsonBuff, 1024, "{\"n\": %d ,\"list\":[", static_cast<int>(pos.size()));//[123,12],[123,53]]}")
		for (int i = 0; i < pos.size(); i++)
		{
			char buff[99];
			if (i == 0)
			{
				sprintf_s(buff, 99, "[ %lf , %lf ]", pos[i].x * scale, pos[i].y * scale);
			}
			else
			{
				sprintf_s(buff, 99, ",[ %lf , %lf ]", pos[i].x * scale, pos[i].y * scale);
			}
			strncat_s(jsonBuff, 1024, buff, 99);
		}
		strncat_s(jsonBuff, 1024, "]}", 3);
		
		return clear_error_logs();
	}
	sprintf_s(jsonBuff, 99, "{\"n\": 0 ,\"list\":[]}");
	
	return clear_error_logs();
}

bool AutoTrack::GetUID(int& uid)
{
	if (wForAfter.run() == false)
	{
		return false;
	}
	if (getUIDRefMat() == false)
	{
		return false;
	}

	std::vector<cv::Mat> channels;

	split(giUIDRef, channels);

	if (capture->mode == Capture::DirectX)
	{
		cv::cvtColor(giUIDRef, giUIDRef, cv::COLOR_RGBA2GRAY);
	}
	else
	{
		giUIDRef = channels[3];
	}

	int _uid = 0;
	int _NumBit[9] = { 0 };

	int bitCount = 1;
	cv::Mat matchTmp;
	cv::Mat Roi;
	cv::Mat checkUID = res.UID;

#ifdef _DEBUG
	//if (checkUID.rows > Roi.rows)
	//{
	//	cv::resize(checkUID, checkUID, cv::Size(), Roi.rows/ checkUID.rows);
	//}
#endif
	giUIDRef.copyTo(Roi);

	if (checkUID.rows > Roi.rows)
	{
		cv::resize(Roi, Roi, cv::Size(cvRound(1.0 * checkUID.rows / Roi.rows * Roi.cols), checkUID.rows), 0);
	}

	cv::matchTemplate(Roi, checkUID, matchTmp, cv::TM_CCOEFF_NORMED);

	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	//寻找最佳匹配位置
	cv::minMaxLoc(matchTmp, &minVal, &maxVal, &minLoc, &maxLoc);
	if (maxVal > 0.75)
	{
		int x_uid_ = maxLoc.x + checkUID.cols + 7;
		int y_uid_ = 0;
		double tmplis[10] = { 0 };
		int tmplisx[10] = { 0 };
		for (int p = 8; p >= 0; p--)
		{
			_NumBit[p] = 0;
			for (int i = 0; i < 10; i++)
			{
				cv::Rect r(x_uid_, y_uid_, res.UIDnumber[i].cols + 2, giUIDRef.rows);//180-46/9->140/9->16->16*9=90+54=144
				if (x_uid_ + r.width > giUIDRef.cols)
				{
					r = cv::Rect(giUIDRef.cols - res.UIDnumber[i].cols - 2, y_uid_, res.UIDnumber[i].cols + 2, giUIDRef.rows);
				}

				cv::Mat numCheckUID = res.UIDnumber[i];
				Roi = giUIDRef(r);

				cv::matchTemplate(Roi, numCheckUID, matchTmp, cv::TM_CCOEFF_NORMED);

				double minVali, maxVali;
				cv::Point minLoci, maxLoci;
				//寻找最佳匹配位置
				cv::minMaxLoc(matchTmp, &minVali, &maxVali, &minLoci, &maxLoci);

				tmplis[i] = maxVali;
				tmplisx[i] = maxLoci.x + numCheckUID.cols - 1;
				if (maxVali > 0.91)
				{
					_NumBit[p] = i;
					x_uid_ = x_uid_ + maxLoci.x + numCheckUID.cols - 1;
					break;
				}
				if (i == 10 - 1)
				{
					_NumBit[p] = TianLi::Utils::getMaxID(tmplis, 10);
					x_uid_ = x_uid_ + tmplisx[_NumBit[p]];
				}
			}
		}
	}
	_uid = 0;
	for (int i = 0; i < 9; i++)
	{
		_uid += _NumBit[i] * bitCount;
		bitCount = bitCount * 10;
	}
	if (_uid == 0)
	{
		err = { 8,"未能在UID区域检测到有效UID" };
		return false;
	}
	uid = _uid;	
	
	return clear_error_logs();
}

bool AutoTrack::GetInfoLoadPicture(char* path, int& uid, double& x, double& y, double& a)
{
	UNREFERENCED_PARAMETER(path);
	UNREFERENCED_PARAMETER(uid);
	UNREFERENCED_PARAMETER(x);
	UNREFERENCED_PARAMETER(y);
	UNREFERENCED_PARAMETER(a);
	return false;
}

bool AutoTrack::GetInfoLoadVideo(char* path, char* pathOutFile)
{
	UNREFERENCED_PARAMETER(path);
	UNREFERENCED_PARAMETER(pathOutFile);
	return false;
}

int AutoTrack::GetLastError()
{
#ifdef _DEBUG
	std::cout << err;
#endif
	return err;
}

int AutoTrack::GetLastErrMsg(char* msg_buff, int buff_size)
{
	if (msg_buff == NULL || buff_size < 1)
	{
		err = { 291,"缓存区为空指针或是缓存区大小为小于1" };
		return false;
	}
	std::string msg = err.getLastErrorMsg();
	if (msg.size() > buff_size)
	{
		err = { 292,"缓存区大小不足" };
		return false;
	}
	strcpy_s(msg_buff, buff_size, msg.c_str());
	return true;
}

int AutoTrack::GetLastErrJson(char* json_buff, int buff_size)
{
	if (json_buff == NULL || buff_size < 1)
	{
		err = { 291,"缓存区为空指针或是缓存区大小为小于1" };
		return false;
	}
	std::string msg = err.toJson();
	if (msg.size() > buff_size)
	{
		err = { 292,"缓存区大小不足" };
		return false;
	}
	strcpy_s(json_buff, buff_size, msg.c_str());
	return true;
}

bool AutoTrack::getGengshinImpactWnd()
{
	if (genshin_handle.config.is_auto_find_genshin)
	{
		LPCWSTR giWindowName = { L"原神" };
		/* 对原神窗口的操作 */
		giWindowName = L"原神";
		auto handle = FindWindowW(L"UnityWndClass", giWindowName);
		if (handle == NULL)
		{
			giWindowName = L"Genshin Impact";
			handle = FindWindowW(L"UnityWndClass", giWindowName); /* 匹配名称：原神 */
		}
		if (handle == NULL)
		{
			giWindowName = L"云·原神";
			handle = FindWindowW(L"Qt5152QWindowIcon", giWindowName); /* 匹配名称：云·原神 */
		}
		if (handle == NULL)
		{
			giWindowName = L"??";
			handle = FindWindowW(L"UnityWndClass", giWindowName); /* 匹配名称：?? */
		}
		if (handle == NULL)
		{
			giWindowName = L"\u539F\u795E";
			handle = FindWindowW(L"UnityWndClass", giWindowName); /* 匹配名称：原神 */
		}
		if (handle == NULL)
		{
			giWindowName = L"\uC6D0\uC2E0";
			handle = FindWindowW(L"UnityWndClass", giWindowName); /* 匹配名称：?? */
		}
		if (handle == NULL)
		{
			giWindowName = L"原神-调试";
			handle = FindWindowW(NULL, giWindowName); /* 匹配名称：原神-调试 */
		}
		
		if (handle == NULL)
		{
			err = { 10,"无效句柄或指定句柄所指向窗口不存在" };
			return false;
		}
		genshin_handle.handle = handle;
	}
	else
	{
		if (IsWindow(genshin_handle.handle))
		{
			return true;
		}
		else
		{
			err = { 10,"无效句柄或指定句柄所指向窗口不存在" };
			return false;
		}
	}

	capture->setHandle(genshin_handle.handle);

	return (genshin_handle.handle != NULL ? true : false);
}

bool AutoTrack::getGengshinImpactRect()
{
	if (!GetWindowRect(genshin_handle.handle, &giRect))
	{
		err = { 12,"窗口句柄失效" };
		return false;
	}
	if (!GetClientRect(genshin_handle.handle, &giClientRect))
	{
		err = { 12,"窗口句柄失效" };
		return false;
	}

	//获取屏幕缩放比例
	getGengshinImpactScale();

	giClientSize.width = (int)(screen_scale * (giClientRect.right - giClientRect.left));
	giClientSize.height = (int)(screen_scale * (giClientRect.bottom - giClientRect.top));
	int x = giClientSize.width;
	int y = giClientSize.height;
	
	double f = 1, fx = 1, fy = 1;

	if (static_cast<double>(x) / static_cast<double>(y) == 16.0 / 9.0)
	{
		genshin_handle.size_frame = cv::Size(1920, 1080);
	}
	else if (static_cast<double>(x) / static_cast<double>(y) > 16.0 / 9.0)
	{

		//高型，以宽为比例

		// x = (y * 16) / 9;
		f = y / 1080.0;
		//将giFrame缩放到1920*1080的比例
		fx = x / f;
		// 将图片缩放
		genshin_handle.size_frame = cv::Size(static_cast<int>(fx), 1080);

	}
	else if (static_cast<double>(x) / static_cast<double>(y) < 16.0 / 9.0)
	{

		//宽型，以高为比例

		// x = (y * 16) / 9;
		f = x / 1920.0;
		//将giFrame缩放到1920*1080的比例
		fy = y / f;
		// 将图片缩放
		genshin_handle.size_frame = cv::Size(1920, static_cast<int>(fy));
	}
	x = genshin_handle.size_frame.width;
	y = genshin_handle.size_frame.height;
	// 派蒙可能性区域计算参数
	int paimon_mayArea_left = 0;
	int paimon_mayArea_top = 0;
	int paimon_mayArea_width = static_cast<int>(x * 0.10);
	int paimon_mayArea_height = static_cast<int>(y * 0.10);
	// 派蒙可能性区域
	//cv::Rect Area_Paimon_mayArea(
	genshin_handle.rect_paimon_maybe=cv::Rect(
		paimon_mayArea_left,
		paimon_mayArea_top,
		paimon_mayArea_width,
		paimon_mayArea_height);
	//genshin_handle.rect_paimon_maybe = Area_Paimon_mayArea;

	// 小地图标定可能性区域计算参数
	int miniMap_Cailb_mayArea_left = static_cast<int>(x * 0.10);
	int miniMap_Cailb_mayArea_top = 0;
	int miniMap_Cailb_mayArea_width = static_cast<int>(x * 0.10);
	int miniMap_Cailb_mayArea_height = static_cast<int>(y * 0.10);
	// 小地图标定可能性区域
	cv::Rect Area_MiniMap_Cailb_mayArea(
		miniMap_Cailb_mayArea_left,
		miniMap_Cailb_mayArea_top,
		miniMap_Cailb_mayArea_width,
		miniMap_Cailb_mayArea_height);
	genshin_handle.rect_minimap_cailb_maybe = Area_MiniMap_Cailb_mayArea;

	// 小地图可能性区域计算参数
	int miniMap_mayArea_left = 0;
	int miniMap_mayArea_top = 0;
	int miniMap_mayArea_width = static_cast<int>(x * 0.18);
	int miniMap_mayArea_height = static_cast<int>(y * 0.22);
	// 小地图可能性区域
	cv::Rect Area_MiniMap_mayArea(
		miniMap_mayArea_left,
		miniMap_mayArea_top,
		miniMap_mayArea_width,
		miniMap_mayArea_height);
	genshin_handle.rect_minimap_maybe = Area_MiniMap_mayArea;

	// UID可能性区域计算参数
	int UID_mayArea_left = static_cast<int>(x * 0.88);
	int UID_mayArea_top = static_cast<int>(y * 0.97);
	int UID_mayArea_width = x - UID_mayArea_left;
	int UID_mayArea_height = y - UID_mayArea_top;
	// UID可能性区域
	//cv::Rect Area_UID_mayArea(
	genshin_handle.rect_uid_maybe =cv::Rect(
		UID_mayArea_left,
		UID_mayArea_top,
		UID_mayArea_width,
		UID_mayArea_height);
	//genshin_handle.rect_uid_maybe = Area_UID_mayArea;
	
	int UID_Rect_x = cvCeil(x - x * (1.0 - 0.865));
	int UID_Rect_y = cvCeil(y - 1080.0 * (1.0 - 0.9755));
	int UID_Rect_w = cvCeil(1920 * 0.11);
	int UID_Rect_h = cvCeil(1920 * 0.0938 * 0.11);


	genshin_handle.rect_uid = cv::Rect(UID_Rect_x, UID_Rect_y, UID_Rect_w, UID_Rect_h);

	// 左侧已获取物品可能性区域计算参数
	int leftGetItems_mayArea_left = static_cast<int>(x * 0.570);
	int leftGetItems_mayArea_top = static_cast<int>(y * 0.250);
	int leftGetItems_mayArea_width = static_cast<int>(x * 0.225);
	int leftGetItems_mayArea_height = static_cast<int>(y * 0.500);
	// 左侧已获取物品可能性区域
	cv::Rect Area_LeftGetItems_mayArea(
		leftGetItems_mayArea_left,
		leftGetItems_mayArea_top,
		leftGetItems_mayArea_width,
		leftGetItems_mayArea_height);
	genshin_handle.rect_left_give_items_maybe = Area_LeftGetItems_mayArea;

	// 右侧可捡取物品可能性区域计算参数
	int rightGetItems_mayArea_left = static_cast<int>(x * 0.050);
	int rightGetItems_mayArea_top = static_cast<int>(y * 0.460);
	int rightGetItems_mayArea_width = static_cast<int>(x * 0.160);
	int rightGetItems_mayArea_height = static_cast<int>(y * 0.480);
	// 右侧可捡取物品可能性区域
	cv::Rect Area_RightGetItems_mayArea(
		rightGetItems_mayArea_left,
		rightGetItems_mayArea_top,
		rightGetItems_mayArea_width,
		rightGetItems_mayArea_height);
	genshin_handle.rect_right_pick_items_maybe = Area_RightGetItems_mayArea;

	return true;
}

bool AutoTrack::getGengshinImpactScale()
{
#ifdef _DEBUG
	//std::cout << "-> getGengshinImpactScale()" << std::endl;
#endif
	// TODO: get window handle on window dirver
	HWND hWnd = GetDesktopWindow();
	HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

	// 获取监视器逻辑宽度与高度
	MONITORINFOEX miex{};
	miex.cbSize = sizeof(miex);
	GetMonitorInfo(hMonitor, &miex);
	int cxLogical = (miex.rcMonitor.right - miex.rcMonitor.left);
	//int cyLogical = (miex.rcMonitor.bottom - miex.rcMonitor.top);

	// 获取监视器物理宽度与高度
	DEVMODE dm{};
	dm.dmSize = sizeof(dm);
	dm.dmDriverExtra = 0;
	EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm);
	int cxPhysical = dm.dmPelsWidth;
	//int cyPhysical = dm.dmPelsHeight;

	double horzScale = ((double)cxPhysical / (double)cxLogical);
	screen_scale = horzScale;

	return true;
}

bool AutoTrack::getGengshinImpactScreen()
{
	cv::Mat Frame;
	if (capture->capture(Frame))
	{
		giFrame = Frame;
		cv::resize(giFrame, giFrame, genshin_handle.size_frame);
		if (capture->mode == Capture::DirectX)
		{
			// 此处应对高文字缩放时不能正常获取到全部画面
			// 此处应为获取去除可能存在的标题栏的区域
			// 但是有问题暂时去除
			//int w = giClientRect.right - giClientRect.left;
			//int h = giClientRect.bottom - giClientRect.top;
			//int x = 0;
			//int y = giFrame.rows - h;
			//giFrame = giFrame(cv::Rect(x, y, w, h));
		}

		cv::resize(giFrame, genshin_screen.img_screen, genshin_handle.size_frame);
		
		giFrame = genshin_screen.img_screen;
		
		genshin_screen.rect_client = cv::Rect(giRect.left, giRect.top, giClientRect.right - giClientRect.left, giClientRect.bottom - giClientRect.top);

		// 获取maybe区域
		genshin_screen.img_paimon_maybe = giFrame(genshin_handle.rect_paimon_maybe);
		genshin_screen.img_minimap_cailb_maybe = giFrame(genshin_handle.rect_minimap_cailb_maybe);
		genshin_screen.img_minimap_maybe = giFrame(genshin_handle.rect_minimap_maybe);
		genshin_screen.img_uid_maybe = giFrame(genshin_handle.rect_uid_maybe);
		genshin_screen.img_left_give_items_maybe = giFrame(genshin_handle.rect_left_give_items_maybe);
		genshin_screen.img_right_pick_items_maybe = giFrame(genshin_handle.rect_right_pick_items_maybe);

		genshin_screen.config.rect_paimon_maybe = genshin_handle.rect_paimon_maybe;
		genshin_screen.config.rect_minimap_cailb_maybe = genshin_handle.rect_minimap_cailb_maybe;
		genshin_screen.config.rect_minimap_maybe = genshin_handle.rect_minimap_maybe;


		genshin_screen.img_uid = giFrame(genshin_handle.rect_uid);
		
		return true;
	}
	else
	{
		err = { 433, "截图失败" };
		return false;
	}
}
bool AutoTrack::getPaimonRefMat()
{
	int& x = giFrame.cols, & y = giFrame.rows;
	double f = 1, fx = 1, fy = 1;

	if (static_cast<double>(x) / static_cast<double>(y) == 16.0 / 9.0)
	{

		//正常，不做处理
		if (x != 1920 && y != 1080)
		{
			cv::resize(giFrame, giFrame, cv::Size(1920, 1080));
		}
	}
	else if (static_cast<double>(x) / static_cast<double>(y) > 16.0 / 9.0)
	{

		//高型，以宽为比例

		// x = (y * 16) / 9;
		f = y / 1080.0;
		//将giFrame缩放到1920*1080的比例
		fx = x / f;
		// 将图片缩放
		cv::resize(giFrame, giFrame, cv::Size(static_cast<int>(fx), 1080));

	}
	else if (static_cast<double>(x) / static_cast<double>(y) < 16.0 / 9.0)
	{

		//宽型，以高为比例

		// x = (y * 16) / 9;
		f = x / 1920.0;
		//将giFrame缩放到1920*1080的比例
		fy = y / f;
		// 将图片缩放
		cv::resize(giFrame, giFrame, cv::Size(1920, static_cast<int>(fy)));
	}
	else
	{
		//出错

	}

	int Paimon_Rect_x = cvCeil(1920 * 0.0135);
	int Paimon_Rect_y = cvCeil(1920 * 0.006075);
	int Paimon_Rect_w = cvCeil(1920 * 0.035);
	int Paimon_Rect_h = cvCeil(1920 * 0.0406);

	if (giFrame.cols == 3440 && giFrame.rows == 1440)
	{
		cv::resize(giFrame, giFrame, cv::Size(2560, 1080));
	}

	if (giFrame.cols == 2560 && giFrame.rows == 1080)
	{
		Paimon_Rect_x = cvCeil(1920 * 0.0135 + 72);
		Paimon_Rect_y = cvCeil(1920 * 0.006075);
		Paimon_Rect_w = cvCeil(1920 * 0.035);
		Paimon_Rect_h = cvCeil(1920 * 0.0406);
	}

	// 派蒙可能性区域计算参数
	int paimon_mayArea_left = 0;
	int paimon_mayArea_top = 0;
	int paimon_mayArea_width = static_cast<int>(x * 0.10);
	int paimon_mayArea_height = static_cast<int>(y * 0.10);
	// 派蒙可能性区域
	Area_Paimon_mayArea = cv::Rect(
		paimon_mayArea_left,
		paimon_mayArea_top,
		paimon_mayArea_width,
		paimon_mayArea_height);

	//giPaimonRef = giFrame(cv::Rect(Paimon_Rect_x, Paimon_Rect_y, Paimon_Rect_w, Paimon_Rect_h));
	giPaimonRef = giFrame(Area_Paimon_mayArea);

#ifdef _DEBUG
	cv::namedWindow("Paimon", cv::WINDOW_FREERATIO);
	cv::imshow("Paimon", giPaimonRef);
	cv::waitKey(1);
	//std::cout << "Show Paimon" << std::endl;
#endif
	return true;
}

bool AutoTrack::getMiniMapRefMat_Dx()
{
	int& x = giFrame.cols, & y = giFrame.rows;
	double f = 1, fx = 1, fy = 1;

	if (static_cast<double>(x) / static_cast<double>(y) == 16.0 / 9.0)
	{

		//正常，不做处理
		if (x != 1920 && y != 1080)
		{
			cv::resize(giFrame, giFrame, cv::Size(1920, 1080));
		}
	}
	else if (static_cast<double>(x) / static_cast<double>(y) > 16.0 / 9.0)
	{

		//高型，以宽为比例

		// x = (y * 16) / 9;
		f = y / 1080.0;
		//将giFrame缩放到1920*1080的比例
		fx = x / f;
		// 将图片缩放
		cv::resize(giFrame, giFrame, cv::Size(static_cast<int>(fx), 1080));

	}
	else if (static_cast<double>(x) / static_cast<double>(y) < 16.0 / 9.0)
	{

		//宽型，以高为比例

		// x = (y * 16) / 9;
		f = x / 1920.0;
		//将giFrame缩放到1920*1080的比例
		fy = y / f;
		// 将图片缩放
		cv::resize(giFrame, giFrame, cv::Size(1920, static_cast<int>(fy)));
	}
	else
	{
		//出错

	}

	int MiniMap_Rect_x = cvRound(1920 * 0.033);
	int MiniMap_Rect_y = cvRound(1920 * 0.01);
	int MiniMap_Rect_w = cvRound(1920 * 0.11);
	int MiniMap_Rect_h = cvRound(1920 * 0.11);

	if (giFrame.cols == 3440 && giFrame.rows == 1440)
	{
		cv::resize(giFrame, giFrame, cv::Size(2560, 1080));
	}

	if (giFrame.cols == 2560 && giFrame.rows == 1080)
	{
		MiniMap_Rect_x = cvRound(1920 * 0.033 + 72);
		MiniMap_Rect_y = cvRound(1920 * 0.01);
		MiniMap_Rect_w = cvRound(1920 * 0.11);
		MiniMap_Rect_h = cvRound(1920 * 0.11);
	}
	Area_Minimap_mayArea = cv::Rect(
		MiniMap_Rect_x,
		MiniMap_Rect_y,
		MiniMap_Rect_w,
		MiniMap_Rect_h);

	giMiniMapRef = giFrame(Area_Minimap_mayArea);

#ifdef _DEBUG
		cv::namedWindow("MiniMap", cv::WINDOW_FREERATIO);
		cv::imshow("MiniMap", giMiniMapRef);
	cv::waitKey(1);
	//std::cout << "Show MiniMap" << std::endl;
#endif
	return true;
}

bool AutoTrack::getMiniMapRefMat_Bitblt()
{
	int& x = giFrame.cols, & y = giFrame.rows;
	double f = 1, fx = 1, fy = 1;

	if (static_cast<double>(x) / static_cast<double>(y) == 16.0 / 9.0)
	{

		//正常，不做处理
		if (x != 1920 && y != 1080)
		{
			cv::resize(giFrame, giFrame, cv::Size(1920, 1080));
		}
	}
	else if (static_cast<double>(x) / static_cast<double>(y) > 16.0 / 9.0)
	{

		//高型，以宽为比例

		// x = (y * 16) / 9;
		f = y / 1080.0;
		//将giFrame缩放到1920*1080的比例
		fx = x / f;
		// 将图片缩放
		cv::resize(giFrame, giFrame, cv::Size(static_cast<int>(fx), 1080));

	}
	else if (static_cast<double>(x) / static_cast<double>(y) < 16.0 / 9.0)
	{

		//宽型，以高为比例

		// x = (y * 16) / 9;
		f = x / 1920.0;
		//将giFrame缩放到1920*1080的比例
		fy = y / f;
		// 将图片缩放
		cv::resize(giFrame, giFrame, cv::Size(1920, static_cast<int>(fy)));
	}
	else
	{
		//出错

	}

	int MiniMap_Rect_x = cvRound(1920 * 0.033);
	int MiniMap_Rect_y = cvRound(1920 * 0.01);
	int MiniMap_Rect_w = cvRound(1920 * 0.11);
	int MiniMap_Rect_h = cvRound(1920 * 0.11);

	if (giFrame.cols == 3440 && giFrame.rows == 1440)
	{
		cv::resize(giFrame, giFrame, cv::Size(2560, 1080));
	}

	if (giFrame.cols == 2560 && giFrame.rows == 1080)
	{
		MiniMap_Rect_x = cvRound(1920 * 0.033 + 72);
		MiniMap_Rect_y = cvRound(1920 * 0.01);
		MiniMap_Rect_w = cvRound(1920 * 0.11);
		MiniMap_Rect_h = cvRound(1920 * 0.11);
	}
	genshin_minimap.rect_minimap = cv::Rect(
		MiniMap_Rect_x,
		MiniMap_Rect_y,
		MiniMap_Rect_w,
		MiniMap_Rect_h);
	genshin_minimap.img_minimap = giFrame(genshin_minimap.rect_minimap);
	giMiniMapRef = giFrame(genshin_minimap.rect_minimap);


	// 检测派蒙 -> 检测小地图标定 -> 计算小地图坐标

	if (TianLi::Match::check_paimon(genshin_screen, genshin_paimon) == false)
	{
		err = { 40001,"Bitblt模式下检测派蒙失败" };
		return false;
	}
	if (genshin_paimon.is_visial == false)
	{
		err = { 50001,"Bitblt模式下未能检测到派蒙" };
		return false;
	}

	genshin_screen.config.rect_paimon = genshin_paimon.rect_paimon;
	genshin_screen.config.is_handle_mode = genshin_paimon.is_handle_mode;

	if (TianLi::Match::match_minimap_cailb(genshin_screen, genshin_minimap_cailb) == false)
	{
		err = { 40002,"Bitblt模式下检测小地图标定失败" };
		return false;
	}
	if (genshin_minimap_cailb.is_visial == false)
	{
		err = { 50002,"Bitblt模式下未能检测到小地图标定" };
		return false;
	}

	genshin_screen.config.rect_minimap_cailb = genshin_minimap_cailb.rect_minimap_cailb;

	if (TianLi::Match::splite_minimap(genshin_screen, genshin_minimap) == false)
	{
		err = { 40003, "Bitblt模式下计算小地图区域失败" };
		return false;
	}

	genshin_screen.config.rect_minimap = genshin_minimap.rect_minimap;
	giMiniMapRef = genshin_minimap.img_minimap;

#ifdef _DEBUG
	cv::namedWindow("MiniMap", cv::WINDOW_FREERATIO);
	cv::imshow("MiniMap", giMiniMapRef);
	cv::waitKey(1);
#endif
	return true;
}

bool AutoTrack::getUIDRefMat()
{
	int& x = giFrame.cols, & y = giFrame.rows;
	double f = 1, fx = 1, fy = 1;

	if (static_cast<double>(x) / static_cast<double>(y) == 16.0 / 9.0)
	{

		//正常，不做处理
		if (x != 1920 && y != 1080)
		{
			cv::resize(giFrame, giFrame, cv::Size(1920, 1080));
		}
	}
	else if (static_cast<double>(x) / static_cast<double>(y) > 16.0 / 9.0)
	{

		//高型，以宽为比例

		// x = (y * 16) / 9;
		f = y / 1080.0;
		//将giFrame缩放到1920*1080的比例
		fx = x / f;
		// 将图片缩放
		cv::resize(giFrame, giFrame, cv::Size(static_cast<int>(fx), 1080));

	}
	else if (static_cast<double>(x) / static_cast<double>(y) < 16.0 / 9.0)
	{

		//宽型，以高为比例

		// x = (y * 16) / 9;
		f = x / 1920.0;
		//将giFrame缩放到1920*1080的比例
		fy = y / f;
		// 将图片缩放
		cv::resize(giFrame, giFrame, cv::Size(1920, static_cast<int>(fy)));
	}
	else
	{
		//出错

	}

	int UID_Rect_x = cvCeil(giFrame.cols - giFrame.cols * (1.0 - 0.865));
	int UID_Rect_y = cvCeil(giFrame.rows - 1080.0 * (1.0 - 0.9755));
	int UID_Rect_w = cvCeil(1920 * 0.11);
	int UID_Rect_h = cvCeil(1920 * 0.0938 * 0.11);

	Area_UID_mayArea = cv::Rect(
		UID_Rect_x,
		UID_Rect_y,
		UID_Rect_w,
		UID_Rect_h);

	giUIDRef = giFrame(Area_UID_mayArea);

#ifdef _DEBUG
	cv::namedWindow("UID", cv::WINDOW_FREERATIO);
	cv::imshow("UID", giUIDRef);
	cv::waitKey(1);
	//std::cout << "Show UID" << std::endl;
#endif
	return true;
}

bool AutoTrack::getAvatarRefMat()
{
	if (giMiniMapRef.empty())
	{
		err = { 21,"获取角色区域时，小地图区域为空"};
		return false;
	}
	int Avatar_Rect_x = cvRound(giMiniMapRef.cols * 0.4);
	int Avatar_Rect_y = cvRound(giMiniMapRef.rows * 0.4);
	int Avatar_Rect_w = cvRound(giMiniMapRef.cols * 0.2);
	int Avatar_Rect_h = cvRound(giMiniMapRef.rows * 0.2);

	Area_Avatar_mayArea = cv::Rect(
		Avatar_Rect_x,
		Avatar_Rect_y,
		Avatar_Rect_w,
		Avatar_Rect_h);

	giAvatarRef = giMiniMapRef(Area_Avatar_mayArea);

#ifdef _DEBUG
	cv::namedWindow("Avatar", cv::WINDOW_FREERATIO);
	cv::imshow("Avatar", giAvatarRef);
	cv::waitKey(1);
#endif
	return true;
}

bool AutoTrack::clear_error_logs()
{
	err = { 0,"调用成功"};
	return true;
}

bool AutoTrack::check_paimon(cv::Rect& paimon_rect)
{
	static cv::Mat paimon_template;
	static std::vector<cv::Mat> split_paimon_template;
	static bool is_first = true;
	if (is_first)
	{
		res.PaimonTemplate.copyTo(paimon_template);
		cv::split(paimon_template, split_paimon_template);
		is_first = false;
	}

	getPaimonRefMat();

	std::vector<cv::Mat>  split_paimon;
	cv::split(giPaimonRef, split_paimon);

	cv::Mat template_result;
	cv::Mat object = split_paimon[3];

	const double check_match_paimon_params_dx = 0.60;
	static double check_match_paimon_param = check_match_paimon_params;
	
	if (capture->mode == Capture::DirectX)
	{
		cv::cvtColor(giPaimonRef, object, cv::COLOR_RGBA2GRAY);
		check_match_paimon_param = check_match_paimon_params_dx;
	}
	else
	{
		check_match_paimon_param = check_match_paimon_params;
	}

	cv::matchTemplate(object, split_paimon_template[3], template_result, cv::TM_CCOEFF_NORMED);

	double paimon_match_minVal, paimon_match_maxVal;
	cv::Point paimon_match_minLoc, paimon_match_maxLoc;
	cv::minMaxLoc(template_result, &paimon_match_minVal, &paimon_match_maxVal, &paimon_match_minLoc, &paimon_match_maxLoc);

#ifdef _DEBUG
	cv::namedWindow("paimon match result", cv::WINDOW_FREERATIO);
	cv::imshow("paimon match result", template_result);
#endif

	if (paimon_match_maxVal < check_match_paimon_param || paimon_match_maxVal == 1)
	{
		err = { 6,"未能匹配到派蒙" };
		return false;
	}
	paimon_rect = cv::Rect(paimon_match_maxLoc, paimon_template.size());
	
	return clear_error_logs();
}

