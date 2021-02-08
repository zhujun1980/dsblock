/*
 * File:   fc.h
 * Author: zhujun
 */

#ifndef FC_H
#define FC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

#ifdef __cplusplus
}
#endif

#include <string>
#include <map>
#include <algorithm>
#include <memory>
#include <mutex>
#include <thread>
#include "utils.h"
#include "murmur_hash3.h"
#include "file_mapping.h"
#include "log.h"

namespace dsblock {

template <typename _Tp>
class FCache {
public:
    typedef uint32_t Key;
    typedef FileMapping<_Tp> Value;
    typedef typename Value::FileStatus FileStatus;
    typedef std::map<Key, Value*> FileCache;
    typedef typename FileCache::const_iterator FileCacheIter;

public:
    FCache() {
    }

    virtual ~FCache() {
    }

    int GetMappingObject(const std::string& bin_file, const _Tp** obj) {
        uint32_t hv = Hash(bin_file);
        FileCacheIter iter = caches_.find(hv);
        std::string header = "Init";

        if (iter != caches_.end()) {
            //Found, check file's status
            FileStatus status = iter->second->CheckFileStatus();
            if (status == Value::UNAVAILABLE) {
                DoLog(Logger::ERROR, "Unavailable filepath: %s (%s:%d)", bin_file.c_str(), __FILE__, __LINE__);
                return -1;
            }
            if (status == Value::UNCHANGED) {
                iter->second->atime = time(NULL);
                *obj = iter->second->object;
                return 0;
            }
            //Changed, get file path
            delete iter->second;
            caches_.erase(hv);
            header = "Reload";
        }

        FileMapping<_Tp>* mapping = FileMappingCreate<_Tp>(bin_file);
        if (!mapping) {
            DoLog(Logger::ERROR, "Map filepath failed: %s (%s:%d)", bin_file.c_str(), __FILE__, __LINE__);
            return -1;
        }
        caches_[hv] = mapping;
        *obj = mapping->object;
        DoLog(Logger::INFO, "%s: `%s` has mapped successfully (addr=%p, mtime=%ld, size=%d bytes).",
              header.c_str(), mapping->filepath.c_str(), mapping->attachAddr, mapping->mtime, mapping->filesize);
        return 0;
    }

    static void Clear() {
        for (FileCacheIter iter = caches_.begin(); iter != caches_.end(); iter++) {
            delete iter->second;
        }
        caches_.clear();
    }

    static void Purge(long unused_threshold) {
        std::vector<typename FileCache::key_type> deleted;
        for (FileCacheIter iter = caches_.begin(); iter != caches_.end(); iter++) {
            if (time(NULL) - unused_threshold > iter->second->atime) {
                delete iter->second;
                deleted.push_back(iter->first);
            }
        }
        for (typename std::vector<typename FileCache::key_type>::const_iterator iter = deleted.begin(); iter != deleted.end(); iter++) {
            caches_.erase(*iter);
        }
    }

private:
    uint32_t Hash(const std::string& kb) const {
        uint32_t index;
        MurmurHash3_x86_32(kb.c_str(), static_cast<int>(kb.size()), 0, &index);
        return index;
    }

    DISABLE_ASSIGN_AND_COPY(FCache);

    static FileCache caches_;
};

template <typename _Tp>
class FCacheMT {
public:
    typedef uint32_t Key;
    typedef FileMapping<_Tp> Value;
    typedef typename Value::FileStatus FileStatus;
    typedef std::map<Key, std::shared_ptr<Value>> FileCache;
    typedef typename FileCache::const_iterator FileCacheIter;

public:
    FCacheMT() {}

    virtual ~FCacheMT() {}

    std::shared_ptr<Value> GetMappingObject(const std::string& bin_file) {
        const std::lock_guard<std::mutex> lock(gmutex_);

        uint32_t hv = Hash(bin_file);
        FileCacheIter iter = caches_.find(hv);
        std::string header = "Init";

        if (iter != caches_.end()) {
            // Found, check file's status
            FileStatus status = iter->second->CheckFileStatus();
            if (status == Value::UNAVAILABLE) {
                DoLog(Logger::ERROR, "Unavailable filepath: %s (%s:%d)", bin_file.c_str(),
                      __FILE__, __LINE__);
                return nullptr;
            }
            if (status == Value::UNCHANGED) {
                return iter->second;
            }
            header = "Reload";
        }
        std::shared_ptr<FileMapping<_Tp>> mapping(FileMappingCreate<_Tp>(bin_file));
        if (!mapping) {
            DoLog(Logger::ERROR, "Map filepath failed: %s (%s:%d)",
                  bin_file.c_str(), __FILE__, __LINE__);
            return nullptr;
        }
        DoLog(Logger::INFO, "%s: `%s` has mapped successfully (addr=%p, mtime=%ld, size=%d bytes) [%X]",
              header.c_str(), mapping->filepath.c_str(), mapping->attachAddr, mapping->mtime,
              mapping->filesize, std::this_thread::get_id());
        caches_[hv] = mapping;
        return caches_[hv];
    }

private:
    uint32_t Hash(const std::string& kb) const {
        uint32_t index;
        MurmurHash3_x86_32(kb.c_str(), static_cast<int>(kb.size()),
                           0, &index);
        return index;
    }

    DISABLE_ASSIGN_AND_COPY(FCacheMT);

    static FileCache caches_;

    static std::mutex gmutex_;
};

template <typename _Tp>
typename FCache<_Tp>::FileCache FCache<_Tp>::caches_;

template <typename _Tp>
typename FCacheMT<_Tp>::FileCache FCacheMT<_Tp>::caches_;

template <typename _Tp>
typename std::mutex FCacheMT<_Tp>::gmutex_;

}  // namespace dsblock

#endif /* FC_H */
