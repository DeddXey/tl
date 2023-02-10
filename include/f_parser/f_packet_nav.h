#pragma once

#include <cstdint>
#include "ranges/range.h"
#include "f_mavlink.h"
#include "f_parse_result.h"


struct packet_rc_nav
{
  constexpr static uint8_t signature = 0xAC;
  uint16_t                 analog_channels[4];

  inline auto fill()
  {
    auto ret = [&](auto ran) -> parse_result<decltype(ran)> {

      auto curr = ran.begin();
      for (uint16_t& i: analog_channels) {
        i = curr.template get_value_le<uint16_t>().value();
        //      ++ran.begin();
      }
      return parse_result(tl::Range(curr, ran.end()));
    };
    return ret;
  }
};
