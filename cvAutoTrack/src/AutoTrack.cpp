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

#include "algorithms/algorithms.direction.h"
#include "algorithms/algorithms.rotation.h"

#include "match/match.star.h"
#include "match/match.uid.h"

#include "genshin/genshin.handle.h"
#include "genshin/genshin.screen.h"

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>


AutoTrack::AutoTrack()
{
	err.enableWirteFile();
	
	MapWorldOffset.x = MapWorldAbsOffset_X - WorldCenter_X;
	MapWorldOffset.y = MapWorldAbsOffset_Y - WorldCenter_Y;
	MapWorldScale = WorldScale;

	genshin_handle.config.capture = new Bitblt();
	genshin_handle.config.capture->init();
	
	filter = new Kalman();

	wForAfter.append(this, &AutoTrack::clear_error_logs, 0, "正常退出");
	wForAfter.append(this, &AutoTrack::getGengshinImpactWnd, 101, "未能找到原神窗口句柄");
	wForAfter.append(this, &AutoTrack::getGengshinImpactScreen, 103, "获取原神画面失败");

}

AutoTrack::~AutoTrack(void)
{
	delete genshin_handle.config.capture;
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
	if (genshin_handle.config.capture == nullptr)
	{
		genshin_handle.config.capture = new Bitblt();
		return true;
	}
	if (genshin_handle.config.capture->mode == Capture::Bitblt)
	{
		return true;
	}

	delete genshin_handle.config.capture;
	genshin_handle.config.capture = new Bitblt();

	return true;
}

