// cvAutoTrack.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "cvAutoTrack.h"

#include "AutoTrack.h"

static AutoTrack _at;


bool __stdcall verison(char* versionBuff)
{
	const char version_cstr[] = "6.4.1";
	strcpy_s(versionBuff,9, version_cstr);
	return true;
}

bool __stdcall init()
{
	return _at.init();
}
bool __stdcall uninit()
{
	return _at.uninit();
}
int __stdcall GetGpuCount()
{
	return false;
}
#pragma warning(disable:4100)
bool __stdcall SetGpuDevice(int deviceId = 0)
{
	return false;
}
bool __stdcall SetUseBitbltCaptureMode()
{
	return _at.SetUseBitbltCaptureMode();
}
bool __stdcall SetUseDx11CaptureMode()
{
	return _at.SetUseDx11CaptureMode();
}
bool __stdcall SetHandle(long long int handle = 0)
{
	return _at.SetHandle(handle);
}
bool __stdcall SetWorldCenter(double x, double y)
{
	return _at.SetWorldCenter(x, y);
}
bool __stdcall SetWorldScale(double scale)
{
	return _at.SetWorldScale(scale);
}
bool __stdcall GetTransform(float &x, float &y, float &a)
{
	return _at.GetTransform(x, y, a);
}
bool __stdcall GetTransformOfMap(float& x, float& y, float& a, int& mapId)
{
	return _at.GetTransformOfMap(x, y, a, mapId);
}
bool __stdcall GetPosition(double & x, double & y)
{
	return _at.GetPosition(x, y);
}
bool __stdcall GetPositionOfMap(double& x, double& y, int& mapId)
{
	return _at.GetPositionOfMap(x, y, mapId);
}
bool __stdcall GetDirection(double & a)
{
	return _at.GetDirection(a);
}
bool __stdcall GetRotation(double & a)
{
	return _at.GetRotation(a);
}
bool __stdcall GetStar(double &x, double &y, bool &isEnd)
{
	return _at.GetStar(x, y, isEnd);
}
bool GetStarJson(char * jsonBuff)
{
	return _at.GetStarJson(jsonBuff);
}
bool __stdcall GetUID(int & uid)
{
	return _at.GetUID(uid);
}
bool __stdcall GetInfoLoadPicture(char * path, int & uid, double & x, double & y, double & a)
{
	return _at.GetInfoLoadPicture(path, uid, x, y, a);
}
bool __stdcall GetInfoLoadVideo(char * path, char * pathOutFile)
{
	return _at.GetInfoLoadVideo(path, pathOutFile);
}
int __stdcall GetLastErr()
{
	return _at.GetLastError();
}
const char * __stdcall GetLastErrStr()
{
	return _at.GetLastErrorStr();
}
int __stdcall GetLastErrMsg(char* msg_buff, int buff_size)
{
	return _at.GetLastErrMsg(msg_buff, buff_size);
}
bool __stdcall DebugCapture()
{
	return _at.DebugCapture();
}
bool __stdcall startServe()
{
	return _at.startServe();
}
bool __stdcall stopServe()
{
	return _at.stopServe();
}