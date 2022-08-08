/*
 * File:   tst_unittest.cc
 * Author: zhujun
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include "gtest/gtest.h"
#include "fc.h"
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

const dsblock::TriSearchTries* GetTst() {
    const char *bin = "testdict.tst";
    dsblock::FCacheMT<dsblock::TriSearchTries> caches;
    auto ret = caches.GetMappingObject(bin);
    assert(ret != nullptr);
    return ret->object;
}

TEST(Tst, Build) {
    const char *txt = "testdict.txt";
    {
        std::ofstream ostrm(txt, std::ios::trunc);
        ostrm << "中国大学" << std::endl;
        ostrm << "中国共产党" << std::endl;
        ostrm << "巨奖" << std::endl;
        ostrm << "习近平" << std::endl;
        ostrm << "porn" << std::endl;
        ostrm << "org" << std::endl;

        ostrm << "突然" << std::endl;
        ostrm << "突然袭击" << std::endl;
    }
    std::unique_ptr<TriSearchTries> tries(TriSearchTries::Build(txt));
    EXPECT_NE(tries.get(), nullptr);
    const char *bin = "testdict.tst";
    auto sz = tries->Save(bin);
    EXPECT_GE(sz, 0);

    dsblock::FCacheMT<dsblock::TriSearchTries> caches;
    auto ret = caches.GetMappingObject(bin);
    EXPECT_NE(ret, nullptr);
}

TEST(Tst, Match) {
    auto tst = GetTst();
    EXPECT_NE(tst, nullptr);
    std::vector<std::string> words;

    std::string longtext = "pornNetflix的这组实验希望验证的是Interleaving方法相比传统AB Test，需要多少样本就能够验证出算法A和算法B的优劣 习近平 我们之前 巨奖一再强调线上测试资源的紧张，因此这里自然希望Interleaving能够利用较少的线上资源，较少的测试用户就解决评估问题。这就是所谓的“灵敏度比较”。图5是实验结果，横轴是参与实验的样本数量，纵轴Netflix没有给出非常精准的解释，但我们可以理解为是判定算法A是否比算法B好的“错误”概率。可以看出的是interleaving的方法利用10^3个样本就能够判定算法A是否比B好，而AB test则需要10^5个样本才能够将错误率降到5%以下。这就意味着利用一组AB Test的资源，我们可以做100组Interleaving实验。这无疑大大加强了线上测试的能力。porn";
    int ret = tst->Match(longtext, words);
    EXPECT_EQ(ret, 4);
    EXPECT_EQ(words[0], "porn");
    EXPECT_EQ(words[1], "习近平");
    EXPECT_EQ(words[2], "巨奖");
    EXPECT_EQ(words[3], "porn");

    std::string stext = "Ne";
    words.clear();
    ret = tst->Match(stext, words);
    EXPECT_EQ(ret, 0);

    words.clear();
    ret = tst->Match("", words);
    EXPECT_EQ(ret, 0);

    words.clear();
    ret = tst->Match("pornpornpornpornpornpornpornpornpornporn", words);
    EXPECT_EQ(ret, 10);

    words.clear();
    ret = tst->Match("porgornornporn", words);
    EXPECT_EQ(ret, 2);
    EXPECT_EQ(words[0], "org");
    EXPECT_EQ(words[1], "porn");

    words.clear();
    ret = tst->Match("porpornornporn巨奖orn", words);
    EXPECT_EQ(ret, 3);
    EXPECT_EQ(words[0], "porn");
    EXPECT_EQ(words[1], "porn");
    EXPECT_EQ(words[2], "巨奖");

    words.clear();
    ret = tst->Match("突然袭击", words);
    EXPECT_EQ(ret, 1);
    EXPECT_EQ(words[0], "突然");

    words.clear();
    ret = tst->Match("突然，闪出一人发出突然袭击", words);
    EXPECT_EQ(ret, 2);
    EXPECT_EQ(words[0], "突然");
    EXPECT_EQ(words[1], "突然");

    words.clear();
    ret = tst->Match("突", words);
    EXPECT_EQ(ret, 0);

    words.clear();
    ret = tst->Match("突 然", words);
    EXPECT_EQ(ret, 0);

    // for(auto w : words) {
    //     std::cout << w << std::endl;
    // }
}

TEST(Tst, PrefixSearch) {
    auto tst = GetTst();
    EXPECT_NE(tst, nullptr);

    bool with_prefix = true;
    std::vector<std::string> words;
    tst->PrefixSearch("中国", words, with_prefix);
    EXPECT_EQ(words.size(), 2);
    EXPECT_EQ(words[0], "中国共产党");
    EXPECT_EQ(words[1], "中国大学");

    with_prefix = false;
    words.clear();
    tst->PrefixSearch("中国", words, with_prefix);
    EXPECT_EQ(words.size(), 2);
    EXPECT_EQ(words[0], "共产党");
    EXPECT_EQ(words[1], "大学");
}
