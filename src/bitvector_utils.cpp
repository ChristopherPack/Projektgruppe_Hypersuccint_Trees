//
// Created by User on 29.04.2021.
//

#include <pht/bitvector_utils.h>

using namespace pht;

bool BitvectorUtils::HuffmanComparator::operator()(const Bitvector &a, const Bitvector &b) const {
    return ((a.size() != b.size()) ? a.size() < b.size() : a < b);
}

uint32_t BitvectorUtils::encodeNumber(Bitvector& bitvector, uint32_t num, NumberEncoding encoding) {
    return encodeNumber(std::inserter(bitvector, bitvector.end()), num, encoding);
}

uint32_t BitvectorUtils::encodeNumber(std::insert_iterator<Bitvector> iterator, uint32_t num, NumberEncoding encoding) {
    switch (encoding) {
        case NumberEncoding::BINARY:
            return encodeBinary(iterator, num);
        case NumberEncoding::ELIAS_GAMMA:
            return encodeEliasGamma(iterator, num);
        default:
            assert(false);
            return 0;
    }
}

Bitvector BitvectorUtils::encodeNumberReturn(uint32_t num) {
    Bitvector res;
    uint32_t i = 0;
    if (num == 0) {
        res.push_back(false);
        return res;
    }
    uint32_t size = static_cast<uint32_t>(floor(log2(num))) + 1;
    for(; i < size; i++) {
        res.push_back((num>>(size-1-i))&1);
    }
    return res;
}

uint32_t BitvectorUtils::decodeNumber(const Bitvector& bitvector, NumberEncoding encoding) {
    auto iter = bitvector.cbegin();
    return decodeNumber(iter, bitvector.cend(), encoding);
}

uint32_t BitvectorUtils::decodeNumber(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, NumberEncoding encoding) {
    assert(iterator <= end);
    switch (encoding) {
        case NumberEncoding::BINARY:
            return decodeBinary(iterator, end);
        case NumberEncoding::ELIAS_GAMMA:
            return decodeEliasGamma(iterator, end);
        default:
            assert(false);
            return 0;
    }
}

Bitvector BitvectorUtils::getEntry(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, BitvectorEncoding encoding, IndexingInformation information) {
    assert(iterator <= end);
    switch (encoding) {
        case BitvectorEncoding::ELIAS_GAMMA:
            return getEntryAtEliasGamma(iterator, offset, end, information.multiplier);
        case BitvectorEncoding::VECTOR_INDEX:
            return getEntryAtVectorIndex(iterator, offset, end, information.indexStart, information.indexEnd);
        case BitvectorEncoding::STATIC:
            return getEntryAtStatic(iterator, offset, end, information.staticSize);
        case BitvectorEncoding::HUFFMAN:
            return getEntryAtHuffman(iterator, offset, end, information.huffmanTable);
        case BitvectorEncoding::PURE_ELIAS_GAMMA:
            return getEntryAtPureEliasGamma(iterator, offset, end);
        case BitvectorEncoding::STATIC_MATRIX_COLUMN:
            return getEntryAtStaticMatrixColumn(iterator, offset, end, information.staticSize);
        default:
            assert(false);
            return {};
    }
}

uint32_t BitvectorUtils::getEntryCount(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, BitvectorEncoding encoding, IndexingInformation information) {
    assert(iterator <= end);
    switch (encoding) {
        case BitvectorEncoding::ELIAS_GAMMA:
            return getEntryCountEliasGamma(iterator, end, information.multiplier);
        case BitvectorEncoding::VECTOR_INDEX:
            return getEntryCountVectorIndex(iterator, end, information.indexStart, information.indexEnd);
        case BitvectorEncoding::STATIC:
            return getEntryCountStatic(iterator, end, information.staticSize);
        case BitvectorEncoding::HUFFMAN:
            return getEntryCountHuffman(iterator, end, information.huffmanTable);
        default:
            assert(false);
            return 0;
    }
}

std::vector<std::pair<Bitvector::const_iterator,Bitvector::const_iterator>> BitvectorUtils::findMatches(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, const std::string& patternString) {
    //TODO std::regex_match()???
    std::vector<std::pair<Bitvector::const_iterator,Bitvector::const_iterator>> res;
    Bitvector pattern = convertToBitvector(patternString);
    //TODO: Size check does NOT work if vector is too small ???
    if(static_cast<size_t>(std::distance(iterator, end)) < pattern.size()) {
        uint32_t patternID = decodeNumber(pattern, NumberEncoding::BINARY);
        uint32_t patternSize = static_cast<uint32_t>(pattern.size());
        auto iterStart = iterator;
        auto iterEnd = iterator + patternSize;
        for (; iterEnd >= end; iterStart++, iterEnd++) {
            uint32_t currentPattern = decodeNumber(iterStart, iterEnd, NumberEncoding::BINARY);
            if (patternID == currentPattern) {
                res.emplace_back(iterStart, iterEnd);
                iterStart += patternSize;
                iterEnd += patternSize;
            }
        }
    }

    return res;
}

