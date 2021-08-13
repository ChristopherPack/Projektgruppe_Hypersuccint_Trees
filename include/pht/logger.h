#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_LOGGER_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_LOGGER_H_

#include <cstdint>
#include <cstdlib>

#include <regex>
#include <string>

#include "pht/timer.h"

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#define PHT_LOGGER_DEBUG(tag, param)   pht::Logger::log(pht::Logger::LogLevel::PHT_DEBUG,   tag, param, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_INFO(tag, param)    pht::Logger::log(pht::Logger::LogLevel::PHT_INFO,    tag, param, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_WARNING(tag, param) pht::Logger::log(pht::Logger::LogLevel::PHT_WARNING, tag, param, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_ERROR(tag, param)   pht::Logger::log(pht::Logger::LogLevel::PHT_ERROR,   tag, param, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_FATAL(tag, param)   pht::Logger::log(pht::Logger::LogLevel::PHT_FATAL,   tag, param, __FILE__, __LINE__, __func__)

namespace pht {
    class __declspec(dllexport) Logger {
    public:
        enum class __declspec(dllexport) LogLevel {
            PHT_DEBUG = 0, PHT_INFO = 1, PHT_WARNING = 2, PHT_ERROR = 3, PHT_FATAL = 4
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

#undef DLL_API
#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_LOGGER_H_