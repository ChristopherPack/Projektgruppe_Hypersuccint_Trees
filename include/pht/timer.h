#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_TIMER_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_TIMER_H_

#include <cstdint>

#include <chrono>
#include <ostream>


extern "C" {
    namespace pht {
        class Timer;
    }

    std::ostream &operator<<(std::ostream &out, const pht::Timer &timer);

    namespace pht {
        class __declspec(dllexport) Timer {
        public:
            void start();

            void stop();

            std::chrono::duration<uint32_t, std::micro> getDuration() const;

            std::string toString() const;

            friend std::ostream &::operator<<(std::ostream &out, const Timer &timer);

        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
            std::chrono::time_point<std::chrono::high_resolution_clock> stopTime = std::chrono::high_resolution_clock::now();
        };
    }

}
#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_TIMER_H_