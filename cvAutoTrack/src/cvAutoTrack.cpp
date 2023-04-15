// cvAutoTrack.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "cvAutoTrack.h"
#include "ErrorCode.h"
#include "AutoTrack.h"

#define INSTALL_DUMP(at_func)\
    INSTALL_DUMP_();\
	return at_func

static AutoTrack _at;

bool __stdcall verison(char* versionBuff)
{
	INSTALL_DUMP(_at.GetVersion(versionBuff,32));
}
bool __stdcall init()
{
	INSTALL_DUMP(_at.init());
}
bool __stdcall uninit()
{
	INSTALL_DUMP(_at.uninit());
}
bool __stdcall SetUseBitbltCaptureMode()
{
	INSTALL_DUMP(_at.SetUseBitbltCaptureMode());
}
bool __stdcall SetUseDx11CaptureMode()
{
	INSTALL_DUMP(_at.SetUseDx11CaptureMode());
}
bool __stdcall SetHandle(long long int handle = 0)
{
	INSTALL_DUMP(_at.SetHandle(handle));
}
bool __stdcall SetWorldCenter(double x, double y)
{
	INSTALL_DUMP(_at.SetWorldCenter(x, y));
}
bool __stdcall SetWorldScale(double scale)
{
	INSTALL_DUMP(_at.SetWorldScale(scale));
}
bool __stdcall ImportMapBlock(int id_x, int id_y, const char* image_data, int image_data_size, int image_width, int image_height)
{
	INSTALL_DUMP(_at.ImportMapBlock(id_x, id_y, image_data, image_data_size, image_width, image_height));
}
bool __stdcall ImportMapBlockCenter(int x, int y)
{
	INSTALL_DUMP(_at.ImportMapBlockCenter(x, y));
}
bool __stdcall ImportMapBlockCenterScale(int x, int y, double scale)
{
	INSTALL_DUMP(_at.ImportMapBlockCenterScale(x, y, scale));
}
bool __stdcall GetTransformOfMap(double& x, double& y, double& a, int& mapId)
{
	INSTALL_DUMP(_at.GetTransformOfMap(x, y, a, mapId));
}
bool __stdcall GetPositionOfMap(double& x, double& y, int& mapId)
{
	INSTALL_DUMP(_at.GetPositionOfMap(x, y, mapId));
}
bool __stdcall GetDirection(double & a)
{
	INSTALL_DUMP(_at.GetDirection(a));
}
bool __stdcall GetRotation(double & a)
{
	INSTALL_DUMP(_at.GetRotation(a));
}
bool __stdcall GetStar(double &x, double &y, bool &isEnd)
{
	INSTALL_DUMP(_at.GetStar(x, y, isEnd));
}
bool __stdcall GetStarJson(char * jsonBuff)
{
	INSTALL_DUMP(_at.GetStarJson(jsonBuff));
}
bool __stdcall GetUID(int & uid)
{
	INSTALL_DUMP(_at.GetUID(uid));
}
bool __stdcall GetAllInfo(double& x, double& y, int& mapId, double& a, double& r, int& uid)
{
	INSTALL_DUMP(_at.GetAllInfo(x, y, mapId, a, r, uid));
}
bool __stdcall GetInfoLoadPicture(char * path, int & uid, double & x, double & y, double & a)
{
	INSTALL_DUMP(_at.GetInfoLoadPicture(path, uid, x, y, a));
}
bool __stdcall GetInfoLoadVideo(char * path, char * pathOutFile)
{
	INSTALL_DUMP(_at.GetInfoLoadVideo(path, pathOutFile));
}
int __stdcall GetLastErr()
{
	INSTALL_DUMP(_at.GetLastError());
}
int __stdcall GetLastErrMsg(char* msg_buff, int buff_size)
{
	INSTALL_DUMP(_at.GetLastErrMsg(msg_buff, buff_size));
}
int __stdcall GetLastErrJson(char* json_buff, int buff_size)
{
	INSTALL_DUMP(_at.GetLastErrJson(json_buff, buff_size));
}
bool __stdcall SetDisableFileLog()
{
	INSTALL_DUMP(_at.SetDisableFileLog());
}
bool __stdcall SetEnableFileLog()
{
	INSTALL_DUMP(_at.SetEnableFileLog());
}
bool __stdcall GetCompileVersion(char* version_buff, int buff_size)
{
	INSTALL_DUMP(_at.GetVersion(version_buff, buff_size));
}
bool __stdcall GetCompileTime(char* time_buff, int buff_size)
{
	INSTALL_DUMP(_at.GetCompileTime(time_buff, buff_size));
}
bool __stdcall GetMapIsEmbedded()
{
	INSTALL_DUMP(_at.GetMapIsEmbedded());
}
bool __stdcall DebugCapture()
{
	INSTALL_DUMP(_at.DebugCapture());
}
bool __stdcall DebugCapturePath(const char* path_buff, int buff_size)
{
	INSTALL_DUMP(_at.DebugCapturePath(path_buff, buff_size));
}
bool __stdcall startServe()
{
	INSTALL_DUMP(_at.startServe());
}
bool __stdcall stopServe()
{
	INSTALL_DUMP(_at.stopServe());
}