#pragma once

#include "ranges/range.h"
#include <cstdint>
//#include "terminal.h"
#include "f_parse_result.h"
#include "f_packet_nav.h"
#include "f_packet_full.h"
#include "fifo_atomic.h"
//#include "value.h"


/// 6 * 2 = 12 bytes = 96 bits
/// 2400 => 25Hz


///// Context with all packet state
//struct device_context
//{
//  constexpr static uint8_t id = 0x33;
//  packet_rc_nav rc_nav;
//  packet_rc_full rc_full;
//};


/// Base functional parser class
template<typename T, typename Ctx, size_t sz>
class packet_parser_base {

public:

  using fifo_type = tl::fifo_atomic<uint8_t, sz>;


  using result_type = parse_result<
      tl::Range<typename fifo_type::iterator,
          typename fifo_type::iterator >>;
protected:
    constexpr static bool debug = false;
    fifo_type fifo;
    Ctx *context;

  auto correct_fifo()
  {
    auto ret = [this](auto ran) -> decltype(ran){
      if constexpr (debug) {
        con.debug() << "corr_f: ";
      }

      if (ran) {
        if (tl::makeRange(fifo) == ran.value()) {
          if constexpr (debug) {
            con.debug()
                << Attr::bright << "skip\n"
                << Attr::reset;
          }
          fifo.pop();
          return result_type(tl::Range(ran.value().begin() - 1, ran.value().end()));
        }
        if constexpr (debug) {
          con.debug() << Attr::bright
                      << "corr on p\n"
                      << Attr::reset;
        }
        auto correction = ran.value().begin() - fifo.begin();
        if constexpr (debug) {
          con.debug() << "corr " << Use::dec << (int32_t )correction << "\n";
          con.debug()
            << " corr rng:\n" << ran.value();
          con.debug()
            << "f b:\ne"<< tl::makeRange(fifo);

        }

        fifo.pop(static_cast<size_t>(correction));
        if constexpr (debug) {

          con.debug()
            << "f a:\n"<< tl::makeRange(fifo);
        }

        return ran;
      }

      if (ran.error() == parse_result_t::not_enough_data) {
        if constexpr (debug) {
          con.debug() << "not_en_dat\n";
        }
        return ran;
      }
      return ran;
    };
    return ret;
  }
  auto check_byte(const uint8_t signature)
  {
    auto ret = [signature](auto ran) -> parse_result<decltype(ran)> {

      parser prs(ran);
      auto a = prs.template get_value_le<uint8_t>();
//        auto a = ran.begin().template get_value_le<uint8_t>();
//      if constexpr (debug) {
//        con.debug()
//          << "check byte: expected: "
//          << Use::hex
//          << signature
//          << " found: "
//          << (a.value())
//          << Use::endl;
//      }
      if (a) {
        if (a.value() == signature) {
          // Return range without signature
          //        return std::make_optional(tl::Range(ran.begin() + 1, ran.end()));
          return result_type(tl::Range(ran.begin() + 1, ran.end()));
        }
      }
      return result_type(parse_result_t::signature_not_found);
    };
    return ret;
  }


  /// Check bits by mask
  /// Not changed the range
  template<typename S>
  auto check_bits(const S mask)
  {
    auto ret = [mask](const auto ran) -> parse_result<decltype(ran)> {

      parser prs(ran);
      auto optval = prs.template get_value_le<S>();

      if (optval) {
        if ((optval.value() & mask) == mask) {
          return result_type(tl::Range(ran.begin(), ran.end()));
        }
      }
      return result_type(parse_result_t::signature_not_found);
    };
    return ret;
  }


