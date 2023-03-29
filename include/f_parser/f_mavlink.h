#pragma once

#include <cstdint>

/// Calculate mavlink - type crc (no salt)
inline auto crc_mav = [](auto begin, auto end) -> uint16_t
{
  uint16_t acc = 0xffff;

  while (begin != end) {
    uint16_t data = *begin;
    uint16_t tmp = data ^ (acc & 0xff);
    tmp ^= static_cast<uint16_t>(tmp << 4U);
    acc = static_cast<uint16_t>((acc >> 8U) ^ (tmp << 3U) ^ (tmp >> 4U));
    ++begin;
  }
//  con.debug() << Use:: hex << "crc: " << acc << "\n";
  return acc;
};
