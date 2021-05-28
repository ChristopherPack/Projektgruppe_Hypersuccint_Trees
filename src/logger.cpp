#include "pht/logger.h"

#include <filesystem>
#include <iostream>
#include <sstream>

#define PHT_LOGGER_TAG_WIDTH 10
#define PHT_LOGGER_MSG_WIDTH 150
#define PHT_LOGGER_MSG_OFFSET 22+PHT_LOGGER_TAG_WIDTH
#define PHT_LOGGER_COLORIZE

pht::Logger::LogLevel pht::Logger::logLevel = pht::Logger::LogLevel::DEBUG;
pht::Timer pht::Logger::timer = pht::Timer();
std::regex pht::Logger::unifyLinebreakRegex = std::regex("\r\n?");
std::streambuf* pht::Logger::stdoutBuf = std::cout.rdbuf();
std::ostream pht::Logger::stdOutWrapper = std::ostream(pht::Logger::stdoutBuf);

void pht::Logger::log(pht::Logger::LogLevel level, const std::string& tag, const pht::Timer& message, const std::string& file, uint32_t line, const std::string& func) {
    log(level, tag, timer.toString(), file, line, func);
}

void pht::Logger::log(pht::Logger::LogLevel level, const std::string& tag, const std::string& message, const std::string& file, uint32_t line, const std::string& func) {
    if(logLevel > level) {
        return;
    }

    std::string msg = std::regex_replace(message, unifyLinebreakRegex, "\n");

    #ifdef PHT_LOGGER_COLORIZE
    switch(level) {
        case LogLevel::DEBUG:   stdOutWrapper << "\033[3m"; break;
        case LogLevel::INFO:    stdOutWrapper << "\033[1m"; break;
        case LogLevel::WARNING: stdOutWrapper << "\033[1;33m"; break;
        case LogLevel::ERROR:   stdOutWrapper << "\033[1;31m"; break;
        case LogLevel::FATAL:   stdOutWrapper << "\033[1;37;41m"; break;
    }
    #endif

    timer.stop();
    stdOutWrapper << timer;

    stdOutWrapper << " [";
    switch(level) {
        case LogLevel::DEBUG:   stdOutWrapper << "DBG"; break;
        case LogLevel::INFO:    stdOutWrapper << "INF"; break;
        case LogLevel::WARNING: stdOutWrapper << "WRN"; break;
        case LogLevel::ERROR:   stdOutWrapper << "ERR"; break;
        case LogLevel::FATAL:   stdOutWrapper << "FAT"; break;
        default:                stdOutWrapper << "???"; break;
    }
    stdOutWrapper << "]";

    std::string tagFill(tag.length()>PHT_LOGGER_TAG_WIDTH?0:PHT_LOGGER_TAG_WIDTH-tag.length(), ' ');
    stdOutWrapper << "[" << tag.substr(0, tag.length()>PHT_LOGGER_TAG_WIDTH?PHT_LOGGER_TAG_WIDTH-3:tag.length()) << (tag.length()>PHT_LOGGER_TAG_WIDTH?"...":"") << "]: " << tagFill;

    std::istringstream ss(msg);
    std::string msgLine;
    bool firstLine = true;
    size_t charCount = 0; //HACK Fixes last-empty-line-skip-bug
    while(std::getline(ss, msgLine)) {
        if(firstLine) {
            firstLine = false;
        } else {
            stdOutWrapper << "\n" << std::string(PHT_LOGGER_MSG_OFFSET, ' ');
        }
        stdOutWrapper << msgLine.substr(0, msgLine.length()>PHT_LOGGER_MSG_WIDTH?PHT_LOGGER_MSG_WIDTH-3:msgLine.length()) << (msgLine.length()>PHT_LOGGER_MSG_WIDTH?"...":"");
        charCount += msgLine.length()+1;
    }
    charCount--;
    if(charCount != msg.length() && !msg.empty() && msg.at(msg.length()-1) == '\n') {
        stdOutWrapper << "\n" << std::string(PHT_LOGGER_MSG_OFFSET, ' ');
    }
    if(msgLine.length() < PHT_LOGGER_MSG_WIDTH) {
        stdOutWrapper << std::string(PHT_LOGGER_MSG_WIDTH-msgLine.length(), ' ');
    }

    std::filesystem::path filePath = file;
    stdOutWrapper << " (" << filePath.filename().string() << ":" << line << ":" << func << "(...))";

    #ifdef PHT_LOGGER_COLORIZE
    stdOutWrapper << "\033[0m" << std::endl;
    #endif
}

pht::Logger::LogLevel pht::Logger::getLogLevel() {
    return logLevel;
}

void pht::Logger::setLogLevel(pht::Logger::LogLevel level) {
    logLevel = level;
}

void pht::Logger::setStdOutEnabled(bool enabled) {
    if(enabled) {
        std::cout.rdbuf(stdoutBuf);
    } else {
        stdoutBuf = std::cout.rdbuf();
        std::cout.rdbuf(nullptr);
    }
}