bool BitvectorUtils::findBeginningMatch(const Bitvector::const_iterator &iterator, const Bitvector::const_iterator& end, const Bitvector& patternBitvector) {
    auto iter = iterator;
    if(static_cast<size_t>(std::distance(iter, end)) < patternBitvector.size()) {
        return false;
    }
    for(bool i : patternBitvector) {
        if(*iter != i) {
            return false;
        }
        iter++;
    }
    return true;
}

Bitvector BitvectorUtils::convertToBitvector(const std::string& input) {
    Bitvector result;
    for(char i : input){
        result.push_back(i=='1');
    }
    return result;
}

uint32_t BitvectorUtils::decodeEliasGamma(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end) {
    if(iterator == end) {
        return 0;
    } else if(*iterator) {
        iterator++;
        return 1;
    }

    uint32_t size = 0;
    for(; !*iterator && iterator <= end; iterator++, size++);
    if(iterator == end) {
        throw std::runtime_error("Invalid Elias Gamma Code");
    }
    size++;

    uint32_t res = 0;
    for(uint32_t i = 0; i < size; i++, iterator++) {
        res <<= 1;
        res = res|(*iterator?1:0);
        if(iterator == end) {
            if(i < size-1) {
                throw std::runtime_error("Invalid Elias Gamma Code");
            }
            break;
        }
    }
    return res;
}

uint32_t BitvectorUtils::decodeBinary(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end) {
    uint32_t res = 0;
    auto start = iterator;
    for(; iterator != end && (iterator-start < 32) ; iterator++) {
        res <<= 1;
        res = res|(*iterator?1:0);
    }
    return res;
}

uint32_t BitvectorUtils::encodeBinary(std::insert_iterator<Bitvector>& iterator, uint32_t num) {
    uint32_t i = 0;
    if (num == 0) {
        iterator = false;
        return 1;
    }
    uint32_t size = static_cast<uint32_t>(floor(log2(num))) + 1;
    for(; i < size; i++) {
        iterator = ((num>>(size-1-i))&1);
    }
    return i;
}

uint32_t BitvectorUtils::encodeEliasGamma(std::insert_iterator<Bitvector>& iterator, uint32_t num) {
    //assert(num != 0); //TODO
    uint32_t count = 0;
    int32_t logSize = static_cast<uint32_t>(floor((log2(num))));
    for(int i = 0; i < logSize; i++, count++) {
        iterator = false;
    }
    for(int i = 0; i < logSize+1; i++, count++) {
        iterator = (num>>(logSize-i))%2==1;
    }
    return count;
}

Bitvector BitvectorUtils::getEntryAtEliasGamma(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, uint32_t multiplier) {
    for(uint32_t i = 0; i < offset; i++) {
        uint32_t length = decodeEliasGamma(iterator, end)*multiplier;
        if(end-iterator < length) {
            throw std::runtime_error("Invalid Offset!");
        }
        iterator += length; //Skip entry
    }
    uint32_t length = decodeEliasGamma(iterator, end)*multiplier;
    if(end-iterator < length) {
        throw std::runtime_error("Invalid Offset!");
    }
    auto temp = iterator;
    iterator += length;
    return Bitvector(temp, iterator);
}

Bitvector BitvectorUtils::getEntryAtVectorIndex(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, Bitvector::const_iterator& indexStart, const Bitvector::const_iterator& indexEnd) {
    for(uint32_t i = 0; i < offset; i++) {
        Bitvector fid = getEntryAtEliasGamma(indexStart, 0, indexEnd, 1);
        uint32_t indexLength = countOccurences(fid.cbegin(), fid.cend());
        if(end-iterator < indexLength) {
            throw std::runtime_error("Invalid Offset!");
        }
        iterator += indexLength; //skip Entry
    }
    Bitvector fid = getEntryAtEliasGamma(indexStart, 0, indexEnd, 1);
    uint32_t indexLength = countOccurences(fid.cbegin(), fid.cend());
    if(end-iterator < indexLength) {
        throw std::runtime_error("Invalid Offset!");
    }
    auto temp = iterator;
    iterator += indexLength;
    return Bitvector(temp, iterator);
}

