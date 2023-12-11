#pragma once

namespace tianli::global
{
    class error_info
    {

    };
    class error_resigter
    {

    };



    // 日志基类
    class logger
    {
    public:
        enum class level
        {
            debug,
            info,
            warn,
            error,
            fatal
        };
        virtual void write(level lv, const char* msg) = 0;
    };
    
} // namespace tianli::global
