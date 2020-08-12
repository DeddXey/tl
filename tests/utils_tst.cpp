#include "gtest/gtest.h"

#include "utility.h"

//----------------------------------------------------
TEST(utility, setBits)
{
    uint8_t a[] = {3, 1};
    uint8_t b[] = {1, 2};
    auto out =
        tl::setBits(a, 1,
                    b, 3);

    EXPECT_EQ(0xe, out);
}

//----------------------------------------------------
TEST(utility, readAdcSample)
{
    uint8_t a[] = { 0x22, 0x33, 0x44, 0x55 };

    auto out = tl::readAdcSample(a);

    EXPECT_EQ(0x223344, out);
}

//----------------------------------------------------
struct Reg {
    constexpr static uint8_t a[]{ 0, 4 };
    constexpr static uint8_t b[]{ 4, 4 };
    constexpr static uint8_t c[]{ 8, 1 };
    constexpr static uint8_t d[]{ 9, 1 };
};

//----------------------------------------------------
TEST(utility, setRegister)
{
    uint32_t reg = 0xaaaaaa;

    tl::setRegister(reg,
                    Reg::a, 3,
                    Reg::c, 1);

    EXPECT_EQ(0xaaaba3, reg);
}

//----------------------------------------------------
TEST(utility, getRegField)
{
    uint32_t reg = 0xaaaaaa;

    auto out = tl::getRegField(reg, Reg::b);

    EXPECT_EQ(0xa, out);
}

//----------------------------------------------------
TEST(utility, hexLetterToByte)
{
    EXPECT_EQ(15, tl::hexLetterToByte('f'));
    EXPECT_EQ(0, tl::hexLetterToByte('0'));
    EXPECT_EQ(10, tl::hexLetterToByte('a'));
    EXPECT_EQ(9, tl::hexLetterToByte('9'));
}

//----------------------------------------------------
TEST(utility, decLetterToByte)
{
    EXPECT_EQ(5, tl::decLetterToByte('5'));
    EXPECT_EQ(0, tl::decLetterToByte('0'));
    EXPECT_EQ(0, tl::decLetterToByte('f'));
    EXPECT_EQ(9, tl::decLetterToByte('9'));
}

//----------------------------------------------------
TEST(utility, bcd2Byte)
{
    EXPECT_EQ(22, tl::bcd2Byte(0x22));
    EXPECT_EQ(30, tl::bcd2Byte(0x30));
    EXPECT_EQ(05, tl::bcd2Byte(0x05));
}

//----------------------------------------------------
TEST(utility, reverse16s)
{
    EXPECT_EQ(0x3412, tl::reverse16s(0x1234));
}
