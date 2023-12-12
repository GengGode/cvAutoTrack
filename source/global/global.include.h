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
        logger() = default;
        virtual void write(level lv, const char *msg) { return; }
    };

} // namespace tianli::global
