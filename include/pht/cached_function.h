#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_CACHED_FUNCTION_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_CACHED_FUNCTION_H_

#include <functional>
#include <map>
#include <mutex>

namespace pht {
    template<class R, class... Args> class CachedFunction {
    private:
        std::function<R(Args...)> func;
        std::map<std::tuple<Args...>, R> cache;
        std::mutex lock;

    public:
        CachedFunction(std::function<R(Args...)> func) : func(func) {}
        
        void invalidate() {
            std::scoped_lock<std::mutex> cLock(lock);
            for(std::pair<std::tuple<Args...>, R> entry : cache) {
                cache.insert_or_assign(entry.first, std::apply(func, entry.first));
            }
        }
        
        void clear() {
            std::scoped_lock<std::mutex> cLock(lock);
            cache.clear();
        }

        R operator()(Args... args) {
            std::unique_lock<std::mutex> cLock(lock);
            if(cache.find(std::tuple(args...)) == cache.end()) {
                cache.insert_or_assign(std::tuple(args...), func(args...));
            }
            return cache.at(std::tuple(args...));
            cLock.unlock();
        }
    };
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_CACHED_FUNCTION_H_