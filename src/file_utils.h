/*
 * File:   file_utils.h
 * Author: zhujun
 */

#ifndef __BloomFilter__file_utils__
#define __BloomFilter__file_utils__

#include <iostream>
#include <cstdio>

namespace dsblock {

FILE* OpenFile(const char* filepath, const char* mode);
bool ReadFromFD(int fd, char* buffer, size_t bytes);
int WriteFile(const char* filepath, const char* data, int size);
int WriteFD(const int fd, const char* data, int size);
bool CloseFile(FILE* file);
bool GetFileInfo(const char* file, struct stat* st);
bool GetFileSize(const char* file, size_t& size);

}  // namespace dsblock

#endif /* defined(__BloomFilter__file_utils__) */
