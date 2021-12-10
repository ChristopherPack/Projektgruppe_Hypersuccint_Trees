#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"

#define PHT_TEST

#include "pht/xml_reader.h"
#include "pht/hypersuccinct_tree.h"
#include "pht/hypersuccinct_tree_factory.h"
#include "pht/bitvector_utils.h"

TEST(BitvectorUtilsTest, stupidTest) {
    std::vector<bool> b = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0};
    std::vector<bool> b2 = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1};
    PHT_LOGGER_DEBUG("Test") << b.size() << "\n" << pht::Logger::endl();
    uint32_t c = pht::BitvectorUtils::decodeNumber(b,pht::BitvectorUtils::NumberEncoding::BINARY);
    uint32_t c2 = pht::BitvectorUtils::decodeNumber(b2,pht::BitvectorUtils::NumberEncoding::BINARY);
    PHT_LOGGER_DEBUG("Test") << c << "\n" << pht::Logger::endl();
    PHT_LOGGER_DEBUG("Test") << c2 << "\n" << pht::Logger::endl();
    PHT_LOGGER_DEBUG("Test") << UINT32_MAX << "\n" << pht::Logger::endl();
}

TEST(BitvectorUtilsTest, encodeBinaryTest) {
    std::vector<bool> b;
    pht::BitvectorUtils::encodeNumber(b, 2, pht::BitvectorUtils::NumberEncoding::BINARY);
    EXPECT_THAT(b, ::testing::ElementsAre(1,0));
    b.clear();
    pht::BitvectorUtils::encodeNumber(b, 5, pht::BitvectorUtils::NumberEncoding::BINARY);
    EXPECT_THAT(b, ::testing::ElementsAre(1,0,1));
    b.clear();
    pht::BitvectorUtils::encodeNumber(b, 3, pht::BitvectorUtils::NumberEncoding::BINARY);
    EXPECT_THAT(b, ::testing::ElementsAre(1,1));
    b.clear();
    pht::BitvectorUtils::encodeNumber(b, 0, pht::BitvectorUtils::NumberEncoding::BINARY);
    EXPECT_THAT(b, ::testing::ElementsAre(0));
}

TEST(BitvectorUtilsTest, decodeBinaryTest) {
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({1,0}, pht::BitvectorUtils::NumberEncoding::BINARY), 2);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({1,0,1}, pht::BitvectorUtils::NumberEncoding::BINARY), 5);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({1,1}, pht::BitvectorUtils::NumberEncoding::BINARY), 3);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({1}, pht::BitvectorUtils::NumberEncoding::BINARY), 1);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({1,0,0}, pht::BitvectorUtils::NumberEncoding::BINARY), 4);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({1,1,0}, pht::BitvectorUtils::NumberEncoding::BINARY), 6);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({1,0,0,1,1,0,1,1,1,1,0,1,1,1}, pht::BitvectorUtils::NumberEncoding::BINARY), 9975);
}

TEST(BitvectorUtilsTest, decodeEliasGammaTest) {
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({0,0,1,1,0}, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA), 6);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,1,1,1,1,0,1,1,1}, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA), 9975);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({1}, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA), 1);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({0,1,0}, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA), 2);
    EXPECT_EQ(pht::BitvectorUtils::decodeNumber({0,0,1,0,1}, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA), 5);
}

