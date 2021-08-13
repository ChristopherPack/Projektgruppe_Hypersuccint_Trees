#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_NODE_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_NODE_H_

#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

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

        ~Node() {
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

        bool isMiniDummy() {
            return miniDummy;
        }

        void setMiniDummy(bool val = true) {
            miniDummy = val;
        }

        void setMiniTree(uint32_t val = -1) {
            miniTree = val;
        }

        uint32_t getMiniTree() {
            return miniTree;
        }

        void setMicroTree(uint32_t val = -1) {
            microTree = val;
        }

        uint32_t getMicroTree() {
            return microTree;
        }

    private:
        T value;///The stored value of the node 
        bool miniDummy = false;
        uint32_t miniTree;
        uint32_t microTree;
    };
}

#undef DLL_API
#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_NODE_H_