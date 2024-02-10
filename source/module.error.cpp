#include "pch.h"
#include "cvAutoTrack.h"
#include "ErrorCode.h"

#ifdef _DEBUG
    #include <iostream>
#endif

#define err ErrorCode::getInstance()

int __stdcall GetLastErr()
{
#ifdef _DEBUG
    std::cout << err;
#endif
    return err;
}
int __stdcall GetLastErrMsg(char* msg_buff, int buff_size)
{
    if (msg_buff == NULL || buff_size < 1)
    {
        err = { 291, "缓存区为空指针或是缓存区大小为小于1" };
        return false;
    }
    std::string msg = err.getLastErrorMsg();
    if (msg.size() > buff_size)
    {
        err = { 292, "缓存区大小不足" };
        return false;
    }
    strcpy_s(msg_buff, buff_size, msg.c_str());
    return true;
}
int __stdcall GetLastErrJson(char* json_buff, int buff_size)
{
    if (json_buff == NULL || buff_size < 1)
    {
        err = { 291, "缓存区为空指针或是缓存区大小为小于1" };
        return false;
    }
    std::string msg = err.toJson();
    if (msg.size() > buff_size)
    {
        err = { 292, "缓存区大小不足" };
        return false;
    }
    strcpy_s(json_buff, buff_size, msg.c_str());
    return true;
}