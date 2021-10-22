#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_TIMED_CACHED_FUNCTION_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_TIMED_CACHED_FUNCTION_H_

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace pht {
    template<class R, class... Args> class TimedCachedFunction {
    private:
        std::function<R(Args...)> func;
        std::map<std::tuple<Args...>, std::tuple<R, std::chrono::time_point<std::chrono::steady_clock>>> cache;
        std::unique_ptr<std::thread> garbageCollector = nullptr;
        std::atomic<bool> garbageCollectorRunning = true;
        std::mutex cacheLock;

    public:
        TimedCachedFunction(std::function<R(Args...)> func, uint32_t validityIntervallMs = 60000, uint32_t garbageCollectorIntervallMs = 1000) : func(func) {
            garbageCollector = std::unique_ptr<std::thread>(new std::thread([this, validityIntervallMs, garbageCollectorIntervallMs](){
                while(garbageCollectorRunning) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(garbageCollectorIntervallMs));
                    cacheLock.lock();
                    for(auto iter = cache.begin(), iterEnd = cache.end(); iter != iterEnd; ++iter) {
                        if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-std::get<1>(iter->second)).count() >= validityIntervallMs) {
                            iter = cache.erase(iter);
                        }
                        if(iter == iterEnd) {
                            break;
                        }
                    }
                    cacheLock.unlock();
                };
            }));
        }

        void add(Args... args) {
            cacheLock.lock();
            _add(args...);
            cacheLock.unlock();
        }
        
        void invalidate() {
            cacheLock.lock();
            for(std::pair<std::tuple<Args...>, R> entry : cache) {
                _add(entry.first);
            }
            cacheLock.unlock();
        }
        
        void clear() {
            cacheLock.lock();
            cache.clear();
            cacheLock.unlock();
        }
        
        R operator()(Args... args) {
            cacheLock.lock();
            if(cache.find(std::tuple(args...)) == cache.end()) {
                _add(args...);
            }
            std::tuple<R, std::chrono::time_point<std::chrono::steady_clock>> entry = cache.at(std::tuple(args...));
            cacheLock.unlock();
            return std::get<0>(entry);
        }

        ~TimedCachedFunction() {
            garbageCollectorRunning = false;
            garbageCollector->join();
        }

    private:
        void _add(Args... args) {
            std::tuple<R, std::chrono::time_point<std::chrono::steady_clock>> data = {func(args...), std::chrono::steady_clock::now()};
            cache.insert_or_assign(std::tuple(args...), data);
        }
    };
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_TIMED_CACHED_FUNCTION_H_