  auto check_size(size_t size)
  {
    auto ret = [=](auto ran) -> parse_result<decltype(ran)> {
//      if constexpr (debug) {
//        uint32_t szz = (ran.end() - ran.begin());
//        con.debug()
//          << "check_size: real: "
//          << Use::w0
//          << Use::dec
//          << szz
//          << " need: "
//          << size - 1
//          << Use::endl;
//      }

      if ((ran.end() - ran.begin()) < static_cast<ssize_t>(size) - 1 ) {
        // Return empty range
        return result_type(parse_result_t::not_enough_data);
      }
      if constexpr (debug) {
        con.debug() << "***" << (int32_t)(ran.end() - ran.begin()) << "\n";
        con.debug() << "i1 " << ran.begin().get_idx() << "\n";
        con.debug() << "i2 " << ran.end().get_idx() << "\n";

        con.debug() << tl::Range(ran.begin(), ran.begin() + size - 1);
      }
      return result_type(tl::Range(ran.begin(), ran.begin() + static_cast<ssize_t>(size) - 1));
    };
    return ret;
  }

  auto check_crc(auto crc_fun)
  {
    auto ret = [=](auto ran) -> parse_result<decltype(ran)> {
      // std::cout << "check_crc" << std::endl;
//      auto crc_itr = ran.end() - 2;

      parser prs(tl::Range(ran.end() - 2, ran.end()));
//        uint16_t packet_crc = crc_itr.template get_value_le<uint16_t>().value();
      uint16_t packet_crc = prs.template get_value_le<uint16_t>().value();
      if constexpr (debug) {
        uint16_t crc = crc_fun(ran.begin(), ran.end() - 2);
        con.debug()
          << Fg::red
          << Use::w4
          << "crc: exp: "
          << Use::hex << crc
          << " found: "
          << packet_crc
          << Attr::reset
          << Use::endl;
      }

      if constexpr (debug) {
        con.debug() << tl::Range(ran.begin(), ran.end() - 2);
      }

      if (packet_crc == crc_fun(ran.begin(), ran.end() - 2)) {
        if constexpr (debug) {
          con.debug() << "Ok\n";
        }
        return result_type(ran);

      }
      // Return empty range
      return result_type(parse_result_t::bad_crc);
    };
    return ret;
  }

  auto unwrap_if_fail(auto default_value)
  {
    auto ret = [default_value](auto opt) -> decltype(opt) {

      if (opt) {
        auto out = tl::Range(opt.value().end(),
                             default_value.value().end());

        if constexpr (debug) {
          con.debug() << "u_i_f: s\n";
          con.debug()
            << "def:\n" << default_value.value();
          con.debug()
            << "in:\n" << opt.value();
          con.debug()
            << "out:\n" << out;
        }
        return parse_result(out);
      }
      if (opt.error() == parse_result_t::not_enough_data) {
          return opt;
      }
      if constexpr (debug) {
          con.debug() << "unwrap_if_fail: default\n";
      }
      return default_value;
    };
    return ret;
  }
  auto mbind(auto fun)
  {
    auto ret = [=](auto opt) -> decltype(fun(opt.value())) {
      if (opt) {
        return fun(opt.value());
      }
      return opt;
    };
    return ret;
  }

public:

  explicit packet_parser_base(Ctx* d_context) : context(d_context) {}

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
template<typename C, size_t fifo_len = 300>
class packet_parser
  : public packet_parser_base<packet_parser<C, fifo_len>, C, fifo_len> {

  using base = packet_parser_base<packet_parser<C, fifo_len>, C, fifo_len>;
//  C& context;




  template <typename T>
  auto parse_rc(T&& packet)
  {
    auto ret = [this, &packet](auto ran)  -> decltype(ran) {
//      con.debug() <<"@";
      auto out =
        ran
        | base::mbind(base::check_byte(packet.signature)) // signature
        | base::mbind(base::check_size(packet.size())) // id + crc
        | base::mbind(base::check_crc(crc_mav))
        | base::mbind(base::check_byte(base::context->id))                       // id
        | base::mbind(packet.fill())
        | base::unwrap_if_fail(ran);

      return out;
    };

    return ret;
  }
public:
  explicit packet_parser(C* d_context):
    packet_parser_base<packet_parser<C>, C, fifo_len>(d_context) {}

  auto parse()
  {
    auto ret = [this](auto ran) -> decltype(ran) {

      auto out =
        ran
        | parse_rc(packet_rc_full(base::context))
        | parse_rc(packet_rc_nav(base::context))

        ;
      return out;
    };

    return ret;
  }
};
