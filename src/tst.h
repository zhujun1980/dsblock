/*
 * File:   tst.h
 * Author: zhujun
 */
#pragma once

#include <stdint.h>
#include <memory>
#include <functional>
#include "utils.h"
#include "consts.h"

namespace dsblock {

struct TstNode {
    u_int32_t value;
    int lokid;
    int eqkid;
    int hikid;
    bool end;
};

class TriSearchTries {
public:
    static const char* EXT_NAME;

    typedef std::function<void(const std::vector<u_int32_t>&, size_t idx)> TstNodeVisitor;

public:
    TriSearchTries(size_t count);

    TriSearchTries(TstNode* data, size_t count, size_t frees);

    ~TriSearchTries();

    bool Add(const std::string& word);

    bool IsPresent(const std::string& word) const;

    void PrefixSearch(const std::string& prefix,
                      std::vector<std::string>& words, bool with_prefix = false) const;

    void NearNeighborSearch(const std::string& word,
                            int distance, std::vector<std::string>& neighbors) const;

    void Traverse(std::vector<u_int32_t>& prefix,
                  size_t idx, TstNodeVisitor vistor) const;

    size_t Save(const std::string& binfile);

    size_t Capacity() const {
        return count_;
    }

    size_t Size() const {
        return free_;
    }

private:
    size_t GetNode(const std::string& word) const;

    void NearNeighborSearch(const std::string& word,
                            int distance, int word_idx, int tree_idx,
                            std::vector<u_int32_t>& result,
                            std::vector<std::string>& neighbors) const;

public:
    static TriSearchTries* Load(void* addr, size_t size);

    static TriSearchTries* Build(const std::string& txtfile);

private:
    TstNode* data_;

    size_t free_;

    size_t count_;

    DataLoc data_loc_;
};

}  // namespace dsblock
