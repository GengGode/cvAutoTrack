// cvAutoTrack.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "cvAutoTrack.h"
#include "ErrorCode.h"
#include "AutoTrack.h"
#include <atomic>

#define INSTALL_DUMP(at_func) \
	INSTALL_DUMP_();          \
	return at_func
#define UNINIT_RETURN()\
    if(_inited.load()==false) return false;
static std::atomic<bool> _inited = true;


bool CVAUTOTRACK_API init()
{
    return InitResource();
}

bool CVAUTOTRACK_API uninit()
{
    return UnInitResource();
}

bool __stdcall InitResource()
{
    INSTALL_DUMP(AutoTrack::Instance().init());
    _inited = true;
}
bool __stdcall UnInitResource()
{
    INSTALL_DUMP(AutoTrack::Instance().uninit());
}
bool __stdcall SetUseBitbltCaptureMode()
{
    INSTALL_DUMP(AutoTrack::Instance().SetUseBitbltCaptureMode());
}
bool __stdcall SetUseGraphicsCaptureMode()
{
    INSTALL_DUMP(AutoTrack::Instance().SetUseDx11CaptureMode());
}
bool __stdcall SetHandle(long long int handle = 0)
{
    INSTALL_DUMP(AutoTrack::Instance().SetHandle(handle));
}
bool __stdcall SetWorldCenter(double x, double y)
{
    INSTALL_DUMP(AutoTrack::Instance().SetWorldCenter(x, y));
}
bool __stdcall SetWorldScale(double scale)
{
    INSTALL_DUMP(AutoTrack::Instance().SetWorldScale(scale));
}
bool __stdcall ImportMapBlock(int id_x, int id_y, const char* image_data, int image_data_size, int image_width, int image_height)
{
    INSTALL_DUMP(AutoTrack::Instance().ImportMapBlock(id_x, id_y, image_data, image_data_size, image_width, image_height));
}
bool __stdcall ImportMapBlockCenter(int x, int y)
{
    INSTALL_DUMP(AutoTrack::Instance().ImportMapBlockCenter(x, y));
}
bool __stdcall ImportMapBlockCenterScale(int x, int y, double scale)
{
    INSTALL_DUMP(AutoTrack::Instance().ImportMapBlockCenterScale(x, y, scale));
}
bool __stdcall GetTransformOfMap(double& x, double& y, double& a, int& mapId)
{
    UNINIT_RETURN();
    INSTALL_DUMP(AutoTrack::Instance().GetTransformOfMap(x, y, a, mapId));
}
bool __stdcall GetPositionOfMap(double& x, double& y, int& mapId)
{
    UNINIT_RETURN();
    INSTALL_DUMP(AutoTrack::Instance().GetPositionOfMap(x, y, mapId));
}
bool __stdcall GetDirection(double& a)
{
    UNINIT_RETURN();
    INSTALL_DUMP(AutoTrack::Instance().GetDirection(a));
}
bool __stdcall GetRotation(double& a)
{
    UNINIT_RETURN();
    INSTALL_DUMP(AutoTrack::Instance().GetRotation(a));
}
bool __stdcall GetStar(double& x, double& y, bool& isEnd)
{
    UNINIT_RETURN();
    INSTALL_DUMP(AutoTrack::Instance().GetStar(x, y, isEnd));
}
bool __stdcall GetStarJson(char* json_buff, int buff_size)
{
    UNINIT_RETURN();
    INSTALL_DUMP(AutoTrack::Instance().GetStarJson(json_buff));
}
bool __stdcall GetUID(int& uid)
{
    UNINIT_RETURN();
    INSTALL_DUMP(AutoTrack::Instance().GetUID(uid));
}
bool __stdcall GetAllInfo(double& x, double& y, int& mapId, double& a, double& r, int& uid)
{
    UNINIT_RETURN();
    INSTALL_DUMP(AutoTrack::Instance().GetAllInfo(x, y, mapId, a, r, uid));
    return AutoTrack::Instance().GetAllInfo(x, y, mapId, a, r, uid);
}
bool __stdcall GetInfoLoadPicture(const char* path, int& uid, double& x, double& y, double& a)
{
    UNINIT_RETURN();
    INSTALL_DUMP(AutoTrack::Instance().GetInfoLoadPicture(path, uid, x, y, a));
}
bool __stdcall GetInfoLoadVideo(const char* path, const char* out_path)
{
    UNINIT_RETURN();
    INSTALL_DUMP(AutoTrack::Instance().GetInfoLoadVideo(path, out_path));
}
int __stdcall GetLastErr()
{
    INSTALL_DUMP(AutoTrack::Instance().GetLastError());
}
int __stdcall GetLastErrMsg(char* msg_buff, int buff_size)
{
    INSTALL_DUMP(AutoTrack::Instance().GetLastErrMsg(msg_buff, buff_size));
}
int __stdcall GetLastErrJson(char* json_buff, int buff_size)
{
    INSTALL_DUMP(AutoTrack::Instance().GetLastErrJson(json_buff, buff_size));
}
bool __stdcall SetDisableFileLog()
{
    INSTALL_DUMP(AutoTrack::Instance().SetDisableFileLog());
}
bool __stdcall SetEnableFileLog()
{
    INSTALL_DUMP(AutoTrack::Instance().SetEnableFileLog());
}

#include "Ver.h"
bool __stdcall GetCompileVersion(char* version_buff, int buff_size)
{
    INSTALL_DUMP(GetCoreCompileVersion(version_buff, buff_size));
}
bool __stdcall GetCompileTime(char* time_buff, int buff_size)
{
    INSTALL_DUMP(GetCoreCompileTime(time_buff, buff_size));
}
bool __stdcall GetMapIsEmbedded()
{
    INSTALL_DUMP(AutoTrack::Instance().GetMapIsEmbedded());
}
bool __stdcall DebugCapture()
{
    INSTALL_DUMP(AutoTrack::Instance().DebugCapture());
}
bool __stdcall DebugCapturePath(const char* path_buff, int buff_size)
{
    INSTALL_DUMP(AutoTrack::Instance().DebugCapturePath(path_buff, buff_size));
}
bool __stdcall startServe()
{
    INSTALL_DUMP(AutoTrack::Instance().startServe());
}
bool __stdcall stopServe()
{
    INSTALL_DUMP(AutoTrack::Instance().stopServe());
}
