#pragma once
#include <pch.h>
//取消宏定义以防止潜在的命名冲突
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

namespace TianLi::Utils {
  inline std::string wstring2string(std::wstring wstr)
  {
    std::string result;
    //获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    char* buffer = new char[len + 1];
    //宽字节编码转换成多字节编码  
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    //删除缓冲区并返回值  
    result.append(buffer);
    delete[] buffer;
    return result;
  }

  static constexpr int DEFAULT_EXPIRATION_TIME = 100;
  static constexpr LogType DEFAULT_LOG_LEVEL = LogType::NONE;

  enum class LogLevel {
    ALL,
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    OFF
  };

  //对日志分类以实现日志压缩
  enum class LogType {
    //不分类，直接使用 message 作为类别
    NONE,
    //用户定义的错误类型
    INFO_RES_TRACK,       //自动追踪输出结果

    //其他不重要的信息枚举
    TRACE_UNIMPORTANT,
    DEBUG_UNIMPORTANT,
    INFO_UNIMPORTANT,
    WARN_UNIMPORTANT,
    ERROR_UNIMPORTANT
  };

  class LogEntry {
  public:
    LogLevel level;             //日志级别
    LogType logType;            //日志类别
    std::string message;        //日志信息
    std::time_t timestamp;      //时间戳，用于输出日志时间
    std::time_t microsecondStamp;   //微秒时间戳，用于区分日志的发起前后
    std::time_t expirationTime; //过期时间

    LogEntry() {};

    LogEntry(LogLevel level, LogType logType, const std::string& message, std::time_t expirationTime)
      : level(level), logType(logType), message(message), expirationTime(expirationTime) {
      timestamp = std::time(nullptr);

      auto currentTime = std::chrono::system_clock::now();
      microsecondStamp = std::chrono::duration_cast<std::chrono::microseconds>(currentTime.time_since_epoch()).count();
    }

    bool isExpired() const {
      return std::time(nullptr) >= expirationTime;
    }
  };

  class Logger {
  private:
    bool consoleOutput;     //是否开启控制台输出
    LogLevel logLevel;      //日志级别
    std::fstream logFile;   //日志的文件流
    size_t maxSize;

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
    std::hash<std::string> hash_fun;      //散列函数
    std::map<size_t, LogEntry> logEntries;   //近期日志字典表
    std::map<time_t, size_t> logQueue;       //日志队列，用于清理过期日志

    Logger(const std::string& filename) :logLevel(LogLevel::INFO) {
#ifdef _DEBUG
      consoleOutput = true;
#else
      consoleOutput = false;
#endif
      logFile.open(filename, std::ios_base::in | std::ios_base::out);
    }

  public:
    static Logger& getInstance(const std::string& filename = "") {
      static Logger instance(filename);
      return instance;
    }

    void setLogLevel(LogLevel level) {
      logLevel = level;
    }

    void enableConsoleOutput(bool enable) {
      consoleOutput = enable;
    }

    void trace(const std::string& message, LogType logType = DEFAULT_LOG_LEVEL, std::time_t expirationTime = DEFAULT_EXPIRATION_TIME) {
      log(LogLevel::TRACE, message, logType, expirationTime);
    }

    void debug(const std::string& message, LogType logType = DEFAULT_LOG_LEVEL, std::time_t expirationTime = DEFAULT_EXPIRATION_TIME) {
      log(LogLevel::DEBUG, message, logType, expirationTime);
    }

    void info(const std::string& message, LogType logType = DEFAULT_LOG_LEVEL, std::time_t expirationTime = DEFAULT_EXPIRATION_TIME) {
      log(LogLevel::INFO, message, logType, expirationTime);
    }

    void warn(const std::string& message, LogType logType = DEFAULT_LOG_LEVEL, std::time_t expirationTime = DEFAULT_EXPIRATION_TIME) {
      log(LogLevel::WARN, message, logType, expirationTime);
    }

    void error(const std::string& message, LogType logType = DEFAULT_LOG_LEVEL, std::time_t expirationTime = DEFAULT_EXPIRATION_TIME) {
      log(LogLevel::ERROR, message, logType, expirationTime);
    }

    void fatal(const std::string& message, LogType logType = DEFAULT_LOG_LEVEL, std::time_t expirationTime = DEFAULT_EXPIRATION_TIME) {
      log(LogLevel::FATAL, message, logType, expirationTime);
    }

