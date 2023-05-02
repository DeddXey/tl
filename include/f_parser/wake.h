
#include "ranges/range.h"
#include <cstdint>
#include <optional>
#include "bufferdescriptor.h"
#include "value.h"


struct wake_frame {
  constexpr static uint8_t fend = 0xC0;
  constexpr static uint8_t fesc = 0xDB;
  constexpr static uint8_t tfend = 0xDC;
  constexpr static uint8_t tfesc = 0xDD;


  void stuff(const uint8_t byte, auto send_fun) {
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
  template<typename Rng>
  void send_to_port(Rng &ran, auto send_fun, bool crc_enable = true) {
    uint8_t crc = 0;
    if (crc_enable) {
    // TODO: crc calc
    }
    send_fun(fend);
    for (auto i: ran) {
      stuff(i, send_fun);
    }
    if (crc_enable) {
      stuff(crc, send_fun);
    }
  }





};