TEST(BitvectorUtilsTest, getEntryEliasGammaTest) {
    std::vector<bool> bitvector = {0,1,1, 1,1,0,1,0,0 ,0,1,0 ,1,1,0,0 ,0,0,1,0,0 ,1,1,1,0,1,0,0,0 ,1, 1,0 ,0,0,1,0,1 ,1,1,1,0,0,1,0,1,0,0, 0,1,0,1,1,0,0,0,0,1,1,0,1,1,0,1,1,0,0,1,0,1,0,0,0,1,0,1,1,0,0,0,0,1,1,0,1,1,1,0,0,1,0,1,1,0,0,0};
    auto iter = bitvector.cbegin();
    std::vector<bool> res = pht::BitvectorUtils::getEntry(iter, 0, bitvector.cend(), pht::BitvectorUtils::BitvectorEncoding::ELIAS_GAMMA, {pht::BitvectorUtils::nullIterator(), pht::BitvectorUtils::nullIterator(), 2, 0});
    EXPECT_THAT(res, ::testing::ElementsAre(1,1,0,1,0,0));
    res = pht::BitvectorUtils::getEntry(iter, 0, bitvector.cend(), pht::BitvectorUtils::BitvectorEncoding::ELIAS_GAMMA, {pht::BitvectorUtils::nullIterator(), pht::BitvectorUtils::nullIterator(), 2, 0});
    EXPECT_THAT(res, ::testing::ElementsAre(1,1,0,0));
    res = pht::BitvectorUtils::getEntry(iter, 0, bitvector.cend(), pht::BitvectorUtils::BitvectorEncoding::ELIAS_GAMMA, {pht::BitvectorUtils::nullIterator(), pht::BitvectorUtils::nullIterator(), 2, 0});
    EXPECT_THAT(res, ::testing::ElementsAre(1,1,1,0,1,0,0,0));
    res = pht::BitvectorUtils::getEntry(iter, 0, bitvector.cend(), pht::BitvectorUtils::BitvectorEncoding::ELIAS_GAMMA, {pht::BitvectorUtils::nullIterator(), pht::BitvectorUtils::nullIterator(), 2, 0});
    EXPECT_THAT(res, ::testing::ElementsAre(1,0));
    res = pht::BitvectorUtils::getEntry(iter, 0, bitvector.cend(), pht::BitvectorUtils::BitvectorEncoding::ELIAS_GAMMA, {pht::BitvectorUtils::nullIterator(), pht::BitvectorUtils::nullIterator(), 2, 0});
    EXPECT_THAT(res, ::testing::ElementsAre(1,1,1,0,0,1,0,1,0,0));

    std::vector<bool> bitvector2 = {0,0,1,0,1,1,0,1,1,1,1,1,0,1,0,1,1,0,1,1,1,0,0,0,0,1,0,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0};
    auto iter2 = bitvector2.cbegin();
    res = pht::BitvectorUtils::getEntry(iter2, 0, bitvector2.cend(), pht::BitvectorUtils::BitvectorEncoding::ELIAS_GAMMA, {pht::BitvectorUtils::nullIterator(), pht::BitvectorUtils::nullIterator(), 1, 0});
    EXPECT_THAT(res, ::testing::ElementsAre(1,0,1,1,1));
    res = pht::BitvectorUtils::getEntry(iter2, 0, bitvector2.cend(), pht::BitvectorUtils::BitvectorEncoding::ELIAS_GAMMA, {pht::BitvectorUtils::nullIterator(), pht::BitvectorUtils::nullIterator(), 1, 0});
    EXPECT_THAT(res, ::testing::ElementsAre(1));
    res = pht::BitvectorUtils::getEntry(iter2, 0, bitvector2.cend(), pht::BitvectorUtils::BitvectorEncoding::ELIAS_GAMMA, {pht::BitvectorUtils::nullIterator(), pht::BitvectorUtils::nullIterator(), 1, 0});
    EXPECT_THAT(res, ::testing::ElementsAre(1,1));
}

