#pragma once

#include "common.hpp"

#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_FATAL 1

namespace graphics {

// console colorize utilities
struct ConsoleColor {
    const char* head;
    const char* tail;
};

constexpr ConsoleColor BLACK_FRONT_STR = ConsoleColor{.head = "\x1b[30m", .tail = "\x1b[39m"};
constexpr ConsoleColor RED_FRONT_STR = ConsoleColor{.head = "\x1b[31m", .tail = "\x1b[39m"};
constexpr ConsoleColor GREEN_FRONT_STR = ConsoleColor{.head = "\x1b[32m", .tail = "\x1b[39m"};
constexpr ConsoleColor YELLOW_FRONT_STR = ConsoleColor{.head = "\x1b[33m", .tail = "\x1b[39m"};
constexpr ConsoleColor BLUE_FRONT_STR = ConsoleColor{.head = "\x1b[34m", .tail = "\x1b[39m"};
constexpr ConsoleColor MAGENTA_FRONT_STR = ConsoleColor{.head = "\x1b[35m", .tail = "\x1b[39m"};
constexpr ConsoleColor CYAN_FRONT_STR = ConsoleColor{.head = "\x1b[36m", .tail = "\x1b[39m"};
constexpr ConsoleColor GREY_FRONT_STR = ConsoleColor{.head = "\x1b[37m", .tail = "\x1b[39m"};

constexpr ConsoleColor BLACK_BACK_STR = ConsoleColor{.head = "\x1b[40m", .tail = "\x1b[49m"};
constexpr ConsoleColor RED_BACK_STR = ConsoleColor{.head = "\x1b[41m", .tail = "\x1b[49m"};
constexpr ConsoleColor GREEN_BACK_STR = ConsoleColor{.head = "\x1b[42m", .tail = "\x1b[49m"};
constexpr ConsoleColor YELLOW_BACK_STR = ConsoleColor{.head = "\x1b[43m", .tail = "\x1b[49m"};
constexpr ConsoleColor BLUE_BACK_STR = ConsoleColor{.head = "\x1b[44m", .tail = "\x1b[49m"};
constexpr ConsoleColor MAGENTA_BACK_STR = ConsoleColor{.head = "\x1b[45m", .tail = "\x1b[49m"};
constexpr ConsoleColor CYAN_BACK_STR = ConsoleColor{.head = "\x1b[46m", .tail = "\x1b[49m"};
constexpr ConsoleColor GREY_BACK_STR = ConsoleColor{.head = "\x1b[47m", .tail = "\x1b[49m"};

// from https://pyopyopyo.hatenablog.com/entry/2021/09/06/200342
// /dev/null like output stream implementation

class NullStreambuf : public std::streambuf {
    // can remove buffer?
    // char buffer_[64];

protected:
    // virtual int overflow(int c) override {
    //     setp(buffer_, buffer_ + sizeof(buffer_));
    //     return c == traits_type::eof() ? '\0' : c;
    // }
    virtual int overflow(int) override {
        setp(nullptr, nullptr);
        return '\0';
    }
};

class NullStream : private NullStreambuf, public std::ostream {
public:
    NullStream() : std::ostream(this) {}
    NullStreambuf* rdbuf() const { return (NullStreambuf*)this; }

    // should return false because it should not be used
    explicit operator bool() const { return false; }
};

extern inline NullStream null_stream{};

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL,
};

class Logger {
    static constexpr ConsoleColor COLOR_LEVEL_DEBUG = GREY_FRONT_STR;
    static constexpr ConsoleColor COLOR_LEVEL_INFO = CYAN_FRONT_STR;
    static constexpr ConsoleColor COLOR_LEVEL_WARNING = YELLOW_FRONT_STR;
    static constexpr ConsoleColor COLOR_LEVEL_ERROR = RED_FRONT_STR;
    static constexpr ConsoleColor COLOR_LEVEL_FATAL = MAGENTA_FRONT_STR;

    // TODO: use std::optional for output stream?
    std::ostream& file_out_;
    std::ostream& console_out_;

