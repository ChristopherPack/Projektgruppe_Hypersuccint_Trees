#include <iostream>
#include <filesystem>
#include <direct.h>

#include <irrXML.h>

#include "xml_reader.h"
using namespace std::filesystem;

std::shared_ptr<pht::UnorderedTree<std::string>> pht::XMLReader::read(const std::string& path) {
    std::shared_ptr<pht::UnorderedTree<std::string>> xmlTree = std::make_shared<pht::UnorderedTree<std::string>>();
    std::shared_ptr<pht::Node<std::string>> current = nullptr;

    std::shared_ptr<irr::io::IrrXMLReader> xml = std::shared_ptr<irr::io::IrrXMLReader>(irr::io::createIrrXMLReader(path.c_str()));
    if(!xml || !xml->read())
        throw std::runtime_error("Failed to read xml file");

    uint32_t element = 0;
    do {
        element++;
        if(xml->getNodeType() == irr::io::EXN_ELEMENT) {
            std::shared_ptr<pht::Node<std::string>> node = std::make_shared<pht::Node<std::string>>(xml->getNodeName());
            xmlTree->add(node, current);
            if(!xml->isEmptyElement())
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

/**
 * Reads the specified file located in Projektgruppe_Hypersuccint_Trees/example_service
 *
 * @param name
 * @return unorderedTree from xml
 */
std::shared_ptr<pht::UnorderedTree<std::string>> pht::XMLReader::readByName(const std::string &name) {

    char temp[256];
    _getcwd( temp, 256); //der Programmpfad ist jetzt in 'temp' gespeichert
    path myRoot(temp);
    path directory = myRoot;

    while (directory.stem() != "Projektgruppe_Hypersuccint_Trees"){
        directory = directory.parent_path();
    }

    directory /= "example_service\\";

    return read(directory.string() + name);

}