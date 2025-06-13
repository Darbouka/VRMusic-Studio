#include "Logger.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <filesystem>
#include <mutex>

namespace VRMusicStudio {
namespace Core {

namespace {
    constexpr const char* LOG_DIR = "logs";
    constexpr const char* LOG_FILE = "vrmusicstudio.log";
    constexpr size_t MAX_FILE_SIZE = 5 * 1024 * 1024;  // 5MB
    constexpr size_t MAX_FILES = 3;
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    try {
        // Erstelle Logs-Verzeichnis
        std::filesystem::create_directories(LOG_DIR);

        // Konfiguriere Logger
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::debug);
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");

        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            std::string(LOG_DIR) + "/" + LOG_FILE,
            MAX_FILE_SIZE,
            MAX_FILES
        );
        file_sink->set_level(spdlog::level::debug);
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");

        std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
        logger_ = std::make_shared<spdlog::logger>("VRMusicStudio", sinks.begin(), sinks.end());
        logger_->set_level(spdlog::level::debug);
        logger_->flush_on(spdlog::level::debug);

        spdlog::register_logger(logger_);
        spdlog::set_default_logger(logger_);
    } catch (const std::exception& e) {
        // Fallback auf Konsolen-Logger
        logger_ = spdlog::stdout_color_mt("VRMusicStudio");
        logger_->set_level(spdlog::level::debug);
        logger_->error("Fehler bei der Logger-Initialisierung: {}", e.what());
    }
}

Logger::~Logger() {
    spdlog::shutdown();
}

void Logger::setLogLevel(LogLevel level) {
    logger_->set_level(static_cast<spdlog::level::level_enum>(level));
}

LogLevel Logger::getLogLevel() const {
    return static_cast<LogLevel>(logger_->level());
}

void Logger::flush() {
    logger_->flush();
}

void Logger::info(const std::string& msg) {
    if (logger_) logger_->info(msg);
    else spdlog::info(msg);
}

void Logger::error(const std::string& msg) {
    if (logger_) logger_->error(msg);
    else spdlog::error(msg);
}

} // namespace Core
} // namespace VRMusicStudio 