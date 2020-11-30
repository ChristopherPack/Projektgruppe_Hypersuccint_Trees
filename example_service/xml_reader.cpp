#include <iostream>

#include <irrXML.h>

#include "xml_reader.h"

std::shared_ptr<pht::UnorderedTree<std::string>> pht::XMLReader::read(const std::string path) {
    std::shared_ptr<pht::UnorderedTree<std::string>> xmlTree = std::make_shared<pht::UnorderedTree<std::string>>();
    std::shared_ptr<pht::Node<std::string>> current = nullptr;

    std::shared_ptr<irr::io::IrrXMLReader> xml = std::shared_ptr<irr::io::IrrXMLReader>(irr::io::createIrrXMLReader(path.c_str()));
    if(!xml || !xml->read())
        throw std::runtime_error("Failed to read xml file");

    uint32_t element = 0;
    do {
        element++;
        if(element%1000 == 0)
            std::cout << element << std::endl;
        if(element >= 5963000)
            std::cout << element << " " << xml->getNodeType() << " " << xml->getAttributeCount() << " " << xml->getNodeName() << std::endl;
        if(xml->getNodeType() == irr::io::EXN_ELEMENT) {
            std::shared_ptr<pht::Node<std::string>> node = std::make_shared<pht::Node<std::string>>(xml->getNodeName());
            xmlTree->add(node, current);
            current = node;
            for(int i = 0; i < xml->getAttributeCount(); i++) {
                std::shared_ptr<pht::Node<std::string>> attributeName = std::make_shared<pht::Node<std::string>>(std::string(xml->getAttributeName(i)));
                xmlTree->add(attributeName, current);
                std::shared_ptr<pht::Node<std::string>> attributeValue = std::make_shared<pht::Node<std::string>>(std::string(xml->getAttributeValue(i)));
                xmlTree->add(attributeValue, attributeName);
            }
        } else if(xml->getNodeType() == irr::io::EXN_ELEMENT_END) {
            if(!xmlTree->isRoot(current)) {
                current = xmlTree->getDirectAncestor(current);
            }
        } else if(xml->getNodeType() == irr::io::EXN_TEXT) {
            std::string text = std::string(xml->getNodeData());
            if(!std::all_of(text.begin(), text.end(), isspace)) {
                std::shared_ptr<pht::Node<std::string>> node = std::make_shared<pht::Node<std::string>>(text);
                xmlTree->add(node, current);
            }
        }
    } while(xml->read());

    return xmlTree;
}