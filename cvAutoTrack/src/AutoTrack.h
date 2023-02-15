#pragma once
#include <opencv2/opencv.hpp>
#include "FlowWork.h"
#include "ErrorCode.h"
#include "resources/Resources.h"
#include "match/type/MatchType.h"


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
	bool GetTransformOfMap(double& x, double& y, double& a, int& mapId);
	bool GetPosition(double &x, double &y);
	bool GetPositionOfMap(double& x, double& y, int& mapId);
	bool GetDirection(double& a);
	bool GetRotation(double& a);
	//获取发现的神瞳坐标,isEnd为真则为当前画面中的最后一个神瞳
	bool GetStar(double& x, double& y, bool& isEnd);
	//获取发现的神瞳坐标，以json字符串格式
	bool GetStarJson(char *jsonBuff);
	bool GetUID(int &uid);
	bool GetAllInfo(double& x, double& y, int& mapId, double& a, double& r, int& uid);
	/*********/
	bool GetInfoLoadPicture(char* path, int& uid, double& x, double& y, double& a);
	bool GetInfoLoadVideo(char* path, char* pathOutFile);
	/*********/
	int GetLastError();
	int GetLastErrMsg(char* msg_buff, int buff_size);
	int GetLastErrJson(char* json_buff, int buff_size);
	
	bool startServe();
	bool stopServe();

	bool SetDisableFileLog();
	bool SetEnableFileLog();

	bool GetVersion(char* version_buff, int buff_size);
	bool GetCompileTime(char* time_buff, int buff_size);
	
	
	bool DebugCapture();
	bool DebugCapturePath(const char* path_buff, int buff_size);

private:
	Resources& res = Resources::getInstance();
	ErrorCode& err = ErrorCode::getInstance();


	FlowWork wForAfter;
private:
	//用户定义映射关系参数
	double UserWorldOrigin_X = 0;
	double UserWorldOrigin_Y = 0;
	double UserWorldScale = 1.0;
	
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

	//2022-07-13 还是没想起来
	double MapWorldScale = 1.0;

private:
	GenshinHandle genshin_handle;
	GenshinScreen genshin_screen;
	GenshinPaimon genshin_paimon;
	GenshinMinimapCailb genshin_minimap_cailb;
	GenshinMinimap genshin_minimap;
	GenshinAvatarPosition genshin_avatar_position;
	
	cv::Mat& giFrame = genshin_screen.img_screen;
private:
	bool getGengshinImpactWnd();
	bool getGengshinImpactScreen();

	bool getMiniMapRefMat();
private:
	bool clear_error_logs();
};
