#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_NODE_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_NODE_H_

#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>

namespace pht {
    /**
     * Stores information for use in a tree.
     * 
     * This class stores the information in a tree. It functions as a simple wrapper
     * and is necessary to allow duplicate values in the tree. 
     * 
     * @tparam T The type of data stored. Has to be comparable with < and printable via <<.
     */
    template<class T> class Node {
    public:
        /**
         * Constructs a new node with the given value. 
         * 
         * @param[in] value The value to store in the node. 
         */
        Node(const T value) : value(value) {
        }

        /**
         * Changes the value to the new value. 
         * 
         * @param[in] value The new value. 
         */
        void setValue(const T value) {
            this->value = value;
        }

        /**
         * Returns the stored value. 
         * 
         * @return The value. 
         */
        T getValue() const {
            return value;
        }

    private:
        T value;///The stored value of the node 
    };
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_NODE_H_