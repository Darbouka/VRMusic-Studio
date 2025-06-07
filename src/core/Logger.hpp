#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <memory>
#include <spdlog/spdlog.h>

namespace VR_DAW {

class Logger {
public:
    explicit Logger(const std::string& name);
    ~Logger();

    // Logging-Methoden
    template<typename... Args>
    void trace(const std::string& fmt, Args&&... args) {
        logger->trace(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void debug(const std::string& fmt, Args&&... args) {
        logger->debug(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(const std::string& fmt, Args&&... args) {
        logger->info(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warn(const std::string& fmt, Args&&... args) {
        logger->warn(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(const std::string& fmt, Args&&... args) {
        logger->error(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void critical(const std::string& fmt, Args&&... args) {
        logger->critical(fmt, std::forward<Args>(args)...);
    }

    // Konfiguration
    void setLogLevel(spdlog::level::level_enum level);
    void setLogFile(const std::string& filePath);
    void setConsoleOutput(bool enable);
    void setFileOutput(bool enable);

private:
    std::shared_ptr<spdlog::logger> logger;
    std::string name;
    std::string logFilePath;
    bool consoleOutput;
    bool fileOutput;
    std::mutex mutex;

    void initializeLogger();
    void updateLogger();
};

} // namespace VR_DAW 