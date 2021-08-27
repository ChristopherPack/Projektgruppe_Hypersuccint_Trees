
#ifndef PROJEKTSUCCINCTTREES_C_WRAPPER_H
#define PROJEKTSUCCINCTTREES_C_WRAPPER_H

extern "C" {

    class __declspec(dllexport) C_WRAPPER {
        public:
        /**
         * Returns if given Node is ancestor of Dummy within the Node's MiniTree
         * TODO: Look at the Micro Version - This can clearly be optimized
         *
         * @param node The Node as HstNode
         * @return if Node is ancestor of MiniDummy as bool
         */
        bool isDummyAncestorWithinMiniTree(HstNode node) {
        };

        /**
         * Returns if  given Node is ancestor of Dummy within the Node's MicroTree
         *
         * @param node  The Node as HstNode
         * @return if Node is ancestor of MicroDummy as bool
         */
        bool isDummyAncestorWithinMicroTree(HstNode node) {
        };

        /**
         * Returns the ith child of a given Node, if it exists
         *
         * @param parent The parent Node as HStNode
         * @param index The index as unint32_t
         * @return The ith child as HstNode
         */
        HstNode child(HstNode parent, uint32_t index) {
        };

        /**
         * Returns the Child Rank of a given Node
         *
         * @param node The Node as HstNode
         * @return The Child Rank as int
         */
        uint32_t childRank(HstNode node) {
        };

        /**
         * Finds the direct Parent of the given Node
         * Very important helper function!
         * TODO: Could be private
         *
         * @param node The node as HSTNode
         * @return The parent as HstNode
         */
        HstNode getParent(HstNode node) {
        };

        /**
         * Returns the degree of a given Node
         *
         * @param node The Node as HstNode
         * @return the degree as int
         */
        uint32_t degree(HstNode node) {
        };

        /**
         * Returns the Subtree size of a given Node
         *
         * @param node The Node as HstNode
         * @return Subtree size as int
         */
        uint32_t subtreeSize(HstNode node) {
        };

        /**
         * Returns the Depth of a given Node
         *
         * @param node The Node as HstNode
         * @return The Depth as uint32_t
         */
        uint32_t depth(HstNode node) {
        };

        /**
         * Returns the Depth of a given Node
         *
         * @param node The Node as HstNode
         * @return The Depth as uint32_t
         */
        uint32_t height(HstNode node) {
        };

        /**
         * Returns the leftmost leaf of a given Node
         *
         * @param node The Node as HstNode
         * @return The leftmost leaf as HstNode
         */
        HstNode leftmostLeaf(HstNode node) {
        };

        /**
         * Returns the rightmost leaf of a given Node
         *
         * @param node The Node as HstNode
         * @return The rightmost leaf as HstNode
         */
        HstNode rightmostLeaf(HstNode node) {
        };

        /**
         * Returns the Leaf size of a given Node
         *
         * @param node The Node as HstNode
         * @return the Leaf Size as uint32_t
         */
        uint32_t leafSize(HstNode node) {
        };

        /**
         * Returns the Leaf Rank of a given Node
         * Leaf Rank is the amount of Leaves coming before the Node, in node order
         *
         * @param node The Node as HstNode
         * @return the Leaf Size as uint32_t
         */
        uint32_t leafRank(HstNode node) {
        };

        /**
         * TODO: Unfinished
         *
         * @param level
         * @param node
         * @return
         */
        HstNode levelAncestor(HstNode node, uint32_t level) {
        };

};


#endif //PROJEKTSUCCINCTTREES_C_WRAPPER_H

