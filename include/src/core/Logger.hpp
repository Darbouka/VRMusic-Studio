#pragma once

#include <string>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace VRMusicStudio {

class Logger {
public:
    explicit Logger(const std::string& name);
    ~Logger();

    template<typename... Args>
    void info(const char* fmt, Args&&... args) {
        if (logger) {
            logger->info(fmt, std::forward<Args>(args)...);
        }
    }

    template<typename... Args>
    void error(const char* fmt, Args&&... args) {
        if (logger) {
            logger->error(fmt, std::forward<Args>(args)...);
        }
    }

private:
    std::shared_ptr<spdlog::logger> logger;
    static void initializeLogger();
    static bool initialized;
};

} // namespace VRMusicStudio 