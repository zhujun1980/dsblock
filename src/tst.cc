/*
 * File:   tst.cc
 * Author: zhujun
 */
#include <fstream>
#include <vector>
#include <cassert>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <new>
#include <functional>
#include <algorithm>
#include "tst.h"
#include "utf8.h"
#include "file_utils.h"
#include "log.h"

namespace dsblock {

const char* TriSearchTries::EXT_NAME = "tst";

static int BuildFromMiddle(TriSearchTries* tries, const std::vector<std::string>& words, int start, int stop) {
    int total = 0;
    size_t mid = start + (int)ceil((stop - start) / 2.0) - 1;

    if (start < stop) {
        tries->Add(words[mid]);
        total = 1;
        total += BuildFromMiddle(tries, words, start, mid);
        total += BuildFromMiddle(tries, words, mid + 1, stop);
    }
    return total;
}

TriSearchTries::TriSearchTries(size_t count) {
    data_ = (TstNode*)malloc((count + 1) * sizeof(TstNode));
    if (!data_) {
        throw std::bad_alloc{};
    }
    memset(data_, '\0', (count + 1) * sizeof(TstNode));
    free_ = 0L;
    count_ = count;
    data_loc_ = IN_HEAP;
    data_[0].eqkid = data_[0].lokid = data_[0].hikid = count_;
}

TriSearchTries::TriSearchTries(TstNode* data, size_t frees, size_t count) : data_(data), free_(frees), count_(count), data_loc_(SHM_MEM) {
}

TriSearchTries::~TriSearchTries() {
    if (data_loc_ == IN_HEAP && data_)
        free(data_);
}

bool TriSearchTries::Add(const std::string& word) {
    u_int32_t value;
    int i = 0;
    size_t idx = 0;
    int* ref = nullptr;
    bool exists = true;

    while ((value = u8_nextchar(word.c_str(), &i)) != 0) {
        while (1) {
            if (idx == free_ || idx == count_) {
                if (free_ + 1 > count_) {
                    throw std::runtime_error("Max count exceeded!!!");
                }
                idx = free_++;
                data_[idx].end = false;
                data_[idx].value = value;
                data_[idx].eqkid = data_[idx].lokid = data_[idx].hikid = count_;
                if (ref)
                    *ref = idx;
                exists = false;
            }
            if (data_[idx].value == value) {
                if ((size_t)i == word.size())  // the last char
                    data_[idx].end = true;
                ref = &(data_[idx].eqkid);
                idx = data_[idx].eqkid;
                break;
            }
            else if (data_[idx].value > value) {
                ref = &(data_[idx].lokid);
                idx = data_[idx].lokid;
            }
            else {
                ref = &(data_[idx].hikid);
                idx = data_[idx].hikid;
            }
        }
    }
    return exists;
}

size_t TriSearchTries::GetNode(const std::string& word) const {
    u_int32_t value;
    int i = 0;
    size_t idx = 0L;

    value = u8_nextchar(word.c_str(), &i);
    while (idx < free_) {
        if (data_[idx].value == value) {
            value = u8_nextchar(word.c_str(), &i);
            if (value == 0) {
                return idx;
            }
            idx = data_[idx].eqkid;
        }
        else if (data_[idx].value > value) {
            idx = data_[idx].lokid;
        }
        else {
            idx = data_[idx].hikid;
        }
    }
    return count_;
}

bool TriSearchTries::IsPresent(const std::string& word) const {
    size_t idx = GetNode(word);
    if (idx != count_ && data_[idx].end)
        return true;
    return false;
}

static void PushCharToArray(TstNode* data,
                            std::vector<std::string>& words,
                            const std::vector<u_int32_t>& prefix, size_t idx) {
    if (data[idx].end) {
        int ret;
        char temp[4];
        std::string word = "";

        for (auto v : prefix) {
            ret = u8_wc_toutf8(temp, v);
            for (int i = 0; i < ret; ++i)
                word += temp[i];
        }
        ret = u8_wc_toutf8(temp, data[idx].value);
        for (int i = 0; i < ret; ++i)
            word += temp[i];
        words.push_back(word);
    }
}

void TriSearchTries::PrefixSearch(const std::string& prefix,
                                  std::vector<std::string>& words, bool with_prefix) const {
    using namespace std::placeholders;
    std::vector<u_int32_t> pf;

    if (with_prefix) {
        int i = 0;
        u_int32_t value;
        while ((value = u8_nextchar(prefix.c_str(), &i)) != 0) {
            pf.push_back(value);
        }
    }

    size_t idx = GetNode(prefix);
    if (idx == count_) {
        return;
    }
    Traverse(pf, data_[idx].eqkid, std::bind(&PushCharToArray, data_, std::ref(words), _1, _2));
    return;
}

void TriSearchTries::Traverse(std::vector<u_int32_t>& prefix, size_t idx, TstNodeVisitor vistor) const {
    if (idx == count_) {
        return;
    }
    Traverse(prefix, data_[idx].lokid, vistor);
    vistor(prefix, idx);
    prefix.push_back(data_[idx].value);
    Traverse(prefix, data_[idx].eqkid, vistor);
    prefix.pop_back();
    Traverse(prefix, data_[idx].hikid, vistor);
}

void TriSearchTries::NearNeighborSearch(const std::string& word,
                                        int distance, std::vector<std::string>& neighbors) const {
    std::vector<u_int32_t> result;
    NearNeighborSearch(word, distance, 0, 0, result, neighbors);
}

void TriSearchTries::NearNeighborSearch(const std::string& word,
                                        int distance, int word_idx, int tree_idx,
                                        std::vector<u_int32_t>& result,
                                        std::vector<std::string>& neighbors) const {
    int i;
    u_int32_t value;

    if (Size() == 0 || distance < 0 || (size_t)tree_idx >= free_)
        return;

    i = word_idx;
    value = u8_nextchar(word.c_str(), &i);

    if (distance > 0 || data_[tree_idx].value > value) {
        NearNeighborSearch(word, distance, word_idx,
                           data_[tree_idx].lokid, result, neighbors);
    }

    result.push_back(data_[tree_idx].value);
    int dist = (data_[tree_idx].value == value) ? distance : distance - 1;
    if (data_[tree_idx].end) {
        // std::cout << "remain: " << u8_strlen(word.c_str() + i)
        //           << ", remain bytes: " << (word.size() - word_idx - 1)
        //           << ", distance: " << distance << std::endl;
        if (u8_strlen(word.c_str() + i) <= dist) {
            int len;
            char temp[4];
            std::string neighbor = "";
            for (auto v : result) {
                len = u8_wc_toutf8(temp, v);
                for (int i = 0; i < len; ++i)
                    neighbor += temp[i];
            }
            neighbors.push_back(neighbor);
        }
    }
    NearNeighborSearch(word,
                       dist,
                       ((size_t)word_idx < word.size()) ? i : word_idx,
                       data_[tree_idx].eqkid,
                       result,
                       neighbors);
    result.pop_back();

    if (distance > 0 || data_[tree_idx].value < value) {
        NearNeighborSearch(word, distance, word_idx,
                           data_[tree_idx].hikid, result, neighbors);
    }
}

size_t TriSearchTries::Save(const std::string& binfile) {
    FILE* idx = OpenFile(binfile.c_str(), "w+");
    if (idx == NULL) {
        DoLog(Logger::ERROR, "Open Tst file failed %d (%s:%d)", errno, __FILE__, __LINE__);
        return false;
    }
    int fd = fileno(idx);

    int ret;
    uint16_t sec;
    uint64_t sec_long;

    sec = TRI_SEARCH_TRIES;
    ret = WriteFD(fd, reinterpret_cast<const char*>(&sec), sizeof(sec));
    if (ret < 0) {
        DoLog(Logger::ERROR, "write Tst file failed %d (%s:%d)", errno, __FILE__, __LINE__);
        CloseFile(idx);
        return false;
    }

    sec_long = free_;
    ret = WriteFD(fd, reinterpret_cast<const char*>(&sec_long), sizeof(sec_long));
    if (ret < 0) {
        DoLog(Logger::ERROR, "write Tst file failed %d (%s:%d)", errno, __FILE__, __LINE__);
        CloseFile(idx);
        return false;
    }

    sec_long = count_;
    ret = WriteFD(fd, reinterpret_cast<const char*>(&sec_long), sizeof(sec_long));
    if (ret < 0) {
        DoLog(Logger::ERROR, "write Tst file failed %d (%s:%d)", errno, __FILE__, __LINE__);
        CloseFile(idx);
        return false;
    }

    ret = WriteFD(fd, reinterpret_cast<const char*>(data_), (count_ + 1) * sizeof(TstNode));
    if (ret < 0) {
        DoLog(Logger::ERROR, "write Tst data failed %d (%s:%d)", errno, __FILE__, __LINE__);
        return false;
    }
    return Size();
}

TriSearchTries* TriSearchTries::Load(void* addr, size_t size) {
    uint16_t* sec;
    uint64_t* sec_long;
    TstNode* data;
    size_t frees;
    size_t count;

    if (size == 0) {
        DoLog(Logger::WARNING, "size equal zero (%s:%d)", __FILE__, __LINE__);
        return NULL;
    }
    sec = reinterpret_cast<uint16_t*>(addr);
    if (*sec++ != TRI_SEARCH_TRIES) {
        DoLog(Logger::WARNING, "file invalid (%s:%d)", __FILE__, __LINE__);
        return NULL;
    }
    sec_long = reinterpret_cast<uint64_t*>(sec);
    frees = *sec_long++;
    count = *sec_long++;
    data = reinterpret_cast<TstNode*>(sec_long);
    return new TriSearchTries(data, frees, count);
}

TriSearchTries* TriSearchTries::Build(const std::string& txtfile) {
    std::string line;
    std::vector<std::string> words;
    std::ifstream fs(txtfile);
    size_t total = 0L;
    int nodes = 0;
    TriSearchTries* tries = nullptr;

    if (!fs.is_open()) {
        DoLog(Logger::ERROR, "build Tst data failed, txt: %s (%s:%d)", txtfile.c_str(), __FILE__, __LINE__);
        return nullptr;
    }
    while (getline(fs, line)) {
        trim(line);
        if (line.size()) {
            words.push_back(line);
        }
    }
    std::sort(words.begin(), words.end());

    total += u8_strlen(words[0].c_str());
    for (size_t i = 1; i < words.size(); ++i) {
        int len = u8_prefixlen(words[i - 1].c_str(), words[i - 1].size(),
                               words[i].c_str(), words[i].size());
        int inc = u8_strlen(words[i].c_str()) - len;
        assert(inc >= 0);
        total += inc;
    }

    tries = new TriSearchTries(total);
    nodes = BuildFromMiddle(tries, words, 0, words.size());
    assert(tries->Capacity() == total);
    assert(tries->Size() == total);
    assert((size_t)nodes == words.size());
    return tries;
}

}  // namespace dsblock