uint32_t BitvectorUtils::countOccurences(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, bool countZeros) {
    int occurences = 0;
    for(Bitvector::const_iterator indexIterator = iterator; indexIterator < end; indexIterator++) {
        if(*indexIterator != countZeros) {
            occurences++;
        }
    }
    return occurences;
}

Bitvector BitvectorUtils::getEntryAtStatic(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, uint32_t size) {
    if(end-iterator < size*offset) {
        throw std::runtime_error("Invalid Offset!");
    }
    iterator += size*offset;
    auto temp = iterator;
    iterator += size;
    return Bitvector(temp, iterator);
}

Bitvector BitvectorUtils::getEntryAtHuffman(Bitvector::const_iterator &iterator, uint32_t offset, const Bitvector::const_iterator &end, std::set<Bitvector, HuffmanComparator> huffmanCodes) {
    for(uint32_t i = 0; i < offset; i++) {
        for(Bitvector code : huffmanCodes) {
            if(findBeginningMatch(iterator,end, code)) {
                if(static_cast<size_t>(std::distance(iterator, end)) < code.size()) {
                    throw std::runtime_error("Invalid Offset!");
                }
                iterator+=code.size();
                break;
            }
        }
    }
    auto temp = iterator;
    for(Bitvector code : huffmanCodes) {
        if(findBeginningMatch(iterator,end, code)) {
            if(static_cast<size_t>(std::distance(iterator, end)) < code.size()) {
                throw std::runtime_error("Invalid Offset!");
            }
            iterator+=code.size();
            break;
        }
    }
    return Bitvector(temp, iterator);
}

Bitvector BitvectorUtils::getEntryAtPureEliasGamma(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end) {
    for(uint32_t i = 0; i < offset; i++) {
        decodeEliasGamma(iterator, end);
    }
    return readEliasGamma(iterator, end);
}

Bitvector BitvectorUtils::readEliasGamma(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end) {
    if(iterator == end) {
        return {false};
    } else if(*iterator) {
        iterator++;
        return {true};
    }

    uint32_t size = 0;
    for(; !*iterator && iterator <= end; iterator++, size++);
    if(iterator == end) {
        throw std::runtime_error("Invalid Elias Gamma Code");
    }
    size++;

    Bitvector res;
    for(uint32_t i = 0; i < size; i++, iterator++) {
        res.push_back(*iterator);
        if(iterator == end) {
            if(i < size-1) {
                throw std::runtime_error("Invalid Elias Gamma Code");
            }
            break;
        }
    }
    return res;
}

Bitvector BitvectorUtils::getEntryAtStaticMatrixColumn(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, uint32_t size) {
    if(iterator > end) {
        throw std::runtime_error("Invalid Offset!");
    }
    iterator += offset;
    Bitvector res;
    while(end - iterator >= size) {
        res.push_back(*iterator);
        iterator += size;
    }
    res.push_back(*iterator);
    return res;
}

uint32_t BitvectorUtils::getEntryCountEliasGamma(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, uint32_t multiplier) {
    uint32_t count = 0;
    auto it = iterator;
    while(it != end) {
        uint32_t length = decodeEliasGamma(it, end)*multiplier;
        it += length;
        count++;
    }
    return count;
}

uint32_t BitvectorUtils::getEntryCountVectorIndex(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, const Bitvector::const_iterator& indexStart, const Bitvector::const_iterator& indexEnd) {
    uint32_t count = 0;
    auto iter = iterator;
    auto it = indexStart;
    for(int i = 0; iter != end; i++) {
        Bitvector fid = getEntryAtEliasGamma(it, 0, indexEnd, 1);
        uint32_t indexLength = countOccurences(fid.cbegin(), fid.cend());
        iter += indexLength;
        count++;
    }
    return count;
}

uint32_t BitvectorUtils::getEntryCountStatic(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, uint32_t size) {
    assert((end-iterator)%size == 0);
    return static_cast<uint32_t>(std::distance(iterator, end))/size;
}

uint32_t BitvectorUtils::getEntryCountHuffman(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, std::set<Bitvector, HuffmanComparator> huffmanCodes) {
    uint32_t count = 0;
    auto it = iterator;
    while(it != end) {
        for(Bitvector code : huffmanCodes) {
            if(findBeginningMatch(it,end, code)) {
                if(static_cast<size_t>(std::distance(it, end)) < code.size()) {
                    throw std::runtime_error("Invalid Code!");
                }
                it+=code.size();
                break;
            }
        }
        count++;
    }
    return count;
}