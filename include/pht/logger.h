#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_LOGGER_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_LOGGER_H_

#include <cstdint>
#include <cstdlib>

#include <regex>
#include <string>

#include "pht/timer.h"

#define PHT_LOGGER_DEBUG(tag, param)   pht::Logger::log(pht::Logger::LogLevel::DEBUG,   tag, param, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_INFO(tag, param)    pht::Logger::log(pht::Logger::LogLevel::INFO,    tag, param, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_WARNING(tag, param) pht::Logger::log(pht::Logger::LogLevel::WARNING, tag, param, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_ERROR(tag, param)   pht::Logger::log(pht::Logger::LogLevel::ERROR,   tag, param, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_FATAL(tag, param)   pht::Logger::log(pht::Logger::LogLevel::FATAL,   tag, param, __FILE__, __LINE__, __func__)

namespace pht {
    class Logger {
    public:
        enum class LogLevel {
            DEBUG = 0, INFO = 1, WARNING = 2, ERROR = 3, FATAL = 4
        };

    public:
        static void log(LogLevel level, const std::string& tag, const pht::Timer& timer, const std::string& file, uint32_t line, const std::string& func);
        static void log(LogLevel level, const std::string& tag, const std::string& message, const std::string& file, uint32_t line, const std::string& func);

        static LogLevel getLogLevel();
        static void setLogLevel(LogLevel level);

        static void setStdOutEnabled(bool enabled);

    private:
        static LogLevel logLevel;
        static Timer timer;
        static std::regex unifyLinebreakRegex;
        static std::ostream stdOutWrapper;
        static std::streambuf* stdoutBuf;
    };
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_LOGGER_H_