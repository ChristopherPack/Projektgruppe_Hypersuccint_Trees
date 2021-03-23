#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"

#define PHT_TEST

#include "pht/hypersuccinct_tree.h"
#include "pht/hypersuccinct_tree_factory.h"

class HypersuccinctTreeTest : public ::testing::Test {
protected:
    std::shared_ptr<pht::UnorderedTree<char>> example = std::make_shared<pht::UnorderedTree<char>>();
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

    void SetUp() override {
        example->add(a);
        example->add(b, a);
        example->add(c, a);
        example->add(d, a);
        example->add(e, a);
        example->add(f, a);
        example->add(g, b);
        example->add(h, b);
        example->add(i, b);
        example->add(j, b);
        example->add(k, d);
        example->add(l, g);
        example->add(m, i);
        example->add(n, k);
        example->add(o, k);
        example->add(p, k);
        example->add(q, o);
        example->add(r, o);
        example->add(s, q);
        example->add(t, q);
        example->add(u, q);
        example->add(v, q);
        example->add(w, q);
        example->add(x, r);
        example->add(y, r);
        example->add(z, r);
        example->add(A, t);
        example->add(B, x);
    }
};
