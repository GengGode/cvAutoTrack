#include "pch.h"
#include "AutoTrack.h"

#include "ErrorCode.h"
#include "resources/Resources.h"

#include "capture/dxgi/Dxgi.h"
#include "capture/bitblt/Bitblt.h"
#include "utils/Utils.h"
#include "utils/workflow/utils.workflow.h"

#include "algorithms/algorithms.direction.h"
#include "algorithms/algorithms.rotation.h"

#include "match/match.star.h"
#include "match/match.uid.h"

#include "genshin/genshin.h"

#include "version/Version.h"

Resources& res = Resources::getInstance();
ErrorCode& err = ErrorCode::getInstance();

AutoTrack::AutoTrack()
{
	err.enableWirteFile();
	MapWorldOffset.x = MapWorldAbsOffset_X - WorldCenter_X;
	MapWorldOffset.y = MapWorldAbsOffset_Y - WorldCenter_Y;
	MapWorldScale = WorldScale;

	genshin_handle.config.capture = new Bitblt();
	genshin_handle.config.capture->init();
	
	workflow_for_begin = new TianLi::Utils::Workflow();
	workflow_for_begin->append(clear_error_logs, 0, "正常退出");
	workflow_for_begin->append([this]() {return getGengshinImpactWnd(); }, 101, "未能找到原神窗口句柄");
	workflow_for_begin->append([this]() {return getGengshinImpactScreen(); }, 103, "获取原神画面失败");
}

AutoTrack::~AutoTrack(void)
{
	delete genshin_handle.config.capture;
	delete workflow_for_begin;
}

