#!/usr/bin/env python
# encoding: utf-8

from ctypes import *
from .dll import dll_utils


def bloomfilter_is_present(bin_file, key):
    ret = dll_utils.BloomFilterIsPresent(c_char_p(bin_file.encode()), len(bin_file),
                                         c_char_p(key.encode()), len(key))
    return ret == 1


def bloomfilter_create_bin_from_txt(txt_file, bin_file, num_elements, data_vers, hash_count, bits_per_element):
    cnt = dll_utils.BloomFilterCreateBinaryFromText(c_char_p(txt_file.encode()), len(txt_file),
                                                    c_char_p(bin_file.encode()), len(bin_file),
                                                    num_elements, data_vers, hash_count, bits_per_element)
    return cnt


def bloomfilter_get_stats(bin_file):
    bit_size = c_int()
    byte_size = c_int()
    hashs_cnt = c_int()
    data_vers = c_uint()
    ret = dll_utils.BloomFilterGetStats(c_char_p(bin_file.encode()), len(bin_file),
                                        pointer(bit_size), pointer(byte_size),
                                        pointer(hashs_cnt), pointer(data_vers))
    if ret != 0:
        return {}
    return {
        "bit_size": bit_size.value,
        "byte_size": byte_size.value,
        "hashs_cnt": hashs_cnt.value,
        "data_vers": data_vers.value,
    }
