// cvAutoTrack.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "cvAutoTrack.h"
#include "ErrorCode.h"
#include "AutoTrack.h"


//定义一个装饰器做异常处理
#define TRY_CATCH(func,defaultVal)\
try {\
	return func;\
}\
catch (std::exception ex)\
{\
	err = { 999,std::format("std错误：{0}",ex.what()) };\
	return defaultVal;\
}


static AutoTrack _at;
static ErrorCode& err = ErrorCode::getInstance();

bool __stdcall verison(char* versionBuff)
{
	TRY_CATCH(_at.GetVersion(versionBuff,32),false);
}
bool __stdcall init()
{
	TRY_CATCH(_at.init(),false);
}
bool __stdcall uninit()
{
	TRY_CATCH(_at.uninit(),false);
}
bool __stdcall SetUseBitbltCaptureMode()
{
	TRY_CATCH(_at.SetUseBitbltCaptureMode(),false);
}
bool __stdcall SetUseDx11CaptureMode()
{
	TRY_CATCH(_at.SetUseDx11CaptureMode(),false);
}
bool __stdcall SetHandle(long long int handle = 0)
{
	TRY_CATCH(_at.SetHandle(handle),false);
}
bool __stdcall SetWorldCenter(double x, double y)
{
	TRY_CATCH(_at.SetWorldCenter(x, y),false);
}
bool __stdcall SetWorldScale(double scale)
{
	TRY_CATCH(_at.SetWorldScale(scale),false);
}
bool __stdcall ImportMapBlock(int id_x, int id_y, const char* image_data, int image_data_size, int image_width, int image_height)
{
	TRY_CATCH(_at.ImportMapBlock(id_x, id_y, image_data, image_data_size, image_width, image_height),false);
}
bool __stdcall ImportMapBlockCenter(int x, int y)
{
	TRY_CATCH(_at.ImportMapBlockCenter(x, y),false);
}
bool __stdcall ImportMapBlockCenterScale(int x, int y, double scale)
{
	TRY_CATCH(_at.ImportMapBlockCenterScale(x, y, scale),false);
}
bool __stdcall GetTransformOfMap(double& x, double& y, double& a, int& mapId)
{
	TRY_CATCH(_at.GetTransformOfMap(x, y, a, mapId),false);
}
bool __stdcall GetPositionOfMap(double& x, double& y, int& mapId)
{
	TRY_CATCH(_at.GetPositionOfMap(x, y, mapId),false);
}
bool __stdcall GetDirection(double & a)
{
	TRY_CATCH(_at.GetDirection(a),false);
}
bool __stdcall GetRotation(double & a)
{
	TRY_CATCH(_at.GetRotation(a),false);
}
bool __stdcall GetStar(double &x, double &y, bool &isEnd)
{
	TRY_CATCH(_at.GetStar(x, y, isEnd),false);
}
bool __stdcall GetStarJson(char * jsonBuff)
{
	TRY_CATCH(_at.GetStarJson(jsonBuff),false);
}
bool __stdcall GetUID(int & uid)
{
	TRY_CATCH(_at.GetUID(uid),false);
}
bool __stdcall GetAllInfo(double& x, double& y, int& mapId, double& a, double& r, int& uid)
{
	TRY_CATCH(_at.GetAllInfo(x, y, mapId, a, r, uid),false);
}
bool __stdcall GetInfoLoadPicture(char * path, int & uid, double & x, double & y, double & a)
{
	TRY_CATCH(_at.GetInfoLoadPicture(path, uid, x, y, a),false);
}
bool __stdcall GetInfoLoadVideo(char * path, char * pathOutFile)
{
	TRY_CATCH(_at.GetInfoLoadVideo(path, pathOutFile),false);
}
int __stdcall GetLastErr()
{
	TRY_CATCH(_at.GetLastError(),999);
}
int __stdcall GetLastErrMsg(char* msg_buff, int buff_size)
{
	TRY_CATCH(_at.GetLastErrMsg(msg_buff, buff_size),999);
}
int __stdcall GetLastErrJson(char* json_buff, int buff_size)
{
	TRY_CATCH(_at.GetLastErrJson(json_buff, buff_size),999);
}
bool __stdcall SetDisableFileLog()
{
	TRY_CATCH(_at.SetDisableFileLog(),false);
}
bool __stdcall SetEnableFileLog()
{
	TRY_CATCH(_at.SetEnableFileLog(),false);
}
bool __stdcall GetCompileVersion(char* version_buff, int buff_size)
{
	TRY_CATCH(_at.GetVersion(version_buff, buff_size),false);
}
bool __stdcall GetCompileTime(char* time_buff, int buff_size)
{
	TRY_CATCH(_at.GetCompileTime(time_buff, buff_size),false);
}
bool __stdcall GetMapIsEmbedded()
{
	TRY_CATCH(_at.GetMapIsEmbedded(),false);
}
bool __stdcall DebugCapture()
{
	TRY_CATCH(_at.DebugCapture(),false);
}
bool __stdcall DebugCapturePath(const char* path_buff, int buff_size)
{
	TRY_CATCH(_at.DebugCapturePath(path_buff, buff_size),false);
}
bool __stdcall startServe()
{
	TRY_CATCH(_at.startServe(),false);
}
bool __stdcall stopServe()
{
	TRY_CATCH(_at.stopServe(),false);
}