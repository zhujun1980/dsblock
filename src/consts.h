/*
 * File:   consts.h
 * Author: zhujun
 */

#ifndef CONSTS_H
#define CONSTS_H

enum CACHE_DATA_FORMAT_TYPE {
    INVALID_TYPE = 0,
    BLOOM_FILTER,
    TRI_SEARCH_TRIES,
    UNKNOWN_TYPE
};

enum DataLoc {
    INVALID = 0,
    SHM_MEM,
    IN_HEAP,
};

#define SHM_IPC_KEYS "/tmp/fcache"

#define MAX_HASH_COUNT 256

#endif /* CONSTS_H */
