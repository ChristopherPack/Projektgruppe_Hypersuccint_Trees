//
// Created by User on 29.04.2021.
//

#include <pht/bitvector_utils.h>
#include <regex>

using namespace pht;

uint32_t Bitvector_Utils::encodeNumber(Bitvector& bitvector, uint32_t num, NumberEncoding encoding) {
    return encodeNumber(std::inserter(bitvector, bitvector.begin()), num, encoding);
}

uint32_t Bitvector_Utils::encodeNumber(std::insert_iterator<Bitvector> iterator, uint32_t num, NumberEncoding encoding) {
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

uint32_t Bitvector_Utils::decodeNumber(const Bitvector& bitvector, NumberEncoding encoding) {
    auto iter = bitvector.cbegin();
    return decodeNumber(iter, bitvector.cend(), encoding);
}

uint32_t Bitvector_Utils::decodeNumber(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, NumberEncoding encoding) {
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

Bitvector Bitvector_Utils::getEntry(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, BitvectorEncoding encoding, IndexingInformation information) {
    assert(iterator <= end);
    switch (encoding) {
        case BitvectorEncoding::ELIAS_GAMMA:
            return getEntryAtEliasGamma(iterator, offset, end, information.multiplier);
        case BitvectorEncoding::VECTOR_INDEX:
            return getEntryAtVectorIndex(iterator, offset, end, information.indexStart, information.indexEnd);
        case BitvectorEncoding::STATIC:
            return getEntryAtStatic(iterator, offset, end, information.staticSize);
        default:
            assert(false);
            return {};
    }
}

uint32_t Bitvector_Utils::getEntryCount(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, BitvectorEncoding encoding, IndexingInformation information) {
    assert(iterator <= end);
    switch (encoding) {
        case BitvectorEncoding::ELIAS_GAMMA:
            return getEntryCountEliasGamma(iterator, end, information.multiplier);
        case BitvectorEncoding::VECTOR_INDEX:
            return getEntryCountVectorIndex(iterator, end, information.indexStart, information.indexEnd);
        case BitvectorEncoding::STATIC:
            return getEntryCountStatic(iterator, end, information.staticSize);
        default:
            assert(false);
            return 0;
    }
}

std::vector<std::pair<Bitvector::iterator,Bitvector::iterator>> Bitvector_Utils::findMatches(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, const std::string& patternString) {
    //TODO
    std::vector<std::pair<Bitvector::iterator,Bitvector::iterator>> res;
    Bitvector pattern = convertToBitvector(patternString);
    assert(iterator+pattern.size() < end);
    auto iterStart = iterator;
    auto iterEnd = iterator+pattern.size();
    for(;iterEnd >= end; iterStart++, iterEnd++) {

    }
    //std::regex_match()
    return res;
}

Bitvector Bitvector_Utils::convertToBitvector(const std::string& input) {
    Bitvector result;
    for(char i : input){
        result.push_back(i=='1');
    }
    return result;
}

uint32_t Bitvector_Utils::decodeEliasGamma(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end) {
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

uint32_t Bitvector_Utils::decodeBinary(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end) {
    uint32_t res = 0;
    for(; iterator != end; iterator++) {
        res <<= 1;
        res = res|(*iterator?1:0);
    }
    return res;
}

uint32_t Bitvector_Utils::encodeBinary(std::insert_iterator<Bitvector>& iterator, uint32_t num) {
    uint32_t i = 0;
    if (num == 0) {
        iterator = false;
        return 1;
    }
    int size = floor(log2(num)) + 1;
    for(; i < size; i++) {
        iterator = ((num>>(size-1-i))&1);
    }
    return i;
}

uint32_t Bitvector_Utils::encodeEliasGamma(std::insert_iterator<Bitvector>& iterator, uint32_t num) {
    //assert(num != 0); TODO
    uint32_t count = 0;
    int32_t logSize = floor((log2(num)));
    for(int i = 0; i < logSize; i++, count++) {
        iterator = false;
    }
    for(int i = 0; i < logSize+1; i++, count++) {
        iterator = (num>>(logSize-i))%2==1;
    }
    return count;
}

Bitvector Bitvector_Utils::getEntryAtEliasGamma(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, uint32_t multiplier) {
    for(uint32_t i = 0; i < offset; i++) {
        uint32_t length = decodeEliasGamma(iterator, end)*multiplier;
        iterator += length; //Skip entry
        if(iterator+length > end) {
            throw std::runtime_error("Invalid Offset!");
        }
    }
    uint32_t length = decodeEliasGamma(iterator, end)*multiplier;
    if(iterator+length > end) {
        throw std::runtime_error("Invalid Offset!");
    }
    auto temp = iterator;
    iterator += length;
    return Bitvector(temp, iterator);
}

Bitvector Bitvector_Utils::getEntryAtVectorIndex(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, Bitvector::const_iterator& indexStart, const Bitvector::const_iterator& indexEnd) {
    for(int i = 0; i < offset; i++) {
        Bitvector fid = getEntryAtEliasGamma(indexStart, 0, indexEnd, 1);
        uint32_t indexLength = countOccurences(fid.cbegin(), fid.cend());
        iterator += indexLength; //skip Entry
        if(iterator+indexLength > end) {
            throw std::runtime_error("Invalid Offset!");
        }
    }
    Bitvector fid = getEntryAtEliasGamma(indexStart, 0, indexEnd, 1);
    uint32_t indexLength = countOccurences(fid.cbegin(), fid.cend());
    if(iterator+indexLength > end) {
        throw std::runtime_error("Invalid Offset!");
    }
    auto temp = iterator;
    iterator += indexLength;
    return Bitvector(temp, iterator);
}

uint32_t Bitvector_Utils::countOccurences(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, bool countZeros) {
    int occurences = 0;
    for(Bitvector::const_iterator indexIterator = iterator; indexIterator < end; indexIterator++) {
        if(*indexIterator != countZeros) {
            occurences++;
        }
    }
    return occurences;
}

Bitvector Bitvector_Utils::getEntryAtStatic(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, uint32_t size) {
    if(iterator+size*offset > end) {
        throw std::runtime_error("Invalid Offset!");
    }
    iterator += size*offset;
    auto temp = iterator;
    iterator += size;
    return Bitvector(temp, iterator);
}

uint32_t Bitvector_Utils::getEntryCountEliasGamma(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, uint32_t multiplier) {
    uint32_t count = 0;
    auto it = iterator;
    while(it != end) {
        uint32_t length = decodeEliasGamma(it, end)*multiplier;
        it += length;
        count++;
    }
    return count;
}

uint32_t Bitvector_Utils::getEntryCountVectorIndex(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, const Bitvector::const_iterator& indexStart, const Bitvector::const_iterator& indexEnd) {
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

uint32_t Bitvector_Utils::getEntryCountStatic(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, uint32_t size) {
    assert((end-iterator)%size == 0);
    return (end-iterator)/size;
}

Bitvector::const_iterator Bitvector_Utils::nullIterator = Bitvector::const_iterator();