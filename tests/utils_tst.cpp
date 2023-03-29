#include "doctest.h"

#include "utility.h"
#include <cstdint>

//----------------------------------------------------
TEST_CASE("Utility::isFloatingSame") {

  CHECK(tl::isFloatingSame(1.0f / 3.0f, 2.0f / 6.0f));
  CHECK(!tl::isFloatingSame(1.0f / 3.0f, 0.3333333f));
}

//----------------------------------------------------
TEST_CASE("utility, readAdcSample") {
  uint8_t a[] = {0x22, 0x33, 0x44, 0x55};

  auto out = tl::readAdcSample(a);

  CHECK(0x223344 == out);
}

//----------------------------------------------------
struct Reg {
  constexpr static uint8_t a[]{0, 4};
  constexpr static uint8_t b[]{4, 4};
  constexpr static uint8_t c[]{8, 1};
  constexpr static uint8_t d[]{9, 1};
  constexpr static uint8_t e[]{10, 1};
};

//----------------------------------------------------
TEST_CASE("utility, setRegister") {
  uint32_t reg = 0xaaaaaa;

  tl::setRegister(reg, Reg::a, 3, Reg::c, 1);

  CHECK(0xaaaba3 == reg);
}

//----------------------------------------------------
TEST_CASE("utility, getRegField") {
  uint32_t reg = 0xaaaaaa;

  auto out = tl::getRegField(reg, Reg::b);

  CHECK(0xa == out);
}

//----------------------------------------------------
TEST_CASE("utility, isRegFieldsSet") {
  uint32_t reg = 0xaaa1aa;

  auto out1 = tl::isRegFieldsSet(reg, Reg::b, Reg::c);
  CHECK(out1);

  auto out2 = tl::isRegFieldsSet(reg, Reg::e, Reg::d);
  CHECK(!out2);

  auto out3 = tl::isRegFieldsSet(reg, Reg::a);
  CHECK(out3);
}

//----------------------------------------------------
TEST_CASE("utility, hexLetterToByte") {
  CHECK_EQ(15, tl::hexLetterToByte('f'));
  CHECK_EQ(0, tl::hexLetterToByte('0'));
  CHECK_EQ(10, tl::hexLetterToByte('a'));
  CHECK_EQ(9, tl::hexLetterToByte('9'));
}

//----------------------------------------------------
TEST_CASE("utility, decLetterToByte") {
  CHECK_EQ(5, tl::decLetterToByte('5'));
  CHECK_EQ(0, tl::decLetterToByte('0'));
  CHECK_EQ(0, tl::decLetterToByte('f'));
  CHECK_EQ(9, tl::decLetterToByte('9'));
}

//----------------------------------------------------
TEST_CASE("utility, bcd2Byte") {
  CHECK_EQ(22, tl::bcd2Byte(0x22));
  CHECK_EQ(30, tl::bcd2Byte(0x30));
  CHECK_EQ(05, tl::bcd2Byte(0x05));
}

//----------------------------------------------------
TEST_CASE("utility, reverse16s") { CHECK_EQ(0x3412, tl::reverse16s(0x1234)); }
