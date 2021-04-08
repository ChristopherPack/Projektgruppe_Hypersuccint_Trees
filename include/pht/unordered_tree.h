#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_UNORDERED_TREE_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_UNORDERED_TREE_H_

#include <map>
#include <sstream>
#include <iostream>
#include <memory>
#include <cassert>
#include <string>
#include <vector>
#include <bitset>

#include "node.h"
#include "list_utils.h"

#define ASSERT(condition, msg) assert((condition) && msg);

namespace pht {
    /**
     * Stores the topology of an unordered tree. 
     * 
     * This class only stores the topology of a tree, all information is stored in nodes, 
     * which are referenced via pointers. The topology is stored via a list of nodes and 
     * two maps for ancestor and descendant lookup. 
     * 
     * @tparam T The type of data stored in the nodes. 
     */
    //TODO Caching
    //Todo: Rewrite tree for consistent ORDER
    template<class T> class UnorderedTree {
    public:
        UnorderedTree() {
        }

        ~UnorderedTree() {
        }
        
        /**
         * Adds a node to the tree. 
         * 
         * This method will add a new node to the tree. The node will be a child of the given ancestor. 
         * A root node can be added by not setting the second parameter or using nullptr. Adding a root 
         * node to a non-empty tree will fail. 
         * 
         * @param[in] node A pointer to the node to add. A node cannot be added multiple times to the same 
         *  tree, use a new node with the same value instead. 
         * @param[in] ancestor Optional. A pointer to the ancestor of the new node, or nullptr to add a root. 
         *     Can only be nullptr if the tree is empty. Adding a node to an empty tree will ignore this parameter.
         */
        void add(const std::shared_ptr<pht::Node<T>> node, const std::shared_ptr<pht::Node<T>> ancestor = nullptr) {
            ASSERT(node, "Invalid node");
            ASSERT(ancestor || !root, "Invalid ancestor");
            ASSERT(!root || std::find(nodes.begin(), nodes.end(), ancestor) != nodes.end(), "Ancestor not found");
            ASSERT(std::find(nodes.begin(), nodes.end(), node) == nodes.end(), "Duplicated node");

            refreshEnumerate = true;
            nodes.push_back(node);
            descendants.insert({node, std::vector<std::shared_ptr<pht::Node<T>>>()});
            if(root == nullptr) {
                ancestors.insert({node, nullptr});
                root = node;
            } else {
                ancestors.insert({node, ancestor});
                descendants.at(ancestor).push_back(node);
            }
        }

        /**
         * Todo: Rewrite tree for consistent ORDER
         * Adds a node to the tree.
         *
         * This method will add a new node to the tree. The node will be a child of the given ancestor.
         * A root node can be added by not setting the second parameter or using nullptr. Adding a root
         * node to a non-empty tree will fail.
         *
         * @param[in] node A pointer to the node to add. A node cannot be added multiple times to the same
         *  tree, use a new node with the same value instead.
         * @param[in] ancestor Optional. A pointer to the ancestor of the new node, or nullptr to add a root.
         *     Can only be nullptr if the tree is empty. Adding a node to an empty tree will ignore this parameter.
         */
        void insert(const std::shared_ptr<pht::Node<T>> node, uint32_t index, const std::shared_ptr<pht::Node<T>> ancestor = nullptr) {
            ASSERT(node, "Invalid node");
            ASSERT(ancestor || !root, "Invalid ancestor");
            ASSERT(!root || std::find(nodes.begin(), nodes.end(), ancestor) != nodes.end(), "Ancestor not found");
            ASSERT(std::find(nodes.begin(), nodes.end(), node) == nodes.end(), "Duplicated node");
            ASSERT(ancestor == nullptr || index <= descendants.at(ancestor).size() , "Invalid index");

            refreshEnumerate = true;
            nodes.push_back(node);
            descendants.insert({node, std::vector<std::shared_ptr<pht::Node<T>>>()});
            if(root == nullptr) {
                ancestors.insert({node, nullptr});
                root = node;
            } else {
                ancestors.insert({node, ancestor});
                descendants.at(ancestor).insert(descendants.at(ancestor).begin() + index, node);
            }
        }

        /**
         * Adds a subtree to the tree. 
         * 
         * This method will add the nodes in the subtree to the tree. The subtree root will be a child of the 
         * given ancestor. A copy of the subtree can be made by not setting the second parameter or using nullptr. 
         * Using no ancestor on a non-empty tree will fail. 
         * 
         * @param[in] tree A std::shared_ptr to an unordered tree to add. A tree cannot be added if it contains already contained node. 
         * @param[in] ancestor Optional. A pointer to the ancestor of the new subtree, or nullptr to add at the root. 
         *     Can only be nullptr if the tree is empty. Adding a tree to an empty tree will ignore this parameter. 
         */
        void add(const std::shared_ptr<UnorderedTree<T>> tree, const std::shared_ptr<pht::Node<T>> ancestor = nullptr) {
            ASSERT(tree.get() != this, "Invalid tree");
            ASSERT(tree, "Invalid tree");
            ASSERT(ancestor || !root, "Invalid ancestor");
            ASSERT(!root || std::find(nodes.begin(), nodes.end(), ancestor) != nodes.end(), "Ancestor not found");
            std::vector<std::shared_ptr<pht::Node<T>>> newNodes = tree->nodes;
            for(std::shared_ptr<pht::Node<T>> node : newNodes) {
                ASSERT(std::find(nodes.begin(), nodes.end(), node) == nodes.end(), "Duplicated node");
            }

            refreshEnumerate = true;
            nodes.insert(nodes.end(), newNodes.begin(), newNodes.end());
            descendants.insert(tree->descendants.begin(), tree->descendants.end());
            ancestors.insert(tree->ancestors.begin(), tree->ancestors.end());
            if(root == nullptr) {
                ancestors.insert({tree->root, nullptr});
                root = tree->root;
            } else {
                ancestors.insert({tree->root, ancestor});
                descendants.at(ancestor).push_back(tree->root);
            }
        }

        /**
         * Returns the current root of the tree.
         * 
         * @return A pointer to the root of the tree, may be nullptr.
         */
        std::shared_ptr<pht::Node<T>> getRoot() const {
            return root;
        }

        /**
         * Returns all nodes in this tree.
         * 
         * @return A list of pointers to the nodes of the tree.
         */
        std::vector<std::shared_ptr<pht::Node<T>>> getNodes() const {
            return nodes;
        }

        /**
         * Returns all direct descendants of the given node. 
         * 
         * @param[in] node A pointer to the ancestor node. Cannot be nullptr. 
         * @return A list which contains pointers to all direct descendants of the given node. 
         */
        std::vector<std::shared_ptr<pht::Node<T>>> getDirectDescendants(const std::shared_ptr<pht::Node<T>> node) const {
            ASSERT(node, "Invalid node");
            ASSERT(std::find(nodes.begin(), nodes.end(), node) != nodes.end(), "Node not found");

            return descendants.at(node);
        }

        /**
         * Returns all direct descendants of the given node. 
         * 
         * @param[in] node A pointer to the ancestor node. Cannot be nullptr. 
         * @param[in] size The size as of a node is considered heavy. 
         * @return A list which contains pointers to all direct heavy descendants of the given node. 
         */
        std::vector<std::shared_ptr<pht::Node<T>>> getHeavyDirectDescendants(const std::shared_ptr<pht::Node<T>> node, const uint32_t size) const {
            ASSERT(node, "Invalid node");
            ASSERT(std::find(nodes.begin(), nodes.end(), node) != nodes.end(), "Node not found");
            ASSERT(size >= 1, "Size cannot be 0");

            std::vector<std::shared_ptr<pht::Node<T>>> children = getDirectDescendants(node);
            children.erase(std::remove_if(children.begin(), children.end(), [this, size](std::shared_ptr<pht::Node<T>> child){ return !isHeavy(child, size); }), children.end());
            children.shrink_to_fit();
            return children;
        }

        /**
         * Returns all descendants of the given node. 
         * 
         * This method recursively collects all descendants of the given node. 
         * 
         * @param[in] node A pointer to the ancestor node. Cannot be nullptr. 
         * @return A list which contains pointers to all descendants of the given node. 
         */
        std::vector<std::shared_ptr<pht::Node<T>>> getAllDescendants(const std::shared_ptr<pht::Node<T>> node) const {
            ASSERT(node, "Invalid node");
            ASSERT(std::find(nodes.begin(), nodes.end(), node) != nodes.end(), "Node not found");

            std::vector<std::shared_ptr<pht::Node<T>>> result(descendants.at(node));
            for(std::shared_ptr<pht::Node<T>> desc : descendants.at(node)) {
                std::vector<std::shared_ptr<pht::Node<T>>> descDesc = getAllDescendants(desc);
                result.insert(result.end(), descDesc.begin(), descDesc.end());
            }
            return result;
        }

        /**
         * Returns the direct ancestor of the given node. 
         * 
         * @param[in] node A pointer to the descendant node. Cannot be nullptr. 
         * @return A pointer to the direct ancestor of the given node or nullptr if the node is the root. 
         */
        std::shared_ptr<pht::Node<T>> getDirectAncestor(const std::shared_ptr<pht::Node<T>> node) const {
            ASSERT(node, "Invalid node");
            ASSERT(std::find(nodes.begin(), nodes.end(), node) != nodes.end(), "Node not found");

            return ancestors.at(node);
        }

        /**
         * Returns all ancestor of the given node. 
         * 
         * This method recursively collects all ancestors of the given node. 
         * 
         * @param[in] node A pointer to the descendant node. Cannot be nullptr. 
         * @return A list which contains pointers to all ancestors of the given node. 
         */
        std::vector<std::shared_ptr<pht::Node<T>>> getAllAncestors(const std::shared_ptr<pht::Node<T>> node) const {
            ASSERT(node, "Invalid node");
            ASSERT(std::find(nodes.begin(), nodes.end(), node) != nodes.end(), "Node not found");

            std::vector<std::shared_ptr<pht::Node<T>>> result;
            std::shared_ptr<pht::Node<T>> current = node;
            while(current != root) {
                current = ancestors.at(current);
                result.push_back(current);
            }
            return result;
        }

        /**
         * Returns the subtree at the current node. 
         * 
         * This method recursively collects all descendants an builds the subtree 
         * which is formed by taking the branch rooted in node. 
         * 
         * @param[in] node A pointer to the subtree root node. Cannot be nullptr. 
         * @return A pointer to the subtree. 
         */
        std::shared_ptr<pht::UnorderedTree<T>> getSubtree(const std::shared_ptr<pht::Node<T>> node) const {
            ASSERT(node, "Invalid node");
            ASSERT(std::find(nodes.begin(), nodes.end(), node) != nodes.end(), "Node not found");

            std::shared_ptr<pht::UnorderedTree<T>> subtree = std::make_shared<UnorderedTree<T>>();
            subtree->add(node);
            for(std::shared_ptr<pht::Node<T>> desc : descendants.at(node)) {
                subtree->add(getSubtree(desc), node);
            }
            return subtree;
        }

        /**
         * Removes the give node from the tree. 
         * 
         * This method remove the node and all references to it from the tree. 
         * The handling of the descendants can be influcend with the migrateDescendants parameter. 
         * 
         * @param[in] node A pointer to the node to delete. Cannot be nullptr. Has to be in the tree. 
         * @param[in] migrateDescendants Optional. When true, descendants of the delete node will be
         *     migrated to the ancestor of the delete node, instead of being deleted. This parameter
         *     will be ignored if node is the root.
         */
        void remove(const std::shared_ptr<pht::Node<T>> node, const bool migrateDescendants = false) {
            ASSERT(node, "Invalid node");
            ASSERT(std::find(nodes.begin(), nodes.end(), node) != nodes.end(), "Node not found");

            refreshEnumerate = true;
            if(migrateDescendants && root != node) {
                for(std::shared_ptr<pht::Node<T>> desc : descendants.at(node)) {
                    ancestors.erase(desc);
                    ancestors.insert({desc, ancestors.at(node)});
                    descendants.at(ancestors.at(node)).push_back(desc);
                }
                descendants.at(ancestors.at(node)).erase(std::remove(descendants.at(ancestors.at(node)).begin(), descendants.at(ancestors.at(node)).end(), node), descendants.at(ancestors.at(node)).end());
                descendants.at(ancestors.at(node)).shrink_to_fit();
                descendants.erase(node);
                ancestors.erase(node);
                nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end());
                nodes.shrink_to_fit();
            } else if(node == root) {
                nodes.clear();
                ancestors.clear();
                descendants.clear();
                root = nullptr;
            } else {
                while(!descendants.at(node).empty()) {
                    remove(descendants.at(node).at(0));
                }
                descendants.erase(node);
                descendants.at(ancestors.at(node)).erase(std::remove(descendants.at(ancestors.at(node)).begin(), descendants.at(ancestors.at(node)).end(), node), descendants.at(ancestors.at(node)).end());
                descendants.at(ancestors.at(node)).shrink_to_fit();
                ancestors.erase(node);
                nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end());
                nodes.shrink_to_fit();
            }
        }

        uint32_t enumerate(const std::shared_ptr<pht::Node<T>> node) {
            if(refreshEnumerate) {
                enumerateCalculator();
                refreshEnumerate = false;
            } else
            {
                return enumeratedCache.at(node);
            }
        }

        void enumerateCalculator() {
            enumeratedCache.clear();
            uint32_t i = 1;
            enumeratedCache.insert({root,0});
            std::vector<std::shared_ptr<pht::Node<T>>> nodes = getDirectDescendants(root);
            while(!nodes.empty()) {
                std::shared_ptr<pht::Node<T>> current = nodes.front();
                nodes.erase(nodes.begin());
                ListUtils::combine(nodes, getDirectDescendants(current));
                enumeratedCache.insert({current,i});
                i++;
            }
        }

        /**
         * Calculates the size of this tree. 
         * 
         * @return The count of nodes in this tree.
         */
        uint32_t getSize() const {
            if(root == nullptr)
                return 0;
            return nodes.size();
        }

        /**
         * Checks if the tree is empty. 
         * 
         * @return True if this tree does not contain any nodes.
         */
        bool isEmpty() const {
            return root == nullptr;
        }

        /**
         * Calculates the height of this tree. 
         * 
         * @return The count of edges in the longest path in this tree. -1 if empty. 
         */
        virtual uint32_t getHeight() const {
            if(root == nullptr)
                return -1;
            return getHeight(root);
        }

        /**
         * Returns true if this tree contains the given node. 
         * 
         * @param[in] node A pointer to the searched node. Cannot be nullptr. 
         * @return True when contained. 
         */
        bool contains(const std::shared_ptr<pht::Node<T>> node) const {
            ASSERT(node, "Invalid node");

            return std::find(nodes.begin(), nodes.end(), node) != nodes.end();
        }

        /**
         * Returns the degree of the given node. 
         * 
         * @param[in] node A pointer to the node. Cannot be nullptr. Has to be in the tree. 
         * @return The count of children of the given node.
         */
        uint32_t getDegree(const std::shared_ptr<pht::Node<T>> node) const {
            ASSERT(node, "Invalid node");
            ASSERT(std::find(nodes.begin(), nodes.end(), node) != nodes.end(), "Node not found");

            return (uint32_t)descendants.at(node).size();
        }

        /**
         * Returns the height of the given node. 
         * 
         * @param[in] node A pointer to the node. Cannot be nullptr. Has to be in the tree. 
         * @return The count of edges in the longest path to a leaf. 
         */
        uint32_t getHeight(const std::shared_ptr<pht::Node<T>> node) const {
            ASSERT(node, "Invalid node");
            ASSERT(std::find(nodes.begin(), nodes.end(), node) != nodes.end(), "Node not found");

            if(descendants.at(node).empty()) {
                return 0;
            }
            uint32_t max = 0;
            for(std::shared_ptr<pht::Node<T>> desc : descendants.at(node)) {
                max = std::max(max, getHeight(desc)+1);
            }
            return max;
        }

        /**
         * Returns the depth of the given node. 
         * 
         * @param[in] node A pointer to the node. Cannot be nullptr. Has to be in the tree. 
         * @return The count of edges in the path to the root. 
         */
        uint32_t getDepth(const std::shared_ptr<pht::Node<T>> node) const {
            ASSERT(node, "Invalid node");
            ASSERT(std::find(nodes.begin(), nodes.end(), node) != nodes.end(), "Node not found");

            if(root == node) {
                return 0;
            }
            std::shared_ptr<pht::Node<T>> current = node;
            uint32_t depth = 0;
            while(current != root) {
                current = ancestors.at(current);
                depth++;
            }
            return depth;
        }

        /**
         * Checks if the given node is the root. 
         * 
         * @param[in] node A pointer to the node. Cannot be nullptr. Has to be in the tree. 
         * @return True if the node is the root. 
         */
        bool isRoot(const std::shared_ptr<pht::Node<T>> node) const {
            ASSERT(node, "Invalid node");
            ASSERT(std::find(nodes.begin(), nodes.end(), node) != nodes.end(), "Node not found");

            return root == node;
        }

        /**
         * Checks if the given node is a leaf. 
         * 
         * @param[in] node A pointer to the node. Cannot be nullptr. Has to be in the tree. 
         * @return True if the node does not have any descendants. 
         */
        bool isLeaf(const std::shared_ptr<pht::Node<T>> node) const {
            ASSERT(node, "Invalid node");
            ASSERT(std::find(nodes.begin(), nodes.end(), node) != nodes.end(), "Node not found");

            return descendants.at(node).empty();
        }

        /**
         * Checks if the given node is heavy as defined by the farzan-munro-algorithm. 
         * 
         * This method calculates the size of the subtree defined by the given node and returns 
         * true if the size is bigger than the given parameter.
         * 
         * @param[in] node A pointer to the node. Cannot be nullptr. Has to be in the tree. 
         * @param[in] size The count of descendants (inclusive node) by which a heavy node is defined. 
         * @return True if the node has greater or equal to size-1 descendants. 
         */
        bool isHeavy(const std::shared_ptr<pht::Node<T>> node, const uint32_t size) const {
            ASSERT(node, "Invalid node");
            ASSERT(std::find(nodes.begin(), nodes.end(), node) != nodes.end(), "Node not found");
            ASSERT(size >= 1, "Size cannot be 0");

            return getSize(node) >= size;
        }

        /**
         * Converts this tree into a string in a human readable debug format. 
         * 
         * The order of the nodes in the returned string is sorted with value1 < value2 to ensure consistency. 
         * 
         * @return The string representation of this tree. 
         */
        std::string toString() const {
            if(root == nullptr) {
                return "";
            }
            return toString(root, false);
        }

        /**
         * Converts this tree into a string in the newick format. 
         * 
         * This method is just for debug purposes. For output, the toString() method should be used. 
         * 
         * @return The newick representation of this tree. 
         */
        std::string toNewickString() const {
            if(root == nullptr) {
                return "";
            }
            return toString(root, true)+std::string(";");
        }

        std::vector<bool> toBalancedParenthesis() {
            std::vector<bool> bp;
            if(root == nullptr) {
                return bp;
            }
            toBP(root,bp);
            return bp;
        }

        /**
         * Helper function to calculate the size of the tree.
         *
         * Used for recursive calls, calulcates the size of a subtree rooted at node.
         * @param[in] node A pointer to the root node of a subtree to calculate the size of.
         * @return The size of the subtree.
         */
        uint32_t getSize(std::shared_ptr<pht::Node<T>> node) const {
            uint32_t size = 1;
            for(std::shared_ptr<pht::Node<T>> desc : descendants.at(node)) {
                size += getSize(desc);
            }
            return size;
        }

    private:
        std::shared_ptr<pht::Node<T>> root; ///The root of the tree. 
        std::vector<std::shared_ptr<pht::Node<T>>> nodes; ///The nodes which are part of this tree topology. 
        std::map<std::shared_ptr<pht::Node<T>>, std::vector<std::shared_ptr<pht::Node<T>>>> descendants; ///The connection info of the topology. 
        std::map<std::shared_ptr<pht::Node<T>>, std::shared_ptr<pht::Node<T>>> ancestors; ///Map for faster and easyer ancestor lookup.
        std::map<std::shared_ptr<pht::Node<T>>, uint64_t > enumeratedCache; ///todo Map for enumeration of nodes in tree

        bool refreshEnumerate = true;



        /**
         * Helper function to stringify the tree (in order, relevant for testing). 
         * 
         * Used for recursive calls, stringify a subtree rooted at node. 
         * @param[in] node A pointer to the root node of a subtree to stringify.
         * @param[in] newick Selects the format, debug when false,, newick when true.
         * @return The subtree as string.
         */
        std::string toString(std::shared_ptr<pht::Node<T>> node, bool newick) const {
            std::stringstream string;
            if(newick) {
                if(!isLeaf(node)) {
                    string << "(";
                }
            } else {
                string << node->getValue() << "{";
            }

            std::vector<std::shared_ptr<pht::Node<T>>> descs = descendants.at(node);
            std::sort(descs.begin(), descs.end(), [](std::shared_ptr<pht::Node<T>> a, std::shared_ptr<pht::Node<T>> b){ return a->getValue() < b->getValue(); });
            for(int i = 0; i < descs.size(); i++) {
                if(i != 0) {
                    string << ",";
                }
                string << toString(descs.at(i), newick);
            }

            if(newick) {
                if(!isLeaf(node)) {
                    string << ")";
                }
                string << node->getValue();
            } else {
                string << "}";
            }
            return string.str();
        }

        /**
         * Helper function to BPify the tree (in order, relevant for testing).
         *
         * Used for recursive calls, BPify a subtree rooted at node.
         * @param[in] node A pointer to the root node of a subtree to stringify.
         * @param[in,out] bitset
         * @param[in] index
         * @return The subtree as string.
         */
        void toBP(std::shared_ptr<pht::Node<T>> node,std::vector<bool>& bp) const {
            bp.push_back(true);
            for(std::shared_ptr<pht::Node<T>> child : getDirectDescendants(node)) {
                toBP(child, bp);
            }
            bp.push_back(false);
        }
   };

    /**
     * Friend function to allow the output of trees in std::ostreams via the << operator. 
     *
     * @relatesalso UnorderedTree
     */
    template<class T> std::ostream& operator<<(std::ostream& out, const UnorderedTree<T>& tree) {
        return out << tree.toString();
    }
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_UNORDERED_TREE_H_