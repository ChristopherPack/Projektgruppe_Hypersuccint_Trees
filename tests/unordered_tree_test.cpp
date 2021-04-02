#include <iostream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"

#include "pht/unordered_tree.h"

class UnorderedTreeTest : public ::testing::Test {
protected:
    std::shared_ptr<pht::UnorderedTree<char>> empty = std::make_shared<pht::UnorderedTree<char>>();
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

TEST_F(UnorderedTreeTest, ConstructorTest) {
    EXPECT_EQ(empty->getRoot(), nullptr);
}

TEST_F(UnorderedTreeTest, LeftShiftOperatorTest) {
    std::stringstream string;
    string << *example;
    EXPECT_STREQ(string.str().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, AddTest1) {
    EXPECT_DEATH({ empty->add((std::shared_ptr<pht::Node<char>>)nullptr, nullptr); }, "Invalid node");
    EXPECT_DEATH({ empty->add((std::shared_ptr<pht::Node<char>>)nullptr, a); }, "Invalid node");
    empty->add(a, b);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_DEATH({ empty->add(b, nullptr); }, "Invalid ancestor");
    EXPECT_DEATH({ empty->add(b, c); }, "Ancestor not found");
    EXPECT_DEATH({ empty->add(a, a); }, "Duplicated node");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    empty->add(c, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{},c{}}");
    empty->add(d, b);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{d{}},c{}}");
}

TEST_F(UnorderedTreeTest, AddTest2) {
	std::shared_ptr<pht::UnorderedTree<char>> addTree = std::make_shared<pht::UnorderedTree<char>>();
    addTree->add(a);
    addTree->add(b, a);
    addTree->add(c, a);
	std::shared_ptr<pht::UnorderedTree<char>> tree = std::make_shared<pht::UnorderedTree<char>>();
    EXPECT_DEATH({ tree->add((std::shared_ptr<pht::UnorderedTree<char>>)tree, nullptr); }, "Invalid tree");
    EXPECT_DEATH({ tree->add((std::shared_ptr<pht::UnorderedTree<char>>)nullptr, nullptr); }, "Invalid tree");
    EXPECT_DEATH({ tree->add((std::shared_ptr<pht::UnorderedTree<char>>)nullptr, a); }, "Invalid tree");
    tree->add(addTree, b);
    EXPECT_STREQ(tree->toString().c_str(), "a{b{},c{}}");
    EXPECT_DEATH({ tree->add(addTree, nullptr); }, "Invalid ancestor");
    EXPECT_DEATH({ tree->add(addTree, d); }, "Ancestor not found");
    EXPECT_DEATH({ tree->add(addTree, a); }, "Duplicated node");
}

TEST_F(UnorderedTreeTest, GetRootTest) {
    EXPECT_STREQ(empty->toString().c_str(), "");
    EXPECT_EQ(empty->getRoot(), nullptr);
    EXPECT_STREQ(empty->toString().c_str(), "");
    empty->add(a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_EQ(empty->getRoot(), a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_EQ(empty->getRoot(), a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
}

TEST_F(UnorderedTreeTest, GetNodesTest) {
    EXPECT_STREQ(empty->toString().c_str(), "");
    EXPECT_TRUE(empty->getNodes().empty());
    EXPECT_STREQ(empty->toString().c_str(), "");
    empty->add(a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_THAT(empty->getNodes(), ::testing::UnorderedElementsAre(a));
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_THAT(empty->getNodes(), ::testing::UnorderedElementsAre(a,b));
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_THAT(example->getNodes(), ::testing::UnorderedElementsAre(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, GetDirectDescendantsTest) {
    EXPECT_DEATH({ empty->getDirectDescendants(nullptr); }, "Invalid node");
    EXPECT_DEATH({ empty->getDirectDescendants(a); }, "Node not found");
    empty->add(a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_THAT(empty->getDirectDescendants(a), ::testing::UnorderedElementsAre());
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_THAT(empty->getDirectDescendants(a), ::testing::UnorderedElementsAre(b));
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_THAT(example->getDirectDescendants(b), ::testing::UnorderedElementsAre(g,h,i,j));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, GetAllDescendantsTest) {
	EXPECT_DEATH({ empty->getDirectDescendants(nullptr); }, "Invalid node");
    EXPECT_DEATH({ empty->getDirectDescendants(a); }, "Node not found");
    empty->add(a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_THAT(empty->getAllDescendants(a), ::testing::UnorderedElementsAre());
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_THAT(empty->getAllDescendants(a), ::testing::UnorderedElementsAre(b));
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_THAT(example->getAllDescendants(b), ::testing::UnorderedElementsAre(g,h,i,j,l,m));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, GetDirectAncestorTest) {
	EXPECT_DEATH({ empty->getDirectAncestor(nullptr); }, "Invalid node");
    EXPECT_DEATH({ empty->getDirectAncestor(a); }, "Node not found");
    empty->add(a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_EQ(empty->getDirectAncestor(a), nullptr);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_EQ(empty->getDirectAncestor(b), a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->getDirectAncestor(q), o);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, GetAllAncestorsTest) {
	EXPECT_DEATH({ empty->getAllAncestors(nullptr); }, "Invalid node");
    EXPECT_DEATH({ empty->getAllAncestors(a); }, "Node not found");
    empty->add(a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_THAT(empty->getAllAncestors(a), ::testing::UnorderedElementsAre());
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_THAT(empty->getAllAncestors(b), ::testing::UnorderedElementsAre(a));
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_THAT(example->getAllAncestors(q), ::testing::UnorderedElementsAre(o,k,d,a));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, GetSubtreeTest) {
	EXPECT_DEATH({ empty->getSubtree(nullptr); }, "Invalid node");
    EXPECT_DEATH({ empty->getSubtree(a); }, "Node not found");
    empty->add(a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_STREQ(empty->getSubtree(a)->toString().c_str(), "a{}");
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(empty->getSubtree(b)->toString().c_str(), "b{}");
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_STREQ(example->getSubtree(a)->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_STREQ(example->getSubtree(b)->toString().c_str(), "b{g{l{}},h{},i{m{}},j{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_STREQ(example->getSubtree(r)->toString().c_str(), "r{x{B{}},y{},z{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_STREQ(example->getSubtree(A)->toString().c_str(), "A{}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, RemoveTest) {
	EXPECT_DEATH({ empty->remove(nullptr); }, "Invalid node");
    EXPECT_DEATH({ empty->remove(a); }, "Node not found");
	EXPECT_DEATH({ empty->remove(nullptr, true); }, "Invalid node");
    EXPECT_DEATH({ empty->remove(a, true); }, "Node not found");
    empty->add(a);
    empty->add(b, a);
    empty->remove(a, true);
    EXPECT_STREQ(empty->toString().c_str(), "");
    empty->add(a);
    empty->add(b, a);
    empty->remove(a);
    EXPECT_STREQ(empty->toString().c_str(), "");
    empty->add(a);
    empty->add(b, a);
    empty->add(c, b);
    empty->remove(b, true);
    EXPECT_STREQ(empty->toString().c_str(), "a{c{}}");
    empty->remove(a);
    EXPECT_STREQ(empty->toString().c_str(), "");
    empty->add(a);
    empty->add(b, a);
    empty->add(c, b);
    empty->remove(b);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    example->remove(b, true);
    EXPECT_STREQ(example->toString().c_str(), "a{c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{},g{l{}},h{},i{m{}},j{}}");
    example->remove(A);
    EXPECT_STREQ(example->toString().c_str(), "a{c{},d{k{n{},o{q{s{},t{},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{},g{l{}},h{},i{m{}},j{}}");
    example->remove(q);
    EXPECT_STREQ(example->toString().c_str(), "a{c{},d{k{n{},o{r{x{B{}},y{},z{}}},p{}}},e{},f{},g{l{}},h{},i{m{}},j{}}");
}

TEST_F(UnorderedTreeTest, GetSizeTest) {
    EXPECT_STREQ(empty->toString().c_str(), "");
    EXPECT_EQ(empty->getSize(), 0);
    EXPECT_STREQ(empty->toString().c_str(), "");
    empty->add(a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_EQ(empty->getSize(), 1);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_EQ(empty->getSize(), 2);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->getSize(), 28);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, IsEmptyTest) {
	EXPECT_STREQ(empty->toString().c_str(), "");
    EXPECT_TRUE(empty->isEmpty());
    EXPECT_STREQ(empty->toString().c_str(), "");
    empty->add(a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_FALSE(empty->isEmpty());
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_FALSE(empty->isEmpty());
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_FALSE(example->isEmpty());
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, GetHeightTest1) {
	EXPECT_STREQ(empty->toString().c_str(), "");
    EXPECT_EQ(empty->getHeight(), -1);
    EXPECT_STREQ(empty->toString().c_str(), "");
    empty->add(a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_EQ(empty->getHeight(), 0);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_EQ(empty->getHeight(), 1);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    empty->add(c, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{},c{}}");
    EXPECT_EQ(empty->getHeight(), 1);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{},c{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->getHeight(), 6);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, ContainsTest) {
    std::shared_ptr<pht::Node<char>> x = std::make_shared<pht::Node<char>>('X');
	EXPECT_DEATH({ empty->contains(nullptr); }, "Invalid node");
    empty->add(a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_TRUE(empty->contains(a));
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_FALSE(empty->contains(b));
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_TRUE(empty->contains(a));
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_TRUE(empty->contains(b));
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_TRUE(example->contains(a));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_TRUE(example->contains(A));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_TRUE(example->contains(l));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_FALSE(example->contains(x));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, GetDegreeTest) {
	EXPECT_DEATH({ empty->getAllAncestors(nullptr); }, "Invalid node");
    EXPECT_DEATH({ empty->getAllAncestors(a); }, "Node not found");
    empty->add(a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_EQ(empty->getDegree(a), 0);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_EQ(empty->getDegree(a), 1);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->getDegree(a), 5);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->getDegree(A), 0);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->getDegree(l), 0);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->getDegree(d), 1);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, GetHeightTest2) {
	EXPECT_DEATH({ empty->getHeight(nullptr); }, "Invalid node");
    EXPECT_DEATH({ empty->getHeight(a); }, "Node not found");
    empty->add(a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_EQ(empty->getHeight(a), 0);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_EQ(empty->getHeight(a), 1);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->getHeight(a), 6);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->getHeight(A), 0);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->getHeight(b), 2);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, GetDepthTest) {
	EXPECT_DEATH({ empty->getDepth(nullptr); }, "Invalid node");
    EXPECT_DEATH({ empty->getDepth(a); }, "Node not found");
    empty->add(a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_EQ(empty->getDepth(a), 0);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_EQ(empty->getDepth(a), 0);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->getDepth(a), 0);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->getDepth(A), 6);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->getDepth(b), 1);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, IsRootTest) {
	EXPECT_DEATH({ empty->isRoot(nullptr); }, "Invalid node");
    EXPECT_DEATH({ empty->isRoot(a); }, "Node not found");
    empty->add(a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_TRUE(empty->isRoot(a));
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_TRUE(empty->isRoot(a));
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_TRUE(example->isRoot(a));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_FALSE(example->isRoot(A));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_FALSE(example->isRoot(b));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, IsLeafTest) {
	EXPECT_DEATH({ empty->isLeaf(nullptr); }, "Invalid node");
    EXPECT_DEATH({ empty->isLeaf(a); }, "Node not found");
    empty->add(a);
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_TRUE(empty->isLeaf(a));
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_FALSE(empty->isLeaf(a));
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_FALSE(example->isLeaf(a));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_TRUE(example->isLeaf(A));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_FALSE(example->isLeaf(b));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, IsHeavyTest) {
	EXPECT_DEATH({ empty->isHeavy(nullptr, 5); }, "Invalid node");
    EXPECT_DEATH({ empty->isHeavy(a, 5); }, "Node not found");
    empty->add(a);
    EXPECT_DEATH({ empty->isHeavy(a, 0); }, "Size cannot be 0");
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_FALSE(empty->isHeavy(a, 5));
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_TRUE(empty->isHeavy(a, 1));
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_FALSE(empty->isHeavy(a, 5));
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_TRUE(empty->isHeavy(a, 2));
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_TRUE(empty->isHeavy(a, 1));
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_TRUE(example->isHeavy(a, 5));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_TRUE(example->isHeavy(b, 5));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_TRUE(example->isHeavy(q, 5));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_FALSE(example->isHeavy(g, 5));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_FALSE(example->isHeavy(j, 5));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_FALSE(example->isHeavy(A, 5));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, GetHeavyDirectDescendantsTest) {
	EXPECT_DEATH({ empty->getHeavyDirectDescendants(nullptr, 5); }, "Invalid node");
    EXPECT_DEATH({ empty->getHeavyDirectDescendants(a, 5); }, "Node not found");
    empty->add(a);
    EXPECT_DEATH({ empty->getHeavyDirectDescendants(a, 0); }, "Size cannot be 0");
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_THAT(empty->getHeavyDirectDescendants(a, 5), ::testing::UnorderedElementsAre());
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    EXPECT_THAT(empty->getHeavyDirectDescendants(a, 1), ::testing::UnorderedElementsAre());
    EXPECT_STREQ(empty->toString().c_str(), "a{}");
    empty->add(b, a);
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_THAT(empty->getHeavyDirectDescendants(a, 5), ::testing::UnorderedElementsAre());
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_THAT(empty->getHeavyDirectDescendants(a, 1), ::testing::UnorderedElementsAre(b));
    EXPECT_STREQ(empty->toString().c_str(), "a{b{}}");
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_THAT(example->getHeavyDirectDescendants(a, 5), ::testing::UnorderedElementsAre(b,d));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_THAT(example->getHeavyDirectDescendants(a, 1), ::testing::UnorderedElementsAre(b,c,d,e,f));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_THAT(example->getHeavyDirectDescendants(a, 100), ::testing::UnorderedElementsAre());
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_THAT(example->getHeavyDirectDescendants(o, 5), ::testing::UnorderedElementsAre(q,r));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_THAT(example->getHeavyDirectDescendants(o, 1), ::testing::UnorderedElementsAre(q,r));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_THAT(example->getHeavyDirectDescendants(o, 100), ::testing::UnorderedElementsAre());
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, ToStringTest) {
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, ToNewickStringTest) {
    EXPECT_STREQ(example->toNewickString().c_str(), "(((l)g,h,(m)i,j)b,c,((n,((s,(A)t,u,v,w)q,((B)x,y,z)r)o,p)k)d,e,f)a;");
}

TEST_F(UnorderedTreeTest, ToBalancedParenthesisTest) {
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_THAT(example->toBalancedParenthesis(), ::testing::UnorderedElementsAre(true, true, true, true, false, false, true, false, true, true, false, false, true, false, false, true, false, true, true, true, false, true, true, true, false, true, true, false, false, true, false, true, false, true, false, false, true, true, true, false, false, true, false, true, false, false, false, true, false, false, false, true, false, true, false, false));
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}

TEST_F(UnorderedTreeTest, EnumerateTest) {
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->enumerate(f), 5);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->enumerate(a), 0);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->enumerate(r), 17);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
    EXPECT_EQ(example->enumerate(w), 22);
    EXPECT_STREQ(example->toString().c_str(), "a{b{g{l{}},h{},i{m{}},j{}},c{},d{k{n{},o{q{s{},t{A{}},u{},v{},w{}},r{x{B{}},y{},z{}}},p{}}},e{},f{}}");
}