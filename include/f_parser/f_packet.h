#pragma once

#include "range.h"
#include <cstdint>
#include <iostream>
#include "f_parse_result.h"
#include "f_packet_nav.h"
#include "f_packet_full.h"
#include "fifo_atomic.h"

/// 6 * 2 = 12 bytes = 96 bits
/// 2400 => 25Hz


/// Context with all packet state
struct device_context
{
  constexpr static uint8_t id = 0x33;
  packet_rc_nav rc_nav;
  packet_rc_full rc_full;
};


/// Base functional parser class
template<typename T, typename Ctx, int sz>
class packet_parser_base {

private:
  using fifo_type = tl::fifo_atomic<uint8_t, sz>;
  fifo_type fifo;

public:
  using result_type = parse_result<
    tl::Range<tl::fifo_read_iterator<fifo_type>,
              tl::fifo_read_iterator<fifo_type>>>;
protected:
  constexpr static bool debug = false;
  auto correct_fifo()
  {
    auto ret = [&](auto ran) -> decltype(ran){
      if constexpr (debug) {
        std::cout << "correct_fifo: \n";
      }

      if (ran) {
        if (tl::makeRange(fifo) == ran.value()) {
          if constexpr (debug) {
            std::cout << "\tskip one byte\n";
          }
          auto correction = ran.value().begin() - fifo.begin();
          fifo.pop();
          return result_type(tl::Range(ran.value().begin() - 1, ran.value().end()));
        }
        if constexpr (debug) {
          std::cout << "\t corrected on packet\n";
        }
        auto correction = ran.value().begin() - fifo.begin();
        fifo.pop(correction);
        return ran;
      }

      if (ran.error() == parse_result_t::not_enough_data) {
        if constexpr (debug) {
          std::cout << "\t not_enough_data\n";
        }
        return ran;
      }
      return ran;
    };
    return ret;
  }
  inline auto check_byte(const uint8_t signature)
  {
    auto ret = [signature](auto ran) -> parse_result<decltype(ran)> {
      auto a = ran.begin().template get_value_le<uint8_t>();
      if constexpr (debug) {
        std::cout
          << "check byte:\n\texpected: "
          << std::hex
          << (int)signature
          << "\n\tfound: "
          << (int)(a.value())
          << std::endl;
      }
      if (a.value() == signature) {
        // Return range without signature
        //        return std::make_optional(tl::Range(ran.begin() + 1, ran.end()));
        return result_type(tl::Range(ran.begin() + 1, ran.end()));
      }
      return result_type(parse_result_t::signature_not_found);
    };
    return ret;
  }
  inline auto check_size(auto size)
  {
    auto ret = [=](auto ran) -> parse_result<decltype(ran)> {
      uint32_t szz = (ran.end() - ran.begin());
      if constexpr (debug) {
        std::cout
          << "check_size: \n\t"
          << "real: "
          << std::dec
          << szz
          << "\n\tneed: "
          << size
          << std::endl;
      }

      if ((ran.end() - ran.begin()) < size) {
        // Return empty range
        return result_type(parse_result_t::not_enough_data);
      }
      //      return std::make_optional(tl::Range(ran.begin(), ran.begin() + depth));
      return result_type(tl::Range(ran.begin(), ran.begin() + size));
      //    std::cout << "Ok" << std::endl;
    };
    return ret;
  }
  inline auto check_crc(auto crc_fun)
  {
    auto ret = [=](auto ran) -> parse_result<decltype(ran)> {
      //    std::cout << "check_crc" << std::endl;
      auto crc_itr = ran.end() - 2;
      uint16_t packet_crc = crc_itr.template get_value_le<uint16_t>().value();
      uint16_t crc = crc_fun(ran.begin(), ran.end() - 2);
      if constexpr (debug) {
        std::cout
          << "check_crc: \n\t"
          << "expected: "
          << std::hex
          << crc
          << "\n\tfound: "
          << packet_crc
          << std::endl;
      }


      if (packet_crc == crc_fun(ran.begin(), ran.end() - 2)) {
        //      std::cout << "Ok" << std::endl;
        return result_type(ran);
      }
      // Return empty range
      return result_type(parse_result_t::bad_crc);
    };
    return ret;
  }
  inline auto unwrap_if_fail(auto default_value)
  {
    auto ret = [default_value](auto opt) -> decltype(opt) {

      if (opt) {
        auto out = tl::Range(opt.value().end(),
                             default_value.value().end());

        if constexpr (debug) {
          std::cout << "unwrap_if_fail: success " << std::endl;
        }
        return parse_result(out);
      }
      if (opt.error() == parse_result_t::not_enough_data) {
          return opt;
      }
      if constexpr (debug) {
          std::cout << "unwrap_if_fail: default " << std::endl;
      }
      return default_value;
    };
    return ret;
  }
  inline auto mbind(auto fun)
  {
    auto ret = [=](auto opt) -> decltype(fun(opt.value())) {
      if (opt) {
        return fun(opt.value());
      }
      return opt;
    };
    return ret;
  }

  Ctx& context;

public:
  explicit packet_parser_base(Ctx& d_context) : context(d_context) {}

  void add_byte(uint8_t val)
  {
    fifo.push(val);
  }

  bool parse_buffer()
  {
    result_type(tl::makeRange(fifo))
    | static_cast<T*>(this)->parse()
    | correct_fifo();

    return true;
  }
};

/// Custom functional parser class/ Should be in user code
class packet_parser : public packet_parser_base<packet_parser, device_context, 1000> {

  inline auto parse_rc(auto& packet)
  {
    auto ret = [this, &packet](auto ran)  -> decltype(ran) {
      auto out =
        ran
        | mbind(check_byte(packet.signature)) // signature
        | mbind(check_size(sizeof(packet) + 3)) // id + crc
        | mbind(check_crc(crc_mav))
        | mbind(check_byte(device_context::id))                       // id
        | mbind(packet.fill())
        | unwrap_if_fail(ran);
      return out;
    };

    return ret;
  }
public:
  explicit packet_parser(device_context& d_context): packet_parser_base(d_context) {}

  inline auto parse()
  {
    auto ret = [this](auto ran) -> decltype(ran) {
      if constexpr (debug) {
        std::cout << "###parse: \n";
      }
      auto out =
        ran
        | parse_rc(context.rc_nav)
        | parse_rc(context.rc_full)

        ;
      return out;
    };

    return ret;
  }
};
