#!/usr/bin/env python
# encoding: utf-8

import sys
from ctypes import *
from .dll import dll_utils

DEF_BUF_SIZE = 2048

def tst_create_bin_from_txt(txt_file, bin_file):
    cnt = dll_utils.TstCreateBinaryFromText(c_char_p(txt_file.encode()), len(txt_file),
                                            c_char_p(bin_file.encode()), len(bin_file))
    return cnt

def tst_is_present(bin_file, key):
    ret = dll_utils.TstIsPresent(c_char_p(bin_file.encode()), len(bin_file),
                                 c_char_p(key.encode()), len(key.encode()))
    return ret == 1

def tst_prefix_search(bin_file, prefix, with_prefix=True, buf_len=DEF_BUF_SIZE):
    buffer = create_string_buffer(b'\0', buf_len + 1)
    out_len = c_uint(buf_len)
    total = c_int()
    wp = c_int(1) if with_prefix else c_int(0)

    ret = dll_utils.TstPrefixSearch(c_char_p(bin_file.encode()), len(bin_file),
                                    c_char_p(prefix.encode()), len(prefix.encode()),
                                    wp, buffer, pointer(out_len), pointer(total))

    data = buffer.raw
    words = []
    start = 0

    for stop in range(out_len.value + 1):
        if stop == out_len.value or data[stop] == 0:
            if stop > start:
                words.append(data[start:stop].decode("utf-8"))
                start = stop + 1

    return {
        "total_words": total.value,
        "return_words": ret,
        "read_size": out_len.value,
        "words": words,
    }

def tst_near_neighbors_search(bin_file, word, distance, buf_len=DEF_BUF_SIZE):
    buffer = create_string_buffer(b'\0', buf_len + 1)
    out_len = c_uint(buf_len)
    total = c_int()
    dist = c_int(distance)

    ret = dll_utils.TstNearNeighborSearch(c_char_p(bin_file.encode()), len(bin_file),
                                          c_char_p(word.encode()), len(word.encode()),
                                          dist, buffer, pointer(out_len), pointer(total))

    data = buffer.raw
    neighbors = []
    start = 0

    for stop in range(out_len.value + 1):
        if stop == out_len.value or data[stop] == 0:
            if stop > start:
                neighbors.append(data[start:stop].decode("utf-8"))
                start = stop + 1

    return {
        "total_neighbors": total.value,
        "return_neighbors": ret,
        "read_size": out_len.value,
        "neighbors": neighbors,
    }
