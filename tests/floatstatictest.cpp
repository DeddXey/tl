#include "floatstatic.h"

#include <iostream>
#include <gtest/gtest.h>



TEST(Qnumber, makeFloatStatic)
{
    auto a = makeFloatStatic<int16_t, 12>(-4095);
    EXPECT_DOUBLE_EQ(-4095, toDouble(a));
}

TEST(Qnumber, directInit)
{
    FloatStatic<int16_t, -9> b;
    b.fraction = 4096;
    EXPECT_DOUBLE_EQ(8, toDouble(b));
}

TEST(Qnumber, addition)
{
    auto a = makeFloatStatic<int16_t, 12>(-4095);
    auto b = makeFloatStatic<int16_t, 12>(356);

    EXPECT_DOUBLE_EQ(-3739, toDouble(a + b));
}

TEST(Qnumber, substraction)
{
    auto a = makeFloatStatic<int16_t, 12>(-4095);
    auto b = makeFloatStatic<int16_t, 12>(356);

    auto result = (a - b);

    EXPECT_DOUBLE_EQ(-4451, toDouble(result));
}
