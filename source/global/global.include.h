#pragma once
#include <chrono>
#include <list>
#include <memory>
#include <string>
#include <utility>

#include <Windows.h>

#include <opencv2/opencv.hpp>

#if cplusplus >= 202002L
    #include <source_location>
#endif


namespace tianli::global
{

#if cplusplus >= 202002L
    template<size_t n>
    struct source_location
    {
        constexpr source_location(const char (&path)[n], size_t line, size_t col)
        {
            std::copy_n(path, n, this->path);
            this->line = line;
            this->col = col;
        }
        char path[n];
        size_t line;
        size_t col;
    };
    template<size_t n>
    struct error_message
    {
        constexpr error_message(const char (&msg)[n])
        {
            std::copy_n(msg, n, this->msg);
        }
        char msg[n];
    };
    
    class error_invoker
    {
        public:
        struct location
        {
            std::string path;
            size_t line;
            size_t col;
            std::string error_msg;
        };
        inline static std::vector<location> locations = {{"default", 0, 0, "error"}};
        error_invoker(const std::string &path, size_t line, size_t col, const std::string &error_msg)
        {
            locations.emplace_back(location{path, line, col, error_msg});
        }
    };

    template <typename proxyer, source_location location, error_message message>
    class error_proxy
    {
    public:
        inline static proxyer proxy = {location.path, location.line, location.col, message.msg};
        template <typename err_fun, typename... Args>
        static auto callback(err_fun&f, Args &&...args)
        {
            return f(args...);
        }
    };
    
    inline int error_impl(const char *sz)
    {
        int index = 0;
        for (auto &[_, __, ___, message] : error_invoker::locations)
        {
            if (message == sz)
                break;
            index++;
        }
        return index;
    }

#define error(msg) tianli::global::error_proxy<tianli::global::error_invoker, tianli::global::source_location(__FILE__, std::source_location::current().line(), std::source_location::current().column()), tianli::global::error_message(msg)>::callback(tianli::global::error_impl, msg)
#else
#define error(msg) 
#endif


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
    };

    class error_info
    {
        public:
            std::chrono::system_clock::time_point time;
            std::chrono::system_clock::time_point begin_time;
            std::chrono::system_clock::time_point end_time;
            std::list<std::shared_ptr<error_type>> errors;

        public:
            error_info &operator=(const std::pair<int, std::string> &err_code_msg)
            {
                errors.emplace_back(std::make_shared<error_type>(error_type{err_code_msg.first, err_code_msg.second}));
                return *this;
            }
            operator std::pair<int, std::string>()
            {
                return std::make_pair(errors.back()->code, errors.back()->msg);
            }
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
        virtual void log(error_type error) { return; }
        // 用于启用一次计时测量
        virtual void perl(std::string perl_label) { return; }
        virtual void perl_end(std::string perl_label = "") { return; }
    };


} // namespace tianli::global
