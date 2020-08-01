#!/usr/bin/env python
# encoding: utf-8

import os
import logging
from ctypes import *
from platform import *


class LibraryNotFound(Exception):
    pass


def find_lib(base_path=None):
    cdll_names = {
        'Darwin': 'libdsblock.dylib',
        'Linux': 'libdsblock.so',
    }

    if base_path:
        lib_path = os.path.join(base_path, cdll_names[system()])
        if not os.path.exists(lib_path) or not os.path.isfile(lib_path):
            raise LibraryNotFound('Cannot find library in the path: ' + base_path)
        return lib_path

    file_path = os.path.dirname(os.path.abspath(os.path.expanduser(__file__)))
    dll_path = [
        os.getcwd(),
        file_path,
        os.path.join(sys.prefix, 'lib'),
        os.path.join(sys.prefix, 'lib64')
    ]
    dll_path.extend(sys.path)
    dll_path = [os.path.join(p, cdll_names[system()]) for p in dll_path]
    lib_paths = [p for p in dll_path if os.path.exists(p) and os.path.isfile(p)]
    if not lib_paths:
        raise LibraryNotFound('Cannot find library in the path: \n' + ('\n'.join(dll_path)))
    return lib_paths[0]


class DLL:
    def __init__(self, base_path=None):
        lib_path = find_lib(base_path)
        logging.info("Library found:" + lib_path)
        self.dll = CDLL(lib_path)

        self.BloomFilterIsPresent = self.dll.BloomFilterIsPresent
        self.BloomFilterIsPresent.argtypes = [c_char_p, c_ulong, c_char_p, c_ulong]
        self.BloomFilterIsPresent.restype = c_int

        self.BloomFilterCreateBinaryFromText = self.dll.BloomFilterCreateBinaryFromText
        self.BloomFilterCreateBinaryFromText.argtypes = [c_char_p, c_ulong, c_char_p, c_ulong,
                                                         c_int, c_uint, c_int, c_int]
        self.BloomFilterCreateBinaryFromText.restype = c_int

        self.BloomFilterGetStats = self.dll.BloomFilterGetStats
        self.BloomFilterGetStats.argtypes = [c_char_p, c_ulong, POINTER(c_int),
                                             POINTER(c_int), POINTER(c_int), POINTER(c_uint)]
        self.BloomFilterGetStats.restype = c_int

        self.TstCreateBinaryFromText = self.dll.TstCreateBinaryFromText
        self.TstCreateBinaryFromText.argtypes = [c_char_p, c_ulong, c_char_p, c_ulong]
        self.TstCreateBinaryFromText.restype = c_int

        self.TstIsPresent = self.dll.TstIsPresent
        self.TstIsPresent.argtypes = [c_char_p, c_ulong, c_char_p, c_ulong]
        self.TstIsPresent.restype = c_int

        self.TstPrefixSearch = self.dll.TstPrefixSearch
        self.TstPrefixSearch.argtypes = [c_char_p, c_ulong, c_char_p, c_ulong, c_int,
                                         c_char_p, POINTER(c_uint), POINTER(c_int)]
        self.TstPrefixSearch.restype = c_int

        self.TstNearNeighborSearch = self.dll.TstNearNeighborSearch
        self.TstNearNeighborSearch.argtypes = [c_char_p, c_ulong, c_char_p, c_ulong, c_int,
                                               c_char_p, POINTER(c_uint), POINTER(c_int)]
        self.TstNearNeighborSearch.restype = c_int

dll_utils = DLL()