bool AutoTrack::init()
{
	if (!genshin_minimap.is_init_finish)
	{
		genshin_minimap.is_run_init_start = true;
		TianLi::Genshin::Match::get_avatar_position(genshin_minimap, genshin_avatar_position);
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
		TianLi::Genshin::Match::get_avatar_position(genshin_minimap, genshin_avatar_position);
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

bool AutoTrack::ImportMapBlock(int id_x, int id_y, const char* image_data, int image_data_size, int image_width, int image_height)
{
	if (image_data_size != image_width * image_height * 4)
	{
		err = { 9001,"传入图片通道不对应" };
		return false;
	}
	auto map_block = cv::Mat(image_height, image_width, CV_8UC4, (void*)image_data, cv::Mat::AUTO_STEP);
	if (map_block.empty())
	{
		err = { 9002,"传入图片为空 " };
		return false;
	}
	//res.set_map_block(id_x, id_y, map_block);
	UNREFERENCED_PARAMETER(id_x);
	UNREFERENCED_PARAMETER(id_y);

	return false;
}

bool AutoTrack::ImportMapBlockCenter(int x, int y)
{
	UNREFERENCED_PARAMETER(x);
	UNREFERENCED_PARAMETER(y);
	return false;
}

bool AutoTrack::ImportMapBlockCenterScale(int x, int y, double scale)
{
	UNREFERENCED_PARAMETER(x);
	UNREFERENCED_PARAMETER(y);
	UNREFERENCED_PARAMETER(scale);
	return false;
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

bool AutoTrack::GetMapIsEmbedded()
{
	return res.map_is_embedded();
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
		cv::rectangle(out_info_img, genshin_paimon.rect_paimon, cv::Scalar(0, 0, 255), 2);
		// 绘制miniMap Rect
		cv::rectangle(out_info_img, genshin_minimap.rect_minimap, cv::Scalar(0, 0, 255), 2);
		cv::Rect Avatar = genshin_minimap.rect_avatar;
		Avatar.x += genshin_minimap.rect_minimap.x;
		Avatar.y += genshin_minimap.rect_minimap.y;

		// 绘制avatar Rect
		cv::rectangle(out_info_img, Avatar, cv::Scalar(0, 0, 255), 2);
		// 绘制UID Rect
		cv::rectangle(out_info_img, genshin_handle.rect_uid, cv::Scalar(0, 0, 255), 2);
		break;
	}
	case Capture::DirectX:
	{
		// 绘制paimon Rect
		cv::rectangle(out_info_img, genshin_paimon.rect_paimon, cv::Scalar(0, 0, 255), 2);
		// 绘制miniMap Rect			   
		cv::rectangle(out_info_img, genshin_minimap.rect_minimap, cv::Scalar(0, 0, 255), 2);
		cv::Rect Avatar = genshin_minimap.rect_avatar;
		Avatar.x += genshin_minimap.rect_minimap.x;
		Avatar.y += genshin_minimap.rect_minimap.y;

		// 绘制avatar Rect
		cv::rectangle(out_info_img, Avatar, cv::Scalar(0, 0, 255), 2);
		// 绘制UID Rect
		cv::rectangle(out_info_img, genshin_handle.rect_uid, cv::Scalar(0, 0, 255), 2);
	}
	}

	auto last_time_stream = std::stringstream();
	last_time_stream << genshin_screen.last_time;
	std::string last_time_str = last_time_stream.str();

	cv::putText(out_info_img, last_time_str, cv::Point(out_info_img.cols/2, out_info_img.rows/2), 1,1, cv::Scalar(128,128,128, 255), 1, 16, 0);
	auto err_msg_str = err.toJson();
	cv::putText(out_info_img, err_msg_str, cv::Point(0, out_info_img.rows / 2 - 100), 1, 1, cv::Scalar(128, 128, 128, 128), 1, 16, 0);

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
		cv::rectangle(out_info_img, genshin_paimon.rect_paimon, cv::Scalar(0, 0, 255), 2);
		// 绘制miniMap Rect		      
		cv::rectangle(out_info_img, genshin_minimap.rect_minimap, cv::Scalar(0, 0, 255), 2);
		cv::Rect Avatar = genshin_minimap.rect_avatar;
		Avatar.x += genshin_minimap.rect_minimap.x;
		Avatar.y += genshin_minimap.rect_minimap.y;

		// 绘制avatar Rect
		cv::rectangle(out_info_img, Avatar, cv::Scalar(0, 0, 255), 2);
		// 绘制UID Rect
		cv::rectangle(out_info_img, genshin_handle.rect_uid, cv::Scalar(0, 0, 255), 2);
		break;
	}
	case Capture::DirectX:
	{
		// 绘制paimon Rect
		cv::rectangle(out_info_img, genshin_paimon.rect_paimon, cv::Scalar(0, 0, 255), 2);
		// 绘制miniMap Rect			      
		cv::rectangle(out_info_img, genshin_minimap.rect_minimap, cv::Scalar(0, 0, 255), 2);
		cv::Rect Avatar = genshin_minimap.rect_avatar;
		Avatar.x += genshin_minimap.rect_minimap.x;
		Avatar.y += genshin_minimap.rect_minimap.y;

		// 绘制avatar Rect
		cv::rectangle(out_info_img, Avatar, cv::Scalar(0, 0, 255), 2);
		// 绘制UID Rect
		cv::rectangle(out_info_img, genshin_handle.rect_uid, cv::Scalar(0, 0, 255), 2);
	}
	}

	auto last_time_stream = std::stringstream();
	last_time_stream << genshin_screen.last_time;
	std::string last_time_str = last_time_stream.str();

	cv::putText(out_info_img, last_time_str, cv::Point(out_info_img.cols / 2, out_info_img.rows / 2), 1, 1, cv::Scalar(128, 128, 128, 255), 1, 16, 0);
	auto err_msg_str = err.toJson();
	cv::putText(out_info_img, err_msg_str, cv::Point(0, out_info_img.rows / 2 - 100), 1, 1, cv::Scalar(128, 128, 128, 128), 1, 16, 0);

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
	if (workflow_for_begin->run() == false)
	{
		return false;
	}
	if (!genshin_minimap.is_init_finish)
	{
		err = { 1, "没有初始化" };
		return false;
	}
	if (getMiniMapRefMat()==false)
		{
			//err = { 1001, "获取坐标时，没有识别到paimon" };
			return false;
		}

	if (genshin_minimap.img_minimap.empty())
	{
		err = { 5, "原神小地图区域为空" };
		return false;
	}
	genshin_minimap.config.is_find_paimon = true;

	TianLi::Genshin::Match::get_avatar_position(genshin_minimap, genshin_avatar_position);
	
	cv::Point2d pos = genshin_avatar_position.position;

	cv::Point2d abs_pos = TianLi::Utils::TransferTianLiAxes(pos * MapAbsScale, MapWorldOffset, MapWorldScale);
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
		
	auto tr_res = TianLi::Utils::TransferTianLiAxes(pos_tr.x, pos_tr.y);
	cv::Point2d pos = TianLi::Utils::TransferTianLiAxes(cv::Point2d(tr_res.first.x, tr_res.first.y), cv::Point2d(0, 0), MapWorldScale);
	pos = TianLi::Utils::TransferUserAxes(pos, 0, 0, 1);
	x = pos.x;
	y = pos.y;
	mapId = tr_res.second;

	return clear_error_logs();
}

