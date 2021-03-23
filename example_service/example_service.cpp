#include <iostream>
#include <string>

#include <irrXML.h>
#define PHT_TEST
#include "pht/unordered_tree.h"
#include "pht/farzan_munro.h"
#include "xml_reader.h"
#include "pht/hypersuccinct_tree_factory.h"
#include "pht/hypersuccinct_tree.h"
#include "pht/bitvector_utils.h"

std::shared_ptr<pht::UnorderedTree<char>> createTestTree();


void printBitvector(std::vector<bool> bitvector) {
    for(bool bit: bitvector) {
        std::cout << bit;
    }
    std::cout << std::endl;
}

int main() {
    //todo: Needs complete restructuring

    std::shared_ptr<pht::UnorderedTree<char>> tree = createTestTree();



    //Couts are necessary like this
    std::cout << "Original tree:\n" << *tree << "\n\n";
    std::cout << std::endl;



    std::shared_ptr<pht::UnorderedTree<std::string>> xmlTree = pht::XMLReader::read("D:\\Nutzerdaten\\Dokumente\\Studium_Informatik\\Projektgruppe TheoInf\\ProjektSuccinctTrees\\XML\\DBLP.xml");

    pht::HypersuccinctTree<std::string> hst = pht::HypersuccinctTreeFactory::create(xmlTree);

    std::cout << "Original Tree data:" << std::endl;
    std::cout << xmlTree->getSize() << std::endl;
    std::cout << xmlTree->toNewickString() << std::endl;

    std::cout << "Hypersuccinct Tree:" << std::endl;
    std::cout << "MiniSize:  ";
    printBitvector(hst.getMiniSize());
    std::cout << "MicroSize:  ";
    printBitvector(hst.getMicroSize());
    std::cout << std::endl;

    std::cout << "Amount of MiniTrees: " << hst.getMiniTrees().size() << std::endl;
    std::cout << "MiniFIDs:  ";
    printBitvector(hst.getMiniFIDs());
    std::cout << "MiniTypeVectors:  ";
    printBitvector(hst.getminiTypeVectors());
    std::cout << "MiniDummys:  ";
    printBitvector(hst.getMiniDummys());

    pht::MiniTree miniTree = hst.getMiniTree(0);
    std::cout << "MicroTreesinMiniTree0:  ";
    printBitvector(hst.getMiniTree(0).microTrees);
    std::cout << "MicroTreeFIDs:  ";
    printBitvector(hst.getMiniTree(0).FIDs);
    std::cout << "MicroTreeTypeVectors:  ";
    printBitvector(hst.getMiniTree(0).typeVectors);
    std::cout << "MicroTreeDummys:  ";
    printBitvector(hst.getMiniTree(0).dummys);
    return 0;
}

std::shared_ptr<pht::UnorderedTree<char>> createTestTree() {
    std::shared_ptr<pht::UnorderedTree<char>> tree = std::make_shared<pht::UnorderedTree<char>>();
    std::shared_ptr<pht::Node<char>> a = std::make_shared<pht::Node<char>>('a');
    std::shared_ptr<pht::Node<char>> b = std::make_shared<pht::Node<char>>('b');
    std::shared_ptr<pht::Node<char>> c = std::make_shared<pht::Node<char>>('c');
    std::shared_ptr<pht::Node<char>> d = std::make_shared<pht::Node<char>>('d');
    std::shared_ptr<pht::Node<char>> e = std::make_shared<pht::Node<char>>('e');
    std::shared_ptr<pht::Node<char>> f = std::make_shared<pht::Node<char>>('f');
    std::shared_ptr<pht::Node<char>> g = std::make_shared<pht::Node<char>>('g');
    std::shared_ptr<pht::Node<char>> h = std::make_shared<pht::Node<char>>('h');
    std::shared_ptr<pht::Node<char>> i = std::make_shared<pht::Node<char>>('i');
    std::shared_ptr<pht::Node<char>> j = std::make_shared<pht::Node<char>>('j');
    std::shared_ptr<pht::Node<char>> k = std::make_shared<pht::Node<char>>('k');
    std::shared_ptr<pht::Node<char>> l = std::make_shared<pht::Node<char>>('l');
    std::shared_ptr<pht::Node<char>> m = std::make_shared<pht::Node<char>>('m');
    std::shared_ptr<pht::Node<char>> n = std::make_shared<pht::Node<char>>('n');
    std::shared_ptr<pht::Node<char>> o = std::make_shared<pht::Node<char>>('o');
    std::shared_ptr<pht::Node<char>> p = std::make_shared<pht::Node<char>>('p');
    std::shared_ptr<pht::Node<char>> q = std::make_shared<pht::Node<char>>('q');
    std::shared_ptr<pht::Node<char>> r = std::make_shared<pht::Node<char>>('r');
    std::shared_ptr<pht::Node<char>> s = std::make_shared<pht::Node<char>>('s');
    std::shared_ptr<pht::Node<char>> t = std::make_shared<pht::Node<char>>('t');
    std::shared_ptr<pht::Node<char>> u = std::make_shared<pht::Node<char>>('u');
    std::shared_ptr<pht::Node<char>> v = std::make_shared<pht::Node<char>>('v');
    std::shared_ptr<pht::Node<char>> w = std::make_shared<pht::Node<char>>('w');
    std::shared_ptr<pht::Node<char>> x = std::make_shared<pht::Node<char>>('x');
    std::shared_ptr<pht::Node<char>> y = std::make_shared<pht::Node<char>>('y');
    std::shared_ptr<pht::Node<char>> z = std::make_shared<pht::Node<char>>('z');
    std::shared_ptr<pht::Node<char>> A = std::make_shared<pht::Node<char>>('A');
    std::shared_ptr<pht::Node<char>> B = std::make_shared<pht::Node<char>>('B');

    tree->add(a);
    tree->add(b, a);
    tree->add(c, a);
    tree->add(d, a);
    tree->add(e, a);
    tree->add(f, a);
    tree->add(g, b);
    tree->add(h, b);
    tree->add(i, b);
    tree->add(j, b);
    tree->add(k, d);
    tree->add(l, g);
    tree->add(m, i);
    tree->add(n, k);
    tree->add(o, k);
    tree->add(p, k);
    tree->add(q, o);
    tree->add(r, o);
    tree->add(s, q);
    tree->add(t, q);
    tree->add(u, q);
    tree->add(v, q);
    tree->add(w, q);
    tree->add(x, r);
    tree->add(y, r);
    tree->add(z, r);
    tree->add(A, t);
    tree->add(B, x);

    return tree;
}