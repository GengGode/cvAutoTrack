#pragma once
#include <string>

namespace tianli::global
{
    class error_info
    {
    public:
        std::string msg;
        // std::source_location local;
        std::string file;
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
        logger() = default;
        virtual void write(level lv, const char *msg) { return; }
        virtual void log(error_info err) { return; }
    };

} // namespace tianli::global
