#ifndef PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY_H
#define PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY_H

#include <iostream>

namespace pht {
    class Bitvector {
    public://private:
        uint8_t* data;
        size_t size;
    public:
        Bitvector(const size_t size = 8) : size(size) {
            size_t byteSize = ceil(size/8.0f);
            data = new uint8_t[byteSize];
            memset(data, 0x0, byteSize);
        }

        void resize(const size_t size) {
            size_t byteSize = ceil(size/8.0f);
            uint8_t* newData = new uint8_t[byteSize];
            memset(newData, 0x0, byteSize);
            memcpy(newData, data, byteSize);
            delete[] data;
            data = newData;
            this->size = size;
        }

        void inline set(const size_t idx) { 
            data[(size_t)floor(idx/8.0f)] |= 0x1<<(idx%8);
        }

        void inline toggle(const size_t idx) { 
            data[(size_t)floor(idx/8.0f)] ^= 0x1<<(idx%8);
        }

        void inline reset(const size_t idx) { 
            data[(size_t)floor(idx/8.0f)] &= 0x0<<(idx%8);
        }

        bool inline at(const size_t idx) const { 
            return (data[(size_t)floor(idx/8.0f)]>>(idx%8))&0x1;
        }

        ~Bitvector() {
            delete[] data;
        }
    };
}

#endif//PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY_H