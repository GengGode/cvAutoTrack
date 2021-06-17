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
bool __stdcall GetTransforn(float &x, float &y, float &a)
{
	return _at->GetTransforn(x, y, a);
}
bool __stdcall GetUID(int &uid)
{
	return _at->GetUID(uid);
}
int __stdcall GetLastErr()
{
	return _at->GetLastError();
}
bool __stdcall uninit()
{
	return _at->uninit();
}
