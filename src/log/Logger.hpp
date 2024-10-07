#pragma once

#include <chrono>
#include <format>
#include <iostream>
#include <mutex>
#include <ostream>
#include <string>

namespace log {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL,
};

class Logger final {
    std::ostream* dst_;

    Logger() noexcept : dst_(&std::cerr) {}

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    static Logger& get() {
        static Logger instance{};
        return instance;
    }

public:
    static void Log();
};

}