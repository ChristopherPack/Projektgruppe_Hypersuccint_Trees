#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_CACHED_FUNCTION_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_CACHED_FUNCTION_H_

#include <functional>
#include <map>

namespace pht {
    template<class R, class... Args> class CachedFunction {
    private:
        std::function<R(Args...)> func;
        std::map<std::tuple<Args...>, R> cache;

    public:
        CachedFunction(std::function<R(Args...)> func) : func(func) {}

        void add(Args... args) {
            cache.insert_or_assign(std::tuple(args...), func(args...));
        }
        
        void invalidate() {
            for(std::pair<std::tuple<Args...>, R> entry : cache) {
                add(entry.first);
            }
        }
        
        void clear() {
            cache.clear();
        }

        R operator()(Args... args) {
            if(cache.find(std::tuple(args...)) == cache.end()) {
                add(args...);
            }
            return cache.at(std::tuple(args...));
        }
    };
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_CACHED_FUNCTION_H_