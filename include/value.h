#pragma once
#include <cstdint>
#include <optional>


template <typename R>
class parser {

  template<typename T>
  union value {
    T       value;
    uint8_t buff[sizeof(T)];
  };

  R range;
public:
  explicit parser(R& rng) : range(rng) {}
  explicit parser(R&& rng) : range(rng) {}


  R get_range() const
  {
    return range;
  }

  /// Get LE value from byte buffer
  template<typename T>
  std::enable_if<
    std::is_same_v<typename R::value_type, uint8_t>,
    std::optional<T>>::type
  get_value_le()
  {
    value<T> val;
    uint8_t  cnt = 0;

    auto curr = range.begin();

    do {
      if (curr != range.end()) {
        val.buff[cnt] = *curr;
        ++curr;
      }
      else {
        return std::nullopt;
      }
      ++cnt;
    } while (cnt < sizeof(T));

    range = R(curr, range.end());

    return std::optional<T>{ val.value };
  }
};



