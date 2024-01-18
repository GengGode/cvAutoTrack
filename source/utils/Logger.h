#pragma once
#include <pch.h>

// 取消宏定义以防止潜在的命名冲突
#pragma region UNDEF_LEVEL
#undef ALL
#undef TRACE
#undef DEBUG
#undef INFO
#undef WARN
#undef ERROR
#undef FATAL
#undef OFF
#pragma endregion UNDEF_LEVEL

namespace TianLi::Utils
{
    // 对日志分类以实现日志压缩
    enum class LogType
    {
        // 不分类，直接使用 message 作为类别
        NONE,
        // 用户定义的错误类型
        INFO_RES_TRACK, // 自动追踪输出结果

        // 其他不重要的信息枚举
        TRACE_UNIMPORTANT,
        DEBUG_UNIMPORTANT,
        INFO_UNIMPORTANT,
        WARN_UNIMPORTANT,
        ERROR_UNIMPORTANT
    };

    inline std::string wstring2string(std::wstring wstr)
    {
        std::string result;
        // 获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的
        int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
        char* buffer = new char[len + 1];
        // 宽字节编码转换成多字节编码
        WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.size(), buffer, len, NULL, NULL);
        buffer[len] = '\0';
        // 删除缓冲区并返回值
        result.append(buffer);
        delete[] buffer;
        return result;
    }

    static constexpr int DEFAULT_EXPIRATION_TIME = 100;
    static constexpr LogType DEFAULT_LOG_TYPE = LogType::NONE;

    enum class LogLevel
    {
        ALL,
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        OFF
    };

    class LogEntry
    {
    public:
        LogLevel level;               // 日志级别
        LogType logType;              // 日志类别
        std::string message;          // 日志信息
        std::time_t timestamp;        // 时间戳，用于输出日志时间
        std::time_t microsecondStamp; // 微秒时间戳，用于区分日志的发起前后
        std::time_t expirationTime;   // 过期时间

        LogEntry(){};

        LogEntry(LogLevel level, LogType logType, const std::string& message, std::time_t expirationTime) : level(level), logType(logType), message(message), expirationTime(expirationTime)
        {
            timestamp = std::time(nullptr);

            auto currentTime = std::chrono::system_clock::now();
            microsecondStamp = std::chrono::duration_cast<std::chrono::microseconds>(currentTime.time_since_epoch()).count();
        }

        bool isExpired() const { return std::time(nullptr) >= expirationTime; }
    };

    class Logger
    {
    private:
        const std::string fileName = "AutoTrack.log";
        bool consoleOutput = false;         // 是否开启控制台输出
        LogLevel logLevel = LogLevel::INFO; // 日志级别
        std::fstream logFile;               // 日志的文件流
        size_t maxSize = 5 * 1024 * 1024;   // 日志的最大大小

        /// <summary>
        /// logEntries用于保存近期的日志，如果在输出日志时发现这个日志在近期日志中出现过，
        /// 则新的日志会覆盖老的日志，不会使logEntries变大，
        /// 这是消极的过期日志清理
        /// 使用散列值而不是日志本身作为键，是为了适配使用LogType单独标记类型的情况
        ///
        /// logQueue用于按照时间戳排序日志，用于积极的过期日志清理
        /// 使用微秒时间戳精确记录日志前后关系
        /// 在消极的过期日志清理时能自动调整结构
        /// 当logQueue过长会执行积极的日志清理
        /// 积极的清理最好还是不要发生，这意味着日志的分类不合理，比如没有把含有变量的日志单独分类
        /// 如果发生积极的清理行为，则会额外打印WARN日志提醒
        /// </summary>
        std::hash<std::string> hash_fun;       // 散列函数
        std::map<size_t, LogEntry> logEntries; // 近期日志字典表
        std::map<time_t, size_t> logQueue;     // 日志队列，用于清理过期日志

        Logger();

    public:
        static Logger& getInstance()
        {
            static Logger instance;
            return instance;
        };

        void setLogLevel(LogLevel level);

        void enableConsoleOutput(bool enable);

        void trace(const std::string& message, LogType logType = DEFAULT_LOG_TYPE, std::time_t expirationTime = DEFAULT_EXPIRATION_TIME);

        void debug(const std::string& message, LogType logType = DEFAULT_LOG_TYPE, std::time_t expirationTime = DEFAULT_EXPIRATION_TIME);

        void info(const std::string& message, LogType logType = DEFAULT_LOG_TYPE, std::time_t expirationTime = DEFAULT_EXPIRATION_TIME);

        void warn(const std::string& message, LogType logType = DEFAULT_LOG_TYPE, std::time_t expirationTime = DEFAULT_EXPIRATION_TIME);

        void error(const std::string& message, LogType logType = DEFAULT_LOG_TYPE, std::time_t expirationTime = DEFAULT_EXPIRATION_TIME);

        void fatal(const std::string& message, LogType logType = DEFAULT_LOG_TYPE, std::time_t expirationTime = DEFAULT_EXPIRATION_TIME);

        void putLogEntry(LogEntry);

    private:
        void log(LogLevel level, const std::string& message, LogType logType, std::time_t expirationTime);

        std::string getLogLevelString(LogLevel level);

        void log2File(std::string logString);

        std::string getSystemInfo();

        std::string get_sys_version();
        std::string get_gpu_name();
    };
    auto& logger = Logger::getInstance();
}; // namespace TianLi::Utils
