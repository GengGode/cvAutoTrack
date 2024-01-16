// cvAutoTrack.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include <cvAutoTrack.h>
#include "ErrorCode.h"
#include "AutoTrack.h"
#include "module.debug.h"
#include <atomic>



    
static AutoTrack _at;
static std::atomic<bool> _inited = true;

#ifdef _DEBUG
bool CVAUTOTRACK_API LoadDbgMapImg(const char* path)
{
    return tianli::debug::LoadDbgMapImg(path);
}
#endif // _DEBUG


bool __stdcall InitResource()
{
    INSTALL_DUMP(_at.init());
    _inited = true;
}
bool __stdcall UnInitResource()
{
    INSTALL_DUMP(_at.uninit());
}

#include "module.frame.h"
bool __stdcall SetUseBitbltCaptureMode()
{
    INSTALL_DUMP(create_capture_bitblt(_at.get_source()));
}
bool __stdcall SetUseGraphicsCaptureMode()
{
    INSTALL_DUMP(create_capture_graphics(_at.get_source()));
}
bool __stdcall SetUseDwmCaptureMode()
{
    INSTALL_DUMP(create_capture_dwm(_at.get_source()));
}
bool __stdcall SetUseLocalPictureMode()
{
    INSTALL_DUMP(create_local_picture(_at.get_source()));
}
bool __stdcall SetUseLocalVideoMode()
{
    INSTALL_DUMP(create_local_video(_at.get_source()));
}
bool __stdcall SetCaptureHandle(long long int handle = 0)
{
    INSTALL_DUMP(set_capture_handle(_at.get_source(), (HWND)handle));
}
bool __stdcall SetCaptureHandleCallback(long long int (*callback)())
{
    INSTALL_DUMP(set_capture_handle_callback(_at.get_source(), (HWND(*)())callback));
}
bool __stdcall SetScreenSourceCallback(void (*callback)(const char *image_encode_data, int &image_data_size))
{
    INSTALL_DUMP(set_source_frame_callback(_at.get_source(), callback));
}
bool __stdcall SetScreenSourceCallbackEx(void (*callback)(const char *image_data, int &image_width, int &image_height, int &image_channels))
{
    INSTALL_DUMP(set_source_frame_callback(_at.get_source(), callback));
}
bool __stdcall SetScreenSourceImage(const char *image_encode_data, int image_data_size)
{
    INSTALL_DUMP(set_local_frame(_at.get_source(), image_encode_data, image_data_size));
}
bool __stdcall SetScreenSourceImageEx(const char *image_data, int image_width, int image_height, int image_channels)
{
    INSTALL_DUMP(set_local_frame(_at.get_source(), image_data, image_width, image_height, image_channels));
}
bool __stdcall SetScreenClientRectCallback(void (*callback)(int &x, int &y, int &width, int &height))
{
    INSTALL_DUMP(set_source_client_rect_callback(_at.get_source(), callback));
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
bool __stdcall GetDirection(double& a)
{
    INSTALL_DUMP(_at.GetDirection(a));
}
bool __stdcall GetRotation(double& a)
{
    INSTALL_DUMP(_at.GetRotation(a));
}
bool __stdcall GetStar(double& x, double& y, bool& isEnd)
{
    INSTALL_DUMP(_at.GetStar(x, y, isEnd));
}
bool __stdcall GetStarJson(char* json_buff, int buff_size)
{
    INSTALL_DUMP(_at.GetStarJson(json_buff));
}
bool __stdcall GetUID(int& uid)
{
    INSTALL_DUMP(_at.GetUID(uid));
}
bool __stdcall GetAllInfo(double& x, double& y, int& mapId, double& a, double& r, int& uid)
{
    INSTALL_DUMP(_at.GetAllInfo(x, y, mapId, a, r, uid));
    return _at.GetAllInfo(x, y, mapId, a, r, uid);
}
bool __stdcall GetInfoLoadPicture(const char* path, int& uid, double& x, double& y, double& a)
{
    INSTALL_DUMP(_at.GetInfoLoadPicture(path, uid, x, y, a));
}
bool __stdcall GetInfoLoadVideo(const char* path, const char* out_path)
{
    INSTALL_DUMP(_at.GetInfoLoadVideo(path, out_path));
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

#include "module.version.h"
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
