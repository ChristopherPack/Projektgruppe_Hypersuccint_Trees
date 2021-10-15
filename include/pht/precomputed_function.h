#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_PRECOMPUTED_FUNCTION_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_PRECOMPUTED_FUNCTION_H_

#include <functional>
#include <map>

namespace pht {
    template<class R, class... Args> class PrecomputedFunction {
    private:
        bool dirty = true;
        std::map<std::tuple<Args...>, R> cache;
        std::function<void(std::map<std::tuple<Args...>, R>&)> func;

    public:
        PrecomputedFunction(std::function<void(std::map<std::tuple<Args...>, R>&)> func) : func(func) {}
        
        PrecomputedFunction() {}
        
        void markDirty() {
            dirty = true;
        }

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