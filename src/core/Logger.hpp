#pragma once

#include <spdlog/spdlog.h>
#include <memory>
#include <string>

namespace VRMusicStudio {
namespace Core {

enum class LogLevel {
    Trace = SPDLOG_LEVEL_TRACE,
    Debug = SPDLOG_LEVEL_DEBUG,
    Info = SPDLOG_LEVEL_INFO,
    Warn = SPDLOG_LEVEL_WARN,
    Error = SPDLOG_LEVEL_ERROR,
    Critical = SPDLOG_LEVEL_CRITICAL,
    Off = SPDLOG_LEVEL_OFF
};

class Logger {
public:
    static Logger& getInstance();
    
    void initialize(const std::string& logFile = "vrmusicstudio.log");
    void shutdown();
    
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;
    void flush();
    
    template<typename... Args>
    void trace(const char* fmt, const Args&... args) {
        logger_->trace(fmt, args...);
    }
    
    template<typename... Args>
    void debug(const char* fmt, const Args&... args) {
        logger_->debug(fmt, args...);
    }
    
    template<typename... Args>
    void info(const char* fmt, const Args&... args) {
        logger_->info(fmt, args...);
    }
    
    template<typename... Args>
    void warn(const char* fmt, const Args&... args) {
        logger_->warn(fmt, args...);
    }
    
    template<typename... Args>
    void error(const char* fmt, const Args&... args) {
        logger_->error(fmt, args...);
    }
    
    template<typename... Args>
    void critical(const char* fmt, const Args&... args) {
        logger_->critical(fmt, args...);
    }

    void info(const std::string& msg);
    void error(const std::string& msg);

    // Verhindere Kopieren und Verschieben
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

private:
    Logger();
    ~Logger();
    
    std::shared_ptr<spdlog::logger> logger_;
    void initializeLogger();
};

} // namespace Core
} // namespace VRMusicStudio 