#pragma once

#include <cstdint>
#include <optional>
#include "ranges/range.h"
#include "f_mavlink.h"

/// 9  2 = 18 bytes = 144 bits
/// 2400 => 16 Hz
struct packet_rc_full
{
  constexpr static uint8_t signature = 0xAB;
  uint16_t analog_channels[6];
  uint16_t digital_channels;
  inline auto fill()
  {
    auto ret = [&](auto ran) -> parse_result<decltype(ran)> {

      auto curr = ran.begin();
      for (uint16_t& i: analog_channels) {
        i = curr.template get_value_le<uint16_t>().value();
        //      ++ran.begin();
      }
      digital_channels = curr.template get_value_le<uint16_t>().value();

      return parse_result(tl::Range(curr, ran.end()));
    };
    return ret;
  }

};