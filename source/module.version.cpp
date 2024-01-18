#include "pch.h"
#include "cvAutoTrack.h"

#include "utils/convect.string.h"
#include "resource/version.h"
#include "ErrorCode.h"
#ifndef err
#define err ErrorCode::getInstance()
#endif
#include <global/global.include.h>

#if cplusplus >= 202002L
#define regerr(msg) err = {error(msg),utils::to_gbk(msg)}
#else
#define regerr(msg) err = {-1,utils::to_gbk(msg)}
#endif


bool __stdcall GetCompileVersion(char* version_buff, int buff_size)
{
    INSTALL_DUMP_();
	if (version_buff == nullptr || buff_size < 1)
	{
		regerr("缓存区为空指针或是缓存区大小为小于1");
		return false;
	}
	if (TianLi::Version::build_version.size() > buff_size)
	{
		//err("缓存区大小不足");
		regerr("缓存区大小不足");
		return false;
	}
	strcpy_s(version_buff, buff_size, TianLi::Version::build_version.c_str());
	return true;
}
bool __stdcall GetCompileTime(char* time_buff, int buff_size)
{
    INSTALL_DUMP_();
	if (time_buff == nullptr || buff_size < 1)
	{
		//err("缓存区为空指针或是缓存区大小为小于1");
		regerr("缓存区为空指针或是缓存区大小为小于1");
		return false;
	}
	if (TianLi::Version::build_time.size() > buff_size)
	{
		//err("缓存区大小不足");
		regerr("缓存区大小不足");
		return false;
	}
	strcpy_s(time_buff, buff_size, TianLi::Version::build_time.c_str());
	return true;
}
#undef err