#!/usr/bin/env python
# encoding: utf-8

import os
import uuid
import tempfile
import unittest
from .bloomfilter import *


class TestBloomfilter(unittest.TestCase):
    def setUp(self):
        fd, txt = tempfile.mkstemp()
        s = set([str(uuid.uuid1()) for _ in range(1000)])
        for line in s:
            line += "\n"
            os.write(fd, line.encode())
        os.close(fd)

        fd, binf = tempfile.mkstemp()
        os.close(fd)

        self.txt_file = txt
        self.bin_file = binf
        self.data_set = s
        self.data_vers = 1
        self.hashs_cnt = 8
        self.bits_per_element = 32
        self.num_elements = len(self.data_set)
        cnt = bloomfilter_create_bin_from_txt(self.txt_file, self.bin_file,
                                              self.num_elements, self.data_vers,
                                              self.hashs_cnt, self.bits_per_element)
        self.assertEqual(cnt, self.num_elements)
        # print("txt: " + self.txt_file)
        # print("bin: " + self.bin_file)

    def test_1_stats(self):
        stats = bloomfilter_get_stats(self.bin_file)
        self.assertEqual(self.data_vers, stats['data_vers'])
        self.assertEqual(self.hashs_cnt, stats['hashs_cnt'])

    def test_2_is_present(self):
        for k in self.data_set:
            self.assertTrue(bloomfilter_is_present(self.bin_file, k))

    def test_3_is_not_present(self):
        for _ in range(1000):
            self.assertFalse(bloomfilter_is_present(self.bin_file, str(uuid.uuid1())))

    def test_4_bin_file_not_exist(self):
        self.assertFalse(bloomfilter_is_present("notexists.bf", str(uuid.uuid1())))

    def tearDown(self):
        os.unlink(self.txt_file)
        os.unlink(self.bin_file)
