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

#ifdef PHT_LOGGER_QUIET
#define PHT_LOGGER_DEBUG(tag)   pht::Logger::log(pht::Logger::LogLevel::PHT_DEBUG,   tag, __FILE__, __LINE__, __func__,true)
#define PHT_LOGGER_INFO(tag)    pht::Logger::log(pht::Logger::LogLevel::PHT_INFO,    tag, __FILE__, __LINE__, __func__,true)
#define PHT_LOGGER_WARNING(tag) pht::Logger::log(pht::Logger::LogLevel::PHT_WARNING, tag, __FILE__, __LINE__, __func__,true)
#define PHT_LOGGER_ERROR(tag)   pht::Logger::log(pht::Logger::LogLevel::PHT_ERROR,   tag, __FILE__, __LINE__, __func__,true)
#define PHT_LOGGER_FATAL(tag)   pht::Logger::log(pht::Logger::LogLevel::PHT_FATAL,   tag, __FILE__, __LINE__, __func__,true)
#else
#define PHT_LOGGER_DEBUG(tag)   pht::Logger::log(pht::Logger::LogLevel::PHT_DEBUG,   tag, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_INFO(tag)    pht::Logger::log(pht::Logger::LogLevel::PHT_INFO,    tag, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_WARNING(tag) pht::Logger::log(pht::Logger::LogLevel::PHT_WARNING, tag, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_ERROR(tag)   pht::Logger::log(pht::Logger::LogLevel::PHT_ERROR,   tag, __FILE__, __LINE__, __func__)
#define PHT_LOGGER_FATAL(tag)   pht::Logger::log(pht::Logger::LogLevel::PHT_FATAL,   tag, __FILE__, __LINE__, __func__)
#endif

namespace pht {
    /**
     * This class allows the printing of neatly formated, information-enriched log messages. 
     * Example usage:
     * PHT_LOGGER_INFO("Test") << "This is a test" << pht::Logger::endl();
     */
    class __declspec(dllexport) Logger {
        friend class LogStream;

    public:
        /**
         * This enum defines the different LogLevels. More important log-levels will supress lower-level messages if used as filter. 
         */
        enum class __declspec(dllexport) LogLevel {
            PHT_DEBUG = 0, PHT_INFO = 1, PHT_WARNING = 2, PHT_ERROR = 3, PHT_FATAL = 4
        };

    public:

        /**
         * This class allows the printing of messages as stream instead of function-arguments. 
         * DO NOT save objects of this class. 
         * DO NOT use/instantiate/... this class expect for their standard use in Logger. 
         */
        //TODO Change to better process of stream-printing messages. 
        class __declspec(dllexport) LogStream {
            friend class Logger;

        public:
            /**
             * Append a value to the current message. Must be terminated with pht::Logger::endl() to be printed. 
             * This method will fail if this stream is not the most current, i.e. another logging call was made before this on was finished or this stream was already printed. 
             * @tparam T A printable type. 
             * @param value The value to print. 
             * @return LogStream& This stream, for call-chaining. 
             */
            template<class T> LogStream& operator<<(const T& value) {
                if(Logger::getCurrentLogStream() != this) {
                    throw std::runtime_error("Invalid log stream");
                }
                content << value;
                return *this;
            }

            /**
             * Template specialization to detect ending of messages.
             * @param value The void* const to print. 
             * @return LogStream& This stream, for call-chaining. DO NOT USE!
             */
            template<> LogStream& operator<<(void* const& value) {
                if(Logger::getCurrentLogStream() != this) {
                    throw std::runtime_error("Invalid log stream");
                }
                if(value == Logger::endl()) {
                    if(!quiet) {
                        Logger::_log(level, tag, content.str(), file, line, func);
                    }
                    content.clear();
                } else {
                    content << value;
                }
                return *this;
            }

        private:
            #pragma warning(disable:4251)
            std::stringstream content;
            LogLevel level;
            const std::string tag;
            const std::string file;
            uint32_t line;
            const std::string func;
            bool quiet;
            #pragma warning(default:4251)

            LogStream(LogLevel level, const std::string& tag, const std::string& file, uint32_t line, const std::string& func, bool quiet) : level(level), tag(tag), file(file), line(line), func(func), quiet(quiet) {}
        };

    public:
        /**
         * Create a new log stream to print a message to.  
         * DO NOT CALL THIS FUNCTION DIRECTLY!!!
         * Use the PHT_LOGGER_... macros intead. 
         * @param level The log level to use for this message/log-stream. 
         * @param tag The tag (category) of this message/log-stream. 
         * @param file The file from which this message/log-stream originates. 
         * @param line The line in the file from which this message/log-stream originates.
         * @param func The function from which this message/log-stream originates. 
         * @param quiet Disable printing. 
         * @return LogStream& A stream to print a message to. 
         */
        static LogStream& log(LogLevel level, const std::string& tag, const std::string& file, uint32_t line, const std::string& func, bool quiet = false);

        /**
         * Returns the current log-level used in filtering messages.
         * @return LogLevel The current log-level. 
         */
        static LogLevel getLogLevel();

        /**
         * Sets the log-level to filter messages.
         * The default level is DEBUG. 
         * @param level The new level. 
         */
        static void setLogLevel(LogLevel level);

        /**
         * Allows disabling of the standard output. 
         * @param enabled The new state of the standard output.
         */
        static void setStdOutEnabled(bool enabled);

        /**
         * Used to end logging messages. 
         * Must be appended to every message to print it. 
         * @return void* A marker value to end a log-stream and print its value. 
         */
        static void* endl();

        /**
         * The most current (and only valid) log-stream. 
         * @return LogStream* The log-stream.
         */
        static LogStream* getCurrentLogStream();

    private:
        #pragma warning(disable:4251)
        static LogLevel logLevel;
        static Timer timer;
        static std::regex unifyLinebreakRegex;
        static std::ostream stdOutWrapper;
        static std::streambuf* stdoutBuf;
        static std::unique_ptr<LogStream> currentLogStream;
        #pragma warning(default:4251)

        static void _log(LogLevel level, const std::string& tag, const std::string& message, const std::string& file, uint32_t line, const std::string& func);
    };
}

#undef DLL_API
#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_LOGGER_H_