  private:
    void log(LogLevel level, const std::string& message, LogType logType, std::time_t expirationTime) {
      if (level < logLevel) {
        return;
      }

      std::size_t key;
      if (logType == LogType::NONE)
        key = hash_fun(getLogLevelString(level) + message);
      else
        key = hash_fun(getLogLevelString(level)) + (size_t)logType;

      // 如果相同内容的日志已存在且未过期，则更新过期时间并返回
      if (logEntries.find(key) != logEntries.end()) {
        LogEntry& entry = logEntries[key];
        if (!entry.isExpired()) {
          entry.expirationTime = std::time(nullptr) + expirationTime;
          return;
        }
      }

      // 记录新的日志条目
      logEntries[key] = LogEntry(level, logType, message, std::time(nullptr) + expirationTime);

      // 获取当前时间
      std::time_t now = std::time(nullptr);
      std::tm tm_now;
      localtime_s(&tm_now, &now);
      char timestamp[20];
      std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm_now);

      // 构建日志格式
      std::stringstream logStream;
      logStream << "[" << timestamp << "]" << getLogLevelString(level) << ":" << message;
      std::string logString = logStream.str();

      // 输出到文件
      if (logFile.is_open()) {
        log2File(logString);
      }

      // 输出到控制台
      if (consoleOutput) {
        std::cout << logString << std::endl;
      }
    }

    std::string getLogLevelString(LogLevel level) {
      switch (level) {
      case LogLevel::TRACE:
        return " TRACE";
      case LogLevel::DEBUG:
        return " DEBUG";
      case LogLevel::INFO:
        return " INFO";
      case LogLevel::WARN:
        return " WARN";
      case LogLevel::ERROR:
        return " ERROR";
      case LogLevel::FATAL:
        return " FATAL";
      default:
        return "";
      }
    }

    void log2File(std::string logString)
    {
      if (logFile.tellp() >= maxSize)     //日志过大，裁剪其中一半内容
      {
        auto buf = std::make_unique<char[]>(maxSize);
        logFile.seekp(maxSize / 2);
        logFile.read(buf.get(), maxSize);
        logFile.seekp(0);

        std::string systemInfo = getSystemInfo().c_str();
        logFile.write(systemInfo.c_str(), systemInfo.size());
        logFile.write(buf.get(), maxSize);

      }
      logFile << logString << std::endl;
    }

    std::string getSystemInfo()
    {
      return get_sys_version() + "\n" + get_gpu_name();
    }

    std::string get_sys_version()
    {
      std::string result = "";
      std::string ProductName;
      std::string DisplayVersion;
      std::string CurrentBuildNumber;
      int UBR;
      if (!TianLi::Utils::getRegValue_REG_SZ(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)", LR"(ProductName)", ProductName, 64))
        ProductName = "null";
      if (!TianLi::Utils::getRegValue_REG_SZ(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)", LR"(DisplayVersion)", DisplayVersion, 64))
        DisplayVersion = "null";
      if (!TianLi::Utils::getRegValue_REG_SZ(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)", LR"(CurrentBuildNumber)", CurrentBuildNumber, 64))
        CurrentBuildNumber = "null";
      if (!TianLi::Utils::getRegValue_DWORD(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)", LR"(UBR)", UBR))
        UBR = 0;
      result = std::format("{0}-{1}-{2}.{3}", ProductName, DisplayVersion, CurrentBuildNumber, UBR);
      return result;
    }
    std::string get_gpu_name()
    {
      // Get the name of the GPU
      IDXGIAdapter* pAdapter = nullptr;
      IDXGIFactory* pFactory = nullptr;
      try
      {
        HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));
        if (FAILED(hr))
        {
          return "Unknown";
        }

        hr = pFactory->EnumAdapters(0, &pAdapter);
        if (FAILED(hr))
        {
          return "Unknown";
        }

        DXGI_ADAPTER_DESC desc;
        hr = pAdapter->GetDesc(&desc);
        if (FAILED(hr))
        {
          return "Unknown";
        }

        std::wstring ws(desc.Description);
        return wstring2string(ws);
      }
      catch (...) {
        return "获取GPU信息失败";
      }
    }
  };
};