// cvAutoTrack.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "framework.h"
#include "cvAutoTrack.h"

#include "AutoTrack.h"

static AutoTrack _at;

bool init()
{
	return _at.init();
}
bool uninit()
{
	return _at.uninit();
}
bool SetHandle(long long int handle)
{
	return _at.SetHandle(handle);
}
bool SetWorldCenter(double x, double y)
{
	return _at.SetWorldCenter(x, y);
}
bool SetWorldScale(double scale)
{
	return _at.SetWorldScale(scale);
}
bool GetTransform(float &x, float &y, float &a)
{
	return _at.GetTransform(x, y, a);
}
bool GetPosition(double & x, double & y)
{
	return _at.GetPosition(x, y);
}
bool GetDirection(double & a)
{
	return _at.GetDirection(a);
}
bool GetRotation(double & a)
{
	return _at.GetRotation(a);
}
bool GetStar(double &x, double &y, bool &isEnd)
{
	return _at.GetStar(x, y, isEnd);
}
bool GetStar(char * jsonBuff)
{
	return _at.GetStar(jsonBuff);
}
bool GetUID(int & uid)
{
	return _at.GetUID(uid);
}
bool GetInfoLoadPicture(char * path, int & uid, double & x, double & y, double & a)
{
	return _at.GetInfoLoadPicture(path, uid, x, y, a);
}
bool GetInfoLoadVideo(char * path, char * pathOutFile)
{
	return _at.GetInfoLoadVideo(path, pathOutFile);
}
int GetLastErr()
{
	return _at.GetLastError();
}
const char * GetLastErrStr()
{
	return _at.GetLastErrorStr();
}
bool startServe()
{
	return _at.startServe();
}
bool stopServe()
{
	return _at.stopServe();
}

#ifdef _DEBUG

bool test(char* str)
{
	str[1024] = '2';
	return true;
}

//void testLocalVideo(std::string path);

#endif