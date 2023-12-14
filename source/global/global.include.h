#pragma once
#include <string>
#include <list>
#include <memory>
#include <chrono>
#if cplusplus >= 202002L
#include <source_location>
#endif
namespace tianli::global
{
    class error_type
    {
    public:
        int code;
        std::string msg;
#if cplusplus >= 202002L
        std::source_location location;
        #else
        std::string location;
#endif
        std::shared_ptr<error_type> next;
    };

    class error_info
    {
        public:
            std::chrono::system_clock::time_point time;
            std::chrono::system_clock::time_point begin_time;
            std::chrono::system_clock::time_point end_time;
            std::shared_ptr<error_type> error;
    };
    class error_resigter
    {
        public:
            std::list<std::shared_ptr<error_info>> error_list;
            std::shared_ptr<error_info> last_error;
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
        virtual void log(error_type err) { return; }
    };


} // namespace tianli::global
