#ifndef PROJEKTSUCCINCTTREES_BITVECTOR_H
#define PROJEKTSUCCINCTTREES_BITVECTOR_H

#include <iostream>

namespace pht {
    /**
     * This Class implements a simple Bitvector, with optimised space usage (1 bit per bit)
     * todo: This class is currently outdated since std::vector<bool> is already space-optimal
     */
    class BitvectorOld {
    private:
        uint8_t* data;
        size_t size;
    public:
        BitvectorOld(const size_t size = 8) : size(size) {
            size_t byteSize = ceil(size/8.0f);
            data = new uint8_t[byteSize];
            memset(data, 0 ,byteSize);
        }

        void resize(const size_t size) {
            size_t byteSize = ceil(size/8.0f);
            uint8_t* newData = new uint8_t[byteSize];
            memset(newData, 0 ,byteSize);
            memcpy(newData, data, byteSize);
            delete[] data;
            data = newData;
            this->size = size;
        }

        void inline set(const size_t idx) { 
            data[(size_t)floor(idx/8.0f)] |= 0x1<<(7-idx%8);
        }

        void inline toggle(const size_t idx) { 
            data[(size_t)floor(idx/8.0f)] ^= 0x1<<(7-idx%8);
        }

        void inline reset(const size_t idx) { 
            data[(size_t)floor(idx/8.0f)] &= 0x0<<(7-idx%8);
        }

        const bool inline at(const size_t idx) const { 
            return (data[(size_t)floor(idx/8.0f)]>>(7-idx%8))&0x1;
        }

        size_t getSize() const {
            return size;
        }

        ~BitvectorOld() {
            delete[] data;
        }
    };
}

#endif//PROJEKTSUCCINCTTREES_BITVECTOR_H