#include "doctest.h"
#include "floatstatic.h"

#include <iostream>
#include "utility.h"



template<typename T, int16_t used = FloatStaticTraits<T>::bitsOfT>
static bool test_make_float_static_impl(T input)
{
  auto flt_st = makeFloatStatic<T, used>(input);
  auto dbl = toDouble(flt_st);
  bool const out = tl::isFloatingSame(static_cast<double>(input), dbl);
  return out;
}

TEST_CASE("Qnumber, makeFloatStatic")
{
  CHECK(test_make_float_static_impl<int16_t, 12>(-4095));
  CHECK(test_make_float_static_impl<int16_t, 12>(356));
  CHECK(test_make_float_static_impl<int16_t, 12>(-4095));
}

TEST_CASE("Qnumber, directInit")
{
    FloatStatic<int16_t, -9> const b{4096};
    CHECK(tl::isFloatingSame(8.0, toDouble(b)));
}

TEST_CASE("Qnumber, addition")
{
    auto a = makeFloatStatic<int16_t, 12>(-4095);
    auto b = makeFloatStatic<int16_t, 12>(356);

    CHECK(tl::isFloatingSame(-3739.0, toDouble(a + b)));
}

//TEST_CASE("Qnumber, substraction")
//{
//    auto a = makeFloatStatic<int16_t, 12>(-4095);
//    auto b = makeFloatStatic<int16_t, 12>(356);
//
//    auto result = (a - b);
//    auto dbl_result = toDouble(result);
//
//    CHECK(tl::isFloatingSame(-4451.0, dbl_result));
//}
