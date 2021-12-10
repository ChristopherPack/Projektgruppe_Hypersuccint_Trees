#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_CACHED_FUNCTION_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_CACHED_FUNCTION_H_

#include <functional>
#include <map>
#include <mutex>

namespace pht {
    /**
     * This class allows trading of memory for speed by caching results of a function (dynamic caching). 
     * Use for functions which are very often called with the same arguments, where the result does not often change over time and the arguments not known ahead of time. 
     * 
     * Example usage: 
     * std::function<uint32_t(uint32_t, uint32_t)> add = [](uint32_t a, uint32_t b){ std::this_thread::sleep_for(1s); return a+b; }; 
     *  
     * pht::CachedFunction cachedAdd = pht::CachedFunction(add); 
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
    template<class R, class... Args> class CachedFunction {
    private:
        std::function<R(Args...)> func;
        std::map<std::tuple<Args...>, R> cache;
        std::mutex lock;

    public:
        /**
         * Create a new CachedFunction from a previously slow function. 
         * @param func The function to cache results for. 
         */
        explicit CachedFunction(std::function<R(Args...)> func_) : func(func_) {}
        
        /**
         * Reevaluates all cache entries. 
         */
        void invalidate() {
            std::scoped_lock<std::mutex> cLock(lock);
            for(std::pair<std::tuple<Args...>, R> entry : cache) {
                cache.insert_or_assign(entry.first, std::apply(func, entry.first));
            }
        }
        
        /**
         * Clears the cache. 
         */
        void clear() {
            std::scoped_lock<std::mutex> cLock(lock);
            cache.clear();
        }

        /**
         * Override of the call-operator to allow the seamless integration of CachedFunctions as function replacements.
         * @param args The arguments for the cached function. 
         * @return R The result of the function. 
         */
        R operator()(Args... args) {
            std::unique_lock<std::mutex> cLock(lock);
            if(cache.find(std::tuple(args...)) == cache.end()) {
                cache.insert_or_assign(std::tuple(args...), func(args...));
            }
            R tmp = cache.at(std::tuple(args...));
            cLock.unlock();
            return tmp;
        }
    };
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_CACHED_FUNCTION_H_