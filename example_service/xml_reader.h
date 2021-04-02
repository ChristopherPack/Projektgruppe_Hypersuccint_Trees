#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_XML_READER_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_XML_READER_H_

#include "pht/unordered_tree.h"

namespace pht {
    /**
     * 
     */
    class XMLReader {
    public:
        /**
         * 
         */
        static std::shared_ptr<UnorderedTree<std::string>> read(const std::string& path);

        static std::shared_ptr<UnorderedTree<std::string>> readByName(const std::string& name);
    };
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_XML_READER_H_