#!/usr/bin/env python
# encoding: utf-8

import setuptools


__author__ = "Zhu Jun"
__version__ = "0.0.1"


setuptools.setup(
    name="dsblock",
    author=__author__,
    version=__version__,
    author_email="zhujun1980@gmail.com",
    description="DSBlock",
    long_description="DSBlock",
    url="https://github.com/zhujun1980/dsblock",
    packages=setuptools.find_packages(),
    test_suite='dsblock',
)
