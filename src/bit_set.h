//
//  bit_set.h
//  BloomFilter
//
//  Created by zhu jun.
//

#ifndef __BloomFilter__bit_set__
#define __BloomFilter__bit_set__

#include <iostream>
#include <ostream>
#include <vector>
#include <memory>
#include "utils.h"
#include "consts.h"

namespace dsblock {

class BitSet {
public:
    BitSet(long _bit_size, long _byte_size, char* _bytes, DataLoc _data_loc);
    BitSet(long numBits);
    virtual ~BitSet();

    bool Set(long index);
    bool Get(long index);
    bool Clear(long index);
    void Clear();
    long Capacity();

    bool Save(int fd);

    static BitSet* Load(void* addr, size_t size);
    static BitSet* Load(int fd);

    friend class BloomFilter;

    long BitSize() const {
        return bit_size;
    }

    long ByteSize() const {
        return byte_size;
    }

private:
    DISABLE_ASSIGN_AND_COPY(BitSet);

    long bit_size;
    long byte_size;
    char* bytes;
    DataLoc data_loc;
};

}  // namespace dsblock

#endif /* defined(__BloomFilter__bit_set__) */
