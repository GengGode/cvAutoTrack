// cvAutoTrack.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "cvAutoTrack.h"
#include "AutoTrack.h"
#include "ErrorCode.h"
#include "global/global.windump.h"
#include <atomic>

static std::shared_ptr<ErrorCode> _err;
static std::shared_ptr<Resources> _res;
static std::shared_ptr<AutoTrack> _at;
static std::atomic<bool> _inited = false;

bool __stdcall InitResource()
{
    if (_inited == false)
    {
        _err = ErrorCode::getSharedPtr();
        _res = Resources::getSharedPtr();
        _at = std::make_shared<AutoTrack>(*_err, *_res);
        _inited = false;
    }

    INSTALL_DUMP(_at->init());
    _inited = true;
}
bool __stdcall UnInitResource()
{
    INSTALL_DUMP([&]() {
        if (_at == nullptr)
            return true;
        if (_at->uninit() == false)
            return false;
        _at.reset();
        return true;
    }());
}

#include "module.frame.h"
bool __stdcall SetUseBitbltCaptureMode()
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(create_capture_bitblt(_at->get_source()));
}
bool __stdcall SetUseGraphicsCaptureMode()
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(create_capture_graphics(_at->get_source()));
}
bool __stdcall SetUseDwmCaptureMode()
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(create_capture_dwm(_at->get_source()));
}
bool __stdcall SetUseLocalPictureMode()
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(create_local_picture(_at->get_source()));
}
bool __stdcall SetUseLocalVideoMode()
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(create_local_video(_at->get_source()));
}
bool __stdcall SetCaptureHandle(long long int handle = 0)
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(set_capture_handle(_at->get_source(), (HWND)handle));
}
bool __stdcall SetCaptureHandleCallback(long long int (*callback)())
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(set_capture_handle_callback(_at->get_source(), (HWND (*)())callback));
}
bool __stdcall SetScreenSourceCallback(void (*callback)(const char* image_encode_data, int& image_data_size))
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(set_source_frame_callback(_at->get_source(), callback));
}
bool __stdcall SetScreenSourceCallbackEx(void (*callback)(const char* image_data, int& image_width, int& image_height, int& image_channels))
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(set_source_frame_callback(_at->get_source(), callback));
}
bool __stdcall SetScreenSourceImage(const char* image_encode_data, int image_data_size)
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(set_local_frame(_at->get_source(), image_encode_data, image_data_size));
}
bool __stdcall SetScreenSourceImageEx(const char* image_data, int image_width, int image_height, int image_channels)
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(set_local_frame(_at->get_source(), image_data, image_width, image_height, image_channels));
}
bool __stdcall SetScreenClientRectCallback(void (*callback)(int& x, int& y, int& width, int& height))
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(set_source_client_rect_callback(_at->get_source(), callback));
}

bool __stdcall SetWorldCenter(double x, double y)
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(_at->SetWorldCenter(x, y));
}
bool __stdcall SetWorldScale(double scale)
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(_at->SetWorldScale(scale));
}
bool __stdcall GetTransformOfMap(double& x, double& y, double& a, int& mapId)
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(_at->GetTransformOfMap(x, y, a, mapId));
}
bool __stdcall GetPositionOfMap(double& x, double& y, int& mapId)
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(_at->GetPositionOfMap(x, y, mapId));
}
bool __stdcall GetDirection(double& a)
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(_at->GetDirection(a));
}
bool __stdcall GetRotation(double& a)
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(_at->GetRotation(a));
}

bool __stdcall GetUID(int& uid)
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(_at->GetUID(uid));
}
bool __stdcall GetAllInfo(double& x, double& y, int& mapId, double& a, double& r, int& uid)
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(_at->GetAllInfo(x, y, mapId, a, r, uid));
    return _at->GetAllInfo(x, y, mapId, a, r, uid);
}
bool __stdcall SetDisableFileLog()
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(_at->SetDisableFileLog());
}
bool __stdcall SetEnableFileLog()
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(_at->SetEnableFileLog());
}
bool __stdcall DebugCapture()
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(_at->DebugCapture());
}
bool __stdcall DebugCapturePath(const char* path_buff, int buff_size)
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(_at->DebugCapturePath(path_buff, buff_size));
}
bool __stdcall startServe()
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(_at->startServe());
}
bool __stdcall stopServe()
{
    if (_inited == false)
        InitResource();
    INSTALL_DUMP(_at->stopServe());
}

