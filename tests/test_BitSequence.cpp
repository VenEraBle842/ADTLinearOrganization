#include <gtest/gtest.h>
#include "../src/BitSequence.h"

TEST(BitSequence, Construct) {
    int bits[] = {1, 0, 1, 1};
    BitSequence bs(bits, 4);
    EXPECT_EQ(bs.GetLength(), 4);
    EXPECT_EQ(bs.Get(0).value, 1);
    EXPECT_EQ(bs.Get(1).value, 0);
}

TEST(BitSequence, AND) {
    int a[] = {1, 0, 1, 0};
    int b[] = {1, 1, 0, 0};
    BitSequence ba(a, 4), bb(b, 4);
    auto* res = ba.AND(&bb);
    EXPECT_EQ(res->Get(0).value, 1);
    EXPECT_EQ(res->Get(1).value, 0);
    EXPECT_EQ(res->Get(2).value, 0);
    EXPECT_EQ(res->Get(3).value, 0);
    delete res;
}

TEST(BitSequence, OR) {
    int a[] = {1, 0, 1, 0};
    int b[] = {0, 1, 0, 0};
    BitSequence ba(a, 4), bb(b, 4);
    auto* res = ba.OR(&bb);
    EXPECT_EQ(res->Get(0).value, 1);
    EXPECT_EQ(res->Get(1).value, 1);
    EXPECT_EQ(res->Get(2).value, 1);
    EXPECT_EQ(res->Get(3).value, 0);
    delete res;
}

TEST(BitSequence, XOR) {
    int a[] = {1, 0, 1, 0};
    int b[] = {1, 1, 0, 0};
    BitSequence ba(a, 4), bb(b, 4);
    auto* res = ba.XOR(&bb);
    EXPECT_EQ(res->Get(0).value, 0);
    EXPECT_EQ(res->Get(1).value, 1);
    EXPECT_EQ(res->Get(2).value, 1);
    EXPECT_EQ(res->Get(3).value, 0);
    delete res;
}

TEST(BitSequence, NOT) {
    int a[] = {1, 0, 1};
    BitSequence ba(a, 3);
    auto* res = ba.NOT();
    EXPECT_EQ(res->Get(0).value, 0);
    EXPECT_EQ(res->Get(1).value, 1);
    EXPECT_EQ(res->Get(2).value, 0);
    delete res;
}

TEST(BitSequence, ApplyMask) {
    int a[]    = {1, 1, 1, 0};
    int mask[] = {1, 0, 1, 1};
    BitSequence ba(a, 4), bm(mask, 4);
    auto* res = ba.ApplyMask(&bm);
    EXPECT_EQ(res->Get(0).value, 1);
    EXPECT_EQ(res->Get(1).value, 0);
    EXPECT_EQ(res->Get(2).value, 1);
    EXPECT_EQ(res->Get(3).value, 0);
    delete res;
}

TEST(BitSequence, ToString) {
    int bits[] = {1, 0, 1, 1};
    BitSequence bs(bits, 4);
    EXPECT_EQ(bs.ToString(), "1011");
}
