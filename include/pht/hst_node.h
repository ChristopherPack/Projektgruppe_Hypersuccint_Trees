#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_HST_NODE_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_HST_NODE_H_

#include <cstdint>

#include <chrono>
#include <ostream>

namespace pht {
    class HstNode;
}

std::ostream &operator<<(std::ostream &out, const pht::HstNode &node);

namespace pht {
    class __declspec(dllexport) HstNode {
    public:
        HstNode();
        HstNode(uint32_t mini, uint32_t micro, uint32_t node);
        bool operator==(const HstNode &b) const;
        bool operator!=(const HstNode &b) const;

        uint32_t mini;
        uint32_t micro;
        uint32_t node;
    };
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_HST_NODE_H_