cvAutoTrackContextV1* create_cvAutoTrack_context_v1()
{
    cvAutoTrackContextV1* context = new cvAutoTrackContextV1();
    context->DebugLoadMapImagePath = DebugLoadMapImagePath;
    context->InitResource = InitResource;
    context->UnInitResource = UnInitResource;
    context->SetCoreCachePath = SetCoreCachePath;
    context->GetCoreCachePath = GetCoreCachePath;
    context->SetDisableFileLog = SetDisableFileLog;
    context->SetEnableFileLog = SetEnableFileLog;
    context->SetUseBitbltCaptureMode = SetUseBitbltCaptureMode;
    context->SetUseGraphicsCaptureMode = SetUseGraphicsCaptureMode;
    context->SetUseDwmCaptureMode = SetUseDwmCaptureMode;
    context->SetUseLocalPictureMode = SetUseLocalPictureMode;
    context->SetUseLocalVideoMode = SetUseLocalVideoMode;
    context->SetCaptureHandle = SetCaptureHandle;
    context->SetCaptureHandleCallback = SetCaptureHandleCallback;
    context->SetScreenSourceCallback = SetScreenSourceCallback;
    context->SetScreenSourceCallbackEx = SetScreenSourceCallbackEx;
    context->SetScreenSourceImage = SetScreenSourceImage;
    context->SetScreenSourceImageEx = SetScreenSourceImageEx;
    context->SetScreenClientRectCallback = SetScreenClientRectCallback;
    context->SetWorldCenter = SetWorldCenter;
    context->SetWorldScale = SetWorldScale;
    context->GetTransformOfMap = GetTransformOfMap;
    context->GetPositionOfMap = GetPositionOfMap;
    context->GetDirection = GetDirection;
    context->GetRotation = GetRotation;
    context->GetUID = GetUID;
    context->GetAllInfo = GetAllInfo;
    context->DebugCapture = DebugCapture;
    context->GetLastErr = GetLastErr;
    context->GetLastErrMsg = GetLastErrMsg;
    context->GetLastErrJson = GetLastErrJson;
    context->GetCompileVersion = GetCompileVersion;
    context->GetCompileTime = GetCompileTime;
    context->GetCoreModulePath = GetCoreModulePath;
    return context;
}

void destroy_cvAutoTrack_context_v1(cvAutoTrackContextV1* context)
{
    delete context;
}
#include "global/debugger.h"
#include "global/pool.hpp"
cvAutoTrackContextV2* create_cvAutoTrack_context_v2()
{
    cvAutoTrackContextV2* context = new cvAutoTrackContextV2();
    context->Call = +[](const char* command, const char* json_buff, int json_size, char* result_buff, int result_size) {
        if (command == nullptr || json_buff == nullptr)
        {
            return false;
        }
        auto cmd = std::string(command);
        auto args = std::string(json_buff, json_size);
        if (cmd == "debugger")
        {
            if (args == "on")
            {
                if (auto gui = global::shareder<debugger>::get_or_created_get(); gui)
                {
                    gui->initlize();
                }
            }
            else if (args == "off")
            {
                if (auto gui = global::shareder<debugger>::get(); gui)
                {
                    gui->destory();
                }
            }
            else if (args == "wait")
            {
                if (auto gui = global::shareder<debugger>::get(); gui)
                {
                    gui->wait_exit();
                }
            }
        }
        else
        {
            if (auto gui = global::shareder<debugger>::get(); gui)
            {
                gui->call(cmd, args);
            }
        }
        return true;
    };
    // context->DebugLoadMapImagePath = DebugLoadMapImagePath;
    // context->InitResource = InitResource;
    // context->UnInitResource = UnInitResource;
    // context->SetCoreCachePath = SetCoreCachePath;
    // context->GetCoreCachePath = GetCoreCachePath;
    // context->SetDisableFileLog = SetDisableFileLog;
    // context->SetEnableFileLog = SetEnableFileLog;
    // context->SetUseBitbltCaptureMode = SetUseBitbltCaptureMode;
    // context->SetUseGraphicsCaptureMode = SetUseGraphicsCaptureMode;
    // context->SetUseDwmCaptureMode = SetUseDwmCaptureMode;
    // context->SetUseLocalPictureMode = SetUseLocalPictureMode;
    // context->SetUseLocalVideoMode = SetUseLocalVideoMode;
    // context->SetCaptureHandle = SetCaptureHandle;
    // context->SetCaptureHandleCallback = SetCaptureHandleCallback;
    // context->SetScreenSourceCallback = SetScreenSourceCallback;
    // context->SetScreenSourceCallbackEx = SetScreenSourceCallbackEx;
    // context->SetScreenSourceImage = SetScreenSourceImage;
    // context->SetScreenSourceImageEx = SetScreenSourceImageEx;
    // context->SetScreenClientRectCallback = SetScreenClientRectCallback;
    // context->SetWorldCenter = SetWorldCenter;
    // context->SetWorldScale = SetWorldScale;
    context->GetTransformOfMap = GetTransformOfMap;
    context->GetPositionOfMap = GetPositionOfMap;
    context->GetDirection = GetDirection;
    context->GetRotation = GetRotation;
    context->GetUID = GetUID;
    context->GetAllInfo = GetAllInfo;
    // context->DebugCapture = DebugCapture;
    context->GetLastErr = GetLastErr;
    context->GetLastErrMsg = GetLastErrMsg;
    // context->GetLastErrJson = GetLastErrJson;
    context->GetCompileVersion = GetCompileVersion;
    context->GetCompileTime = GetCompileTime;
    context->GetCoreModulePath = GetCoreModulePath;
    return context;
}

void destroy_cvAutoTrack_context_v2(cvAutoTrackContextV2* context)
{
    delete context;
}