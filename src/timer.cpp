#include "pht/timer.h"

#include <cassert>
#include <sstream>

void pht::Timer::start() {
    startTime = std::chrono::high_resolution_clock::now();
}

void pht::Timer::stop() {
    stopTime = std::chrono::high_resolution_clock::now();
}

std::chrono::duration<uint32_t, std::ratio<1,1000>> pht::Timer::getDuration() const {
    return std::chrono::duration_cast<std::chrono::duration<uint32_t, std::ratio<1, 1000>>>(stopTime-startTime);
}

std::string pht::Timer::toString() const {
    std::ostringstream oss;
    oss << *this;
    return oss.str();
}

std::ostream& operator<<(std::ostream& out, const pht::Timer& timer) {
    std::chrono::duration<uint32_t, std::ratio<0001, 1000>> millis = timer.getDuration();
    std::chrono::duration<uint32_t, std::ratio<0001, 0001>> secs   = std::chrono::duration_cast<std::chrono::duration<uint32_t, std::ratio<0001, 0001>>>(millis);
    std::chrono::duration<uint32_t, std::ratio<0060, 0001>> mins   = std::chrono::duration_cast<std::chrono::duration<uint32_t, std::ratio<0060, 0001>>>(millis);
    std::chrono::duration<uint32_t, std::ratio<3600, 0001>> hours  = std::chrono::duration_cast<std::chrono::duration<uint32_t, std::ratio<3600, 0001>>>(millis);
    std::string hoursFill(hours.count()<10?1:0, '0');
    std::string minsFill(mins.count()<10?1:0, '0');
    std::string secsFill(secs.count()<10?1:0, '0');
    std::string millisFill(millis.count()<10?2:(millis.count()<100?1:0), '0');
    out << hoursFill << hours.count()%24 << ":" << minsFill << mins.count()%60 << ":" << secsFill << secs.count()%60 << ":" << millisFill << millis.count()%1000;
    return out;
}