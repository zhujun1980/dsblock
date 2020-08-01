/*
 * File:   fc.h
 * Author: zhujun
 */

#ifndef FC_H
#define	FC_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <time.h>

#ifdef	__cplusplus
}
#endif

#include <string>
#include <map>
#include <algorithm>
#include "utils.h"
#include "murmur_hash3.h"
#include "file_mapping.h"
#include "log.h"

namespace dsblock {

template<typename _Tp>
class FCache {
public:
    typedef uint32_t                                Key;
    typedef FileMapping<_Tp>                        Value;
    typedef typename Value::FileStatus              FileStatus;
    typedef std::map<Key, Value*>                   FileCache;
    typedef typename FileCache::const_iterator      FileCacheIter;

public:
    FCache() {
    }

    virtual ~FCache() {
    }

    int GetMappingObject(const std::string& bin_file, const _Tp** obj) {
        uint32_t hv = Hash(bin_file);
        FileCacheIter iter = caches_.find(hv);

        if(iter != caches_.end()) {
            //Found, check file's status
            FileStatus status = iter->second->CheckFileStatus();
            if(status == Value::UNAVAILABLE) {
                DoLog(Logger::ERROR, "Unavailable filepath: %s (%s:%d)", bin_file.c_str(), __FILE__, __LINE__);
                return -1;
            }
            if(status == Value::UNCHANGED) {
                iter->second->atime = time(NULL);
                *obj = iter->second->object;
                return 0;
            }
            //Changed, get file path
            delete iter->second;
            caches_.erase(hv);
        }

        FileMapping<_Tp>* mapping = FileMappingCreate<_Tp>(bin_file);
        if(!mapping) {
            DoLog(Logger::ERROR, "Map filepath failed: %s (%s:%d)", bin_file.c_str(), __FILE__, __LINE__);
            return -1;
        }
        caches_[hv] = mapping;
        *obj = mapping->object;
        return 0;
    }

    static void Clear() {
        for(FileCacheIter iter = caches_.begin(); iter != caches_.end(); iter++) {
            delete iter->second;
        }
        caches_.clear();
    }

    static void Purge(long unused_threshold) {
        std::vector<typename FileCache::key_type> deleted;
        for(FileCacheIter iter = caches_.begin(); iter != caches_.end(); iter++) {
            if(time(NULL) - unused_threshold > iter->second->atime) {
                delete iter->second;
                deleted.push_back(iter->first);
            }
        }
        for(typename std::vector<typename FileCache::key_type>::const_iterator iter = deleted.begin(); iter != deleted.end(); iter++) {
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

template<typename _Tp>
typename FCache<_Tp>::FileCache FCache<_Tp>::caches_;

}

#endif	/* FC_H */
