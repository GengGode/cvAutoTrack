#include "pch.h"
#include "Logger.h"
#include <utils/Utils.h>

TianLi::Utils::Logger::Logger() :logLevel(LogLevel::INFO) {
#ifdef _DEBUG
    consoleOutput = true;
#endif
    logFile.open(fileName, std::ios_base::in | std::ios_base::out);
}

void TianLi::Utils::Logger::setLogLevel(LogLevel level) {
    logLevel = level;
}

void TianLi::Utils::Logger::enableConsoleOutput(bool enable) {
    consoleOutput = enable;
}

void TianLi::Utils::Logger::trace(const std::string& message, LogType logType, std::time_t expirationTime) {
    log(LogLevel::TRACE, message, logType, expirationTime);
}

void TianLi::Utils::Logger::debug(const std::string& message, LogType logType, std::time_t expirationTime) {
    log(LogLevel::DEBUG, message, logType, expirationTime);
}

void TianLi::Utils::Logger::info(const std::string& message, LogType logType, std::time_t expirationTime) {
    log(LogLevel::INFO, message, logType, expirationTime);
}

void TianLi::Utils::Logger::warn(const std::string& message, LogType logType, std::time_t expirationTime) {
    log(LogLevel::WARN, message, logType, expirationTime);
}

void TianLi::Utils::Logger::error(const std::string& message, LogType logType, std::time_t expirationTime) {
    log(LogLevel::ERROR, message, logType, expirationTime);
}

void TianLi::Utils::Logger::fatal(const std::string& message, LogType logType, std::time_t expirationTime) {
    log(LogLevel::FATAL, message, logType, expirationTime);
}

/// <summary>
/// 打印以LogEntry存储的日志
/// 如果需要记录更深层次的日志，但不方便打印，可以使用LogEntry暂存，并使用此方法在上层打印
/// </summary>
/// <param name=""></param>
void TianLi::Utils::Logger::putLogEntry(LogEntry entry)
{

}

void TianLi::Utils::Logger::log(LogLevel level, const std::string& message, LogType logType, std::time_t expirationTime) {
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

std::string TianLi::Utils::Logger::getLogLevelString(LogLevel level) {
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

void TianLi::Utils::Logger::log2File(std::string logString)
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

std::string TianLi::Utils::Logger::getSystemInfo()
{
    return get_sys_version() + "\n" + get_gpu_name();
}

std::string TianLi::Utils::Logger::get_sys_version()
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
    result = global::format("{0}-{1}-{2}.{3}", ProductName, DisplayVersion, CurrentBuildNumber, UBR);
    return result;
}

std::string TianLi::Utils::Logger::get_gpu_name()
{
    // Get the name of the GPU
//#ifdef BUILD_CAPTURE_DXGI
//    IDXGIAdapter* pAdapter = nullptr;
//    IDXGIFactory* pFactory = nullptr;
//    try
//    {
//        HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));
//        if (FAILED(hr))
//        {
//            return "Unknown";
//        }
//
//        hr = pFactory->EnumAdapters(0, &pAdapter);
//        if (FAILED(hr))
//        {
//            return "Unknown";
//        }
//
//        DXGI_ADAPTER_DESC desc;
//        hr = pAdapter->GetDesc(&desc);
//        if (FAILED(hr))
//        {
//            return "Unknown";
//        }
//
//        std::wstring ws(desc.Description);
//        return wstring2string(ws);
//    }
//    catch (...) {
//        return "获取GPU信息失败";
//    }
//#endif 
    return "获取GPU信息失败";

}
