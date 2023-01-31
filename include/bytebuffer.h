#ifndef REG_BYTEBUFFER_H
#define REG_BYTEBUFFER_H

#include "fifo_atomic.h"
#include <cstdint>
#include <optional>

namespace tl {



/// Parsing all buffer to values
template<typename IB>
class byte_buffer
{
  static_assert(std::is_same<typename IB::value_type, uint8_t>::value,
                "Type should be uint8_t");

  template<typename T>
  union value {
    T       value;
    uint8_t buff[sizeof(T)];
  };

  IB begin_;
  IB end_;
  IB current;

public:
  byte_buffer(IB begin, IB end) : begin_(begin), end_(end), current(begin) {}

  template<typename T>
  std::optional<T> get_value_le()
  {
    value<T> val;
    uint8_t  cnt = 0;
    do {
      if (current != end_) {
        val.buff[cnt] = *current++;
      }
      else {
        return std::nullopt;
      }
      ++cnt;
    } while (cnt < sizeof(T));
    return std::optional<T>{ val.value };
  }

  template<typename T>
  std::optional<T> get_value_be()
  {
    value<T> val;
    uint8_t  cnt = 0;
    do {
      if (current != end_) {
        val.buff[sizeof(T) - 1 - cnt] = *current++;
      }
      else {
        return std::nullopt;
      }
      ++cnt;
    } while (cnt < sizeof(T));
    return std::optional<T>{ val.value };
  }

  /// get uint8 value from buffer
  template<>
  std::optional<uint8_t> get_value_le<uint8_t>()
  {
    if (current != end_) {
      uint8_t val = *static_cast<uint8_t *>(&(*current));
      ++current;
      return std::optional<uint8_t>{ val };
    }
    return std::nullopt;
  }
  /// get int8 value from buffer
  template<>
  std::optional<uint8_t> get_value_le<int8_t>()
  {
    if (current != end_) {
      int8_t val = *static_cast<int8_t *>(&(*current));
      ++current;
      return std::optional<int8_t>{ val };
    }
    return std::nullopt;
  }
};

} // namespace tl

#endif // REG_BYTEBUFFER_H