    Logger(std::ostream& file_out = null_stream, std::ostream& console_out = std::cerr) noexcept : file_out_(file_out), console_out_(console_out) {
        std::cerr << std::format("logger is created.") << std::endl;
    }
    ~Logger() noexcept {
        std::cerr << std::format("logger is destroyed.") << std::endl;
    }
    Logger(const Logger&) = delete;
    auto& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    auto& operator=(Logger&&) = delete;

    std::string timestamp_() const {
        auto time_now = std::chrono::system_clock::now();
        auto time_sec = std::chrono::system_clock::to_time_t(time_now);
        auto time_msec = std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch());
        auto time_local = std::localtime(&time_sec);

        std::array<char, 32> time_str{};
        std::strftime(time_str.data(), time_str.size(), "%Y/%m/%d %H:%M:%S", time_local);
        auto time_msec_val = time_msec.count() % 1000;

        return std::format("{}.{:03d}", time_str.data(), time_msec_val);
    }

    std::string level_str_(LogLevel level) {
        switch(level) {
            case LogLevel::DEBUG:
                return "DEBUG";
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::WARNING:
                return "WARNING";
            case LogLevel::ERROR:
                return "ERROR";
            case LogLevel::FATAL:
                return "FATAL";
            default:
                return "UNKNOWN";
        }
    }

    ConsoleColor level_color_(LogLevel level) {
        switch(level) {
            case LogLevel::DEBUG:
                return COLOR_LEVEL_DEBUG;
            case LogLevel::INFO:
                return COLOR_LEVEL_INFO;
            case LogLevel::WARNING:
                return COLOR_LEVEL_WARNING;
            case LogLevel::ERROR:
                return COLOR_LEVEL_ERROR;
            case LogLevel::FATAL:
                return COLOR_LEVEL_FATAL;
            default:
                return COLOR_LEVEL_DEBUG;
        }
    }

    void output_(LogLevel log_level, const std::string& message) {
        auto timestamp = timestamp_();
        auto level_str = level_str_(log_level);
        auto level_color = level_color_(log_level);

        if(file_out_) {
            file_out_ << std::format("<{}>[{}]: {}", timestamp, level_str, message) << std::endl;
        }

        if(console_out_) {
            console_out_ << std::format("{}<{}>[{}]: {}{}", level_color.head, timestamp, level_str, message, level_color.tail) << std::endl;
        }
    }

public:
    // logger should be initialized once in entry point with proper output streams
    static auto& get(std::ostream& file_out = null_stream, std::ostream& console_out = std::cerr) noexcept {
        static auto logger = Logger(file_out, console_out);

        return logger;
    }

    void debug(const std::string& message) {
        output_(LogLevel::DEBUG, message);
    }

    void info(const std::string& message) {
        output_(LogLevel::INFO, message);
    }

    void warning(const std::string& message) {
        output_(LogLevel::WARNING, message);
    }

    void error(const std::string& message) {
        output_(LogLevel::ERROR, message);
    }

    void fatal(const std::string& message) {
        output_(LogLevel::FATAL, message);
    }
};

#if (LOG_LEVEL_DEBUG)
#define LOG_DEBUG(message) (graphics::Logger::get().debug(message))
#else
#define LOG_DEBUG(message)
#endif

#if (LOG_LEVEL_INFO)
#define LOG_INFO(message) (graphics::Logger::get().info(message))
#else
#define LOG_INFO(message)
#endif

#if (LOG_LEVEL_WARNING)
#define LOG_WARNING(message) (graphics::Logger::get().warning(message))
#else
#define LOG_WARNING(message)
#endif

#if (LOG_LEVEL_ERROR)
#define LOG_ERROR(message) (graphics::Logger::get().error(message))
#else
#define LOG_ERROR(message)
#endif

#if (LOG_LEVEL_FATAL)
#define LOG_FATAL(message) (graphics::Logger::get().fatal(message))
#else
#define LOG_FATAL(message)
#endif

}