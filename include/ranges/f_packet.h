//
// Created by aanti on 03.02.2023.
//

#ifndef REG_F_PACKET_H
#define REG_F_PACKET_H

#include "range.h"
#include <cstdint>
#include <optional>
#include "bytebuffer.h"


/// Calculate mavlink - type crc
auto crc_mav(auto begin, auto end)
{
  uint16_t acc = 0xffff;

  while (begin != end) {
    uint8_t data = *begin;
    uint8_t tmp;
    tmp = data ^ (uint8_t)(acc & 0xff);
    tmp ^= (tmp << 4);
    acc = (acc >> 8) ^ (tmp << 8) ^ (tmp << 3) ^ (tmp >> 4);
    ++begin;
  }
  return acc;
}


/// 9  2 = 18 bytes = 144 bits
/// 2400 => 16 Hz
struct packet_rc_full
{
  uint16_t analog_channels[6];
  uint16_t digital_channels;
};
/// 6 * 2 = 12 bytes = 96 bits
/// 2400 => 25Hz
struct packet_rc_nav
{
  constexpr static uint8_t signature = 0xAC;
  uint16_t analog_channels[4];
};

/// Context with all packet state
struct device_context
{
  packet_rc_nav rc_nav;
} __attribute__((aligned(8)));

///
auto check_signature(uint8_t signature)
{
  auto ret  = [&](auto ran) -> std::optional<decltype(ran)> {
    if (*ran.begin() == signature) {
      // Return range without signature
      return std::make_optional(tl::Range(ran.begin() + 1, ran.end()));
    }
    return {};
  };
  return ret;
}


auto check_size(auto size)
{
  auto ret  = [&](auto ran) -> std::optional<decltype(ran)> {
    if ((ran.end() - ran.begin()) < size) {
      // Return empty range
      return std::make_optional(tl::Range(ran.begin(), ran.begin()));
    }
    return {};
  };
  return ret;
}


auto check_crc(auto crc_fun)
{
  auto ret  = [&](auto ran) -> std::optional<decltype(ran)>  {
    auto bb  = tl::byte_buffer(ran.end() - 2, ran.end());
    auto val = bb.template get_value_le<uint16_t>();
    if (*val == crc_fun(ran.begin(), ran.end() - 2)) {
      return ran;
    }
    // Return empty range
    return {};
  };
  return ret;
}

auto mbind(auto fun)
{
  auto ret  = [&](auto opt) -> decltype(fun(opt.value())) {
    if (opt) {
      return fun(opt.value());
    }
    return {};
  };
  return ret;
}

/// Unwrap optional monad to default value if not exist
auto unwrap_opt_range(auto default_value)
{
  auto ret  = [&](auto opt) -> decltype(opt.value()) {
    if (opt) {
      return opt.value();
    }
    return default_value;
  };
  return ret;
}

/// Returns remaining range after parsing or original range if fail
auto parse_rc_nav(packet_rc_nav& nav_packet)
{
  auto ret  = [&](auto ran)
  {
     auto out = ran
      | mbind(check_signature(nav_packet.signature))
      | mbind(check_size(sizeof(packet_rc_nav) + 4))
      | mbind(check_crc(crc_mav))
      | unwrap_opt_range(ran);

  };

  return ret;
}

/// Correct fifo read pointer to remaining range size
/// If fifo begin iterator is the same as in range, increment it by 1
auto correct_fifo(auto& fifo)
{
  // TODO:
}



template<typename F>
bool parse_dispatch(F &fifo, device_context &context)
{
  tl::makeRange(fifo)
    | parse_rc_nav(context.rc_nav)
    | correct_fifo();
}


#endif // REG_F_PACKET_H