bool AutoTrack::SetUseDx11CaptureMode()
{
	if (genshin_handle.config.capture == nullptr)
	{
		genshin_handle.config.capture = new Dxgi();
		return true;
	}

	if (genshin_handle.config.capture->mode == Capture::DirectX)
	{
		return true;
	}

	delete genshin_handle.config.capture;
	genshin_handle.config.capture = new Dxgi();

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

bool AutoTrack::SetDisableFileLog()
{
	err.disableWirteFile();
	return true;
}

bool AutoTrack::SetEnableFileLog()
{
	err.enableWirteFile();
	return true;
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
	switch (genshin_handle.config.capture->mode)
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

	return clear_error_logs();
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
	switch (genshin_handle.config.capture->mode)
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

	return clear_error_logs();
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
	if (getMiniMapRefMat_Bitblt()==false)
		{
			err = { 1001, "获取坐标时，没有识别到paimon" };
			return false;
		}

	if (giMiniMapRef.empty())
	{
		err = { 5, "原神小地图区域为空" };
		return false;
	}
	genshin_minimap.config.is_find_paimon = true;

	TianLi::Match::get_avatar_position(genshin_minimap, genshin_avatar_position);
	
	cv::Point2d pos = genshin_avatar_position.position;
	auto isConveying = genshin_avatar_position.config.is_coveying;
	auto isContinuity = genshin_avatar_position.config.is_continuity;

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
	if (getMiniMapRefMat_Bitblt() == false)
	{
		err = { 2001, "获取角色朝向时，没有识别到paimon" };
		return false;
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
	
	direction_calculation_config  config;
	direction_calculation(giAvatarRef, a , config);
	if (config.error)
	{
		err = config.err;
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
	if (getMiniMapRefMat_Bitblt() == false)
	{
		err = { 3001, "获取视角朝向时，没有识别到paimon" };
		return false;
	}

	rotation_calculation_config config;
	rotation_calculation(giMiniMapRef, a, config);
	if (config.error)
	{
		err = config.err;
		return false;
	}
	
	return clear_error_logs();
}

bool AutoTrack::GetStar(double& x, double& y, bool& isEnd)
{
	static bool isNotSee = false;
	static vector<cv::Point2d> pos;
	static int seeId = 0;
	static bool isStarVisible = false;
	
	int MAXLOOP = 0;
	bool isLoopMatch = false;
	cv::Mat tmp;
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	double scale = 1.3;
	bool isOnCity = false;

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
		
		if (getMiniMapRefMat_Bitblt() == false)
		{
			err = { 4001, "获取神瞳时，没有识别到paimon" };
			return false;
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
	if (wForAfter.run() == false)
	{
		return false;
	}

	if (getMiniMapRefMat_Bitblt() == false)
	{
		err = { 4001, "获取神瞳时，没有识别到paimon" };
		return false;
	}
	
	if (giMiniMapRef.empty())
	{
		err = { 5, "原神小地图区域为空" };
		return false;
	}
	
	//一个bug 未开游戏而先开应用，开游戏时触发
	cv::cvtColor(giMiniMapRef(cv::Rect(36, 36, giMiniMapRef.cols - 72, giMiniMapRef.rows - 72)),
		giStarRef, cv::COLOR_RGBA2GRAY);
	
	star_calculation_config config;
	config.is_on_city = genshin_avatar_position.config.is_on_city;
	star_calculation(giStarRef, jsonBuff, config);
	if (config.error)
	{
		err = config.err;
		return false;
	}
	
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

	if (genshin_handle.config.capture->mode == Capture::DirectX)
	{
		cv::cvtColor(giUIDRef, giUIDRef, cv::COLOR_RGBA2GRAY);
	}
	else
	{
		giUIDRef = channels[3];
	}
	
	uid_calculation_config config;
	uid_calculation(giUIDRef, uid, config);
	if(config.error)
	{
		err = config.err;
		return false;
	}
	
	return clear_error_logs();
}

bool AutoTrack::GetAllInfo(double& x, double& y, int& mapId, double& a, double& r, int& uid)
{
	if (!genshin_minimap.is_init_finish)
	{
		err = { 1, "没有初始化" };
		return false;
	}
	if (getMiniMapRefMat_Bitblt() == false)
	{
		err = { 1001, "获取所有信息时，没有识别到paimon" };
		return false;
	}

	if (giMiniMapRef.empty())
	{
		err = { 5, "原神小地图区域为空" };
		return false;
	}
	// x,y,mapId
	{
		genshin_minimap.config.is_find_paimon = true;

		TianLi::Match::get_avatar_position(genshin_minimap, genshin_avatar_position);

		cv::Point2d pos = genshin_avatar_position.position;
		auto isConveying = genshin_avatar_position.config.is_coveying;
		auto isContinuity = genshin_avatar_position.config.is_continuity;

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
	// a
	{
		direction_calculation_config  config;
		direction_calculation(giAvatarRef, a, config);
		if (config.error)
		{
			err = config.err;
			return false;
		}
	}
	// r
	{
		rotation_calculation_config config;
		rotation_calculation(giMiniMapRef, a, config);
		if (config.error)
		{
			err = config.err;
			return false;
		}
	}

	if (getUIDRefMat() == false)
	{
		return false;
	}
	// uid
	{
		std::vector<cv::Mat> channels;

		split(giUIDRef, channels);

		if (genshin_handle.config.capture->mode == Capture::DirectX)
		{
			cv::cvtColor(giUIDRef, giUIDRef, cv::COLOR_RGBA2GRAY);
		}
		else
		{
			giUIDRef = channels[3];
		}

		uid_calculation_config config;
		uid_calculation(giUIDRef, uid, config);
		if (config.error)
		{
			err = config.err;
			return false;
		}
	}

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
	TianLi::Genshin::get_genshin_handle(genshin_handle);
	if (genshin_handle.handle == NULL)
	{
		err = { 10,"无效句柄或指定句柄所指向窗口不存在" };
		return false;
	}
	
	genshin_handle.config.capture->setHandle(genshin_handle.handle);

	return true;
}

bool AutoTrack::getGengshinImpactScreen()
{
	TianLi::Genshin::get_genshin_screen(genshin_handle, genshin_screen);
	//giFrame = genshin_screen.img_screen;
	if (genshin_screen.img_screen.empty())
	{
		err = { 433, "截图失败" };
		return false;
	}
	return true;
}

bool AutoTrack::getMiniMapRefMat_Bitblt()
{
	genshin_minimap.img_minimap = giFrame(genshin_minimap.rect_minimap);
	giMiniMapRef = giFrame(genshin_minimap.rect_minimap);

	
	if (genshin_handle.config.capture->mode == Capture::DirectX || genshin_handle.config.is_force_used_no_alpha)
	{
		genshin_screen.config.is_used_alpha = false;
	}
	else
	{
		genshin_screen.config.is_used_alpha = true;
	}
	
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
	giUIDRef = genshin_screen.img_uid;

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