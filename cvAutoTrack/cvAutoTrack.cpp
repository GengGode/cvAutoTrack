// cvAutoTrack.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "framework.h"
#include "cvAutoTrack.h"

#include "AutoTrack.h"

AutoTrack* _at = nullptr;// new AutoTrack();

AutoTrack* getInstance()
{
	if (_at == nullptr)
	{
		_at = new AutoTrack();
	}
	return _at;
}

bool __stdcall init()
{
	return getInstance()->init();
}
bool __stdcall uninit()
{
	return getInstance()->uninit();
}
bool __stdcall SetHandle(long long int handle)
{
	return getInstance()->SetHandle(handle);
}
bool __stdcall SetWorldCenter(double x, double y)
{
	return getInstance()->SetWorldCenter(x, y);
}
bool __stdcall SetWorldScale(double scale)
{
	return getInstance()->SetWorldScale(scale);
}
bool __stdcall GetTransform(float &x, float &y, float &a)
{
	return getInstance()->GetTransform(x, y, a);
}
bool __stdcall GetPosition(double & x, double & y)
{
	return getInstance()->GetPosition(x, y);
}
bool __stdcall GetDirection(double & a)
{
	return getInstance()->GetDirection(a);
}
bool __stdcall GetRotation(double & a)
{
	return getInstance()->GetRotation(a);
}
bool __stdcall GetUID(int & uid)
{
	return getInstance()->GetUID(uid);
}
bool __stdcall GetInfoLoadPicture(char * path, int & uid, double & x, double & y, double & a)
{
	return getInstance()->GetInfoLoadPicture(path, uid, x, y, a);
}
bool __stdcall GetInfoLoadVideo(char * path, char * pathOutFile)
{
	return getInstance()->GetInfoLoadVideo(path, pathOutFile);
}
int __stdcall GetLastErr()
{
	return getInstance()->GetLastError();
}
const char * __stdcall GetLastErrStr()
{
	return getInstance()->GetLastErrorStr();
}
bool __stdcall startServe()
{
	return getInstance()->startServe();
}
bool __stdcall stopServe()
{
	return getInstance()->stopServe();
}