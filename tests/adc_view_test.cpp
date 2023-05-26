#include "doctest.h"
#include "ranges/adc24toint32iterator.h"
#include <ranges>


TEST_CASE("adc24int32_view::from_uint8_buffer")
{

  uint8_t inbufer[] = { 0x80, 0x00, 0x01, 0x23, 0x23, 0x23, 0x12, 0x34, 0x56 };

  int32_t outbuffer[] = { -8388607, 0x232323, 0x123456 };

  static_assert(sizeof(inbufer) / 3 == sizeof(outbuffer) / 4,
                "Test buffers should be equal size!");

  auto adc = tl::adc24int32_view(std::span(inbufer));

  auto beg = adc.begin();

  for (int i = 0; i < sizeof (outbuffer) / 4 ; ++i){

    CHECK(outbuffer[i] == *beg++);
  }
}