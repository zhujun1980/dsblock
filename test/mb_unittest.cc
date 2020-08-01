/*
 * File:   mb_unittest.cc
 * Author: zhujun
 */
#include <iostream>
#include "gtest/gtest.h"
#include "utf8.h"

using namespace dsblock;

TEST(MBString, Len) {
    const char *s1 = "123";
    EXPECT_EQ(3, u8_strlen(s1));

    const char *s2 = u8"你好";
    EXPECT_EQ(2, u8_strlen(s2));

    const char *s3 = u8"你好1";
    EXPECT_EQ(3, u8_strlen(s3));

    EXPECT_EQ(15, u8_strlen("齐齐哈尔车辆厂齐齐哈尔铁路分局"));
    EXPECT_EQ(8, u8_strlen(u8"香港会议展览中心"));

    std::string s = u8"高黎贡山国家级自然保护区";
    EXPECT_EQ(12, u8_strlen(s.c_str()));

    std::string s4 = "高黎贡山国家级自然保护区";
    EXPECT_EQ(12, u8_strlen(s4.c_str()));

    // auto i = 0;
    // u_int32_t c;
    // while((c = u8_nextchar("洒水器机车", &i)) != 0) {
    //     std::cout << c << " ";
    // }
    // std::cout << std::endl;
}

TEST(MBString, PrefixLen) {
    const char *s1 = "123";
    const char *s2 = u8"你好";
    const char *s3 = u8"你好1";

    EXPECT_EQ(0, u8_prefixlen(s1, -1, s2, -1));
    EXPECT_EQ(0, u8_prefixlen(s1, strlen(s1), s2, strlen(s2)));
    EXPECT_EQ(strlen(s1), 3);
    EXPECT_EQ(2, u8_prefixlen(s3, -1, s2, -1));

    EXPECT_EQ(2, u8_prefixlen(u8"一一", -1, u8"一一七", -1));
    EXPECT_EQ(2, u8_prefixlen("一一", -1, "一一七", -1));
    EXPECT_EQ(1, u8_prefixlen(u8"一一道来", -1, u8"一丁", -1));

    EXPECT_EQ(1, u8_prefixlen(u8"一下子", -1, u8"一不做", -1));
    EXPECT_EQ(0, u8_prefixlen(u8"一下子", -1, "", -1));
    EXPECT_EQ(0, u8_prefixlen(u8"一龙一蛇", -1, "丁一卯二", -1));
    EXPECT_EQ(4, u8_prefixlen(u8"一龙一蛇", -1, "一龙一蛇", -1));
    EXPECT_EQ(4, u8_prefixlen(u8"一龙一蛇", -1, "一龙一蛇蛇", -1));
}
