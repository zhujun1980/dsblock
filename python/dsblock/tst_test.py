#!/usr/bin/env python
# encoding: utf-8

import os
import uuid
import tempfile
import unittest
from .tst import *

vocabulary = ['李浩', '互联网式','皮卡特','博客','免费版','陈小川','华能','云泥','河势','庙沟','非典','首页','摩托罗拉','大千世界',
              '皇州','三个代表','李桐','试听','市场经济','贝斯勒','原创性','开发者','圣庙','知识产权局','帖帖','新浪网','邵佳','李践',
              '吴彬','佩雷拉','品牌服饰','疯狂宇宙','二手房','主板','最新型','中关','双鱼牌','陈水扁','高新技术','当日','科学技术部',
              '郭氏','房型','网络版','浦项','维尼','桑福','李会昌','中国共产党','浏览器','铁木真','小小孩','胡锦涛','忍俊','智识者',
              '闽发证券有限公司','江泽慧','江泽民','孙泽民','泽民','核准','萨格尔王','基金金盛','巨奖','弓箭','麻醉科','淡江','人民代表大会','米卢',
              '指明','中国证监会','李俭','习近平','人大常委会','风府','姚明','开洋村','申花','科比','酷刑','避暑','禽流感','戈培尔',
              '点评','质素','博客网','小熊维尼','温家宝','红塔杯','没想到','晋州市','卖点','丹娘','十六中','十六大','乔丹',
              '康佳','未成年人','李光辉','受益','忍不住','李韦','华纳','执行部','切尼','大海掀翻小池塘',
              '张一丰','张二丰','赵三丰', '张建国', '曝三样', '张二丰收', '三丰',
              'content', 'contentious', 'contentive', 'contention', 'contentedly']


class TestTst(unittest.TestCase):
    def setUp(self):
        fd, self.txt_file = tempfile.mkstemp()
        for line in vocabulary:
            line += "\n"
            os.write(fd, line.encode())
        os.close(fd)

        fd, self.bin_file = tempfile.mkstemp()
        os.close(fd)

    def test_create_tst(self):
        # tst_create_bin_from_txt("/recsys/source/service/dsblock/30wdict.txt", "./sorted.txt")
        # tst_create_bin_from_txt("/mnt/code/service/dsblock/30wdict.txt", "./sorted.txt")
        # tst_create_bin_from_txt("/Users/zhujun/Workshop/code/service/dsblock/30wdict.txt", "./dict.tst")
        # tst_create_bin_from_txt("/Users/zhujun/source/sina/git/recsys/service/build/service/dsblock/30wdict.txt", "./dict.tst")
        self.assertTrue(tst_create_bin_from_txt(self.txt_file, self.bin_file) > 0)

    def test_is_present(self):
        tst_create_bin_from_txt(self.txt_file, self.bin_file)
        for v in vocabulary:
            self.assertTrue(tst_is_present(self.bin_file, v))

        for _ in range(100):
            self.assertFalse(tst_is_present(self.bin_file, str(uuid.uuid1())))

        self.assertFalse(tst_is_present(self.bin_file, ""))
        self.assertFalse(tst_is_present(self.bin_file, "没想"))
        self.assertFalse(tst_is_present(self.bin_file, "十六"))
        self.assertTrue(tst_is_present(self.bin_file, "维尼"))
        for v in vocabulary:
            self.assertFalse(tst_is_present(self.bin_file, v[0:1]))

    def test_prefix_search(self):
        tst_create_bin_from_txt(self.txt_file, self.bin_file)
        ret = tst_prefix_search(self.bin_file, "李")
        self.assertEqual(ret["total_words"], 7)
        self.assertEqual(ret["return_words"], 7)
        self.assertSetEqual(set(ret["words"]), {"李浩", "李桐", "李践", "李会昌", "李俭", "李光辉", "李韦"})

    def test_nn_search(self):
        tst_create_bin_from_txt(self.txt_file, self.bin_file)
        ret = tst_near_neighbors_search(self.bin_file, "江泽民", 1)
        self.assertEqual(ret["total_neighbors"], 3)
        self.assertEqual(ret["return_neighbors"], 3)
        self.assertSetEqual(set(ret["neighbors"]), {"江泽民", "江泽慧", "孙泽民"})

        ret = tst_near_neighbors_search(self.bin_file, "张三丰", 1)
        self.assertEqual(ret["total_neighbors"], 3)
        self.assertEqual(ret["return_neighbors"], 3)
        self.assertSetEqual(set(ret["neighbors"]), {"张一丰", "张二丰", "赵三丰"})

        ret = tst_near_neighbors_search(self.bin_file, "张三丰", 2)
        self.assertEqual(ret["total_neighbors"], 6)
        self.assertEqual(ret["return_neighbors"], 6)
        self.assertSetEqual(set(ret["neighbors"]), {"张一丰", "张二丰收", "张二丰", "赵三丰", "张建国", "曝三样"})

        ret = tst_near_neighbors_search(self.bin_file, "content", 0)
        self.assertEqual(ret["total_neighbors"], 1)
        self.assertEqual(ret["return_neighbors"], 1)
        self.assertSetEqual(set(ret["neighbors"]), {"content"})

        ret = tst_near_neighbors_search(self.bin_file, "content", 1)
        self.assertEqual(ret["total_neighbors"], 1)
        self.assertEqual(ret["return_neighbors"], 1)
        self.assertSetEqual(set(ret["neighbors"]), {"content"})

        ret = tst_near_neighbors_search(self.bin_file, "contentive", 2)
        self.assertEqual(ret["total_neighbors"], 2)
        self.assertEqual(ret["return_neighbors"], 2)
        self.assertSetEqual(set(ret["neighbors"]), {"contentive", "contention"})

        ret = tst_near_neighbors_search(self.bin_file, "contentive", 3)
        self.assertEqual(ret["total_neighbors"], 4)
        self.assertEqual(ret["return_neighbors"], 4)
        self.assertSetEqual(set(ret["neighbors"]), {"content", "contentive", "contention", "contentious"})

        ret = tst_near_neighbors_search(self.bin_file, "contentive", 4)
        self.assertEqual(ret["total_neighbors"], 5)
        self.assertEqual(ret["return_neighbors"], 5)
        self.assertSetEqual(set(ret["neighbors"]), {"contentedly", "content", "contentive", "contention", "contentious"})

    def tearDown(self):
        # print("remove ", self.txt_file, self.bin_file)
        os.unlink(self.txt_file)
        os.unlink(self.bin_file)
