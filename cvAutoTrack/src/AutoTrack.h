#pragma once
#include "match/type/MatchType.h"
namespace TianLi::Utils
{
	class Workflow;
}
class AutoTrack {
public:
	AutoTrack(void);
	~AutoTrack(void);
#pragma region 外部接口
	bool init();
	bool uninit();

	bool SetUseBitbltCaptureMode();
	bool SetUseDx11CaptureMode();

	bool ImportMapBlock(int id_x, int id_y, const char* image_data, int image_data_size, int image_width, int image_height);
	bool ImportMapBlockCenter(int x, int y);
	bool ImportMapBlockCenterScale(int x, int y, double scale);

	
	bool SetHandle(long long int handle = 0);
	bool SetWorldCenter(double x, double y);
	bool SetWorldScale(double scale);
	bool GetTransformOfMap(double& x, double& y, double& a, int& mapId);
	bool GetPosition(double& x, double& y);
	bool GetPositionOfMap(double& x, double& y, int& mapId);
	bool GetDirection(double& a);
	bool GetRotation(double& a);
	//获取发现的神瞳坐标,isEnd为真则为当前画面中的最后一个神瞳
	bool GetStar(double& x, double& y, bool& isEnd);
	//获取发现的神瞳坐标，以json字符串格式
	bool GetStarJson(char* jsonBuff);
	bool GetUID(int& uid);
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
	bool GetMapIsEmbedded();
	
	bool DebugCapture();
	bool DebugCapturePath(const char* path_buff, int buff_size);
#pragma endregion
private:
	TianLi::Utils::Workflow* workflow_for_begin = nullptr;
private:
#pragma region 坐标映射相关变量

	//用户定义映射关系参数
	cv::Point2f worldOrigin = { 0,0 };		// 坐标中点
	double worldScale = 1.0;				//坐标缩放

	const cv::Point2f  tianliAxisOrigin = { 5292,3015 };	// 天理坐标中点
	const double tianliScale = 2.557; //天理坐标缩放

#pragma endregion
private:
	GenshinHandle genshin_handle;
	GenshinScreen genshin_screen;
	GenshinPaimon genshin_paimon;
	GenshinMinimap genshin_minimap;
	GenshinAvatarPosition genshin_avatar_position;
private:
	bool getGengshinImpactWnd();
	bool getGengshinImpactScreen();
	bool getMiniMapRefMat();
};
