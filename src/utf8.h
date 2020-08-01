/*
 * File:   utf8.h
 * Author: zhujun
 */
#pragma once

#include <stddef.h>
#include <cstdlib>
#include <string>
#include <algorithm>

#define isutf(c) (((c)&0xC0)!=0x80)

namespace dsblock {

// https://www.cprogramming.com/tutorial/unicode.html

u_int32_t u8_nextchar(const char *s, int *i);

int u8_strlen(const char *s);

int u8_prefixlen(const char *s1, int l1, const char *s2, int l2);

int u8_wc_toutf8(char *dest, u_int32_t ch);

inline void trim(std::string& line) {
    line.erase(line.begin(), find_if_not(line.begin(), line.end(), [](const char c){return std::isspace(c);}));
    line.erase(find_if_not(line.rbegin(), line.rend(), [](const char c){return std::isspace(c);}).base(), line.end());
}

}
