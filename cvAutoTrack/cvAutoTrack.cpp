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
bool SetHandle(long long int handle)
{
	return _at->SetHandle(handle);
}
bool SetWorldCenter(double x, double y)
{
	return _at->SetWorldCenter(x, y);
}
bool SetWorldScale(double scale)
{
	return _at->SetWorldScale(scale);
}
bool __stdcall GetTransform(float &x, float &y, float &a)
{
	return _at->GetTransform(x, y, a);
}
bool GetPosition(double & x, double & y)
{
	return _at->GetPosition(x, y);
}
bool GetDirection(double & a)
{
	return _at->GetDirection(a);
}
bool __stdcall GetUID(int &uid)
{
	return _at->GetUID(uid);
}
bool GetInfoLoadPicture(char * path, int & uid, double & x, double & y, double & a)
{
	return _at->GetInfoLoadPicture(path, uid, x, y, a);
}
int __stdcall GetLastErr()
{
	return _at->GetLastError();
}
const char * GetLastErrStr()
{
	return _at->GetLastErrorStr();
}
bool __stdcall uninit()
{
	return _at->uninit();
}
