
#include "bufferdescriptor.h"
#include "ranges/range.h"
#include "value.h"
#include <cstdint>
#include <optional>
// #include "f_parser/f_packet.h"
#include "checksum.h"
#include "parser/parse_result.h"
namespace wake {



struct frame
{
  constexpr static uint8_t fend      = 0xC0;
  constexpr static uint8_t fesc      = 0xDB;
  constexpr static uint8_t tfend     = 0xDC;
  constexpr static uint8_t tfesc     = 0xDD;
  constexpr static uint8_t addr_mask = 0x80;

  struct header
  {
    uint8_t address     = 0;
    uint8_t command     = 0;
    uint8_t data_length = 0;
    crc8_iterated<0xDE>     crc_calculated;
  };

  template<class SF>
  inline static void stuff(const uint8_t byte, SF &&send_fun)
  {
    switch (byte) {
    case fend:
      send_fun(fesc);
      send_fun(tfend);
      break;

    case fesc:
      send_fun(fesc);
      send_fun(tfesc);
      break;

    default:
      send_fun(byte);
    }
  }

  /// Stuff and send
  /// \param ran range with packet
  /// \param send_fun send byte function
  /// \param crc_enable add crc flag
  template<typename Data, typename SF>
  static void send(uint8_t addr,
                   uint8_t cmd,
                   uint8_t length,
                   Data   &data,
                   SF    &&send_fun,
                   bool    crc_enable = true)
  {
    crc8_iterated crc_calculated;

    send_fun(frame::fend);
    crc_calculated.add_byte(frame::fend);
    if ((addr & frame::addr_mask) != 0) {
      crc_calculated.add_byte(addr & (~frame::addr_mask));
      stuff(addr, std::forward<SF>(send_fun));
    }
    crc_calculated.add_byte(cmd);
    stuff(cmd, std::forward<SF>(send_fun));

    crc_calculated.add_byte(length);
    stuff(length, std::forward<SF>(send_fun));

    for (auto symbol : data) {
      crc_calculated.add_byte(symbol);
      stuff(symbol, std::forward<SF>(send_fun));
    }
    if (crc_enable) {
      stuff(crc_calculated.get(), std::forward<SF>(send_fun));
    }
  }

private:
  bool use_crc = false;
};

struct parser
{
  enum class parse_state {
    wait_fend,
    address,
    command,
    length,
    data,
    crc
  };

  template<typename Buff>
  parse_result<uint8_t> unstuff(Buff &buff)
  {
    auto out = buff.getOut();
    if (!out) {
      return parse_result<uint8_t>(parse_result_t::no_data); // no bytes
    }

    if (out.value() == frame::fend) {
      buff.pop();
      return parse_result<uint8_t>(parse_result_t::frame_start);
    }

    if (out.value() == frame::fesc) {
      if (buff.getUsed() < 2) {
        return parse_result<uint8_t>(
          parse_result_t::not_enough_data); // no enough bytes
      }
      buff.pop();
      auto byte = buff.getOut();
      buff.pop();
      switch (byte.value()) {
      case frame::tfend:
        return parse_result<uint8_t>(frame::fend);
      case frame::tfesc:
        return parse_result<uint8_t>(frame::fesc);
      default:
        return parse_result<uint8_t>(parse_result_t::bad_esc);
      }
    }
    buff.pop();
    return parse_result<uint8_t>(out.value());
  }

  void reset()
  {
    header.crc_calculated.init();
    state = parse_state::wait_fend;
  }

  template<typename Buff>
  void parse(Buff &buff, auto command_dispatcher)
  {
    auto result = unstuff(buff);
    if (!result) {
      if (result.error() != parse_result_t::not_enough_data) {
        reset();
      }
      if (result.error() == parse_result_t::frame_start) {
        state = parse_state::address;
      }
    }
    else {
      switch (state) {
      case parse_state::wait_fend:
        break;
      case parse_state::address:
        if (result.value() & frame::addr_mask) {
          header.address = result.value() & (~frame::addr_mask);
          header.crc_calculated.add_byte(header.address);
          state = parse_state::command;
          break;
        }
      case parse_state::command:
        header.command = result.value();
        header.crc_calculated.add_byte(header.command);
        state = parse_state::length;
        break;
      case parse_state::length:
        header.data_length = result.value();
        header.crc_calculated.add_byte(header.data_length);
        state   = parse_state::data;
        counter = 0;
        break;
      case parse_state::data:
        data_buffer[counter] = result.value();
        header.crc_calculated.add_byte(header.data_length);
        ++counter;
        if (counter == header.data_length) {
          if (use_crc) {
            state = parse_state::crc;
          }
          else {
            command_dispatcher(header, data_buffer);
            state = parse_state::wait_fend;
          }
        }
        break;
      case parse_state::crc:
        if (header.crc_calculated.get() == result.value()) {
          command_dispatcher(header, data_buffer);
        }
        state = parse_state::wait_fend;
      default:;
      }
    }
  }

private:
  parse_state              state = parse_state::wait_fend;
  std::array<uint8_t, 256> data_buffer;
  frame::header            header;
  uint8_t                  counter = 0;
  bool                     use_crc = false;
};

} // namespace wake