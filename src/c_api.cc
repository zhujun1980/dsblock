/*
 * File:   c_api.cc
 * Author: zhujun
 */
#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include <algorithm>

#include "c_api.h"
#include "bloom_filter.h"
#include "fc.h"

#include "tst.h"

#ifdef __cplusplus
extern "C" {
#endif

int BloomFilterIsPresent(const char* bin_file, size_t bin_file_len,
    const char *key, size_t key_len)
{
    int ret;
    std::string bin(bin_file, bin_file_len);
    std::string k(key, key_len);
    const dsblock::BloomFilter* filter = nullptr;
    dsblock::FCache<dsblock::BloomFilter> caches;

    ret = caches.GetMappingObject(bin, &filter);
    if(ret != 0 || !filter) {
        return 0;
    }
    return filter->IsPresent(k) ? 1 : 0;
}

int BloomFilterCreateBinaryFromText(const char* txt_file, size_t txt_file_len,
    const char *bin_file, size_t bin_file_len,
    int num_elements, unsigned int data_vers,
    int hash_count, int bits_per_element)
{
    int cnt = 0;
    std::string txt(txt_file, txt_file_len);
    std::string bin(bin_file, bin_file_len);
    dsblock::BloomFilter filter(hash_count, num_elements, bits_per_element, data_vers);

    std::ifstream fs(txt);
    if(!fs.is_open()) {
        return 0;
    }

    std::string line;
    while (getline(fs, line)) {
        filter.Add(line);
        cnt++;
    }
    if(!filter.Save(bin)) {
        return 0;
    }
    return cnt;
}

int BloomFilterGetStats(const char *bin_file, size_t bin_file_len,
    int* bit_size, int* byte_size, int* hashs_cnt, unsigned int* data_vers)
{
    std::string bin(bin_file, bin_file_len);
    std::unique_ptr<dsblock::BloomFilter> filter(dsblock::BloomFilter::Load(bin));
    if(!filter.get()) {
        return -1;
    }

    dsblock::BloomFilter::Stats stat = filter->Stat();

    *bit_size = stat.bit_size;
    *byte_size = stat.byte_size;
    *data_vers = stat.data_vers;
    *hashs_cnt = stat.hashs_cnt;

    return 0;
}

int TstCreateBinaryFromText(const char* txt_file, size_t txt_file_len,
    const char *bin_file, size_t bin_file_len)
{
    std::string txt(txt_file, txt_file_len);
    std::string bin(bin_file, bin_file_len);
    std::unique_ptr<dsblock::TriSearchTries> tries(dsblock::TriSearchTries::Build(txt));
    return tries->Save(bin);
}

int TstIsPresent(const char* bin_file, size_t bin_file_len,
    const char *key, size_t key_len)
{
    int ret;
    std::string bin(bin_file, bin_file_len);
    std::string k(key, key_len);
    const dsblock::TriSearchTries* tst = nullptr;
    dsblock::FCache<dsblock::TriSearchTries> caches;

    ret = caches.GetMappingObject(bin, &tst);
    if(ret != 0 || !tst) {
        return 0;
    }
    return tst->IsPresent(k) ? 1 : 0;
}

int TstPrefixSearch(const char* bin_file, size_t bin_file_len,
    const char* pf, size_t pf_len, int with_prefix,
    char* out_buffer, size_t* len, int* total)
{
    int ret;
    std::string bin(bin_file, bin_file_len);
    std::string prefix(pf, pf_len);
    const dsblock::TriSearchTries* tst = nullptr;
    dsblock::FCache<dsblock::TriSearchTries> caches;
    std::vector<std::string> words;
    bool wp = (with_prefix != 0) ? true : false;

    int cnt = 0;
    size_t wrote = 0L;
    char *curr = out_buffer;

    ret = caches.GetMappingObject(bin, &tst);
    if(ret != 0 || !tst) {
        return 0;
    }

    tst->PrefixSearch(prefix, words, wp);

    for(auto& word : words) {
        if((wrote + word.size() + 1) > *len) {
            continue;
        }
        std::copy(word.begin(), word.end(), curr);
        curr += word.size();
        *curr = '\0';
        ++curr;
        wrote += word.size() + 1;
        ++cnt;
    }

    *total = words.size();
    *len = wrote;
    return cnt;
}

int TstNearNeighborSearch(const char* bin_file, size_t bin_file_len,
    const char* word, size_t word_len, int distance,
    char* out_buffer, size_t* len, int* total)
{
    int ret;
    std::string bin(bin_file, bin_file_len);
    std::string skey(word, word_len);
    const dsblock::TriSearchTries* tst = nullptr;
    dsblock::FCache<dsblock::TriSearchTries> caches;
    std::vector<std::string> neighbors;

    int cnt = 0;
    size_t wrote = 0L;
    char *curr = out_buffer;

    ret = caches.GetMappingObject(bin, &tst);
    if(ret != 0 || !tst) {
        return 0;
    }

    tst->NearNeighborSearch(skey, distance, neighbors);

    for(auto& word : neighbors) {
        if((wrote + word.size() + 1) > *len) {
            continue;
        }
        std::copy(word.begin(), word.end(), curr);
        curr += word.size();
        *curr = '\0';
        ++curr;
        wrote += word.size() + 1;
        ++cnt;
    }

    *total = neighbors.size();
    *len = wrote;
    return cnt;
}

#ifdef __cplusplus
}
#endif
