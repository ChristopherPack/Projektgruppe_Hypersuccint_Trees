#include "pht/hst_node.h"

std::ostream &operator<<(std::ostream &out, const pht::HstNode &node) {
    return out << "(" << node.mini << ", " << node.micro << ", " << node.node << ")";
}

pht::HstNode::HstNode() : mini(-1), micro(-1), node(-1) {}

pht::HstNode::HstNode(uint32_t mini, uint32_t micro, uint32_t node) : mini(mini), micro(micro), node(node) {}

bool pht::HstNode::operator==(const pht::HstNode &b) const {
    return mini==b.mini && micro==b.micro && node==b.node;
}

bool pht::HstNode::operator!=(const pht::HstNode &b) const {
    return !(*this==b);
}