/*TEST(BitvectorUtilsTest, getEntryEliasGammaOffsetTest) {
    std::shared_ptr<pht::UnorderedTree<std::string>> xmlTree = pht::XMLReader::readByName("treeAlex.xml");
    std::unique_ptr<pht::HypersuccinctTree> hst = pht::HypersuccinctTreeFactory::create(xmlTree);

    pht::MiniTree miniTree1 = hst->getMiniTree(1);
    auto iter = miniTree1.microTrees.cbegin();
    EXPECT_THAT(pht::Bitvector_Utils::getEntry(iter, 0, miniTree1.microTrees.cend(), pht::Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {pht::Bitvector_Utils::nullIterator(), pht::Bitvector_Utils::nullIterator(), 2, 0}), testing::ElementsAre(1,1,1,1,0,0,0,0));
    iter = miniTree1.microTrees.cbegin();
    EXPECT_THAT(pht::Bitvector_Utils::getEntry(iter, 1, miniTree1.microTrees.cend(), pht::Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {pht::Bitvector_Utils::nullIterator(), pht::Bitvector_Utils::nullIterator(), 2, 0}), testing::ElementsAre(1,1,0,1,1,0,0,1,1,1,0,0,0,0));
    iter = miniTree1.microTrees.cbegin();
    EXPECT_THAT(pht::Bitvector_Utils::getEntry(iter, 2, miniTree1.microTrees.cend(), pht::Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {pht::Bitvector_Utils::nullIterator(), pht::Bitvector_Utils::nullIterator(), 2, 0}), testing::ElementsAre(1,1,0,1,0,1,0,0));
}*/

TEST(BitvectorUtilsTest, getEntryVectorIndexTest) {
    std::vector<bool> fids = {0,0,1,0,1,1,0,1,1,1,1,1,0,1,0,1,1,0,1,1,1,0,0,0,0,1,0,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0};
    std::vector<bool> bitvector = {1,0,1,0,1,0,0,1,1,1,1,1};
    auto iter2 = bitvector.cbegin();
    auto iterFIDs = fids.cbegin();
    std::vector<bool> res = pht::BitvectorUtils::getEntry(iter2, 0, bitvector.cend(), pht::BitvectorUtils::BitvectorEncoding::VECTOR_INDEX, {iterFIDs, fids.cend()});
    EXPECT_THAT(res, ::testing::ElementsAre(1,0,1,0));
    res = pht::BitvectorUtils::getEntry(iter2, 0, bitvector.cend(), pht::BitvectorUtils::BitvectorEncoding::VECTOR_INDEX, {iterFIDs, fids.cend()});
    EXPECT_THAT(res, ::testing::ElementsAre(1));
    res = pht::BitvectorUtils::getEntry(iter2, 0, bitvector.cend(), pht::BitvectorUtils::BitvectorEncoding::VECTOR_INDEX, {iterFIDs, fids.cend()});
    EXPECT_THAT(res, ::testing::ElementsAre(0,0));

}

