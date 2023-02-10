#ifndef F_PACKET_TEST_F_MAVLINK_H
#define F_PACKET_TEST_F_MAVLINK_H

#include <cstdint>


/// Calculate mavlink - type crc (no salt)
inline auto crc_mav = [](auto begin, auto end)
{
  uint16_t acc = 0xffff;

  while (begin != end) {
    uint8_t data = *begin;
    uint8_t tmp = data ^ (uint8_t)(acc & 0xff);
    tmp ^= (tmp << 4);
    acc = (acc >> 8) ^ (tmp << 8) ^ (tmp << 3) ^ (tmp >> 4);
    ++begin;
  }
  return acc;
};
#endif // F_PACKET_TEST_F_MAVLINK_H
