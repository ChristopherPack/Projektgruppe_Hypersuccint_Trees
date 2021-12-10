#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_TIMED_CACHED_FUNCTION_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_TIMED_CACHED_FUNCTION_H_

#include <cassert>

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

namespace pht {
    /**
     * This class allows trading of memory for speed by caching results of a function (dynamic caching). 
     * Use for functions which are very often called with the same arguments, where the result does not often change over time and the arguments not known ahead of time. 
     * This function removes old values from the memory to keep the cache smaller.
     * Each TimedCachedFunction uses a new thread as garbage collector, so use this class sparingly. 
     * 
     * Example usage: 
     * std::function<uint32_t(uint32_t, uint32_t)> add = [](uint32_t a, uint32_t b){ std::this_thread::sleep_for(1s); return a+b; }; 
     *  
     * pht::TimedCachedFunction cachedAdd = pht::TimedCachedFunction(add); 
     *  
     * //Slow
     * for(uint32_t i = 0; i < 1000; i++) { 
     *     add(i, i); 
     * } 
     * //Slow
     * for(uint32_t i = 0; i < 1000; i++) { 
     *     cachedAdd(i, i); 
     * } 
     * 
     * //Fast
     * for(uint32_t i = 0; i < 1000; i++) { 
     *     cachedAdd(5, 3); 
     * } 
     *  
     * @tparam R The return-type of the function. 
     * @tparam Args The parameter-types of the function. 
     */
    template<class R, class... Args> class TimedCachedFunction {
    private:
        std::function<R(Args...)> func;
        std::map<std::tuple<Args...>, std::tuple<R, std::chrono::time_point<std::chrono::steady_clock>>> cache;
        std::unique_ptr<std::thread> garbageCollector = nullptr;
        std::atomic<bool> garbageCollectorRunning = true;
        std::mutex cacheLock;

    public:
        /**
         * Create a new TimedCachedFunction from a previously slow function. 
         * @param func The function to cache results for. 
         * @param validityIntervallMs The duration a cache entry is valid, in milliseconds. 
         * @param garbageCollectorIntervallMs The duration between runs of the garbage collector thread. 
         */
        TimedCachedFunction(std::function<R(Args...)> func, uint32_t validityIntervallMs = 60000, uint32_t garbageCollectorIntervallMs = 1000) : func(func) {
            assert(validityIntervallMs>=garbageCollectorIntervallMs);
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
        
        /**
         * Reevaluates all cache entries and resets their lifetime to zero. 
         */
        void invalidate() {
            cacheLock.lock();
            for(std::pair<std::tuple<Args...>, std::tuple<R, std::chrono::time_point<std::chrono::steady_clock>>> entry : cache) {
                std::tuple<R, std::chrono::time_point<std::chrono::steady_clock>> data = {std::apply(func, entry.first), std::chrono::steady_clock::now()};
                cache.insert_or_assign(entry.first, data);
            }
            cacheLock.unlock();
        }
        
        /**
         * Clears the cache. 
         */
        void clear() {
            cacheLock.lock();
            cache.clear();
            cacheLock.unlock();
        }

        /**
         * Override of the call-operator to allow the seamless integration of TimedCachedFunction as function replacements.
         * @param args The arguments for the cached function. 
         * @return R The result of the function. 
         */
        R operator()(Args... args) {
            cacheLock.lock();
            if(cache.find(std::tuple(args...)) == cache.end()) {
                std::tuple<R, std::chrono::time_point<std::chrono::steady_clock>> data = {func(args...), std::chrono::steady_clock::now()};
                cache.insert_or_assign(std::tuple(args...), data);
            }
            std::tuple<R, std::chrono::time_point<std::chrono::steady_clock>> entry = cache.at(std::tuple(args...));
            cacheLock.unlock();
            return std::get<0>(entry);
        }

        /**
         * Destructs a TimedCachedFunction. 
         */
        ~TimedCachedFunction() {
            garbageCollectorRunning = false;
            garbageCollector->join();
        }
    };
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_TIMED_CACHED_FUNCTION_H_