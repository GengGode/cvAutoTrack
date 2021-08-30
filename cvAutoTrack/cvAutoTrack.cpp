// cvAutoTrack.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "framework.h"
#include "cvAutoTrack.h"

#include "AutoTrack.h"

AutoTrack* _at = new AutoTrack();

bool __stdcall init()
{
	return _at->init();
}
bool __stdcall SetHandle(long long int handle)
{
	return _at->SetHandle(handle);
}
bool __stdcall SetWorldCenter(double x, double y)
{
	return _at->SetWorldCenter(x, y);
}
bool __stdcall SetWorldScale(double scale)
{
	return _at->SetWorldScale(scale);
}
bool __stdcall GetTransform(float &x, float &y, float &a)
{
	return _at->GetTransform(x, y, a);
}
bool __stdcall GetPosition(double & x, double & y)
{
	return _at->GetPosition(x, y);
}
bool __stdcall GetDirection(double & a)
{
	return _at->GetDirection(a);
}
bool __stdcall GetRotation(double & a)
{
	return _at->GetRotation(a);
}
bool __stdcall GetUID(int & uid)
{
	return _at->GetUID(uid);
}
bool __stdcall GetInfoLoadPicture(char * path, int & uid, double & x, double & y, double & a)
{
	return _at->GetInfoLoadPicture(path, uid, x, y, a);
}
bool __stdcall GetInfoLoadVideo(char * path, char * pathOutFile)
{
	return _at->GetInfoLoadVideo(path, pathOutFile);
}
int __stdcall GetLastErr()
{
	return _at->GetLastError();
}
const char * __stdcall GetLastErrStr()
{
	return _at->GetLastErrorStr();
}
bool startServe()
{
	return _at->startServe();
}
bool stopServe()
{
	return _at->stopServe();
}