#include "pht/timer.h"

#include <sstream>

void pht::Timer::start() {
    startTime = std::chrono::high_resolution_clock::now();
}

void pht::Timer::stop() {
    stopTime = std::chrono::high_resolution_clock::now();
}

std::chrono::duration<uint32_t, std::micro> pht::Timer::getDuration() const {
    return std::chrono::duration_cast<std::chrono::duration<uint32_t, std::micro>>(stopTime-startTime);
}

std::string pht::Timer::toString() const {
    std::ostringstream oss;
    oss << *this;
    return oss.str();
}

std::ostream& operator<<(std::ostream& out, const pht::Timer& timer) {
    std::chrono::duration<uint32_t, std::micro> micros = timer.getDuration();
    auto millis = std::chrono::duration_cast<std::chrono::duration<uint32_t, std::milli>>(micros);
    auto secs   = std::chrono::duration_cast<std::chrono::duration<uint32_t, std::ratio<1,1>>>(micros);
    auto mins   = std::chrono::duration_cast<std::chrono::duration<uint32_t, std::ratio<60,1>>>(micros);
    auto hours  = std::chrono::duration_cast<std::chrono::duration<uint32_t, std::ratio<3600,1>>>(micros);
    std::string hoursFill((hours.count()%24)<10?1:0, '0');
    std::string minsFill ((mins.count()%60)<10?1:0, '0');
    std::string secsFill((secs.count()%60)<10?1:0, '0');
    std::string millisFill((millis.count()%1000)<10?2:((millis.count()%1000)<100?1:0), '0');
    std::string microsFill((micros.count()%1000)<10?2:((micros.count()%1000)<100?1:0), '0');
    out << hoursFill << hours.count()%24 << ":" << minsFill << mins.count()%60 << ":" << secsFill << secs.count()%60 << "." << millisFill << millis.count()%1000 << "_" << microsFill << micros.count()%1000;
    return out;
}