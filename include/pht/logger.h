#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_LOGGER_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_LOGGER_H_

#include <cstdint>
#include <cstdlib>

#include <memory>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

#include "pht/timer.h"

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#define PHT_LOGGER_DEBUG(tag)   pht::Logger::log(pht::Logger::LogLevel::PHT_DEBUG,   tag, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_INFO(tag)    pht::Logger::log(pht::Logger::LogLevel::PHT_INFO,    tag, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_WARNING(tag) pht::Logger::log(pht::Logger::LogLevel::PHT_WARNING, tag, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_ERROR(tag)   pht::Logger::log(pht::Logger::LogLevel::PHT_ERROR,   tag, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_FATAL(tag)   pht::Logger::log(pht::Logger::LogLevel::PHT_FATAL,   tag, __FILE__, __LINE__, __func__)

namespace pht {
    class __declspec(dllexport) Logger {
            friend class LogStream;

    public:
        enum class __declspec(dllexport) LogLevel {
            PHT_DEBUG = 0, PHT_INFO = 1, PHT_WARNING = 2, PHT_ERROR = 3, PHT_FATAL = 4
        };

    public:
        class __declspec(dllexport) LogStream {
            friend class Logger;

        public:
            template<class T> LogStream& operator<<(const T& value) {
                if(Logger::getCurrentLogStream() != this) {
                    throw std::runtime_error("Invalid log stream");
                }
                content << value;
                return *this;
            }

            template<> LogStream& operator<<(void* const& value) {
                if(Logger::getCurrentLogStream() != this) {
                    throw std::runtime_error("Invalid log stream");
                }
                if(value == Logger::endl()) {
                    Logger::_log(level, tag, content.str(), file, line, func);
                    content.clear();
                } else {
                    content << value;
                }
                return *this;
            }

        private:
            std::stringstream content;
            LogLevel level;
            const std::string tag;
            const std::string file;
            uint32_t line;
            const std::string func;

            LogStream(LogLevel level, const std::string& tag, const std::string& file, uint32_t line, const std::string& func) : level(level), tag(tag), file(file), line(line), func(func) {}
        };

    public:
        static LogStream& log(LogLevel level, const std::string& tag, const std::string& file, uint32_t line, const std::string& func);

        static LogLevel getLogLevel();
        static void setLogLevel(LogLevel level);

        static void setStdOutEnabled(bool enabled);

        static void* endl();

        static LogStream* getCurrentLogStream();

    private:
        static LogLevel logLevel;
        static Timer timer;
        static std::regex unifyLinebreakRegex;
        static std::ostream stdOutWrapper;
        static std::streambuf* stdoutBuf;
        static std::unique_ptr<LogStream> currentLogStream;

        static void _log(LogLevel level, const std::string& tag, const std::string& message, const std::string& file, uint32_t line, const std::string& func);
    };
}

#undef DLL_API
#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_LOGGER_H_