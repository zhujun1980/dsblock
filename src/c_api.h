/*
 * File:   c_api.h
 * Author: zhujun
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int BloomFilterIsPresent(const char* bin_file, size_t bin_file_len,
                         const char* key, size_t key_len);

int BloomFilterCreateBinaryFromText(const char* txt_file, size_t txt_file_len,
                                    const char* bin_file, size_t bin_file_len,
                                    int num_elements, unsigned int data_vers,
                                    int hash_count, int bits_per_element);

int BloomFilterGetStats(const char* bin_file, size_t bin_file_len,
                        int* bit_size, int* byte_size, int* hashs_cnt, unsigned int* data_vers);

int TstCreateBinaryFromText(const char* txt_file, size_t txt_file_len,
                            const char* bin_file, size_t bin_file_len);

int TstIsPresent(const char* bin_file, size_t bin_file_len,
                 const char* key, size_t key_len);

int TstPrefixSearch(const char* bin_file, size_t bin_file_len,
                    const char* prefix, size_t prefix_len, int with_prefix,
                    char* out_buffer, size_t* len, int* total);

int TstNearNeighborSearch(const char* bin_file, size_t bin_file_len,
                          const char* word, size_t word_len, int distance,
                          char* out_buffer, size_t* len, int* total);

#ifdef __cplusplus
}
#endif
