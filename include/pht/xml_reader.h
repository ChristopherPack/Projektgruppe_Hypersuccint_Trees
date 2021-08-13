#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_XML_READER_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_XML_READER_H_

#include "unordered_tree.h"

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

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
#undef DLL_API
#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_XML_READER_H_