TEST(BitvectorUtilsTest, getEntryStaticTest) {
    std::vector<bool> bitvector = {0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    auto iter = bitvector.cbegin();
    std::vector<bool> res = pht::BitvectorUtils::getEntry(iter, 1, bitvector.cend(), pht::BitvectorUtils::BitvectorEncoding::STATIC, {pht::BitvectorUtils::nullIterator(), pht::BitvectorUtils::nullIterator(), 0, 4});
    EXPECT_THAT(res, ::testing::ElementsAre(0,0,0,0));
    res = pht::BitvectorUtils::getEntry(iter, 0, bitvector.cend(), pht::BitvectorUtils::BitvectorEncoding::STATIC, {pht::BitvectorUtils::nullIterator(), pht::BitvectorUtils::nullIterator(), 0, 4});
    EXPECT_THAT(res, ::testing::ElementsAre(0,0,1,1));
}

TEST(BitvectorUtilsTest, encodeEliasGammaTest) {
    std::vector<bool> bitvector;
    pht::BitvectorUtils::encodeNumber(bitvector, 6, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
    EXPECT_THAT(bitvector, ::testing::ElementsAre(0,0,1,1,0));
    bitvector.clear();
    pht::BitvectorUtils::encodeNumber(bitvector, 1, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
    EXPECT_THAT(bitvector, ::testing::ElementsAre(1));
    bitvector.clear();
    pht::BitvectorUtils::encodeNumber(bitvector, 5, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
    EXPECT_THAT(bitvector, ::testing::ElementsAre(0,0,1,0,1));
    bitvector.clear();
    pht::BitvectorUtils::encodeNumber(bitvector, 2, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
    EXPECT_THAT(bitvector, ::testing::ElementsAre(0,1,0));
    bitvector.clear();
    pht::BitvectorUtils::encodeNumber(bitvector, 9975, pht::BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
    EXPECT_THAT(bitvector, ::testing::ElementsAre(0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,1,1,1,1,0,1,1,1));
}

TEST(BitvectorUtilsTest, convertToBitvectorTest){
    pht::Bitvector bitvector = {0,1,0,1,1,0};
    std::string string = "010110";
    EXPECT_EQ(bitvector, pht::BitvectorUtils::convertToBitvector(string));
}

TEST(BitvectorUtilsTest, findMatchesTest) {
    //TODO
    pht::Bitvector bitvector = {0,0,1,0,1,0,0,1,0,1,1,0,1,0,1,1,1,0,0,1,0,1,0,1,0,1,0,0};
    std::string pattern = "101";
    //matches should be at: 2,7,10,19,23
    std::vector<std::pair<pht::Bitvector::const_iterator,pht::Bitvector::const_iterator>> res = pht::BitvectorUtils::findMatches(bitvector.cbegin(),bitvector.cend(),pattern);
}

TEST(BitvectorUtilsTest, getEntyAtHuffmanTest) {
    pht::Bitvector bitvector = {0,1,1,0,0,0,0,1,0,1,1,1,0,0,1,1,1};
    std::set<pht::Bitvector, pht::BitvectorUtils::HuffmanComparator> huffmanCodes;
    huffmanCodes.insert({0,1});
    huffmanCodes.insert({0,0,1,0,1});
    huffmanCodes.insert({0,0,0,1});
    huffmanCodes.insert({0,0,0,0});
    huffmanCodes.insert({1,0,1,0});
    huffmanCodes.insert({1,0,0});
    huffmanCodes.insert({0,0,1,0,0});
    huffmanCodes.insert({1,1});
    huffmanCodes.insert({0,0,1,1,0});
    huffmanCodes.insert({0,0,1,1,1});
    huffmanCodes.insert({1,0,1,1});
    auto iter = bitvector.cbegin();
    std::vector<bool> res = pht::BitvectorUtils::getEntry(iter, 0, bitvector.cend(), pht::BitvectorUtils::BitvectorEncoding::HUFFMAN, {pht::BitvectorUtils::nullIterator(), pht::BitvectorUtils::nullIterator(), 0, 4, huffmanCodes});
    EXPECT_THAT(res, ::testing::ElementsAre(0,1));
    res = pht::BitvectorUtils::getEntry(iter, 0, bitvector.cend(), pht::BitvectorUtils::BitvectorEncoding::HUFFMAN, {pht::BitvectorUtils::nullIterator(), pht::BitvectorUtils::nullIterator(), 0, 4, huffmanCodes});
    EXPECT_THAT(res, ::testing::ElementsAre(1,0,0));
    res = pht::BitvectorUtils::getEntry(iter, 0, bitvector.cend(), pht::BitvectorUtils::BitvectorEncoding::HUFFMAN, {pht::BitvectorUtils::nullIterator(), pht::BitvectorUtils::nullIterator(), 0, 4, huffmanCodes});
    EXPECT_THAT(res, ::testing::ElementsAre(0,0,1,0,1));
    res = pht::BitvectorUtils::getEntry(iter, 0, bitvector.cend(), pht::BitvectorUtils::BitvectorEncoding::HUFFMAN, {pht::BitvectorUtils::nullIterator(), pht::BitvectorUtils::nullIterator(), 0, 4, huffmanCodes});
    EXPECT_THAT(res, ::testing::ElementsAre(1,1));
    res = pht::BitvectorUtils::getEntry(iter, 0, bitvector.cend(), pht::BitvectorUtils::BitvectorEncoding::HUFFMAN, {pht::BitvectorUtils::nullIterator(), pht::BitvectorUtils::nullIterator(), 0, 4, huffmanCodes});
    EXPECT_THAT(res, ::testing::ElementsAre(0,0,1,1,1));
    auto iter2 = bitvector.cbegin();
    res = pht::BitvectorUtils::getEntry(iter2, 3, bitvector.cend(), pht::BitvectorUtils::BitvectorEncoding::HUFFMAN, {pht::BitvectorUtils::nullIterator(), pht::BitvectorUtils::nullIterator(), 0, 4, huffmanCodes});
    EXPECT_THAT(res, ::testing::ElementsAre(1,1));
}

TEST(BitvectorUtilsTest, getEntryCountTest) {
    pht::Bitvector bitvectorEG = {1,1,0,0,1,1,1,1,1,0,0,0,0,1,0,1,1,0,0,0,0,1,0,0,1,1,1,0,1,0,0,0,0,0,1,1,0,1,1,1,0,1,0,0,1,1,0,0,0};
    pht::Bitvector bitvectorTV = {0,0,1,0,1,1,1};
    pht::Bitvector bitvectorStatic = {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0};
    pht::Bitvector bitvectorHuff = {0,1,1,0,0,0,0,1,0,1,1,1,0,0,1,1,1};
    uint32_t mult = 2;
    pht::Bitvector bitvectorTvIndex = {0,1,0,1,1,1,1,0,1,0,1,1,1,1,0,1,0,1,0};
    uint32_t staticSize = bitvectorStatic.size()/5;
    std::set<pht::Bitvector, pht::BitvectorUtils::HuffmanComparator> huffmanCodes;
    huffmanCodes.insert({0,1});
    huffmanCodes.insert({0,0,1,0,1});
    huffmanCodes.insert({0,0,0,1});
    huffmanCodes.insert({0,0,0,0});
    huffmanCodes.insert({1,0,1,0});
    huffmanCodes.insert({1,0,0});
    huffmanCodes.insert({0,0,1,0,0});
    huffmanCodes.insert({1,1});
    huffmanCodes.insert({0,0,1,1,0});
    huffmanCodes.insert({0,0,1,1,1});
    huffmanCodes.insert({1,0,1,1});
    uint32_t res = pht::BitvectorUtils::getEntryCount(bitvectorEG.cbegin(),bitvectorEG.cend(),pht::BitvectorUtils::BitvectorEncoding::ELIAS_GAMMA,{pht::BitvectorUtils::nullIterator(),pht::BitvectorUtils::nullIterator(),mult,0});
    EXPECT_EQ(res, 5);
    auto iter = bitvectorTvIndex.cbegin();
    res = pht::BitvectorUtils::getEntryCount(bitvectorTV.cbegin(),bitvectorTV.cend(),pht::BitvectorUtils::BitvectorEncoding::ELIAS_GAMMA,{iter,bitvectorTvIndex.cend()});
    EXPECT_EQ(res, 3);
    res = pht::BitvectorUtils::getEntryCount(bitvectorStatic.cbegin(),bitvectorStatic.cend(),pht::BitvectorUtils::BitvectorEncoding::STATIC,{pht::BitvectorUtils::nullIterator(),pht::BitvectorUtils::nullIterator(),0,staticSize});
    EXPECT_EQ(res, 5);
    res = pht::BitvectorUtils::getEntryCount(bitvectorHuff.cbegin(),bitvectorHuff.cend(),pht::BitvectorUtils::BitvectorEncoding::HUFFMAN,{pht::BitvectorUtils::nullIterator(),pht::BitvectorUtils::nullIterator(),0,0,huffmanCodes});
    EXPECT_EQ(res, 5);
}