bool AutoTrack::GetDirection(double& a)
{
	if (workflow_for_begin->run() == false)
	{
		return false;
	}
	if (getMiniMapRefMat() == false)
	{
		//err = { 2001, "获取角色朝向时，没有识别到paimon" };
		return false;
	}
	if (genshin_minimap.rect_avatar.empty())
	{
		err = { 11,"原神角色小箭头区域为空" };
		return false;
	}
	
	direction_calculation_config  config;
	direction_calculation(genshin_minimap.img_avatar, a , config);
	if (config.error)
	{
		err = config.err;
		return false;
	}
	
	return clear_error_logs();
}

bool AutoTrack::GetRotation(double& a)
{
	if (workflow_for_begin->run() == false)
	{
		return false;
	}
	if (getMiniMapRefMat() == false)
	{
		//err = { 3001, "获取视角朝向时，没有识别到paimon" };
		return false;
	}

	rotation_calculation_config config;
	rotation_calculation(genshin_minimap.img_minimap, a, config);
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

		if (workflow_for_begin->run() == false)
		{
			return false;
		}
		
		if (getMiniMapRefMat() == false)
		{
			//err = { 4001, "获取神瞳时，没有识别到paimon" };
			return false;
		}

		if (genshin_minimap.img_minimap.empty())
		{
			err = { 5, "原神小地图区域为空" };
			return false;
		}
		cv::Mat giStarRef;

		cv::cvtColor(genshin_minimap.img_minimap(cv::Rect(36, 36, genshin_minimap.img_minimap.cols - 72, genshin_minimap.img_minimap.rows - 72)),
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
			pos.emplace_back(cv::Point2d(maxLoc) -
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
				pos.emplace_back(cv::Point2d(maxLoc) -
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
	if (workflow_for_begin->run() == false)
	{
		return false;
	}

	if (getMiniMapRefMat() == false)
	{
		//err = { 4001, "获取神瞳时，没有识别到paimon" };
		return false;
	}
	
	if (genshin_minimap.img_minimap.empty())
	{
		err = { 5, "原神小地图区域为空" };
		return false;
	}
	cv::Mat giStarRef;

	//一个bug 未开游戏而先开应用，开游戏时触发
	cv::cvtColor(genshin_minimap.img_minimap(cv::Rect(36, 36, genshin_minimap.img_minimap.cols - 72, genshin_minimap.img_minimap.rows - 72)),
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
	if (workflow_for_begin->run() == false)
	{
		return false;
	}

	cv::Mat& giUIDRef = genshin_screen.img_uid;

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
	if (getMiniMapRefMat() == false)
	{
		//err = { 1001, "获取所有信息时，没有识别到paimon" };
		return false;
	}

	if (genshin_minimap.img_minimap.empty())
	{
		err = { 5, "原神小地图区域为空" };
		return false;
	}
	// x,y,mapId
	{
		genshin_minimap.config.is_find_paimon = true;

		TianLi::Genshin::Match::get_avatar_position(genshin_minimap, genshin_avatar_position);
		
		cv::Point2d user_pos = genshin_avatar_position.position;
		auto tr_res = TianLi::Utils::TransferTianLiAxes(user_pos.x, user_pos.y);
		cv::Point2d pos = TianLi::Utils::TransferTianLiAxes(cv::Point2d(tr_res.first.x, tr_res.first.y), cv::Point2d(0, 0), MapWorldScale);
		pos = TianLi::Utils::TransferUserAxes(pos, 0, 0, 1);
		x = pos.x;
		y = pos.y;
		mapId = tr_res.second;
	}

	if (genshin_minimap.rect_avatar.empty())
	{
		err = { 11,"原神角色小箭头区域为空" };
		return false;
	}
	// a
	{
		direction_calculation_config  config;
		direction_calculation(genshin_minimap.img_avatar, a, config);
		if (config.error)
		{
			err = config.err;
			return false;
		}
	}
	// r
	{
		rotation_calculation_config config;
		rotation_calculation(genshin_minimap.img_minimap, r, config);
		if (config.error)
		{
			err = config.err;
			return false;
		}
	}
	cv::Mat& giUIDRef = genshin_screen.img_uid;
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

bool AutoTrack::getMiniMapRefMat()
{
	genshin_minimap.img_minimap = giFrame(genshin_minimap.rect_minimap);

	
	if (genshin_handle.config.capture->mode == Capture::DirectX || genshin_handle.config.is_force_used_no_alpha)
	{
		genshin_screen.config.is_used_alpha = false;
	}
	else
	{
		genshin_screen.config.is_used_alpha = true;
	}
	
	// 检测派蒙 -> 直接计算小地图坐标

	if (TianLi::Genshin::Check::check_paimon(genshin_screen, genshin_paimon) == false)
	{
		if (genshin_handle.config.capture->mode == Capture::Bitblt)
		{
			//err = { 40101,"Bitblt模式下检测派蒙失败" };
			return false;
		}
		else if (genshin_handle.config.capture->mode == Capture::DirectX)
		{
			//err = { 40201,"DirectX模式下检测派蒙失败" };
			return false;
		}
	}
	if (genshin_paimon.is_visial == false)
	{
		if (genshin_handle.config.capture->mode == Capture::Bitblt)
		{
			//err = { 40102,"Bitblt模式下没有检测到派蒙" };
			return false;
		}
		else if (genshin_handle.config.capture->mode == Capture::DirectX)
		{
			//err = { 40202,"DirectX模式下没有检测到派蒙" };
			return false;
		}
	}

	genshin_screen.config.rect_paimon = genshin_paimon.rect_paimon;
	genshin_screen.config.is_handle_mode = genshin_paimon.is_handle_mode;
	genshin_screen.config.is_search_mode = genshin_paimon.is_search_mode;

	if (TianLi::Genshin::Cailb::cailb_minimap(genshin_screen, genshin_minimap) == false)
	{
		if (genshin_handle.config.capture->mode == Capture::Bitblt)
		{
			err = { 40105,"Bitblt模式下计算小地图区域失败" };
			return false;
		}
		else if (genshin_handle.config.capture->mode == Capture::DirectX)
		{

			err = { 40205,"DirectX模式下计算小地图区域失败" };
			return false;
		}
	}
	
#ifdef _DEBUG
	cv::namedWindow("MiniMap", cv::WINDOW_FREERATIO);
	cv::imshow("MiniMap", genshin_minimap.img_minimap);
	cv::waitKey(1);
#endif
	return true;
}

