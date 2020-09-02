#include "doctest.h"
#include "floatstatic.h"

#include <iostream>
#include "utility.h"


TEST_CASE("Qnumber, makeFloatStatic")
{
    auto a = makeFloatStatic<int16_t, 12>(-4095);
    CHECK(isFloatingSame(-4095, toDouble(a)));
}

TEST_CASE("Qnumber, directInit")
{
    FloatStatic<int16_t, -9> b;
    b.fraction = 4096;
    CHECK(isFloatingSame(8, toDouble(b)));
}

TEST_CASE("Qnumber, addition")
{
    auto a = makeFloatStatic<int16_t, 12>(-4095);
    auto b = makeFloatStatic<int16_t, 12>(356);

    CHECK(isFloatingSame(-3739, toDouble(a + b)));
}

TEST_CASE("Qnumber, substraction")
{
    auto a = makeFloatStatic<int16_t, 12>(-4095);
    auto b = makeFloatStatic<int16_t, 12>(356);

    auto result = (a - b);

    CHECK(isFloatingSame(-4451, toDouble(result)));
}
