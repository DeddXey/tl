#ifndef CHECKSUM_H
#define CHECKSUM_H

#include "view.h"
#include <algorithm>
#include <cstdint>
#include <numeric>
#include <tuple>
#include <utility>

//------------------------------------------------------------------------
template<typename T>
auto fletcher8Checksum(T begin, T end)
{
  uint8_t a = 0;
  uint8_t b = 0;

  for (auto i = begin; i != end; ++i) {
    a += *i;
    b = b + a;
  }
  return std::make_tuple(a, b);
}

template<typename T, uint8_t poly = 0xff>
constexpr uint8_t Crc8_16(const T &data)
{
  uint8_t crc = poly;

  for (auto &a : data) {
    crc ^= ((a >> 8U) & 0xffU);
    for (auto i = 0U; i < 8U; i++) {
      crc = crc & 0x80U ? (crc << 1U) ^ 0x31U : crc << 1U;
    }
    crc ^= (a & 0xffU);
    for (auto i = 0U; i < 8; i++) {
      crc = crc & 0x80U ? (crc << 1U) ^ 0x31U : crc << 1U;
    }
  }

  return crc;
}

template<typename T>
constexpr uint8_t Crc8(const T &c)
{
  uint8_t crc = 0xFF;
  // unsigned int i;

  for (auto &a : c) {
    crc ^= ((a >> 8U) & 0xffU);
    for (auto i = 0U; i < 8U; i++)
      crc = crc & 0x80U ? (crc << 1U) ^ 0x31U : crc << 1U;
    crc ^= (a & 0xffU);
    for (auto i = 0U; i < 8; i++)
      crc = crc & 0x80U ? (crc << 1U) ^ 0x31U : crc << 1U;
  }

  return crc;
}

inline std::array<uint32_t, 256> generate_crc_lookup_table() noexcept
{
  //    auto const reversed_polynomial = uint32_t{0x04C11DB7};
  auto const reversed_polynomial = uint32_t{ 0xEDB88320 };

  // This is a function object that calculates the checksum for a value,
  // then increments the value, starting from zero.
  struct byte_checksum
  {
    uint32_t operator()() noexcept
    {
      auto checksum = static_cast<uint32_t>(n++);

      for (auto i = 0; i < 8; ++i)
        checksum =
          (checksum >> 1) ^ ((checksum & 0x1u) ? reversed_polynomial : 0);

      return checksum;
    }

    unsigned n = 0;
  };

  auto table = std::array<uint32_t, 256>{};
  std::generate(table.begin(), table.end(), byte_checksum{});

  return table;
}

// Calculates the CRC for any sequence of values. (You could use type traits and
// a static assert to ensure the values can be converted to 8 bits.)
template<typename InputIterator>
uint32_t crc32(InputIterator first, InputIterator last)
{
  // Generate lookup table only on first use then cache it - this is
  // thread-safe.
  static auto const table = generate_crc_lookup_table();

  // Calculate the checksum - make sure to clip to 32 bits, for systems that
  // don't have a true (fast) 32-bit type.
  return uint32_t{ 0xFFFFFFFFuL }
         & ~std::accumulate(first,
                            last,
                            ~uint32_t{ 0 } & uint32_t{ 0xFFFFFFFFuL },
                            [](std::uint32_t checksum, uint8_t value) {
                              return table[(checksum ^ value) & 0xFFu]
                                     ^ (checksum >> 8);
                            });
}

#endif // CHECKSUM_H
