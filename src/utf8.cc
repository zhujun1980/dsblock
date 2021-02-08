/*
 * File:   utf8.cc
 * Author: zhujun
 */
#include <cstring>
#include <iostream>
#include "utf8.h"

namespace dsblock {

static const u_int32_t offsetsFromUTF8[6] = {
    0x00000000UL, 0x00003080UL, 0x000E2080UL,
    0x03C82080UL, 0xFA082080UL, 0x82082080UL};

u_int32_t u8_nextchar(const char *s, int *i) {
    u_int32_t ch = 0;
    int sz = 0;

    if (s[*i] == 0) {
        return 0;
    }
    do {
        ch <<= 6;
        ch += (unsigned char)s[(*i)++];
        sz++;
    } while (s[*i] && !isutf(s[*i]));
    ch -= offsetsFromUTF8[sz - 1];

    return ch;
}

int u8_strlen(const char *s) {
    int count = 0;
    int i = 0;

    while (u8_nextchar(s, &i) != 0)
        count++;

    return count;
}

int u8_prefixlen(const char *s1, int l1, const char *s2, int l2) {
    int len = 0;
    int cnt = 0;

    if (l1 == -1) {
        l1 = strlen(s1);
    }
    if (l2 == -1) {
        l2 = strlen(s2);
    }
    while (len < l1 && len < l2) {
        int old = len;
        u_int32_t c1 = u8_nextchar(s1, &len);
        u_int32_t c2 = u8_nextchar(s2, &old);
        if (c1 != c2) {
            break;
        }
        ++cnt;
    }
    return cnt;
}

int u8_wc_toutf8(char *dest, u_int32_t ch) {
    if (ch < 0x80) {
        dest[0] = (char)ch;
        return 1;
    }
    if (ch < 0x800) {
        dest[0] = (ch >> 6) | 0xC0;
        dest[1] = (ch & 0x3F) | 0x80;
        return 2;
    }
    if (ch < 0x10000) {
        dest[0] = (ch >> 12) | 0xE0;
        dest[1] = ((ch >> 6) & 0x3F) | 0x80;
        dest[2] = (ch & 0x3F) | 0x80;
        return 3;
    }
    if (ch < 0x110000) {
        dest[0] = (ch >> 18) | 0xF0;
        dest[1] = ((ch >> 12) & 0x3F) | 0x80;
        dest[2] = ((ch >> 6) & 0x3F) | 0x80;
        dest[3] = (ch & 0x3F) | 0x80;
        return 4;
    }
    return 0;
}

}  // namespace dsblock
