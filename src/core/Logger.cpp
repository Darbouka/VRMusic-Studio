#include "Logger.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <filesystem>

namespace VR_DAW {

Logger::Logger(const std::string& name)
    : name(name)
    , consoleOutput(true)
    , fileOutput(false) {
    initializeLogger();
}

Logger::~Logger() {
    spdlog::drop(name);
}

void Logger::initializeLogger() {
    std::vector<spdlog::sink_ptr> sinks;

    if (consoleOutput) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::debug);
        sinks.push_back(console_sink);
    }

    if (fileOutput && !logFilePath.empty()) {
        try {
            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                logFilePath,
                1024 * 1024 * 5,  // 5MB
                3                 // 3 Dateien
            );
            file_sink->set_level(spdlog::level::debug);
            sinks.push_back(file_sink);
        }
        catch (const spdlog::spdlog_ex& ex) {
            // Fehler beim Erstellen der Datei-Sink
            consoleOutput = true;
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(spdlog::level::debug);
            sinks.push_back(console_sink);
        }
    }

    logger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
    logger->set_level(spdlog::level::debug);
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] %v");
    spdlog::register_logger(logger);
}

void Logger::setLogLevel(spdlog::level::level_enum level) {
    std::lock_guard<std::mutex> lock(mutex);
    logger->set_level(level);
}

void Logger::setLogFile(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(mutex);
    logFilePath = filePath;
    fileOutput = true;
    updateLogger();
}

void Logger::setConsoleOutput(bool enable) {
    std::lock_guard<std::mutex> lock(mutex);
    consoleOutput = enable;
    updateLogger();
}

void Logger::setFileOutput(bool enable) {
    std::lock_guard<std::mutex> lock(mutex);
    fileOutput = enable;
    updateLogger();
}

void Logger::updateLogger() {
    spdlog::drop(name);
    initializeLogger();
}

} // namespace VR_DAW 