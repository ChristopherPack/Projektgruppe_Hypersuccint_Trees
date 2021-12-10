#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_PRECOMPUTED_FUNCTION_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_PRECOMPUTED_FUNCTION_H_

#include <functional>
#include <map>
#include <mutex>

namespace pht {
    /**
     * This class allows trading of memory for speed by holding precomputed results of a function in memory (static caching). 
     * Use for functions which are very often called with the same arguments, where the result does not often change over time and the arguments known ahead of time. 
     * 
     * Example usage: 
     * std::function<uint32_t(uint32_t, uint32_t)> add = [](uint32_t a, uint32_t b){ std::this_thread::sleep_for(1s); return a+b; }; 
     *  
     * pht::PrecomputedFunction cachedAdd = pht::PrecomputedFunction([](std::map<std::tuple<uint32_t, uint32_t>, uint32_t> cache) { 
     *     for(uint32_t i = 0; i < 1000; i++) { 
     *         cache.insert({{i, i}, add(i, i)}); 
     *     } 
     * }); 
     *  
     * //Slow
     * for(uint32_t i = 0; i < 1000; i++) { 
     *     add(i, i); 
     * } 
     * 
     * //Fast
     * for(uint32_t i = 0; i < 1000; i++) { 
     *     cachedAdd(i, i); 
     * } 
     *  
     * @tparam R The return-type of the function. 
     * @tparam Args The parameter-types of the function. 
     */
    template<class R, class... Args> class PrecomputedFunction {
    private:
        bool dirty = true;
        std::map<std::tuple<Args...>, R> cache;
        std::function<void(std::map<std::tuple<Args...>, R>&)> func;

    public:
        /**
         * Create a new PrecomputedFunction with the help of another function which populates a map used as cache. 
         * @param func A function which takes a std::map<R,Args...> and populates it with the results of the cached function.  
         */
        explicit PrecomputedFunction(std::function<void(std::map<std::tuple<Args...>, R>&)> func_) : func(func_) {}
        
        /**
         * Marks the content of the cache as dirty and triggers a repopulation of the cache on the next call. 
         */
        void markDirty() {
            dirty = true;
        }

        /**
         * Override of the call-operator to allow the seamless integration of PrecomputedFunctions as function replacements.
         * @param args The arguments for the cached function. 
         * @return R The result of the function. 
         */
        R operator()(Args... args) {
            if(dirty) {
                cache.clear();
                func(cache);
                dirty = false;
            }
            return cache.at(std::tuple(args...));
        }
    };
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_PRECOMPUTED_FUNCTION_H_