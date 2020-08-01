/*
 * File:   tst_unittest.cc
 * Author: zhujun
 */
#include <iostream>
#include <vector>
#include <string>
#include "gtest/gtest.h"
#include "tst.h"

using namespace dsblock;

TEST(Tst, Insert) {
    TriSearchTries *tries = new TriSearchTries(100);
    tries->Add("中国大学生体育协会");
    tries->Add("中国大学");
    tries->Add("中国学");
    tries->Add("中国共产党第一次全国代表大会会址");
    tries->Add("中国共产党第一次全国代表大会");

    EXPECT_EQ(tries->IsPresent("中国共产党第一次全国代表大会会址"), true);
    EXPECT_EQ(tries->IsPresent("中国共产党第一次全国代表大会"), true);
    EXPECT_EQ(tries->IsPresent("博客"), false);

    EXPECT_EQ(tries->IsPresent("中国大学"), true);
    EXPECT_EQ(tries->IsPresent("中国大学生体育协会"), true);
    EXPECT_EQ(tries->IsPresent("中国学"), true);
}

TEST(Tst, Build) {
    const char *txt = "/Users/zhujun/source/sina/git/recsys/service/build/service/dsblock/30wdict.txt";
    // const char *txt = "/Users/zhujun/Workshop/code/service/dsblock/30wdict.txt";

    TriSearchTries *tries = TriSearchTries::Build(txt);
    EXPECT_NE(tries, nullptr);

    bool with_prefix = true;
    std::vector<std::string> words;
    tries->PrefixSearch("中国", words, with_prefix);
    std::cout << words.size() << std::endl;
    for(auto word : words) {
        std::cout << word << std::endl;
    }
}
